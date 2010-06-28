/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef WINMAPCONTROL_H
#define WINMAPCONTROL_H

/* forward declarations */
class WinTCPConnectionHandler;
class HttpClientConnection;
class WinTileMapToolkit;
class WinBitmap;
class MC2Point;
class TileMapHandler;
class TileMapHandlerClickResult;
class MemoryDBufRequester;
class MC2Coordinate;
class TileMapTextSettings;
class PixelBox;
class NetDBufRequester;
class CVectorMapConnection;
class MMessageSender;
class MapSwitcher;
class MapComponentWrapper;
class CursorVisibilityAdapter;
class TileMapKeyHandler;
class CursorSprite;
class TileMapKeyHandlerCallback;

#include "config.h"
#include "WinMapPlotter.h"
#include "Cursor.h"
#include "WinMFDBufRequester.h"
#include "TileMapEventListener.h"
#include "WinMapLibInterface.h"

// Be aware it's just temporary version !!!
// It will be changes soon.
#define TEMPORARY_VERSION

#include "WinMapLib.h"

class ICursorSpritesProvider{
public:
   
   virtual SpriteMover* initCursorSprite(HWND hwnd, HDC hDC, const POINT& pos, int visible) = 0;
   virtual SpriteMover* initHighlightedCursorSprite(HWND hwnd, HDC hDC, const POINT& pos, int visible) = 0;
   
   virtual ~ICursorSpritesProvider() {}
};

/* Handles the connection to the TMap Server 
   and the drawing of the Map */
