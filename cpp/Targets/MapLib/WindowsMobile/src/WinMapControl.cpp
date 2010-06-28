/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "WinMapControl.h"

/* MCShared includes */
#include "MC2Point.h"
#include "SharedHttpDBufRequester.h"
#include "MemoryDBufRequester.h"
#include "WinTCPConnectionHandler.h"
#include "HttpClientConnection.h"
#include "WinTileMapToolkit.h"
#include "WinMapPlotter.h"
#include "WinBitmap.h"
#include "TileMapHandler.h"
#include "MC2Coordinate.h"
#include "TileMapTextSettings.h"
#include "PixelBox.h"
#include "VectorMapConnection.h"
#include "MapSwitcher.h"
#include "CursorVisibilityAdapter.h"
#include "StringTextConv.h"
#include "DxDraw.h"

//#include "SpriteMover.h"

#include <tchar.h>

#include "LogFile.h"

#define _USE_NAV2_LIBRARY_
#define USE_FILE_CACHE
#define USE_VECTOR_MAPS
#define DEFAULT_CACHE_SIZE 8*1024*1024

/* the name of the window class */
static const char* WINCLASSNAME = "MAPWINCLASS";

/* the title of the window */
static const char* WINTITLE = "WinMapClient";

//*******************************************************************

/* private constructor */
WinMapControl2::WinMapControl2(HWND parent)
   :  m_hwnd(parent),
      m_plotter(NULL),
      m_toolkit(NULL),  
      m_vectorMapConnection(NULL),
      m_bCanRepaint(TRUE),
      m_winMapLib( NULL ),
      m_mapDrawingInterface( NULL ),
      m_mapMovingInterface( NULL ),
      m_mapSwitcher( NULL ),
      m_cursorVisibilityAdapter(NULL),
      m_globeWrapper(NULL),
      m_cursor(NULL),
      m_tileMapKeyHandler(NULL),
      m_bUseGlobe(FALSE)

#ifdef TEMPORARY_VERSION
   ,  
      m_httpConn(NULL),
      m_memReq(NULL),
      m_connHandler(NULL),
      m_mapHandler(NULL),
      m_requester(NULL),
      m_fileRequester(NULL)  
#endif
   ,
      m_memTracker(NULL)
{   
}

/* second-phase constructor */
bool WinMapControl2::construct(TileMapEventListener* tileMapEventListener,
                               MMessageSender* sender,
                               LPCREATESTRUCTW cParams, 
                               const char* mapServer, int portNum,
                               const char* sessionId,
                               const char* sessionKey,
                               const wchar_t* appBasePath,
                               TileMapKeyHandlerCallback* tileMapKeyHandlerCallback,
                               const std::string& languagePrefix,
                               BOOL bUseGlobe,
                               ICursorSpritesProvider *csp
   )
{ 
   LogFile::Create();

   setUseGlobe(bUseGlobe);

   /* get the dimensions of the window */
   m_width = cParams->cx;
   m_height = cParams->cy;

   /*Inint the Dx wrapper*/
   DxDraw::getInstance()->Init(m_hwnd);
   

   /* create the toolkit */
   m_toolkit = new WinTileMapToolkit(m_hwnd, cParams->hInstance);
   if(m_toolkit == NULL) return(false);

   /* create the plotter */
   m_plotter = isab::WinMapPlotter::allocate(m_width, m_height);
   if(m_plotter == NULL) return(false);
   /* set the blitting position for the plotter */

#ifdef TEMPORARY_VERSION
   
   m_vectorMapConnection = new CVectorMapConnection( sender );
   if ( NULL == m_vectorMapConnection )
      return false;

   m_requester = new SharedDBufRequester( m_vectorMapConnection );
   if ( NULL == m_requester )
      return false;

   m_nMapCacheSize = DEFAULT_CACHE_SIZE;
# ifdef USE_FILE_CACHE

   m_memTracker = new MemTracker(1024 * 1024);
   // Create file cache with default cache size with the m_requester as parent
   m_fileRequester = new WinMFDBufRequester( m_requester, appBasePath, m_memTracker, m_nMapCacheSize );

   // Create memory cache with the dbuf requester as parent
   m_memReq = new MemoryDBufRequester(m_fileRequester, m_nMapCacheSize);

# else // USE_FILE_CACHE

   // Create memory cache with the iRequester as parent
   m_memReq = new MemoryDBufRequester(m_requester, m_nMapCacheSize);

# endif // USE_FILE_CACHE

   if(m_memReq == NULL) 
   {
      /* delete the requester, since it wont get deleted thru 
         the TileMapHandler's destructor */
      delete m_requester;
      return(false);
   }

   /* create the map handler */
   m_mapHandler = new TileMapHandler(m_plotter,
                                     m_memReq,
                                     m_toolkit,
                                     tileMapEventListener);

   if(m_mapHandler == NULL) return(false);

   /* create the maplib */
   m_winMapLib = new WinMapLib( m_mapHandler );   

#else // TEMPORARY_VERSION
   
   m_vectorMapConnection = new CVectorMapConnection( sender );
   
   if ( NULL == m_vectorMapConnection )
      return false;   

   /* create the maplib */
   m_winMapLib = new WinMapLib( m_plotter, m_toolkit, m_vectorMapConnection );
   if ( NULL == m_winMapLib )
      return false;

   m_winMapLib->setLanguageAsISO639(languagePrefix.c_str());

#endif // TEMPORARY_VERSION


   if (useGlobe()) {
      Rect rect;
      rect.topLeft.x = 0;
      rect.topLeft.y = 0;
      rect.bottomRight.x = m_width;
      rect.bottomRight.y = m_height;

      std::string sDataPath;
      std::wstring dataPath = appBasePath + std::wstring(_T("data\\"));
      StringTextConverter::Unicode2UTF(dataPath,sDataPath);
      m_globeWrapper  = new GlobeMapComponentWrapper(sDataPath.c_str(), rect, NULL, GetDC( m_hwnd ),languagePrefix );   
   }      

   MapSwitcher::SwitcherNotice tilemapNotice;

   if (useGlobe())
      tilemapNotice.m_scale = m_globeWrapper->getMinScale();
   else
      tilemapNotice.m_scale = 2000000000;

   tilemapNotice.m_mapMover = m_winMapLib->getMapMovingInterface();
   tilemapNotice.m_mapDrawer = m_winMapLib->getMapDrawingInterface();
   tilemapNotice.m_mapRect = m_plotter;
   tilemapNotice.m_hasStaticCursor = false;
   m_plotter->setVisible(true);
   tilemapNotice.m_visibles.push_back(m_plotter);
   
   MapSwitcher::SwitcherNotice globeNotice;


   if (useGlobe()) {
      globeNotice.m_scale = 2000000000; // This ought to be enough.
      globeNotice.m_mapMover = &m_globeWrapper->getMapMovingInterface();
      globeNotice.m_mapDrawer = &m_globeWrapper->getMapDrawingInterface();
   
      m_globeWrapper->getVisibilityAdapter()->setVisible(true);
      globeNotice.m_visibles.push_back(m_globeWrapper->getVisibilityAdapter());

      globeNotice.m_hasStaticCursor = true;
      globeNotice.m_mapRect = &m_globeWrapper->getMapRectInterface();
      m_cursorVisibilityAdapter = new CursorVisibilityAdapter(NULL);
      tilemapNotice.m_visibles.push_back(m_cursorVisibilityAdapter);
   }


   vector<MapSwitcher::SwitcherNotice> selectorNotices;
   selectorNotices.push_back(tilemapNotice);
   
   if (useGlobe()) {
      selectorNotices.push_back(globeNotice);
      m_mapSwitcher = new MapSwitcher(selectorNotices, EScale12);
   }
   
   
   if (useGlobe()) {
      m_mapMovingInterface = m_mapSwitcher;
      m_mapDrawingInterface = m_mapSwitcher;
   } else {
      m_mapMovingInterface = m_winMapLib->getMapMovingInterface();
      m_mapDrawingInterface = m_winMapLib->getMapDrawingInterface();
   }   
   
   initKeyHandler( m_hwnd, tileMapKeyHandlerCallback, csp );


   if (useGlobe()) {
      m_mapSwitcher->setCursorHandler(m_tileMapKeyHandler);
      m_cursorVisibilityAdapter->setControl(m_tileMapKeyHandler);
      m_cursorVisibilityAdapter->setVisible(false);
   }
         
   m_nMapCacheSize = DEFAULT_CACHE_SIZE;

# ifdef USE_FILE_CACHE
   m_winMapLib->setDiskCacheSize( m_nMapCacheSize );
# else
   m_winMapLib->setMemoryCacheSize( m_nMapCacheSize );
# endif   

   m_winMapLib->addEventListener( tileMapEventListener );

   //getMapDrawingInterface()->repaintNow();
   
#ifdef USE_VECTOR_MAPS
   m_bUseVectorMaps = TRUE;
#else
   m_bUseVectorMaps = FALSE;
#endif 

   /* success */
   return(true);
}