class WinMapControl2 : public Cursor,
                       public WinMapLibInterface
{
   /** data **/
   private:
      /* the main window */
      HWND m_hwnd;

      /* the width and height of the control */
      int32 m_width, m_height;

      /* the map plotter */
      isab::WinMapPlotter* m_plotter;

      /* the platform toolkit */
      WinTileMapToolkit* m_toolkit;

      BOOL m_bUseVectorMaps;

      int m_nMapCacheSize;

      BOOL m_bCanRepaint;    
      
      WinMapLib* m_winMapLib;

      MapComponentWrapper* m_globeWrapper;

      MapSwitcher* m_mapSwitcher;

      MapMovingInterface* m_mapMovingInterface;

      MapDrawingInterface* m_mapDrawingInterface;

      TileMapKeyHandler* m_tileMapKeyHandler;
      CursorSprite* m_cursor;
      CursorVisibilityAdapter*  m_cursorVisibilityAdapter;
      
      BOOL m_bUseGlobe;


#ifdef TEMPORARY_VERSION

      /* the TCP Connection handler */
      WinTCPConnectionHandler* m_connHandler;

      /* the HTTP Connection handler */
      HttpClientConnection* m_httpConn;

      /* the memory caching requester */
      MemoryDBufRequester* m_memReq;

      /* the Tile Map Handler */
      TileMapHandler* m_mapHandler;

      NetDBufRequester* m_requester;
      WinMFDBufRequester* m_fileRequester;

#endif

      MemTracker* m_memTracker;

   /** methods **/
   private:

      /* private constructor */
      WinMapControl2( HWND parent );

      /* second-phase constructor */
      bool construct(TileMapEventListener* tileMapEventListener,
                     MMessageSender* sender,
                     LPCREATESTRUCTW cParams,
                     const char* mapServer, int portNum,
                     const char* sessionId,
                     const char* sessionKey, 
                     const wchar_t* appBasePath,
                     TileMapKeyHandlerCallback* tileMapKeyHandlerCallback,
                     const std::string& languagePrefix,
                     BOOL bUseGlobe,
                     ICursorSpritesProvider *);

      void initKeyHandler( HWND hWnd, TileMapKeyHandlerCallback* tileMapKeyHandlerCallback , ICursorSpritesProvider *);

   public:

      CursorSprite* getCursor() const;

      TileMapKeyHandler* getTileMapKeyHandler() const;

      CursorVisibilityAdapter* getCursorVisibilityAdapter() const;


      class HttpDBufConnection* m_vectorMapConnection;

      /* allocator */
      static WinMapControl2*
      allocate(HWND parent,
               TileMapEventListener* tileMapEventListener,
               MMessageSender* sender,
               LPCREATESTRUCTW cParams,
               const char* mapServer, int portNum,
               const char* sessionId,
               const char* sessionKey,
               const wchar_t* appBasePath,
               TileMapKeyHandlerCallback* tileMapKeyHandlerCallback,
               const std::string& languagePrefix,
               BOOL bUseGlobe,
               ICursorSpritesProvider *);

      enum ZoomScale
      {
         EScaleMin = 1,
         EScale1   = 2,
         EScale2   = 4,
         EScale3   = 10,
         EScale4   = 20,
         EScale5   = 50,
         EScale6   = 100,
         EScale7   = 500,
         EScale8   = 1000,
         EScale9   = 2000,
         EScale10  = 5000,
         EScale11  = 10000,
         EScale12  = 14000
      }; 

      /* destructor */
      ~WinMapControl2();

      /**
       *	 Set using vector maps flag.
       */
      void setUseVectorMaps( BOOL bUse );

      /**
       *	 Get using vector maps flag.
       */
      BOOL getUseVectorMaps();

      /**
       *	 Set map cache size.
       */
      void setMapCacheSize( int nSize );

      /**
       *	 Clear map cache.
       */
      void clearMapCache();

      /**
       *	 Get map cache size.
       */
      int getMapCacheSize();

      /* gets the handle to the internal window */
      HWND getWindow() const;

      /* displays the map-drawing buffer on the specified DC */
      void blitTo(HDC destDC, int32 dx, int32 dy);

      /* moves the map */
      void moveMap(int32 dx, int32 dy);

      /* rotates the map */
      void rotateMap(int32 angle);

      /* zooms the map */
      void zoomMap(float32 factor);

      /* resets the map rotation to zero degrees */
      void resetRotation();

      /* request a repaint from the handler */
      void doRepaint();

      /* resizes the control to the specified dimensions
         returns true on success, false on error */
      bool resize(int32 nW, int32 nH);

      /* gets the name of the feature at the specified location */
      const char* getFeatureName(const MC2Point& pos);

      /* gets the lat\lon location of the feature at the specified point */
      void getFeatureLocation(const MC2Point& pos, MC2Coordinate& outLoc);

      /* gets the screen position of the feature at the specified coordinate */
      void getFeaturePosition(const MC2Coordinate& loc, MC2Point& outPos);

      void setBlitPoint(int x, int y);      

      /* gets the width of the control */
      int32 width() const;

      /* gets the height of the control */
      int32 height() const;

      /* gets a reference to the internal MapPlotter */
      isab::WinMapPlotter& getPlotter() const;

      /* gets all info for the feature at the specified point */
      void getFeatureInfo(const MC2Point& pos,
                          TileMapHandlerClickResult& result);

      /**
       *   Yet another interface to clicking.
       *   Returns a reference to info for the feature at the specified
       *   position on the screen.
       *   @param point    The point on the screen where
       *                   the info should be found.
       *   @param onlyPois True if only poi info should be returned.
       */
      const ClickInfo&
         getInfoForFeatureAt( const MC2Point& point,
                              bool onlyPois,
                              TileMapInfoCallback* infoCallback = NULL );

      /**
       *   Sets the text settings ( fonts ) for text placement
       *   in maps.
       */
      void setTextSettings( const TileMapTextSettings& settings );

      /**
       *    Implements abstract method in Cursor.
       *    Currently doesn't do anything.
       */
      void setHighlight( bool highlight );

      /**
       *    Implements abstract method in Cursor.
       *    Currently doesn't do anything.
       */
      void setCursorPos( const MC2Point& pos );
      
      /**
       *    Implements abstract method in Cursor.
       *    Get the current cursor position.
       *    @return  The cursor position.
       */
      MC2Point getCursorPos() const;
         
      /**
       *    Implements abstract method in Cursor.
       *    Set if the cursor should be visible or hidden.
       *    @param   visible  If true, the cursor is visible, 
       *                      otherwise hidden.
       */
      void setCursorVisible( bool visible );

      /**
       *    Implements abstract method in Cursor.
       *    Get the box of the cursor.
       *    @param   box   PixelBox that will be set to the current
       *                   box of the cursor.
       */
      void getCursorBox( PixelBox& box ) const;

      /**
       *    Setting listener for painting events
       *    @param pWinPaintCallback listener
       */
      void setWinPaintCallback( WinPaintCallback* pWinPaintCallback = NULL );

      /**
       *	   Connects to the TMap Server and starts reuqesting\displaying maps
       */
      void connect();

      /**
       *	   Sets can repaint state.
       */
      void setCanRepaint(BOOL bCanRepaint); 

      /**
       *    Returns the mapdrawinginteface
       *    Valid during the lifetime of the MapLib.
       */
      MapDrawingInterface* getMapDrawingInterface() const;
   
      /**
       *    Returns the map moving interface.
       *    Valid during the lifetime of the MapLib.
       */
      MapMovingInterface* getMapMovingInterface() const;

      /** 
       *    Returns the MapLib pointer.
       *    Valid during the lifetime of the MapLib.
       */
      MapLib* getMapLib() const;

#ifdef TEMPORARY_VERSION      
      /**
       *    Returns the TileMapHandler reference.
       *    It's the temporary solution.
       */      
      TileMapHandler& getHandler() const;
#endif

      /**
       *    Checks if globe component needed
       */
      BOOL useGlobe() const;

      /**
       *    Sets globe usage flag
       */
      void setUseGlobe(BOOL bUse);
      
      /**
       * True if in 3d mode, false if not.
       */
      bool get3dMode( );

      /**
       * Switch 2d/3d mode
       */  
      void set3dMode(bool on);

      void setNightMode(bool on);

      void setHorizonHeight( uint32 height );

      BOOL isGlobeActive();
      
};


//***********************************************************************

#endif