/* allocator */
WinMapControl2* WinMapControl2::allocate(HWND parent, TileMapEventListener* tileMapEventListener,
                                         MMessageSender* sender, 
                                         LPCREATESTRUCTW cParams,
                                         const char* mapServer, int portNum,
                                         const char* sessionId,
                                         const char* sessionKey,
                                         const wchar_t* appBasePath,
                                         TileMapKeyHandlerCallback* tileMapKeyHandlerCallback,
                                         const std::string& languagePrefix,
                                         BOOL bUseGlobe,
                                         ICursorSpritesProvider *csp
   )
{
   /* create a new object */
   WinMapControl2* newObj = new WinMapControl2( parent );
   if(newObj == NULL) return(NULL);

   /* do second-phase */
   if(!newObj->construct(tileMapEventListener, sender, cParams, mapServer, portNum, sessionId, sessionKey, appBasePath, tileMapKeyHandlerCallback,languagePrefix, bUseGlobe, csp)) {
      /* error while second phase construction */
      delete newObj;
      return(NULL);
   }

   /* success, return the allocated object */
   return(newObj);
}

void WinMapControl2::initKeyHandler( HWND hWnd, TileMapKeyHandlerCallback* tileMapKeyHandlerCallback , ICursorSpritesProvider *csp )
{   
   POINT point = { 0, 0 };

   if (NULL != tileMapKeyHandlerCallback) {

      SpriteMover* highlightCursorSprite;
      SpriteMover* cursorSprite;

      ASSERT(csp);

      cursorSprite = csp->initCursorSprite(
         hWnd,
         getPlotter().getBuffer(),
         point,
         TRUE ); // visible.

      highlightCursorSprite = csp->initHighlightedCursorSprite(
         hWnd,
         getPlotter().getBuffer(),
         point,
         FALSE ); // not visible.

      m_cursor = new CursorSprite( cursorSprite,  highlightCursorSprite );

      m_tileMapKeyHandler = new TileCursorKeyHandler( getMapMovingInterface(), getMapDrawingInterface(),
                                                      m_toolkit,
                                                      m_cursor,
                                                      tileMapKeyHandlerCallback );

      m_tileMapKeyHandler->setMapBox( PixelBox( MC2Point( 0, 0 ) , MC2Point( m_width, m_height ) ) );
   }
   
}

/* destructor */
WinMapControl2::~WinMapControl2()
{
   if (NULL != m_mapSwitcher){
      delete m_mapSwitcher;
   }
   if (NULL != m_cursorVisibilityAdapter){
      delete m_cursorVisibilityAdapter;
   }
   if (NULL != m_globeWrapper){
      delete m_globeWrapper;
   }
   if (NULL != m_cursor){
      delete m_cursor;
   }
   if (NULL != m_tileMapKeyHandler){
      delete m_tileMapKeyHandler;
   }

#ifdef TEMPORARY_VERSION
   // not used 
   //if (NULL != m_connHandler)  delete m_connHandler;
    
   // should be deleted by MapLibInternal
   //if (NULL != m_mapHandler)   delete m_mapHandler;

   // should be deleted by WinMFDBufRequester::MultiFileDBufRequester::DBufRequester 
   //if (NULL != m_requester) delete m_requester;
    
   // should be deleted by MemoryDBufRequester::DBufRequester
   //if (NULL != m_fileRequester)   delete m_fileRequester;
#endif
   
   if (NULL != m_winMapLib)   delete m_winMapLib;
   
   // should be deleted by TileMapHandler::TileMapHandlerDBBufRequester
   //if (NULL != m_memReq) delete m_memReq;

   if (NULL != m_toolkit)  delete m_toolkit;

   // should be deleted by MapLibInternal, but due to we use deprecated MapLib( TileMapHandler* handler ) constructor we have to 
   // delete it ourselves.
   if (NULL != m_plotter)  delete m_plotter;   

   if (NULL != m_memTracker ) delete m_memTracker;
   
   if (NULL != m_vectorMapConnection)  delete m_vectorMapConnection;

   DxDraw::Destroy();

   LogFile::Print("WINCONTROL : Deleted!\n");
}

void WinMapControl2::setUseVectorMaps( BOOL bUse )
{
   m_bUseVectorMaps = bUse;
}

BOOL WinMapControl2::getUseVectorMaps()
{
   return m_bUseVectorMaps;
}

void WinMapControl2::setMapCacheSize( int nSize )
{
   m_nMapCacheSize = nSize;

#ifdef TEMPORARY_VERSION
   
   if( m_fileRequester ){
      if( m_nMapCacheSize == 0 ){
         clearMapCache();
      }
      m_fileRequester->setMaxSize( m_nMapCacheSize*1024*1024 );
   }

#else

# ifdef USE_FILE_CACHE
   m_winMapLib->setDiskCacheSize( m_nMapCacheSize );
# else
   m_winMapLib->setMemoryCacheSize( m_nMapCacheSize );
# endif   

#endif // TEMPORARY_VERSION

}

void WinMapControl2::clearMapCache()
{

#ifdef TEMPORARY_VERSION

   if ( m_fileRequester ) {
      m_fileRequester->clearCache();
   }

#else

# ifdef USE_FILE_CACHE
   m_winMapLib->clearDiskCache();
# else
   m_winMapLib->clearMemoryCache();
# endif 

#endif
} 

int WinMapControl2::getMapCacheSize()
{
   return m_nMapCacheSize;
}

/* gets the handle to the internal window */
HWND WinMapControl2::getWindow() const
{
   return(m_hwnd);
}

/* displays the map drawing buffer on the specified DC */
void WinMapControl2::blitTo(HDC destDC, int32 dX, int32 dY)
{
   m_plotter->blitTo(destDC, dX, dY);
   return;
}


void WinMapControl2::moveMap(int32 dx, int32 dy)
{
   /* tell the map Handler to move the map */
   getMapMovingInterface()->move( dx, dy );   
  
   /* request a repaint */
   doRepaint();

   return;
}

void WinMapControl2::rotateMap(int32 angle)
{
   /* tell the map Handler to rotate the map to the left */
   getMapMovingInterface()->rotateLeftDeg(angle);
   /* request a repaint */
   doRepaint();
   return;
}

void WinMapControl2::zoomMap(float32 factor)
{
   /* tell the map Handler to zoom the map */
   getMapMovingInterface()->zoom(factor);
   /* request a repaint */
   doRepaint();
   return;
}

void WinMapControl2::resetRotation()
{
   /* set the rotation to zero */
   getMapMovingInterface()->setAngle(0);
   /* request a repaint */
   doRepaint();
   return;
}

/* request a repaint from the handler */
void WinMapControl2::doRepaint()
{
   /* request a repaint */
   if (m_bCanRepaint)
   {
      getMapDrawingInterface()->requestRepaint();
   }

   return;
}

/* resizes the control to the specified dimensions
   returns true on success, false on error */
bool WinMapControl2::resize(int32 nW, int32 nH)
{
   bool result2 = true;
   bool result1 =  m_plotter->resize(nW, nH);

   SIZE  screenSize;
   screenSize.cx = GetSystemMetrics(SM_CXSCREEN);
   screenSize.cy = GetSystemMetrics(SM_CYSCREEN);

   if (useGlobe()) {
      //temp fix
      // check if we're in wide mode
      //
      // q-alo: if increasing done - shifted coordinates for globe painting in wide mode,
      // so this should be removed:
      // 
      //      if ((nW > nH) && (nW < screenSize.cx))
      //         result2 = m_globeWrapper->resize(nW + 5, nH);
      //      else
      result2 = m_globeWrapper->resize(nW, nH);
   }
 
   if (NULL != m_tileMapKeyHandler)
      m_tileMapKeyHandler->setMapBox(PixelBox(MC2Point(0,0), MC2Point(nW,nH)));

   /* resize didn't work */
   return   ( result1 && result2);
}

/* gets the name of the feature at the specified location */
const char* WinMapControl2::getFeatureName(const MC2Point& pos)
{
   const ClickInfo& clickInfo = getMapMovingInterface()->getInfoForFeatureAt( pos, false );
   return clickInfo.getName();
}

/* gets the lat\lon location of the feature at the specified point */
void WinMapControl2::getFeatureLocation(const MC2Point& pos, 
                                        MC2Coordinate& outLoc)
{
   /* get the feature location from the TileMapHandler */   
   getMapMovingInterface()->inverseTransform( outLoc, pos );
   return;
}

/* gets the screen position of the feature at the specified coordinate */
void WinMapControl2::getFeaturePosition(const MC2Coordinate& loc, 
                                        MC2Point& outPos)
{
   /* get the feature position from the TileMapHandler */
   getMapMovingInterface()->transform(outPos, loc);
   return;
}

/* gets all info for the feature at the specified point */
void WinMapControl2::getFeatureInfo(const MC2Point& pos,
                                    TileMapHandlerClickResult& result)
{   
   // is never used.
}

const ClickInfo& WinMapControl2::getInfoForFeatureAt( const MC2Point& point,
                                                      bool onlyPois,
                                                      TileMapInfoCallback* infoCallback )
{
   const ClickInfo& clickInfo = getMapMovingInterface()->getInfoForFeatureAt( point, false );
   return clickInfo;
}

void
WinMapControl2::setTextSettings( const TileMapTextSettings& settings )
{
   m_mapHandler->setTextSettings( settings );
}

void 
WinMapControl2::setHighlight( bool highlight )
{
   // The crosshair looks the same, highlighted or not highlighted.
}

void
WinMapControl2::setCursorPos( const MC2Point& pos )
{
   // The cursor is always in the center...
}

MC2Point 
WinMapControl2::getCursorPos() const
{
   return MC2Point( m_width >> 1,
                    m_height >> 1 );
}

void 
WinMapControl2::setCursorVisible( bool visible )
{
   
   m_cursor->setCursorVisible(visible);
}

void 
WinMapControl2::getCursorBox( PixelBox& box ) const
{
   box = PixelBox( MC2Point( 0, 0), 
                   MC2Point( m_width, m_height ) );

}

void WinMapControl2::setWinPaintCallback( WinPaintCallback* pWinPaintCallback)
{
   m_plotter->setWinPaintCallback(pWinPaintCallback);
   if (useGlobe()) {   
      m_globeWrapper->setWinPaintCallback(pWinPaintCallback);
   }
}

void WinMapControl2::setBlitPoint(int x, int y)
{
   m_plotter->setBlitPoint(x, y);
   if (useGlobe()) {   
      m_globeWrapper->setBlitPoint(x, y);
   }
}

void WinMapControl2::connect() {
   /* request a repaint */
   getMapDrawingInterface()->requestRepaint();
   return;
}
 
void WinMapControl2::setCanRepaint(BOOL bCanRepaint){
   m_toolkit->setCanRepaint(bCanRepaint);
   m_bCanRepaint = bCanRepaint;
}

MapDrawingInterface* WinMapControl2::getMapDrawingInterface() const
{
   return m_mapDrawingInterface;
}
   
MapMovingInterface* WinMapControl2::getMapMovingInterface() const
{
   return m_mapMovingInterface;
}

MapLib* WinMapControl2::getMapLib() const
{
   return m_winMapLib;
}

CursorSprite* WinMapControl2::getCursor() const
{
   return m_cursor;
}

TileMapKeyHandler* WinMapControl2::getTileMapKeyHandler() const
{
   return m_tileMapKeyHandler;
}

CursorVisibilityAdapter*  WinMapControl2::getCursorVisibilityAdapter() const
{
   return m_cursorVisibilityAdapter;
}

BOOL WinMapControl2::isGlobeActive()
{
   if (m_mapMovingInterface && m_globeWrapper && useGlobe()) {
      if (m_mapMovingInterface->getScale() >=  m_globeWrapper->getMinScale()) 
         return TRUE;
   }

   return FALSE;
}

#ifdef TEMPORARY_VERSION  
TileMapHandler& WinMapControl2::getHandler() const
{
   return *m_mapHandler;
}
#endif

BOOL WinMapControl2::useGlobe() const
{
   return m_bUseGlobe;
}

void WinMapControl2::setUseGlobe(BOOL bUse)
{
   m_bUseGlobe = bUse;
}

int32 WinMapControl2::width() const
{
   isab::Rectangle rect;
   m_plotter->getMapSizePixels(rect);
   return( rect.getWidth() );
}

int32 WinMapControl2::height() const
{
   isab::Rectangle rect;
   m_plotter->getMapSizePixels(rect);
   return( rect.getHeight() );
}

isab::WinMapPlotter& WinMapControl2::getPlotter() const
{
   return(*m_plotter);
}

/**
 * True if in 3d mode, false if not.
 */
bool WinMapControl2::get3dMode()
{
   return m_mapHandler->get3dMode();
}

void WinMapControl2::set3dMode(bool on)
{
   m_mapHandler->set3dMode(on);
}

void WinMapControl2::setNightMode(bool on)
{
   m_mapHandler->setNightModeL(on);
}

void WinMapControl2::setHorizonHeight( uint32 height )
{  
   m_mapHandler->setHorizonHeight(height);
}

//*******************************************************************


