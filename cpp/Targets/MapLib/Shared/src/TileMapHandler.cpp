/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#define CORE_LOGGING_MODULE_NAME "TileMapHandler"
//#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE

#include "CoreDebugPrint.h"
#include "CoreFunctionPrologue.h"
#include "TileMapConfig.h"

#include <stdio.h>
#include <vector>
#include <set>
#include <algorithm>
#include "MC2SimpleString.h"
#include "PlatformImageLoader.h"
#include <stdlib.h>
#include <math.h>

#include "MapManipulator.h"
#include "StressTester.h"
#include "MapProjectionAdapter.h"
#include "SmoothMapManipulator.h"
#include "DraggingMapManipulator.h"
#include "Camera.h"
#include "ClipUtil.h"
#include "TileMapHandler.h"
#include "TileMapParams.h"
#include "TileMapFormatDesc.h"
#include "TileMapUtil.h"
#include "TileArgNames.h"
#include "TileMap.h"
#include "DBufRequester.h"
#include "MemoryDBufRequester.h"
#include "MC2BoundingBox.h"
#include "MapPlotter.h"
#include "OverlayView.h"
#include "TileMapContainer.h"
#include "TileMapLayerInfo.h"
#include "GfxConstants.h"
#include "MC2Point.h"
#include "GunzipUtil.h"
#include "RouteID.h"
#include "TileMapTextHandler.h"
// #include "TextPlacement/TextPlacementDelegator.h"
#include "TileMapTextHandler.h"
#include "RandomFuncs.h"
#include "PlotterAdapter3d.h"
#include "PositionInterpolator.h"
#include "InterpolationCallback.h"
#include "SelectedMapObjectNotifier.h"
#include "MapLibSettingsNotifier.h"
#include "MapOperationInterface.h"
#include "MapLibOperationHandler.h"
#include "MapLibStartupListener.h"
// For the user defined features.
#include "UserDefinedFeature.h"
#include "UserDefinedBitMapFeature.h"
#include "UserDefinedScaleFeature.h"
#include "DirectedPolygon.h"
#include "MapDrawingCallback.h"

// For events
#include "TileMapEvent.h"
#include "TileMapEventListener.h"

#include "InsideUtil.h"
#include "TileMapTextSettings.h"

#include "TileMapParamTypes.h"

#if defined __SYMBIAN32__ && !defined SYMBIAN_9
#include <hal.h>
#endif

#include "TMKTimingInfo.h"
#include "TileMapInfoCallback.h"
#include "PlotterAdapter3d.h"
#include "ImageBlender.h"
#include "PolygonTriangulation.h"
#include "ExternalRenderer.h"

#ifndef USE_TRACE
#define USE_TRACE
#endif

#undef USE_TRACE

#ifdef USE_TRACE
#include "TraceMacros.h"
#endif

#include "PNGImageLoader.h"
#include "TextureBlock.h"
#include "TextureBlockManager.h"
#include "DrawingContextsConcrete.h"



//#define ENABLE_INTERPOLATION_DEBUGGING
// #define ALGOTRIM_TILEMAPDUMP
#define SERVER_SUPPORTS_SCALING_BITMAPS

#include "Util/StringCache.h"

//#define ENABLE_MAPPERFORMANCE_DEBUGGING

// Whether or not the info about the current text placement algorithm should
// be drawn
//#define DRAW_TPL_INFO

using namespace std;
using namespace isab;

// #define DRAW_BITMAP_SAMPLE

class TileMapHandlerReleaseChecker : public DBufReleaseChecker {
public:
   TileMapHandlerReleaseChecker( TileMapHandler* handler ) {
      m_handler = handler;
   }

   bool releaseToNextAllowed( const MC2SimpleString& descr,
                              const DBufRequester& req ) {
      bool res = true;
      if ( req.getType() == DBufRequester::MEMORY ) {
         if ( TileMapParamTypes::getParamType( descr ) ==
              TileMapParamTypes::TILE ) {
            if ( m_handler->m_mapFormatDesc != NULL &&
                 !m_handler->m_mapFormatDesc->allowedOnDisk( descr ) ) {
               res = false;
            }
         }
      }
      mc2dbg8 << "[TMHRC]: Returning "
             << res
             << " for desc "
             << descr << endl;
      return res;
   }
   
private:
   /** 
    *    The TileMapHandler to call when something happens.
    */
   TileMapHandler* m_handler;   
};

/**
 *   Class that it used as DBufRequestListener for easy change.
 */
class TileMapHandlerDefaultDBListener : public DBufRequestListener {
public:

   /**
    *    Constructor.
    */
   TileMapHandlerDefaultDBListener(TileMapHandler* handler) :
         m_handler(handler) {}
   
   /**
    *    Called when a BitBuffer is received from a
    *    requester.
    */
   void requestReceived(const MC2SimpleString& descr,
                        BitBuffer* dataBuffer,
                        const DBufRequester& /*origin*/ ) {
      m_handler->dataBufferReceived(descr, dataBuffer);
   }
private:
   /** 
    *    The TileMapHandler to call when something happens.
    */
   TileMapHandler* m_handler;
};

/**
 *   Class that handles stuff like checking that maps aren't
 *   requested twice in a row, timeouts etc.
 */
class TileMapHandlerDBBufRequester : public DBufRequester, DBufRequestListener{
public:
   /**
    *   Constructor.
    *   @param realRequester The requester to request the maps from.
    *   @param toolkit Toolkit to get the timer from.
    */
   TileMapHandlerDBBufRequester(TileMapHandler* handler,
                                DBufRequester* realRequester,
                                TileMapToolkit* toolkit,
                                DBufRequestListener* defListener);

   /**
    *   Destructor.
    */
   virtual ~TileMapHandlerDBBufRequester() {};

   
   /**
    *   Called by the Requester when a request is done.
    *   @param descr          The descr used to get the object.
    *   @param receivedObject The received object.
    */
   inline void requestReceived(const MC2SimpleString& descr,
                               BitBuffer* dataBuffer,
                               const DBufRequester& origin );

   /**
    *   The Requester must keep the 
    *   release is called.
    *   @param descr  The description.
    *   @param caller The RequestListener that wants an update
    *                 if a delayed answer comes back.
    *   @param onlyCached True if only cached maps should be requested.
    */ 
   inline void request(const MC2SimpleString& descr,
                       DBufRequestListener* caller,
                       request_t whereFrom );

   /**
    *   Makes it ok for the Requester to delete the BitBuffer.
    *   Default implementation deletes the BitBuffer and removes
    *   it from the set.
    */ 
   inline void release(const MC2SimpleString& descr,
                       BitBuffer* obj);

   /**
    *   Calls cancelAll in the parent.
    */
   inline void cancelAll();
   
private:

   /**
    *   Data for one request.
    */
   class ReqData {
   public:
      /// Should only be used internally in map.      
      ReqData() {}
      ReqData(uint32 timep, DBufRequestListener* reqListener,
              DBufRequester::request_t whereFrom )
            : m_time(timep), m_reqListener(reqListener),
              m_whereFrom( whereFrom ) 
         {
            m_block  = true;
            m_failedTime = 0;
         }
      /// Time of request
      uint32 m_time;
      /// Listener of request.
      DBufRequestListener* m_reqListener;
      /// Where the request may be requested from.
      DBufRequester::request_t m_whereFrom;
      /// True if there should be no more requests for the buffer
      bool m_block;
      /// Time for last failuer.
      uint32 m_failedTime;
   };
   
   /// The toolkit
   TileMapToolkit* m_toolkit;
   /// Type of map to store data in
   typedef map<MC2SimpleString, ReqData> storage_t;
   /// Map containing descriptions and last request time in ms.
   storage_t m_requested;

   /**
    *   Minimum time to wait between re-requests in ms.
    */
   uint32 m_reRequestTimeSec;

   /**
    *   The absolute minimum time to wait before re-requesting.
    */
   uint32 m_minReRequestTimeSec;
   
   /**   
    *   The absolute minimum time to wait before re-requesting.
    */
   uint32 m_maxReRequestTimeSec;

   /**
    *   Time of first failed request.
    */
   uint32 m_firstFailedTimeSec;

   /**
    *   Default listener to send something to when strange stuff happens.
    */
   DBufRequestListener* m_defaultListener;

   /// TileMapHandler is needed to get the mapformatdesc
   TileMapHandler* m_handler;
};

TileMapHandlerDBBufRequester::
TileMapHandlerDBBufRequester(TileMapHandler* handler,
                             DBufRequester* realRequester,
                             TileMapToolkit* toolkit,
                             DBufRequestListener* defaultListener)
      : DBufRequester(realRequester)
{
   m_defaultListener = defaultListener;
   m_toolkit   = toolkit;
   m_reRequestTimeSec    = 5;
   m_minReRequestTimeSec = 5;
   m_maxReRequestTimeSec = 60;
   m_firstFailedTimeSec  = 0; // No failures yet
   m_handler             = handler;
}


inline void
TileMapHandlerDBBufRequester::request(const MC2SimpleString& descr,
                                      DBufRequestListener* caller,
                                      request_t whereFrom )
{
   uint32 curTimeSec = TileMapUtil::currentTimeMillis() / 1000;
   mc2dbg8 << "[THM]: m_firstFailedTimeSec = "
          << m_firstFailedTimeSec << endl;
   mc2dbg8 << "[TMH]: m_requested.size() = " << m_requested.size()
          << endl;
   if ( whereFrom != onlyCache && m_requested.size() >= 30 ) {
      if ( m_firstFailedTimeSec == 0 ) {
         return;
      } else if ( (curTimeSec - m_firstFailedTimeSec) > m_reRequestTimeSec ) {
         // Don't return
      } else {
         return;
      }
   }
   storage_t::iterator it = m_requested.find(descr);
   // Only request maps if not waiting for it.
   // Later we will add timeout that removes old stuff.
   bool update = false;
   
   if ( it == m_requested.end() ) {      
      update = true;
   } else if ( it->second.m_block == false ) {
      update = true;
   } else if ( it->second.m_whereFrom != whereFrom ) {
      mc2dbg8 << "[TMHBBR]: whereFrom changed." << endl;
      update = true;
   } else if ( (curTimeSec - it->second.m_time) > m_reRequestTimeSec ) {
      mc2dbg8 << "[TMHBBR]: Ok to request map again - timediff "
             << (curTimeSec - it->second.m_time) << " s" << endl;
      // Waited too long for the map.      
      update = true;
      // Wait one more second for next one.
      m_reRequestTimeSec = MIN(m_reRequestTimeSec, m_maxReRequestTimeSec);
      m_firstFailedTimeSec = 0;
      for ( storage_t::iterator it = m_requested.begin();
            it != m_requested.end();
            ++it ) {
         m_firstFailedTimeSec = MAX(m_firstFailedTimeSec,
                                    it->second.m_failedTime);
      }
   }
   if ( update ) {
      mc2dbg8 << info << "[TMH]: Requested " << descr << endl;
      m_requested[descr] = ReqData(
         curTimeSec,
         caller, 
         whereFrom );
      m_parentRequester->request(descr, this, whereFrom );
   } else {
      // m_timerRequester->request(descr, this);
   }
}

inline void
TileMapHandlerDBBufRequester::requestReceived(const MC2SimpleString& descr,
                                              BitBuffer* dataBuffer,
                                              const DBufRequester& origin )
{
   if ( origin.getType() & DBufRequester::EXTERNAL ) {
      // Now we must fixup the traffic maps since they are from the
      // in ter net.
      if ( m_handler->m_mapFormatDesc ) {
         dataBuffer =
            TileMap::writeReceiveTime( *m_handler->m_mapFormatDesc,
                                       descr,
                                       dataBuffer );
      }
   }      
   
   storage_t::iterator it = m_requested.find(descr);
   if ( it != m_requested.end() ) {
      mc2dbg8 << "[TMH]: Received " << descr << endl;
      DBufRequestListener* listener = it->second.m_reqListener;
      if ( dataBuffer == NULL ) {
         // Failed - release the buffer.
         // But first wait some time.
         it->second.m_failedTime = TileMapUtil::currentTimeMillis() / 1000;
         if ( m_firstFailedTimeSec == 0 ) {
            m_firstFailedTimeSec = it->second.m_failedTime;               
         }
      } else {
         // Decrease the waiting time
         m_reRequestTimeSec = MAX(m_reRequestTimeSec -1 ,
                                  m_minReRequestTimeSec);         
         m_requested.erase(it);
      }
      listener->requestReceived(descr, dataBuffer, origin);
   } else {
      // Wake up the TileMapHandler.
      // Special for the description format
      if ( descr[0] == 'D' || descr[0] == 'd' ) {
         m_defaultListener->requestReceived( descr, dataBuffer, origin );
      } else {
         // Do not bother the TileMapHandler with the buffer.
         m_defaultListener->requestReceived( descr, NULL, origin );
         release(descr, dataBuffer);
      }
      mc2dbg8 << warn << "[TMH]: req recv: Could not find req data"
             << " for " << descr << endl;
   }
}

inline void
TileMapHandlerDBBufRequester::release(const MC2SimpleString& descr,
                                      BitBuffer* dataBuffer)
{
   if ( dataBuffer == NULL ) {
      return;
   }
   m_requested.erase(descr);
   
   m_parentRequester->release(descr, dataBuffer);

}

inline void
TileMapHandlerDBBufRequester::cancelAll()
{
   // Allow all requests to be resent.
   m_requested.clear();
   m_parentRequester->cancelAll();
}

// --------------------************ TileMapHandler *****************------

class LayerSorter {
public:
   inline bool operator()(const int a, const int b) const {
      if ( a == TileMapTypes::c_routeLayer ) {
         return true;
      } else if ( b == TileMapTypes::c_routeLayer ) {
         return false;
      } else {
         return a < b;
      }
   }
};

class ScopedCounter {
public:
   ScopedCounter( int& var ) : m_var( var ) {
      ++m_var;
   }

   ~ScopedCounter() {
      --m_var;
   }

private:
   int& m_var;
};

typedef TileMapContainer::MIt MIt;
typedef TileMapContainer::PIt PIt;


TileMapHandler::TileMapHandler( DBufRequester* requester,
                                TileMapToolkit* platform,
                                TileMapEventListener* eventListener )
   : m_plotter2d( NULL ),
     m_plotter3d( NULL ),
     m_extRenderer( NULL ),
     m_drawingContext( DrawingContextProxy::NONE ),
     m_toolkit(platform),
     m_mapFormatDesc(NULL),
     m_mapDrawingCallback(NULL),
     // We are not painting.
     m_painting(0),
     m_garbage( platform ),
     m_tileMapCont( new TileMapContainer( &m_projection, &m_garbage ) ),
     m_highlightPoint(-1, -1),
     m_screenCoords( m_realScreenCoords, 0, 0 ),
     m_copyrightPos( 0, 0 ),
     m_showCopyright( true ),
     m_nightMode( false ),
     m_3dOn( false ),
     m_outlinesIn3dEnabled( false ),
     m_acpModeEnabled( true ),
     m_startupListener(NULL),
     m_tileMapLoader(new TileMapLoader),
     m_stringCache(new StringCache(NULL)),
     m_firstDrawDone(false),
     m_hidePOILayers(false),
     m_mapDrawingEnabled(true),
     m_imgTiles(NULL)
{
   m_operationTranslator = new MapProjectionAdapter(this, m_projection);
   m_interpolatingPositions = false;
   
   m_posInterpolator = new PositionInterpolator;
   m_posInterpolator->setAllowedCatchUpFactor( 0.5 );
   m_positionInterpolationRequested = 0;
   m_interpolationCallback = NULL;
   m_inRequestingMaps = 0;
   setDetectMovementBySelf( false );
   m_clickInfo = new TileMapHandlerClickResult();
   m_releaseChecker = new TileMapHandlerReleaseChecker( this );
   m_waitingForInfoString.second = NULL;
   m_outlinesDrawnLastTime = false;
   m_mapArrivedSinceRepaint = true;
   m_descCRCReceived = false;
   m_serverDescCRC   = MAX_UINT32;
   m_tmkTimingInfo = NULL;
   m_rerequestTimer = 0;

   m_dxxBuffer = NULL;
   m_realScreenCoords.resize( 128 );
   m_horizonHeight = 0;
   m_plotter = NULL;
   m_showHighlight = false;
   m_floatAccuracyCheck = false;
   m_maxNumPoints = 0;
   // m_forceRedraws = true;
   m_forceRedraws = false;

   // m_projection.setDPI
   m_selectedMapObjectNotifier = new SelectedMapObjectNotifier;
   m_settingsNotifier = new MapLibSettingsNotifier;


   m_mapManipulator = new MapManipulator(this,
                                         platform);

   m_overlayView = new OverlayView(m_toolkit,
                                   *m_operationTranslator,
                                   getMapManipulator(),
                                   this,
                                   m_selectedMapObjectNotifier);

   m_operationHandler = &m_overlayView->getOperationHandler();

   m_stressTester = new StressTester(platform, m_operationHandler);

   m_mapManipulator->setMapOperationInterface(m_operationHandler);
   
   m_projection.addListener(this);
   m_projection.addListener(m_overlayView);
   m_projection.addListener(&m_overlayView->getOperationHandler());


   if ( eventListener ) {
      m_eventListeners.push_back( eventListener );
   }
   // Create polygon to show when an item can be selected
   m_outlinePolygon = new DirectedPolygon( vector<MC2Point>(),
                                           MC2Point(-10,-10),
                                           false,
                                           0,
                                           3 );
   m_outlinePolygon->setClickable( false );
   m_outlinePolygon->setAngle( 0, false );
   m_repaintAfterMapReceptionTimer = 0;
   srand(TileMapUtil::currentTimeMillis());
   // Create random characters for the description.
   {
      char* randString = RandomFuncs::newRandString(3);
      m_randChars = randString;
      delete [] randString;
   }

   m_userDefinedFeatures = NULL;

   // m_textHandler = new TextPlacementDelegator(*this,
   //                                            NULL,
   //                                            NULL,
   //                                            m_projection,
   //                                            (*m_tileMapCont).endMapsToDraw());
                                              
   m_textHandler = new TileMapTextHandler(NULL,
                                          NULL,
                                          m_projection, // MapProjection
                                          (*m_tileMapCont).endMapsToDraw(),
                                          *this ); // TileMapHandler
   
   m_textHandlerIdleID = 0;

   
   // Default layers are always added (now they will come in the DXXX,
   // but it is possible that there is an old DXXX.
   m_layersToDisplay.insert( TileMapTypes::c_mapLayer );
   m_layersToDisplay.insert( TileMapTypes::c_routeLayer );
   m_layersToDisplay.insert( 2 );
//     m_layersToDisplay.insert( 3 );

   m_routeID = NULL;

   // TEST!
#if 0
  m_routeID = new RouteID("12_40617C3F");
  //m_routeID = new RouteID("88_4064371D");
#endif

   m_repaintTimerRequested = 0;
   m_tileMapLoaderTimerRequested = 0;
   
   m_lastRepaintTime = TileMapUtil::currentTimeMillis();
   // Create requester with 256k of memory.
   
   m_dataBufferListener = new TileMapHandlerDefaultDBListener(this);
   m_requester = new TileMapHandlerDBBufRequester(this,
                                                  requester, platform,
                                                  m_dataBufferListener);

   setLanguage( LangTypes::english );

   // Set the release checker.
   m_requester->setReleaseChecker( m_releaseChecker );
   
   m_detailLevel = MAX_INT32;
   m_matrixChangedSinceDraw = true;
   m_detailRepaintTimerRequested = 0;
   m_detailRepaintTimerHasBeenRun = true;

   m_prevLoadingScale = 0.0f;
   m_prevLoadingScaleIndex = MAX_INT32;
   
   // Use gunzip if available.
   m_useGzip = GunzipUtil::implemented();

   // Get screensize
   getMapSizeFromRenderer( m_lastScreenSize );

   m_projection.setScreenSize( MC2Point(m_lastScreenSize.getHeight(),
                                        m_lastScreenSize.getWidth() ) );

   // Set start bbox and scale
   // This is somewhere around spolegatan
//     MC2Coordinate lower( 664609150, 157263143 );
//     MC2Coordinate upper( 664689150, 157405144 );
   // Strange street here.
//   MC2Coordinate lower( 664380226,156217749 );
//   MC2Coordinate upper( 664357053,156258858 );

//       // LA
//       MC2Coordinate lower( 406810000,-1411746794 );
//       MC2Coordinate upper( 406621361,-1411493910 );
   
//       //   Malm�

      
   MC2Coordinate lower( 663428099, 154983536 );
   MC2Coordinate upper( 663394172, 155043599 );
//    Center
//       MC2Coordinate lower( 500, 500 );
//       MC2Coordinate upper( -500, -500 );

   
//   // Europe
//   MC2Coordinate lower( 756634992, -175799752 );
//   MC2Coordinate upper( 405535631, 278927496 );

//   // Helsinki
//   MC2Coordinate lower( 717870134, 297503083 );
//   MC2Coordinate upper( 717839305, 297556052 );

//       MC2BoundingBox bbox(lower, upper);
   MC2BoundingBox bbox( 663494286,  154818525, 663259250,  155130565 );
   
   m_projection.setBoundingBox(bbox);

#ifdef DRAW_BITMAP_SAMPLE
   m_projection.setCenter( WFAPI::WGS84Coordinate( 48.8754, 2.31653) );
#endif
   
   m_operationTranslator->setAngle(0);
   
#if 0
   // Test putting the pin in the map
   m_userDefinedFeatures = new vector<UserDefinedFeature*>;
   UserDefinedBitMapFeature* mappin =
      new UserDefinedBitMapFeature( lower,
                                    "mappin");
   m_userDefinedFeatures->push_back( mappin );
#endif

   // if(!TileMapConfig::LOAD_IMMEDIATE) {
   m_tileMapLoaderTimerRequested = 0;
   
   // }
#if defined ENABLE_MAPPERFORMANCE_DEBUGGING && not defined NAV2_CLIENT_I386
   m_showCopyright = false;
   //gtk client doesn't like this:
   setForcedRedraws(true);
#endif
}

TileMapHandler::~TileMapHandler()
{
   delete m_stringCache;
   delete m_mapManipulator;
   // Before deleting the tileMapLoader we have to delete the loaded
   // tileMaps and release the related buffers through the sequence of
   // requesters.
   typedef std::vector<TileMapLoader::LoadedData> LoadedDataVec;
   LoadedDataVec loadedTileMaps;
   m_tileMapLoader->acquire(loadedTileMaps);
   for (LoadedDataVec::iterator itr = loadedTileMaps.begin();
        itr != loadedTileMaps.end(); itr++){
      delete itr->tileMap;
      m_requester->release(itr->descr, itr->buffer);
   }
   typedef TileMapLoader::BufferMap UnloadedDataVec;

   // We also have to release the buffers that are queued to be
   // loaded by the tileMapLoader.
   UnloadedDataVec unloadedTileMaps;
   m_tileMapLoader->acquireUnloaded(unloadedTileMaps);
   for (UnloadedDataVec::iterator itr = unloadedTileMaps.begin();
        itr != unloadedTileMaps.end(); itr++){
      m_requester->release(itr->first, itr->second);
   }

   delete m_tileMapLoader;
   delete m_operationTranslator;
   delete m_posInterpolator;
   delete m_overlayView;

   delete m_stressTester;
   delete m_dxxBuffer;
   delete m_outlinePolygon;
   delete m_requester;
   delete m_dataBufferListener;
   
   delete m_selectedMapObjectNotifier;
   delete m_settingsNotifier;
   
   delete m_textHandler;
   
#ifndef QUICK_SHUTDOWN    
   {
      if(m_drawingContext == DrawingContextProxy::PLOTTER){
         for ( bitMapMap_t::iterator it = m_bitMaps.begin();
               it != m_bitMaps.end();
               ++it ) {
            m_plotter->deleteBitMap( (*it).second );
         }
      } else if (m_drawingContext == DrawingContextProxy::EXTERNAL){
         for ( bitMapMap_t::iterator it = m_bitMaps.begin();
               it != m_bitMaps.end();
               ++it ) {
            delete (*it).second;
         }
      }
      
   }

   delete m_mapFormatDesc;
#endif

   delete m_releaseChecker;
   delete m_tileMapCont;
   delete m_clickInfo;
   delete m_plotter3d;
}


void TileMapHandler::projectionUpdated(projectionEvent trigger) 
{
   if(m_extRenderer) {
      m_extRenderer->cameraUpdated();
   }
   // Checks and updates the parameters.
   updateParams();

   // We cannot continue drawing where we left off.
   m_matrixChangedSinceDraw = true;

   // Cancel detail repaint timer
   if ( m_detailRepaintTimerRequested ) {
      m_toolkit->cancelTimer(this, m_detailRepaintTimerRequested);
      m_detailRepaintTimerRequested = 0;
   }
   if ( m_drawingContext != DrawingContextProxy::NONE ) {
      m_textHandler->mapVectorChanged( (*m_tileMapCont).beginMapsToDraw(),
                                       (*m_tileMapCont).endMapsToDraw(),
                                       !m_movementHelper.isMoving() );
      
      // m_textHandler->mapProjectionChanged( (*m_tileMapCont).beginMapsToDraw(),
      //                                      (*m_tileMapCont).endMapsToDraw(),
      //                                      !m_movementHelper.isMoving() );
      if ( m_textHandlerIdleID ) {
         m_toolkit->cancelIdle(this, m_textHandlerIdleID );
         m_textHandlerIdleID = 0;
      }
   }
   m_textHandlerIdleID = 0;

   // requestMaps(10000);
} 

void
TileMapHandler::toggleLayerToDisplay(int layerNo)
{
  // Why a toggle method?
  if(isLayerVisible(layerNo)) {
     /* layer is present, erase it */
     m_layersToDisplay.erase( layerNo );
  } else { /* add the layer to the display list */
     m_layersToDisplay.insert( layerNo );
  }
  /* request a repaint of the screen */
  requestRepaint();
  return;
}

void
TileMapHandler::setLayerVisible(WFAPI::wf_uint32 layerNo, bool visible)
{
   const TileMapLayerInfoVector* vect = getLayerIDsAndDescriptions();
   if ( vect != NULL ) {
      TileMapLayerInfoVector tmliv(*vect);
      for (TileMapLayerInfoVector::iterator it = tmliv.begin(); 
           it != tmliv.end(); ++it) {
         if ((*it).getID() == layerNo) {
            // std::cerr << "TMH::setLayerVisible Updating" << std::endl;
            (*it).setVisible(visible);
            break;
         }
      }
      updateLayerInfo(tmliv);
   } // Else we can't change
}

void
TileMapHandler::setUpdatePeriodMinutes(WFAPI::wf_uint32 layerNo, 
                                       WFAPI::wf_uint32 updateIntervalMinutes)
{
   const TileMapLayerInfoVector* vect = getLayerIDsAndDescriptions();
   if ( vect != NULL ) {
      TileMapLayerInfoVector tmliv(*vect);
      for (TileMapLayerInfoVector::iterator it = tmliv.begin(); 
           it != tmliv.end(); ++it) {
         if ((*it).getID() == layerNo) {
            // std::cerr << "TMH::setTrafficInfoUpdateInterval Updating" 
            //           << std::endl;
            (*it).setUpdatePeriodMinutes(updateIntervalMinutes);
            break;
         }
      }
      updateLayerInfo(tmliv);
   } // Else we can't change
}
   
VectorProxy<MC2Point>& 
TileMapHandler::getScreenCoords( TileMap& tileMap, TilePrimitiveFeature& feature )
{
   m_screenCoords.clear();
   if ( feature.isDrawn() ) {
      // The feature is drawn, i.e. calculate it's coordinates.
      switch ( feature.getType() ) {
         case TilePrimitiveFeature::line:
         case TilePrimitiveFeature::polygon:
         {
            // Polylines and polygons uses prepareCoordinates.
            int period = 0; // We don't do any bounding box checks here.
            MC2Point tmp;
            prepareCoordinates( tileMap, feature,
                                m_projection.getBoundingBox(),
                                tmp,
                                period);
         }
            break;
         case TilePrimitiveFeature::bitmap:
         case TilePrimitiveFeature::circle: {
            // Bitmaps uses getXYFromCoordArg.
            m_realScreenCoords.resize( 1 );
            m_screenCoords.clear();
            MC2Point center(0,0);
            getXYFromCoordArg( feature, m_projection.getBoundingBox(), 
                               center );
            m_screenCoords.push_back( center );
            break;
         } 
      }
   }
   return m_screenCoords;
}


void
TileMapHandler::clearRouteID()
{
   delete m_routeID;
   m_routeID = NULL;
   updateParams();
}

void
TileMapHandler::setRouteID(const RouteID& routeID)
{
   // First clear the route id so that the params are really updated
   clearRouteID();
   // Then set the new one.
   m_routeID = new RouteID(routeID);
   updateParams();
}

const RouteID*
TileMapHandler::getRouteID() const
{
   return m_routeID;
}

void 
TileMapHandler::setLanguage( LangTypes::language_t lang )
{
   m_requester->setPreferredLang( lang );
   m_lang = lang;
   // Ok to request new CRC if language differs.
   m_descCRCReceived = false;
}   

void 
TileMapHandler::setNav2Language( uint32 nav2Lang )
{
   setLanguage( LangTypes::getNavLangAsLanguage( nav2Lang ) );
}
  
const char* 
TileMapHandler::setLanguageAsISO639( const char* isoStr )
{
   // Convert to LangTypes:language_t.
   LangTypes::language_t langType = 
      LangTypes::getISO639AsLanguage( isoStr );
   if ( langType == LangTypes::invalidLanguage ) {
      // Default to english if the language was not supported.
      langType = LangTypes::english;
   } 
  
   // Set the language.
   setLanguage( langType );
   // And return the used language str..
   bool two = strlen( isoStr ) == 2;
   return LangTypes::getLanguageAsISO639( langType, two );
}

#define SQUARE(a) ((a)*(a))

int64
TileMapHandler::sqDistanceCoordToLine(const TileMapCoord& coord,
                                      const TileMapCoord& lineFirst,
                                      const TileMapCoord& lineSecond,
                                      float cosF )
{
   int32 xP = coord.getLon();
   int32 yP = coord.getLat();
   int32 x1 = lineFirst.getLon();
   int32 y1 = lineFirst.getLat();
   int32 x2 = lineSecond.getLon();
   int32 y2 = lineSecond.getLat();
   
   //ex and ey are unitvectors
   //
   //Vector V1 = (x2 - x1) * ex + (y2 - y1) * ey
   //Vector V2 = (xP -Removed all  x1) * ex + (yP - y1) * ey

   //Calculate the scalarproduct V1 * V2
   int64 scalarProd = int64( cosF * (x2 - x1) * cosF * (xP - x1) +
                             float( y2 - y1 ) * float( yP - y1 ));
   
   int64 dist;
   if( (x1 - x2 == 0) && (y1 == y2) )
   {
      //V1 has a length of zero, use plain pythagoras
      dist = SQUARE( int64( cosF*(xP - x1) ) ) + 
             SQUARE( int64( yP - y1 ) );
   }
   else if ( scalarProd < int64(0) )
   {
      //The angle between V1 and V2 is more than pi/2,
      //this means that (xP, yP) is closest to (x1, y1) 
      dist = SQUARE( int64( cosF*(xP - x1) ) ) +
             SQUARE( int64( yP - y1 ) );
   }
   else if ( scalarProd > int64( SQUARE( int64( cosF*(x2 - x1) ) ) +
                                 SQUARE( int64( y2 - y1 ) ) ) )
   {
      //The scalarproduct V1 * V2 is larger than the length of V1
      //this means that (xP, yP) is closest to (x2, y2) 
      dist = SQUARE( int64( cosF*(xP - x2) ) ) +
             SQUARE( int64( yP - y2 ) );
   }
   else 
   {
      //Use the formula for minimumdistance from a point to a line
      //dist = ( V2x * V1y - V2y * V1x )^2 / ( (V1x ^ 2) + (V1y ^ 2) )
      dist = int64( SQUARE( float(y2 - y1) * cosF*(xP - x1) + cosF*(x1 - x2) * float(yP - y1) ) /
             ( SQUARE( float(y2 - y1) ) + SQUARE( cosF*(x1 - x2) ) ) );
   }

   return dist;
}


int64
TileMapHandler::getMinSQDist(TileMap& tileMap,
                             const TileMapCoord& coord,
                             const TilePrimitiveFeature& prim,
                             float cosLat)
{
   if ( prim.getType() == TilePrimitiveFeature::polygon ) {
      // Don't use the polygons, since the border isn't interesting there
      return MAX_INT64;
   }
   const CoordsArg* coords =
      static_cast<const CoordsArg*>(prim.getArg(TileArgNames::coords));
   if ( coords ) {
#ifndef SLIM_TILE_COORDS
      CoordsArg::const_iterator it = coords->begin();
#else
      CoordsArg::const_iterator it = coords->begin( tileMap );
#endif
      CoordsArg::const_iterator lastit = it++;
      int64 minDist = MAX_INT64;
#ifndef SLIM_TILE_COORDS
      while ( it != coords->end() ) {
#else
      while ( it != coords->end(tileMap) ) {
#endif
         int64 curDist = sqDistanceCoordToLine(coord, *it, *lastit, cosLat);
         if ( curDist < minDist ) {
            minDist = curDist;
         }
         ++it;
         ++lastit;
      }
      return minDist;
   } else {
      const CoordArg* coordArg =
         static_cast<const CoordArg*>(prim.getArg(TileArgNames::coord));
      // One coordinate
      if ( coordArg ) {
         return sqDistanceCoordToLine(coord,
                                      coordArg->getCoord(),
                                      coordArg->getCoord(),
                                      cosLat);
      }
   }
   // No coordinates at all.
   return MAX_INT64;
}


pair<const TileMap*, const TilePrimitiveFeature*>
TileMapHandler::getFirstInside(const TileMapCoord& coord) const
{
   MC2Coordinate useCoord(coord.getLat(), coord.getLon());
   const TilePrimitiveFeature* closestSoFar = NULL;
   const TileMap* closestSoFarMap = NULL;

   // Loop again. Try to find the first polygon that has been clicked
   // inside.
   MapsToDrawIt endIt = (*m_tileMapCont).endMapsToDraw();
   for( int level = m_mapFormatDesc->getMaxLevel();
        level >= m_mapFormatDesc->getMinLevel();
        --level ) {
      for( MapsToDrawIt mIt = (*m_tileMapCont).beginMapsToDraw();
          mIt != endIt; ++mIt ) {
         TileMap* curMap = *mIt;
         if ( curMap == NULL ) {
            continue;
         }

         if (!isTileMapVisible(curMap)){
            // Should not be able to click in a hidden tileMap.
            continue;
         }
         const TileMap::primVect_t& prims =
            curMap->getPrimitives(*m_mapFormatDesc,
                                  level);
         for( TileMap::primVect_t::const_iterator it = prims.begin();
              it != prims.end(); ++it ) {
            const TilePrimitiveFeature* curPrim = *it;
            if (curPrim->getType() == TilePrimitiveFeature::polygon ) {
               const CoordsArg* coords =
                  static_cast<const CoordsArg*>
                  (curPrim->getArg(TileArgNames::coords));
               if ( coords->getBBox().contains( useCoord ) ) {
                  // Real inside check here.
#ifndef SLIM_TILE_COORDS
                  if ( InsideUtil::inside( coords->begin(),
                                           coords->end(),
                                           coord ) ) {
#else
                  if ( InsideUtil::inside( coords->begin(*curMap),
                                           coords->end(*curMap),
                                           coord ) ) {
#endif
                     return make_pair(curMap, curPrim);
                  }
               }
            }
         }
      }
   }
   return make_pair(closestSoFarMap, closestSoFar);
}

inline const TilePrimitiveFeature*
TileMapHandler::getClosest(TileMap& tileMap,
                           const MC2Point& point,
                           const MC2Coordinate& coord,
                           TileMap::primVect_t::const_iterator begin,
                           TileMap::primVect_t::const_iterator end,
                           int64& mindist,
                           int primType ) const {
   TileMapCoord tileCoord( coord.lat, coord.lon);
   const TilePrimitiveFeature* closestFeature = NULL;
   float cosLat = m_projection.getCosLat();
   mindist = MAX_INT64;
   
   for(  TileMap::primVect_t::const_iterator it = begin; it != end; ++it ) {
      if ( ( primType != MAX_INT32 ) && 
           ( (*it)->getType() != primType ) ) {
         // Skip this feature. Was not of the desired type.
         continue;
      }
      if ( (*it)->getType() == TileFeature::bitmap ) {
         /*
          * Check if it has been drawn.
          * Check if drawing context is not set to NONE, if NONE then bitmaps are no
          * longer valid.
          */
         if ( (*it)->isDrawn()  && m_drawingContext != DrawingContextProxy::NONE) {            
            if ( getPixelBoxForBitMap( **it ).pointInside ( point ) ) {
               mindist = 0;
               return *it;
            }
         }
      } else {
         int64 curDist = getMinSQDist(tileMap, tileCoord, (**it), cosLat);
         if ( curDist < mindist ) {
            mindist = curDist;
            closestFeature = &(**it);
         }
      }
   }
   return closestFeature;
}

pair<const TileMap*, const TilePrimitiveFeature*>
TileMapHandler::getFeatureAt(const MC2Point& point,
                             uint32* /*distance*/, 
                             int32 primType )
{
   const pair<const TileMap*, const TilePrimitiveFeature*>
      nullPair(NULL, NULL);
   
   if ( m_mapFormatDesc == NULL ) {
      return nullPair;
   }                                     
   // Get the coordinate for the point
   MC2Coordinate coord;
   m_operationTranslator->inverseTransform( coord, point );
   
   const TilePrimitiveFeature* closestSoFar = NULL;
   const TileMap* closestSoFarMap = NULL;
   int64 minDist = MAX_INT64;
   
   // Loop
   MapsToDrawIt endIt = (*m_tileMapCont).endMapsToDraw();
   for( int level = m_mapFormatDesc->getMaxLevel();
        level >= m_mapFormatDesc->getMinLevel();
        --level ) {
      for(MapsToDrawIt it = (*m_tileMapCont).beginMapsToDraw();
          it != endIt; ++it ) {
         TileMap* curMap = *it;
         if ( curMap == NULL ) {
            continue;
         }
         if (!isTileMapVisible(curMap)){
            // We don't want to trigger clicks in a hidden TileMap.
            continue;
         }
         const TileMap::primVect_t& prims =
               curMap->getPrimitives(*m_mapFormatDesc,
                                      level);
//           set<TilePrimitiveFeature*> overlappingBBox;
//           for ( TileMap::primVect_t::const_iterator it = prims.begin();
//                 it != prims.end();
//                 ++it ) {
//              const TilePrimitiveFeature* curPrim = *it;
//              if ( insideBBox(bbox, *curPrim) ) {
//                 overlappingBBox.insert(curPrim);
//              }
//              const TilePrimitiveFeature* prim =
//                 getClosest(coord, overlappingBBox, mindist);
//           }

         int64 curDist;
         const TilePrimitiveFeature* curPrim =
            getClosest(*curMap, point, coord,
                       prims.begin(), prims.end(), 
                       curDist, primType );
         if ( curDist < minDist ) {
            minDist         = curDist;
            closestSoFar    = curPrim;
            closestSoFarMap = curMap;
         }                                   
      }
   }

   float minDistPixels = sqrt(float(minDist)) / m_projection.getPixelScale() * 
      GfxConstants::MC2SCALE_TO_METER;

   mc2dbg8 << "[TMH]: Distance in pixels = " << minDistPixels << endl;

   // Check that we are interested in polygons.
   if ( (( primType == MAX_INT32 ) || 
         (primType == TileFeature::polygon )) &&
        minDistPixels > 10 ) {
      pair<const TileMap*, const TilePrimitiveFeature*> closestInside =
         getFirstInside(TileMapCoord(coord.lat, coord.lon));
      closestSoFarMap = closestInside.first;
      closestSoFar = closestInside.second;
   }
   
   if ( closestSoFarMap ) {
      // mc2dbg    << "[TMH]: Closest feature is " << MC2HEX((uint32)closestSoFar)
      //           << " with type = "
      //           << ( (closestSoFar != NULL) ? closestSoFar->getType() : -65536)
      //           << " and dist " << minDist << " featnbr = "
      //           << closestSoFar->getFeatureNbr() << endl;
      // Set closestSoFar to its parent so I can check what type it has.
      //closestSoFar = closestSoFarMap->getParentFeature(closestSoFar);
   }
   
   return make_pair(closestSoFarMap, closestSoFar);
}

const char*
TileMapHandler::getNameForFeature( const TileMap& theMap,
                                   const TilePrimitiveFeature& theFeature, 
                                   TileMapInfoCallback* infoCallback )
{
   // Moved from another function.
   const TileMap* closestSoFarMap           = &theMap;
   const TilePrimitiveFeature* closestSoFar = &theFeature;
  
   // Copy the params and change it into string map params.
   const TileMapParams* stringParams =
      closestSoFarMap->getStringMapParams(m_lang);
   mc2dbg8 << "[TMH]: New params : " << *stringParams << endl;
   const TileMap* stringMap = getMap(*stringParams);      
   if ( stringMap == NULL ) {
      // Not found or not loaded yet.
      // Notify the tilemap info callback when this map is received.
      m_waitingForInfoString.first = stringParams->getAsString();
      m_waitingForInfoString.second = infoCallback;
      
      requestFromCacheOrForReal(*stringParams);
      mc2dbg8 << "[TMH]: No string map - requesting " << endl;         
      return NULL;
   } else {
      // No callback wanted
      m_waitingForInfoString.second = NULL;
      const char* str = stringMap->getStringForFeature(
         closestSoFar->getFeatureNbr());
      if ( str ) {
         return str;
      } else {
         mc2dbg8 << "[TMH]: No string found in map" << endl;
         return NULL;
      }
   }      
}

const char*
TileMapHandler::getNameForFeatureAt(const MC2Point& point,
                                    uint32* distance )
{
   if ( m_mapFormatDesc == NULL ) {
      return NULL;
   }   
   
   TileMapHandlerClickResult clickRes;
   getInfoForFeatureAt( clickRes, point );
   if ( clickRes.getClickedUserFeature() != NULL ) {
      // A user feature was clicked.
      if ( clickRes.getClickedUserFeature()->getName().empty() ) {
         return NULL;
      } else {
         return clickRes.getClickedUserFeature()->getName().c_str();
      }
   }

   pair<const TileMap*, const TilePrimitiveFeature*> mapAndFeature =
      getFeatureAt(point, distance);
   if ( mapAndFeature.first ) {
      return getNameForFeature( *mapAndFeature.first,
                                *mapAndFeature.second );
   } else {
      return NULL;
   }
}

static inline
MC2SimpleString
createServerString( const char* name,
                    const MC2Coordinate& coord,
                    pair<const TileMap*,
                    const TilePrimitiveFeature*>& mapAndFeature )
{
   const bool closed =
      mapAndFeature.second->getType() == TilePrimitiveFeature::polygon;
   
   static const int latLonLength = 11*2 + 2;
   static const int extraLength = 4 + 100; // Colons and more.
   char* tempString = new char[strlen(name) + latLonLength + extraLength];
   sprintf(tempString, "C:%ld:%ld:%ld:%s",
           (long int)coord.lat, (long int)coord.lon,
           (long int)closed, name);
   // Return and delete the string
   return MC2SimpleStringNoCopy( tempString );
}

static inline
void setDirectedPolyFromPixelBox( DirectedPolygon& poly,
                                  const PixelBox& pixBox )
{
   MC2Point center( pixBox.getCenterPoint() );
   poly.setCenter( center );
   vector<MC2Point> tmpVec;
   for( int i = 0; i < 5; ++i ) {
      tmpVec.push_back( pixBox.getCorner(i) - center );
   }
   poly.swapPoints( tmpVec );
   
}

PixelBox 
TileMapHandler::getPixelBoxForBitMap( 
            const TilePrimitiveFeature& bitmap ) const 
{
   MC2_ASSERT( bitmap.getType() == TileFeature::bitmap );

   MC2Point center(0,0);
   getXYFromCoordArg( bitmap, m_projection.getBoundingBox(), 
         center );

   const StringArg* bitMapName =
      static_cast<const StringArg*>
      (bitmap.getArg(TileArgNames::image_name));

   MC2_ASSERT( bitMapName != NULL );

   bitMapMap_t::const_iterator it = 
      m_bitMaps.find( bitMapName->getValue() );

   MC2_ASSERT( it != m_bitMaps.end() );

   // Fetch bitmap size from the plotter.
   isab::Rectangle rect;
   if ( m_drawingContext == DrawingContextProxy::PLOTTER ) {
      m_plotter->getBitMapAsRectangle( rect, center, it->second );
   } else if ( m_drawingContext == DrawingContextProxy::EXTERNAL ) {
      const TileFeatureArg* arg = bitmap.getArg(TileArgNames::coord);
      
      if ( arg == NULL ) {
         return isab::Rectangle();
      } else {
         const CoordArg* coordArg = static_cast<const CoordArg*>(arg);
         
         m_extRenderer->getBitMapAsRectangle(rect,
                                             coordArg->getCoord(),
                                             it->second);
      }
   }
   
   return rect;
}

// This is the one doing the stuff.
void
TileMapHandler::getInfoForFeatureAt( TileMapHandlerClickResult& res,
                                     const MC2Point& point,
                                     bool highlightable,
                                     TileMapInfoCallback* infoCallback )
   
{
   // Reset the waiting for info callback to avoid crash when exiting
   // VectorMapContainer.
   m_waitingForInfoString.second = NULL;
   
   // Reset the result
   res = TileMapHandlerClickResult();
   
   if ( m_mapFormatDesc == NULL ) {
      return;
   }   

   if ( m_userDefinedFeatures != NULL ) {
      // Go from the top down and check if there are any UserDefined features
      // which the point is inside (bitmaps only)
      UserDefinedFeature* foundUserDef = NULL;
      PixelBox foundPixelBox;
      for ( int i = (*m_userDefinedFeatures).size() - 1;
            i >= 0;
            --i ) {
         UserDefinedFeature* curFeat = (*m_userDefinedFeatures)[i];
         // First some checks for what cannot be done
         if ( curFeat == NULL) continue;
         if ( ! curFeat->wasDrawn() ) continue;
         if ( curFeat->getType() != UserDefinedFeature::bitmap ) continue;
         if ( curFeat->isClickable() == false ) continue;


         // Ok
         UserDefinedBitMapFeature* bmp =
            static_cast<UserDefinedBitMapFeature*>(curFeat);
         
         // Check the pixelbox for the feature (by using the bitmap)
         if ( bmp->getBitMapName() != NULL ) {
            isab::BitMap* bitMap =
               getOrRequestBitMap( bmp->getBitMapName() );
            if ( bitMap != NULL ) {
               //    // Ok to get the pixelboxel
               //    PixelBox tmpBox( m_plotter->getBitMapAsRectangle(
               //       bmp->getScreenCoords().front(),
               //       bitMap ) );
               //    if ( tmpBox.pointInside( point ) ) {
               //       foundUserDef  = curFeat;
               //       foundPixelBox = tmpBox;
               //       break;
               //    }
            }
         }
      }
      if ( foundUserDef != NULL ) {
         // Make outline, then we're done.
         setDirectedPolyFromPixelBox( *m_outlinePolygon,
                                      foundPixelBox );
         m_outlinePolygon->setVisible(true);
         res.m_selectionOutline = m_outlinePolygon;
         res.m_clickedFeature = foundUserDef;
         res.m_name = foundUserDef->getName();
         return; // RETURN - all is done
      }
   }


   // Second part. Try to find feature in the map.
   // If only highlightable features should be found, then set
   // the last param to TilePrimitiveFeature::bitmap, otherwise
   // MAX_INT32.
   pair<const TileMap*, const TilePrimitiveFeature*> mapAndFeature =
      getFeatureAt(point, &res.m_distance, 
             highlightable ? TilePrimitiveFeature::bitmap : MAX_INT32 );
   if ( mapAndFeature.first ) {
      const char* name = getNameForFeature( *mapAndFeature.first,
                                            *mapAndFeature.second,
                                            infoCallback );
      // Everything needs the name of the feature.
      if ( name ) {
         // Check if it is possible to create an outline
         if ( mapAndFeature.second->getType() ==
              TilePrimitiveFeature::bitmap ) {
            if ( mapAndFeature.second->isDrawn() ) {
               

               setDirectedPolyFromPixelBox( *m_outlinePolygon,
                     getPixelBoxForBitMap( *mapAndFeature.second ) );
               m_outlinePolygon->setVisible(true);
               // All is ok - set the stuff
               res.m_selectionOutline = m_outlinePolygon;
            } 
         } else {
            // Not bitmap - cannot create selection outline.
            m_outlinePolygon->setVisible(false);
         }
         // Set name and also server string.
         res.m_name = name;
         // Translate the clicked point
         MC2Coordinate coord;
         if ( mapAndFeature.second->getType() == TileFeature::bitmap ) {
            // Point feature. Add the point of the feature instead of
            // the clicked coord.
            const CoordArg* coordArg =
               static_cast<const CoordArg*>
                  (mapAndFeature.second->getArg(TileArgNames::coord));
            coord.lat = coordArg->getCoord().getLat();
            coord.lon = coordArg->getCoord().getLon();
         } else {
            // Non point feature. Add the clicked coord.
            m_operationTranslator->inverseTransform( coord, point );
         }
         // Get server string.
         res.m_serverString = createServerString( name,
                                                  coord,
                                                  mapAndFeature );
         return;
      }
   }
   // Clear the result.
   res = TileMapHandlerClickResult();
   m_outlinePolygon->setVisible(false);
}

const ClickInfo&
TileMapHandler::getInfoForFeatureAt( const MC2Point& point,
                                     bool onlyPois,
                                     TileMapInfoCallback* infoCallback )
{
   // Reset the waiting for info callback to avoid crash when exiting
   // VectorMapContainer.
   m_waitingForInfoString.second = NULL;

   getInfoForFeatureAt( *m_clickInfo,
                        point,
                        onlyPois,
                        infoCallback );
   return *m_clickInfo;
}

int
TileMapHandler::
getServerIDStringForFeaturesAt( vector<MC2SimpleString>& strings,
                                const MC2Point& point )
{

   // New version, the one to keep, probably.
   TileMapHandlerClickResult res;
   getInfoForFeatureAt( res, point );
   if ( ! res.m_serverString.empty() ) {
      strings.push_back( res.m_serverString );
   }
   return strings.size();
}

void
TileMapHandler::setUserDefinedFeatures( vector<UserDefinedFeature*>* features )
{
   m_userDefinedFeatures = features;
}

std::vector<UserDefinedFeature*>*
TileMapHandler::getUserDefinedFeatures()
{
   return m_userDefinedFeatures;
}

const TileMapLayerInfoVector*
TileMapHandler::
getLayerIDsAndDescriptions() const
{
   if ( m_mapFormatDesc ) {
      return &(m_mapFormatDesc->getLayerIDsAndDescriptions());
   } else {
      return NULL;
   }
}
uint32
TileMapHandler::getDetailLevelForLayer(uint32 layerID) const
{
   return m_mapFormatDesc->getDetailLevelFromLayerID( 
                                          layerID,
                                          uint16(m_projection.getPixelScale()) );
}


class TileCoordsXYHelper {
public:
   inline int32 getX(const TileMapCoord& coord) const {
      return coord.getLon();
   }
   
   inline int32 getY(const TileMapCoord& coord) const {
      return coord.getLat();
   }
   
};

void
TileMapHandler::forceRedrawOnNextRepaint()
{  
   // Simulate that matrix has changed in order to force a redraw
   // next time the map is repainted.
   m_matrixChangedSinceDraw = true;
}

void TileMapHandler::requestPositionInterpolation() {   
   if( !m_positionInterpolationRequested &&
       m_posInterpolator->hasUsefulInterpolationData() ) {
      
      m_positionInterpolationRequested =
         m_toolkit->requestTimer( this, 10 );
   }
}

void TileMapHandler::positionInterpolationTimerExpired() {
   m_positionInterpolationRequested = 0;
   
   if( !m_posInterpolator->hasUsefulInterpolationData() ||
       !getInterpolatingPositions() )
   {
      return;
   }

   unsigned int curTime = TileMapUtil::currentTimeMillis();
   
   if( !m_posInterpolator->withinRange( curTime ) ) {
      return;
   }

   InterpolatedPosition ip =
      m_posInterpolator->getInterpolatedPosition( curTime );
      
   if ( m_interpolationCallback ) {
      m_interpolationCallback->positionInterpolated( ip.coord,
                                                     ip.velocityMPS,
                                                     ip.directionDegrees );
   }
   
   m_lastInterpolatedCenter = ip.coord;
}

void
TileMapHandler::requestRepaint()
{
   CORE_FUNCTION_PROLOGUE();
   
#ifdef USE_TRACE
   TRACE_FUNC();
#endif
   // Check if the screen size has changed.
   isab::Rectangle curScreenSize;
   getMapSizeFromRenderer( curScreenSize );

   bool sizeChanged = curScreenSize != m_lastScreenSize;
   
   if ( sizeChanged ) {
      // Force repaint next time since the size can change back
      // again, but then the backbuffer might have been cleared.
      m_lastScreenSize = isab::Rectangle(0,0,0,0);
   }
   if ( !m_repaintTimerRequested ) {
      // beware if there is more than one
      mc2dbg8 << "[TMH]: Requested repaint timer" << endl;
      m_repaintTimerRequested =
         m_toolkit->requestTimer(this,
                                 m_forceRedraws ? 1 : 20);
   }
}

void TileMapHandler::setMapDrawingEnabled(bool mapDrawingEnabled) {
   m_mapDrawingEnabled = mapDrawingEnabled;
}

PixelBox 
TileMapHandler::getMapSizePixels() const
{
   isab::Rectangle rect;
   getMapSizeFromRenderer( rect );

   return rect;
}

inline TileMap*
TileMapHandler::getMap(const MC2SimpleString& params) const
{
   return (*m_tileMapCont).getMap( params );
}

inline void
TileMapHandler::dumpMapSizes() const
{
#ifdef __unix__
//   (*m_tileMapCont).dumpMapSizes(); 
#endif
}

inline bool
TileMapHandler::haveMap(const MC2SimpleString& params) const
{
   return getMap(params);
}

void
TileMapHandler::dumpParams()
{
#ifdef __unix__
#endif
}


inline bool
TileMapHandler::tileMapReceived(const MC2SimpleString& descr,
                                const TileMapParams& tileMapParams,
                                TileMap* tileMap,
                                vector<TileMapParams>& removeFromCache )
{   
#ifdef USE_TRACE
   TRACE_FUNC();
#endif
   // Add the map using the parameters. Can be different
   // inside the map because of gzip.
   bool mapOK = false;
   bool cancelRequestedMaps = false;

   // It's okay to pass NULL here, it will be a valid argument to addMap
   MapsToDrawIt it = (*m_tileMapCont).addMap( 
      tileMapParams, tileMap, removeFromCache, mapOK, cancelRequestedMaps );
   
   if (it == (*m_tileMapCont).endMapsToDraw()){
      // The tilemap was not stored and should be DESTROYED.
      delete tileMap;
      tileMap = NULL;
   }
   
   if ( ! mapOK ) {
      return false;
   }
   
   if ( cancelRequestedMaps ) {
      mc2dbg8 << "TMH: Calling cancelAll() since empty maps found "
             << "among the requested." << endl;
      m_requester->cancelAll();
      // Request repaint so that the maps are requested again.
      requestRepaint();
   }
   
   // Request another timer to repaint the stuff after everything
   // has arrived
   if ( m_repaintAfterMapReceptionTimer == 0) {
      m_repaintAfterMapReceptionTimer = m_toolkit->requestTimer(this, 1000);
   }

   if ( it != (*m_tileMapCont).endMapsToDraw() ) {
      if ( m_drawingContext != DrawingContextProxy::NONE ) {
         m_textHandler->newMapArrived( it, 
                                       (*m_tileMapCont).beginMapsToDraw(),
                                       (*m_tileMapCont).endMapsToDraw() );
         
         // m_textHandler->updateMaps( (*m_tileMapCont).beginMapsToDraw(),
         //                            (*m_tileMapCont).endMapsToDraw() );
      }
      if ( ! m_outlinesDrawnLastTime ) {
         // Do not do the needful until the outlines have been drawn.
         if ( m_textHandlerIdleID ) {
            m_toolkit->cancelIdle(this, m_textHandlerIdleID );
            m_textHandlerIdleID = 0;
         }
      }
   }
   m_mapArrivedSinceRepaint = true;

   // Check if this new map that arrived is a string map that was
   // requested by getNameForFeature. 
   if ( m_waitingForInfoString.second != NULL && 
        m_waitingForInfoString.first == descr ) {
      m_waitingForInfoString.second->notifyInfoNamesAvailable(); 
      // Clear the old one, 
      // i.e. we are not waiting for any strings now any more.
      m_waitingForInfoString.second = NULL;
   }

   return true;
}

MC2SimpleString
TileMapHandler::bitMapDescToBitMapName(const MC2SimpleString& desc)
{
   // Remove the leading 'B' and part before and including ';'
   // Create a temporary string to work with. It will be shorter than
   // the original.
   char* tmpStr = new char[desc.length()];
   
   // We know that at least the initial character 'b' or 'B' 
   // should be removed.
   uint32 startOfDesc = 1;
   
   char* semicolonPos = strchr(desc.c_str(), ';');
   if (semicolonPos != NULL) {
      // ';' was present in the string. Skip everything
      // up to and including the ';'.
      startOfDesc = (semicolonPos - desc.c_str()) + 1;
   }  

   // Go from after the B to the first '.'
   for( uint32 i = startOfDesc; i < desc.length(); ++i ) {
      tmpStr[i-startOfDesc] = desc.c_str()[i];
      if ( desc.c_str()[i] == '.' ) {
         // Found the dot
         tmpStr[i-startOfDesc] = '\0';
         break;
      }
   }
   return MC2SimpleStringNoCopy(tmpStr);
}

MC2SimpleString
TileMapHandler::bitMapNameToDesc( const MC2SimpleString& name )
{
   // Add B etc. Make room for the B and the name and a 10 char ext
   // (and a dot).
   char* tmpStr = new char[ 1 + name.length() + 10 + 1 + 1 + 1];
   
   // The initial char.
   char initialChar = 'b';
   // Bitmap extension
   const char* bitmapExtension = NULL;
   // Fetch initial char and bitmap extension depending on which
   // renderer that is used.
   if ( m_drawingContext == DrawingContextProxy::EXTERNAL ) {
      initialChar = 'b';
      bitmapExtension = m_imgLoader->getBitMapExtension();
   } else if ( m_drawingContext == DrawingContextProxy::PLOTTER ) {
      if (m_plotter->returnsVisibleSizeOfBitmaps()) {
         initialChar = 'b';
      } else {
         initialChar = 'B';
      }
      bitmapExtension = m_plotter->getBitMapExtension();
   } else {
      // No valid renderer.
      return "berror";
   }

   // First initial char.
   tmpStr[0] = initialChar;
   int offset = 1;

   bool serverSupportsScalingBitmaps = false;
#ifdef SERVER_SUPPORTS_SCALING_BITMAPS
   serverSupportsScalingBitmaps = true;
#endif

//scaled bitmaps looks bad on iphone, so avoid it:
#ifdef IPHONE
   serverSupportsScalingBitmaps = false;
#elif defined(ARCH_OS_WINDOWS)   
   // For now don't use dpi 2 for windows mobile bitmaps.
   // Fix in future so that the dpi factor for both bitmaps
   // and geometry can be set in the api:s.
   serverSupportsScalingBitmaps = false;
#endif

   if (serverSupportsScalingBitmaps && 
       m_projection.getDPICorrectionFactor() == 2) {
      // Add initial parameters, before image name. 
      // Currently no parameters is defaulting to DPI factor 2.
      tmpStr[1] = ';';
      ++offset;
   }

   // Then add the last part of the string, i.e. name + suffix.
   char* lastPartOfString = tmpStr;
   lastPartOfString += offset;

   sprintf(lastPartOfString, "%s%c%s",
           name.c_str(),
           '.',
           bitmapExtension);
   
   return MC2SimpleStringNoCopy(tmpStr);
}

bool
TileMapHandler::requestTextHandlerIdle()
{
   if ( m_drawingContext == DrawingContextProxy::NONE ) {
      return false;
   }
   if ( m_textHandlerIdleID == 0 ) {
      if ( m_textHandler->needsToRun() ) {
         m_textHandlerIdleID = m_toolkit->requestIdle(this);
         return true;
      } else {
         return false;
      }
   }
   return true;
}

void
TileMapHandler::runIdleTask(uint32 id)
{
   //mc2dbg8 << "[TMH]: runIdleTask(" << id << ")" << endl;
   if ( id == m_textHandlerIdleID ) {
      
      unsigned int startTimeMillis = TileMapUtil::currentTimeMillis();

      static const unsigned int maxTimeMillis = 50;

      mc2dbg2 << "[TMH]: Texthandler-> do stuff " << endl;

      while( m_textHandler->needsToRun() &&
             ((TileMapUtil::currentTimeMillis() - startTimeMillis) < maxTimeMillis))
      {
         m_textHandler->doTheNeedful();         
         // m_textHandler->doIteration();         
      }

      m_textHandlerIdleID = 0;
      // Request a new one if needed.
      if ( requestTextHandlerIdle() == false ) {
         if ( m_outlinesDrawnLastTime ) {
            // Only request repaint if the outlines have been
            // drawn. Else there will come a repaint.
            
            // Make the repaint method repaint everything correctly.
            m_detailRepaintTimerHasBeenRun = true;
            m_outlinesDrawnLastTime = false;
            requestRepaint();
         }
      } else {
         coreprintln("Not done placing text after %u ms, requesting timer..",
                     maxTimeMillis);
      }

      coreprintln("Text handling took %u ms",
                  TileMapUtil::currentTimeMillis() - startTimeMillis);
   }
}

void
TileMapHandler::timerExpired(uint32 id)
{
   //mc2dbg8 << "[TMH]: The timer " << id << " expired" << endl;
   if ( id == m_repaintTimerRequested ) {
      mc2dbg8 << "[TMH]: repaint timer" << endl;
      m_repaintTimerRequested = 0;
      repaint(0);
   } else if ( id == m_repaintAfterMapReceptionTimer ) {
      // We would like to request a repaint now. All maps may
      // be downloaded.
      m_repaintAfterMapReceptionTimer = 0;
      mc2dbg8 << "[TMH]: map reception timer" << endl;
      requestRepaint();
   } else if ( id == m_detailRepaintTimerRequested ) {
      
      m_detailRepaintTimerHasBeenRun = true;
      m_detailRepaintTimerRequested = 0;
      mc2dbg8 << "[TMH]: detail timer" << endl;
      // Do not request a repaint if the details have been drawn
      // already.
      if ( m_outlinesDrawnLastTime == false ) {
         requestRepaint();
      }
   } else if ( id == m_rerequestTimer ) {
      if ( m_rerequestTimerValue < c_maxRerequestTimerValue ) {
         // Multiply the timer value with 1.5
         m_rerequestTimerValue = m_rerequestTimerValue +
                                 (m_rerequestTimerValue >> 1);
         
         m_rerequestTimer =
            m_toolkit->requestTimer( this, m_rerequestTimerValue );
         mc2dbg8 << "[TMH]: re-requestTimer new timer ("
                <<  m_rerequestTimerValue << ")" << endl;
      } else {
         // Too long time. No more re-request timers
         m_rerequestTimer = 0;
         mc2dbg8 << "[TMH]: re-requestTimer - no new timers" << endl;
      }
      
      requestRepaint();
   } else if( id == m_positionInterpolationRequested ) {
      positionInterpolationTimerExpired();
   } else if( id == m_tileMapLoaderTimerRequested ) {

      // std::cout << "TileMapLoader timer expired!" << std::endl;
      
      updateTileMapLoader();
      
      if(m_tileMapLoader->hasWork()) {
         m_tileMapLoaderTimerRequested =
            m_toolkit->requestTimer(this,
                                    TileMapLoader::MIN_LOAD_INTERVAL_MS);         
      } else {
         m_tileMapLoaderTimerRequested = 0;
      }
   }
}

void
TileMapHandler::setHighlight( bool onoff )
{
   m_showHighlight = onoff;
}

void
TileMapHandler::setHighlightPoint( const MC2Point& hightlightPoint )
{
   m_highlightPoint = hightlightPoint;
}

bool
TileMapHandler::isLayerVisible(int layerNo) {
   return m_layersToDisplay.find( layerNo ) != m_layersToDisplay.end();
}

void
TileMapHandler::setTextSettings( const TileMapTextSettings& settings )
{
   m_textHandler->setTextSettings( settings );
}

void
TileMapHandler::sendEvent( const TileMapEvent& event )
{
   for ( vector<TileMapEventListener*>::iterator it = m_eventListeners.begin();
         it != m_eventListeners.end();
         ++it ) {
      (*it)->handleTileMapEvent( event );
   }
}

void
TileMapHandler::removeEventListener( TileMapEventListener* listener )
{
   for ( vector<TileMapEventListener*>::iterator it = m_eventListeners.begin();
         it != m_eventListeners.end(); ) {
      if ( *it == listener ) {
         it = m_eventListeners.erase(it);
      } else {
         ++it;
      }
   }
}

void
TileMapHandler::addEventListener( TileMapEventListener* listener )
{
   if ( listener == NULL ) {
      return;
   }
   removeEventListener( listener );
   m_eventListeners.push_back( listener );
}


void TileMapHandler::tileMapLoaded(const MC2SimpleString& descr,
                                   const TileMapParams& tileMapParams,
                                   TileMap* tileMap,
                                   BitBuffer* dataBuffer)
{
   vector<TileMapParams> removeFromCache;

   m_garbage.checkGarbage();
            
   if ( !tileMapReceived(descr, tileMapParams, tileMap, removeFromCache) ) {
      mc2dbg8 << "TMH: Tilemap could not be decoded. "
              << "Requesting new tmfd." << endl;
      // The tilemap could not be decoded. Request a new mapdesc.
      // Request the real new one
      // Create new random characters
      m_randChars = 
         MC2SimpleStringNoCopy(RandomFuncs::newRandString(3));
               
      // Request more => request a new tmfd.
      m_descCRCReceived = false;
   }
      
   m_garbage.checkGarbage();
   
   if ( removeFromCache.empty() ) {
      // We will not use the BitBuffer anymore. Add to cache.
      m_requester->release(descr, dataBuffer);
   } else {
      // Don't add the buffer to the cache. Delete instead.
      mc2dbg8 << "TMH: Remove from cache:" << endl;
      delete dataBuffer;
      for ( uint32 i = 0; i < removeFromCache.size(); ++i ) {
         mc2dbg8 << "TMH: Removing " << removeFromCache[ i ] 
                 << " from cache." << endl;
         m_requester->removeBuffer( removeFromCache[ i ] );
      }
   }
}
                                   

void TileMapHandler::updateTileMapLoader()
{
   m_tileMapLoader->run();

   typedef std::vector<TileMapLoader::LoadedData> LoadedVector;
      
   LoadedVector loaded;
   m_tileMapLoader->acquire(loaded);

   for (LoadedVector::iterator itr = loaded.begin(); itr!=loaded.end(); itr++)
   {
      TileMapLoader::LoadedData& l = *itr;
      tileMapLoaded(l.descr, l.params, l.tileMap, l.buffer);         
   }
   
   loaded.clear();   
}

void TileMapHandler::tileMapBufferReceived(const MC2SimpleString& descr,
                                           BitBuffer* dataBuffer,
                                           bool requestMore )
{
   // Map coming!
   if ( m_mapFormatDesc != NULL ) {
      // TileMapParams tileMapParams( descr );
      // TileMap* curMap = new TileMap;
      // curMap->load(*dataBuffer, *m_mapFormatDesc, tileMapParams);
      // tileMapLoaded(descr, tileMapParams, curMap, dataBuffer);
      
      m_tileMapLoader->loadTileMap(descr, dataBuffer);


      if(m_tileMapLoader->timeSinceLastRunMillis() >
         TileMapLoader::MIN_LOAD_INTERVAL_MS)
      {
         updateTileMapLoader();
      }

      if(m_tileMapLoader->hasWork() && m_tileMapLoaderTimerRequested == 0) {
         m_tileMapLoaderTimerRequested =
            m_toolkit->requestTimer(this, TileMapLoader::MIN_LOAD_INTERVAL_MS);
      }
      
      if(TileMapConfig::LOAD_IMMEDIATE || m_tileMapLoader->insideStartPhase()) {

         while(m_tileMapLoader->hasWork()) {
            updateTileMapLoader();
         }
      }
   } else {
      // No format desc yet, leave it to cache.
      m_requester->release(descr, dataBuffer);
   }
   
   if ( requestMore ) {
      // Request max one new map or similar.
      requestMaps(1);
   }
}


void
TileMapHandler::dataBufferReceived(const MC2SimpleString& descr,
                                   BitBuffer* dataBuffer,
                                   bool requestMore)
{
   // Request another timer to repaint the stuff after everything
   // has arrived
   if ( m_repaintAfterMapReceptionTimer == 0) {
      m_repaintAfterMapReceptionTimer = m_toolkit->requestTimer(this, 1000);
   }

   // Also request the re-request timer if the buffer is NULL
   // and the re-request timer is not running already.
   if ( dataBuffer == NULL && m_rerequestTimer == 0 ) {
      // Take the lowest value for the timer.
      m_rerequestTimerValue = c_minRerequestTimerValue;
      m_rerequestTimer =
         m_toolkit->requestTimer( this, m_rerequestTimerValue );
   }

  
   if ( dataBuffer == NULL ) {
      if ( requestMore ) {
         // Request max one new map or similar.
         // It might be time to wake up.
         requestMaps(1);
      }    
      return;
   }
 
   TileMapParamTypes::param_t paramType = 
            TileMapParamTypes::getParamType( descr.c_str() );
   vector<TileMapParams> removeFromCache;
   
   switch ( paramType ) {
      case TileMapParamTypes::TILE:
      {
         tileMapBufferReceived(descr, dataBuffer, requestMore);
         return;
      } break;
      
   case TileMapParamTypes::BITMAP:
      handleIncomingBitMap( dataBuffer, descr );
      break;
      case TileMapParamTypes::FORMAT_DESC_CRC : {
         // CRC
         TileMapFormatDescCRC crcObj;
         crcObj.load( *dataBuffer );
         m_serverDescCRC = crcObj.getCRC();
         m_descCRCReceived = true;
         mc2dbg8 << "[TMH]: CRC buffer received. CRC = "
                << MC2HEX(m_serverDescCRC) << endl;
      } break;
      
      case TileMapParamTypes::FORMAT_DESC : {
         // Description coming!
         // We don't want a TMFD if we are currently requesting maps,
         // since we use iterators in TileMapContainer which will be
         // invalidated when getting a new TMFD.
         if ( m_inRequestingMaps > 0 ) {
            // Release the TMFD to memory cache below
            mc2dbg8 << "[TMH]: Unsolicited TMFD received when"
                   << " requesting tilemaps. Releasing to memory cache "
                   << "to avoid crash." << endl;
            break;
         }
                                               
         // Only exchange the desc if it is the real one or we don't have
         // any already.
         // Load the description
         TileMapFormatDesc* tmpNewDesc =
            new TileMapFormatDesc();
         // Load it. Transfer stuff from the old one.            
         tmpNewDesc->load( *dataBuffer, m_mapFormatDesc );
         mc2dbg8 << "[TMH]: TMFD: timeStamp (" << descr
                << ")" 
                << tmpNewDesc->getTimeStamp() << endl;
         // Use the new desc if it is newer or we don't have one
         // already.

         // Use it if:
         // No existing TMFD       or
         // No crc received yet    or
         // New tmfd crc matches.
         bool okToUse = ( m_mapFormatDesc == NULL ) ||
                        ( !m_descCRCReceived ) ||
                        ( m_serverDescCRC == tmpNewDesc->getCRC() );
         
         if ( okToUse ) {
            mc2dbg8 << "[TMH]: TMFD is set to " << descr << endl;
            
            setFormatDesc(tmpNewDesc);
            
            // Save the currently used desc
            delete m_dxxBuffer;
            m_dxxBuffer = new BitBuffer( *dataBuffer );

            if( descr.c_str()[0] == 'd' ){
               // Copy the description to another buffer and save it as "dXXX".
               m_requester->release("dXXX", new BitBuffer( *dataBuffer ) );
               // Copy the description to another buffer and save it as "dYYY".
               m_requester->release("dYYY", new BitBuffer( *dataBuffer ) );
            }
            else {
               // Copy the description to another buffer and save it as "DXXX".
               m_requester->release("DXXX", new BitBuffer( *dataBuffer ) );
               // Copy the description to another buffer and save it as "DYYY".
               m_requester->release("DYYY", new BitBuffer( *dataBuffer ) );
            }

         } else {
            // The format desc was no good.
            // Check if we need to fetch a new crc.
            if ( m_mapFormatDesc != NULL &&
                 m_descCRCReceived ) {
               if ( m_serverDescCRC != tmpNewDesc->getCRC() &&
                    m_serverDescCRC != m_mapFormatDesc->getCRC() ) {
                  // Fetch new crc, since neither the old or new
                  // tmfd matched the server crc.
                  m_randChars = 
                     MC2SimpleStringNoCopy(RandomFuncs::newRandString(3));
                  m_descCRCReceived = false;
                  mc2dbg8 << "[TMH]: Requesting new TMFD CRC since mismatch"
                         << " between old and new tmfd against existing CRC." 
                         << endl;
               }
            }
                 
            delete tmpNewDesc;
         }
         updateParams();
      } break;
      case TileMapParamTypes::UNKNOWN: {
         MC2_ASSERT(false);
         break;
      }
   }
   
   if ( removeFromCache.empty() ) {
         // We will not use the BitBuffer anymore. Add to cache.
      m_requester->release(descr, dataBuffer);
   } else {
      // Don't add the buffer to the cache. Delete instead.
      mc2dbg8 << "TMH: Remove from cache:" << endl;
      delete dataBuffer;
      for ( uint32 i = 0; i < removeFromCache.size(); ++i ) {
         mc2dbg8 << "TMH: Removing " << removeFromCache[ i ] 
                << " from cache." << endl;
         m_requester->removeBuffer( removeFromCache[ i ] );
      }
   }
   
   if ( requestMore ) {
      // Request max one new map or similar.
      requestMaps(1);
   }
}

bool
TileMapHandler::requestFromCacheAndCreate(const MC2SimpleString& desc)
{
   if (m_tileMapLoader->inLoadingQueue(desc)) {
      // The map is not actually in the cache, but it is already loaded
      // and waiting to be unpacked.
      return true;
   }
   
   BitBuffer* bitBuf = m_requester->requestCached(desc);
   if ( bitBuf ) {     
      dataBufferReceived(desc, bitBuf, false);     
   }
   return bitBuf != NULL;
}

bool
TileMapHandler::requestFromCacheOrForReal(const MC2SimpleString& desc,
                                 const DBufRequester::request_t& reqType )
{
   if ( ! requestFromCacheAndCreate( desc ) ) {
      // Request for real.
      m_requester->request( desc,
                            m_dataBufferListener,
                            reqType );
      return false; // Could not get it from cache.
   } else {
      return true;  // Got it from cache.
   }
}

void
TileMapHandler::requestCachedTMFD()
{
   if( m_nightMode ){
      // Also request the previously saved one
      requestFromCacheOrForReal( "dXXX", DBufRequester::onlyCache );
      // Also request one from the readonly cache too.
      requestFromCacheOrForReal( "dYYY", DBufRequester::onlyCache );
   } else {
      // Also request the previously saved one
      requestFromCacheOrForReal( "DXXX", DBufRequester::onlyCache );
      // Also request one from the readonly cache too.
      requestFromCacheOrForReal( "DYYY", DBufRequester::onlyCache );
   }
}

void
TileMapHandler::requestMaps(int maxNbr)
{
   
   // Counter for the maxNbr
   int nbrReq = 0;

   // Request the missing format description.

   bool myTMFDHasWrongCRC = m_mapFormatDesc && m_mapFormatDesc->getCRC() != m_serverDescCRC;

   // If no CRC has arrived or we already have a TMFD with the wrong CRC 
   // - request new CRC
   // Either the CRC or the TMFD is wrong
   if ( m_descCRCReceived == false || myTMFDHasWrongCRC ) {
      requestFromCacheOrForReal(
         TileMapFormatDescCRC::createParamString( m_lang,
                                                  m_toolkit->getIDString(),
                                                  m_randChars.c_str(),
                                                  m_nightMode ) );
      mc2dbg8 << "[TMH]: Requesting TMFD CRC because:" << endl;
      if ( ! m_descCRCReceived ) {
         mc2dbg8 << "does not have a valid TMFD CRC" << endl;
      } else {
         mc2dbg8 << "my tmfd has wrong crc." << endl;
      }
   }

   if ( m_descCRCReceived ) {
      if ( ( m_mapFormatDesc == NULL ) ||
           ( m_mapFormatDesc->getCRC() != m_serverDescCRC ) ) {
         // The CRC was wrong
         requestFromCacheOrForReal(
            TileMapFormatDesc::createParamString(
               m_lang,
               m_toolkit->getIDString(),
               m_randChars.c_str(),
               m_nightMode ) );
         mc2dbg8 << "[TMH]: Requesting TMFD because:" << endl;
         if ( m_mapFormatDesc == NULL ) {
            mc2dbg8 << "does not have any TMFD." << endl;
         } else {
            mc2dbg8 << "tmfd crc mismatch." << endl;
         }
      }
   }
         
   if ( m_mapFormatDesc == NULL ) {
      requestCachedTMFD();
   }
   
   // Print the maps to request
   
   ScopedCounter ctr( m_inRequestingMaps );
   
   mc2dbg8 << "[TMH]: Starting to request missing maps." << endl; 
   // Request the missing maps.
   {
      //(*m_tileMapCont).m_mapsToReq.dump();      
      PIt pIt = (*m_tileMapCont).beginParamsToReq();
      for ( MIt mIt = (*m_tileMapCont).beginMapsToReq(); 
            mIt != (*m_tileMapCont).endMapsToReq(); ++pIt, ++mIt ) {
         // WARNING: The requester can call dataBufferReceived directly
         // here if no threads are present.
         DBufRequester::request_t requestType = DBufRequester::cacheOrInternet;
         if ( (*m_tileMapCont).toRequestMap( mIt, requestType ) ) {
            mc2dbg8 << "[TMH]: Requesting " <<  pIt->getAsString() << endl;
            const TileMapParams* params = &(*pIt);
            if ( nbrReq < maxNbr ) {
               bool cached = requestFromCacheOrForReal( *params, 
                                                        requestType );
               if ( ! cached ) {
                  ++nbrReq;
               }   
            } else {               
               // Only cached maps are allowed now.               
               requestFromCacheAndCreate( *params );
            }
         }         
      }
   }
   
   mc2dbg8 << "[TMH]: Finished requesting missing maps." << endl; 
   // Request the reserve maps.
   {
      
      PIt pIt = (*m_tileMapCont).beginReserveParamsToReq();
      for ( MIt mIt = (*m_tileMapCont).beginReserveMapsToReq(); 
            mIt != (*m_tileMapCont).endReserveMapsToReq(); ++pIt, ++mIt ) {
         // WARNING: The requester can call dataBufferReceived directly
         // here if no threads are present.
         const TileMap* curMap = *mIt;
         if ( curMap == NULL ) {
            const TileMapParams* params = &(*pIt);
            mc2dbg8 << "Requested param " << params->getAsString()
               << endl;
            if ( nbrReq < maxNbr ) {
               bool cached = requestFromCacheOrForReal( *params );
               if ( ! cached ) {
                  ++nbrReq;
               }   
            } else {
               // Only cached maps are allowed now.               
               requestFromCacheAndCreate( *params );
            }
         }         
      }
   }
   
   // Request the missing bitmaps.
   // Copy the set so that there will not be trouble if it was cached.
   set<MC2SimpleString> tmpNeeded ( m_neededBitMaps );
   for ( set<MC2SimpleString>::const_iterator it = tmpNeeded.begin();
         it != tmpNeeded.end(); ++it ) {
      if ( m_bitMaps.find( *it ) == m_bitMaps.end() ) {
         // Make temp string
         bool cached = requestFromCacheOrForReal(
            bitMapNameToDesc(*it) );
         if ( ! cached ) {
            if ( nbrReq++ > maxNbr ) {
               // No more requests allowed.
               return;
            }       
         }
      } else {
         // Already there. An error really.
         m_neededBitMaps.erase( *it );
      }
   }
}

void
TileMapHandler::start()
{
   // Requesting maps is what get's everything going.
   if (!isInitialized()) {
      requestMaps(1);
   }
}  

void
TileMapHandler::repaint(uint32 /*maxTimeMillis*/)
{   
   if ( m_painting ) {
      // Already painting.
      return;
   }


#ifdef THIS_CODE_IS_OMITTED_FOR_TESTING
   const int32 minPaintTime = 1*20;
   const int32 timeDiff = TileMapUtil::currentTimeMillis() -
                               m_lastRepaintTime;

   mc2dbg8 << "[TMH]: Timediff = " << timeDiff << endl;
   
   if ( timeDiff < minPaintTime ) {
      if ( !m_repaintTimerRequested ) {
      // If there is more than one
         mc2dbg8 << "[TMH]: Requested repaint timer" << endl;
         m_repaintTimerRequested =
            m_toolkit->requestTimer(this,
                                    minPaintTime - timeDiff);
      }
      mc2dbg8 << "[TMH]: No repaint. Too little diff" << endl;
      return;
   }
#endif  // THIS_CODE_IS_OMITTED_FOR_TESTING
   
   // We are painting
   ++m_painting;

   {
      isab::Rectangle curScreenSize;
      getMapSizeFromRenderer( curScreenSize );

      m_projection.setScreenSize(MC2Point(curScreenSize.getWidth(),
                                          curScreenSize.getHeight()) );
      if ( m_lastScreenSize != curScreenSize ) {
         m_matrixChangedSinceDraw = true;
      }
      m_lastScreenSize = curScreenSize;
   }
   
   requestMaps(10);
   
   // Check if there is a complete level.
   plotWhatWeGot( );

   updateTileMapLoader();
   
   if( getInterpolatingPositions() ) {
      requestPositionInterpolation();
   }
   
   // We are (maybe) not painting (we could have gotten here recursively).
   --m_painting;

   // Check if we need to repaint again. (E.g. a new, better map may have
   // arrived).
   m_lastRepaintTime = TileMapUtil::currentTimeMillis();
   
   //(*m_tileMapCont).m_mapsToReq.dump();
   m_mapArrivedSinceRepaint = false;

   // Reset the state in our overlay view
   m_overlayView->resetStateChanged();
}

inline bool
TileMapHandler::coordsInside(const CoordsArg& /*coords*/,
                             const MC2BoundingBox& /*bbox*/)
{
   // This is now checked in the loop when drawing polygons or polylines
   return true;
}


uint32
TileMapHandler::getTotalMapSize() const
{
   uint32 totSize = 0;
#ifdef __unix__
   totSize = (*m_tileMapCont).getTotalMapSize();
#endif
   return totSize;
}

inline bool
TileMapHandler::
getXYFromCoordArg( const TilePrimitiveFeature& primWithCoordArg,
                   const MC2BoundingBox& bbox,
                   MC2Point& p ) const
{
   const TileFeatureArg* arg = primWithCoordArg.getArg(TileArgNames::coord);
   const CoordArg* coordArg = static_cast<const CoordArg*>(arg);
   if ( arg == NULL ) {
      return false;
   }
   if ( bbox.inside( coordArg->getCoord().getLat(),
                     coordArg->getCoord().getLon() ) ) {
      
      m_projection.transformPointInternalCosLat( p, coordArg->getCoord() );
      return true;
   }
   return false;
}

inline int
TileMapHandler::plotCircleFeature( TilePrimitiveFeature& prim,
                                   const MC2BoundingBox& bbox,
                                   int /*pass*/,
                                   int /*moving*/ )
{
   // Must have a coord arg.
   MC2Point p;
   if ( ! getXYFromCoordArg( prim, bbox, p ) ) {
      return 0;
   }
   int x = p.getX();;
   int y = p.getY();
   const SimpleArg* radiusArg =
      static_cast<const SimpleArg*>(prim.getArg(TileArgNames::radius));
   if ( radiusArg == NULL ) {
      return 0;
   }
   const SimpleArg* colorArg =
      static_cast<const SimpleArg*>(prim.getArg(TileArgNames::color));
   if ( colorArg ) {
      m_plotter->setPenColor( colorArg->getValue() );
   } else {
      return 0;
   }

   uint32 radius = radiusArg->getValue();
   uint32 halfRadius = radius >> 1;
   m_plotter->drawArc( true, isab::Rectangle(x - halfRadius, y - halfRadius,
                                       radius, radius), 0, 360);
   
   return 1;
}

isab::BitMap*
TileMapHandler::getOrRequestBitMap( const MC2SimpleString& bitMapName )
{

   
   bitMapMap_t::const_iterator it = m_bitMaps.find( bitMapName );
   if ( it != m_bitMaps.end() ) {
      return it->second;
   } else {
      // Will be requested later.
      m_neededBitMaps.insert( bitMapName );
   }
   return NULL;
}

inline int
TileMapHandler::plotBitMapFeature( TileMap& curMap,
                                   TilePrimitiveFeature& prim,
                                   const MC2BoundingBox& bbox,
                                   int pass,
                                   int /*moving*/ )
{
   // Should not be drawn on pass 0
   if ( pass == 0 ) {
      return 0;
   }

   // Check if it is in a forbidden category
   const TileFeature* parent = curMap.getParentFeature(&prim);
   
   if ( parent != NULL &&
        ( m_disabledParentTypes.find(parent->getDrawType()) !=
          m_disabledParentTypes.end() ) ) {
      return 0;
   }

   // Must have a coord arg.
   MC2Point center(0,0);
   if ( ! getXYFromCoordArg( prim, bbox, center ) ) {
      return 0;
   }
   
   const StringArg* bitMapName =
      static_cast<const StringArg*>(prim.getArg(TileArgNames::image_name));

   if ( bitMapName == NULL ) {
      return 0;
   }

   isab::BitMap* curBitMap = getOrRequestBitMap( bitMapName->getValue() );
   if ( curBitMap != NULL ) {      
      // Fetch bitmap size from the plotter.
      isab::Rectangle rect;

      m_plotter->getBitMapAsRectangle( rect, 
                                       m_operationTranslator->getPoint3DFrom2D(center),
                                       curBitMap );
      PixelBox pixelBox = rect;
      
      // Debug
#if 0
      m_plotter->setPenColor(0xffff00);
      m_plotter->drawRect(false,
                          rect);
#endif 
      
      if ( m_overlapDetector.addIfNotOverlapping( pixelBox ) ) {
         // Set coordinate
         m_screenCoords.push_back(center);         
         m_plotter->drawBitMap( m_screenCoords.back(), curBitMap );
      } else {
         return 0;
      }
   } else {
      // We will fetch it some other time
      return 0;
   }

   return 1; // Number of coords plotted.
}

int
TileMapHandler::plotBBox( const MC2BoundingBox& bbox,
                          const MC2BoundingBox& /*screenBBox*/ )
{
//    if ( ! screenBBox.overlaps( bbox ) ) {
//       return 0;
//    }
   vector<MC2Coordinate> coords;
   coords.push_back( MC2Coordinate(bbox.getMinLat(), bbox.getMinLon()) );
   coords.push_back( MC2Coordinate(bbox.getMinLat(), bbox.getMaxLon()) );
   coords.push_back( MC2Coordinate(bbox.getMaxLat(), bbox.getMaxLon()) );
   coords.push_back( MC2Coordinate(bbox.getMaxLat(), bbox.getMinLon()) );
   coords.push_back( MC2Coordinate(bbox.getMinLat(), bbox.getMinLon()) );
   vector<MC2Point> points;
   points.reserve( coords.size() );
   MC2Point curPoint(0,0);
   vector<MC2Coordinate>::const_iterator the_end ( coords.end() );
   for ( vector<MC2Coordinate>::const_iterator it ( coords.begin() );
         it != the_end;
         ++it ) {
      m_projection.transformPointInternalCosLat( curPoint.getX(),
                                                 curPoint.getY(),
                                                 *it );
      points.push_back(curPoint);
   }
   m_plotter->drawPolyLine( points.begin(), points.end() );
   return points.size();
}

inline int
TileMapHandler::plotPolyLines(const TilePrimitiveFeature& /*feat*/,
                              uint32 color,
                              int lineWidth )
{
   m_plotter->drawPolyLineWithColor( m_screenCoords.begin(),
                                     m_screenCoords.end(), 
                                     color,
                                     lineWidth );
   return m_screenCoords.size();
}

inline int
TileMapHandler::plotPolygon( const MC2BoundingBox& bbox,
                             TileMap& tilemap,
                             const TilePrimitiveFeature& feat,
                             uint32 colorVal )
{
   
   m_plotter->drawPolygonWithColor( m_screenCoords.begin(),
                                    m_screenCoords.end(), 
                                    colorVal );
   
//       int handle = feat.getFeatureShapeHandle();
   
//       std::vector<MC2Coordinate> tempCoords;

//       for( VertexBuffer::iterator vI = g.triangleData.begin();
//            vI != g.triangleData.end();
//            vI++ ) {

//          tempCoords.push_back( vI->x, vI->y );
//       }
   
//       m_plotter->drawPolyLineWithColor( tempCoords.begin(),
//                                         tempCoords
   
   return m_screenCoords.size();
}

inline uint32
TileMapHandler::getPolylineInnerPixelWidth( 
   const TilePrimitiveFeature& prim ) const
{
   MC2_ASSERT( prim.getType() == TileFeature::line );
   uint32 widthPixels = 1;
      
   // Get the width argument given in pixels
   uint32 tmpWidthPixels = MAX_UINT32;
   const SimpleArg* widthArg = 
      static_cast<const SimpleArg*>((prim.getArg(TileArgNames::width)));
   if ( widthArg != NULL ) {
      tmpWidthPixels = widthArg->getValue( m_scaleIndex );
   }

   // Check for meters arg first, since we must send the
   // old arg for the old clients.
   const SimpleArg* widthMetersArg =
      static_cast<const SimpleArg*>(
         (prim.getArg(TileArgNames::width_meters)));

   bool useMeters = false;
   if ( widthMetersArg != NULL ) {
      uint32 tmpWidthMeters = widthMetersArg->getValue( m_scaleIndex );
      if ( ( tmpWidthMeters & MAX_UINT8 ) != MAX_UINT8 ) {
         useMeters = true;
         widthPixels = uint32(tmpWidthMeters / m_projection.getDPICorrectedScale());
         // Make sure the width is at least the normal width argument
         if ( ( tmpWidthPixels & MAX_UINT8 ) != MAX_UINT8 ) {
            widthPixels = MAX(widthPixels, tmpWidthPixels);
         }
         if ( widthPixels == 0 ) {
            // Must have odd width or more than zero. But zero is even
            // so it will also get here.
            ++widthPixels;
         }
      }
   }
   if ( ! useMeters ) {
      if ( ( tmpWidthPixels & MAX_UINT8 ) != MAX_UINT8 ) {
         widthPixels = tmpWidthPixels;
      }
   }
  
   widthPixels = widthPixels * m_projection.getDPICorrectionFactor();
   
   return widthPixels;
}

uint32
TileMapHandler::getPolylineOuterPixelWidth( 
                              const TilePrimitiveFeature& prim ) const
{
   uint32 width = getPolylineInnerPixelWidth( prim );
   // Outline
   const SimpleArg* borderColorArg = 
      static_cast<const SimpleArg*>(
            (prim.getArg(TileArgNames::border_color)));
   if ( borderColorArg != NULL ) {
      uint32 borderColor = 
         borderColorArg->getValue( m_scaleIndex );
      if ( VALID_TILE_COLOR( borderColor ) ) {
         width += 2;
      }
   }
   return width;   
}

inline int
TileMapHandler::plotPolygonOrLineFeature( const MC2BoundingBox& bbox,
                                          TileMap& tilemap,
                                          const TilePrimitiveFeature& prim,
                                          int pass,
                                          int moving )
{
   int nbrDrawn = 0;
   //MC2_ASSERT( int(curPrim->getType()) < 0 );
   
   const SimpleArg* color = static_cast<const SimpleArg*>(
                                         (prim.getArg(TileArgNames::color)));
      
   if ( prim.getType() == TilePrimitiveFeature::polygon ) {
      mc2dbg8 << "[TMH]: Filling " << endl;
      if ( color && ( pass == 0 ) ) {
         nbrDrawn += plotPolygon( bbox, tilemap, prim, color->getValue( m_scaleIndex ) );
      }
   } else {
      uint32 widthPixels = getPolylineInnerPixelWidth( prim );
      if ( color ) {
         if ( pass == 1 ) {
            // Inside
            nbrDrawn += plotPolyLines(prim,
                                      color->getValue( m_scaleIndex ),
                                      widthPixels );
         } else {
            if ( ( pass == 0 ) && ( moving == 0 ) ) {
               // Outline
               const SimpleArg* borderColorArg = 
                  static_cast<const SimpleArg*>(
                                (prim.getArg(TileArgNames::border_color)));
               if ( borderColorArg != NULL ) {
                  uint32 borderColor = 
                     borderColorArg->getValue( m_scaleIndex );
                  if ( VALID_TILE_COLOR( borderColor ) ) {
                     nbrDrawn += plotPolyLines(prim, borderColor,
                                               widthPixels + 2 );
                  }
               }
            }
         }
      }
   }
   return nbrDrawn;
}

inline int
TileMapHandler::checkScale( const TilePrimitiveFeature& feature ) const
{
   const TileFeatureArg* maxScale = feature.getArg( TileArgNames::max_scale );
   if ( maxScale != NULL ) {
      if ( TileFeatureArg::simpleArgCast( maxScale )->getValue() <= 
           m_projection.getDPICorrectedScale() ) {
         return false;
      } else {
         return true;
      }
   }
   return true;
}

inline void
TileMapHandler::clearCoords( TilePrimitiveFeature& feat )
{
   feat.setDrawn( false );
   m_screenCoords.clear();
}

void
floatCorrect( MC2Coordinate& coord ) {

   float latFloat = static_cast<float> (coord.lat );
   float lonFloat = static_cast<float> (coord.lon );

   coord.lat = static_cast<int>(latFloat);
   coord.lon = static_cast<int>(lonFloat);
}

inline int
TileMapHandler::prepareCoordinates( TileMap& tileMap,
                                    TilePrimitiveFeature& feat,
                                    const MC2BoundingBox& bbox,
                                    const MC2Point& centerOfBBox,
                                    int halfWorldPeriodPixels)
{
   // Get the coords.
   const CoordsArg* coords = static_cast<const CoordsArg*>(
      feat.getArg(TileArgNames::coords));

   if ( coords == NULL ) {
      return 0;
   }
 
   if ( !bbox.overlaps(coords->getBBox() ) ) {
      // Outside completely. And there should be no more coordinates.
      return 0;
   }

#if 0
   m_plotter->setLineWidth( 1 );
   m_plotter->setPenColor( 0 );
   plotBBox( coords->getBBox(), bbox );
#endif
   
   // For now we will use the same amount of points as in the
   // pointvector. Could be different when clipping etc is implemented.
   MC2Point curPoint(0,0);
   // Version with pre-calculated cosLat.
   // The size of the VectorProxy should be the same as the size of
   // the coords vector, or else we're ...
  
   MC2Point lastPoint( MAX_INT32, MAX_INT32 );

   uint32 nbrCoords = coords->size();
   
   m_realScreenCoords.resize( nbrCoords );
   vector<MC2Coordinate> clipCoords;
   
   using namespace PolygonTriangulation;
   
#ifndef SLIM_TILE_COORDS
   if ( feat.getType() == TileFeature::polygon &&
        ClipUtil::clipPolyToBBoxFast( bbox,
                                    clipCoords,
                                    &(*coords->begin()),
                                    &(*coords->end()))) 
#else
      CoordsArg::const_iterator begin = coords->begin(tileMap);
      CoordsArg::const_iterator end = coords->end(tileMap);

      unsigned int dist = std::distance( begin, end );
      
      const MC2Coordinate* begCoord = &(*begin);
      const MC2Coordinate* endCoord = begCoord + dist;

      if ( feat.getType() == TileFeature::polygon &&
        ClipUtil::clipPolyToBBoxFast( bbox,
                                    clipCoords,
                                    begCoord,
                                    endCoord))
#endif     
   { // Bracket is here in order to avoid confusing for vim.
      m_realScreenCoords.resize( clipCoords.size() );
      
      for ( vector<MC2Coordinate>::const_iterator it = clipCoords.begin();
            it != clipCoords.end();
            ++it ) {
         
         MC2Coordinate floatCorrected = *it;

         if( m_floatAccuracyCheck ) {
            floatCorrect( floatCorrected );            
         }
         
         m_projection.transformPointInternalCosLat( curPoint.getX(),
                                                    curPoint.getY(),
                                                    floatCorrected );
         if ( curPoint != lastPoint ) {
            m_screenCoords.push_back(curPoint);
            lastPoint = curPoint;
         }
      }
   } else {
      // Don't clip.
#ifndef SLIM_TILE_COORDS
      if( !coords->size() == 0 )  {
         scale = getScaleParameters( coords->begin(),
                                     coords->end(),
                                     MC2Coordinate::XYHelper() );
      }
      
      CoordsArg::const_iterator the_end ( coords->end() );
      for ( CoordsArg::const_iterator it ( coords->begin() );
#else               
      CoordsArg::const_iterator the_end ( coords->end(tileMap) );
      for ( CoordsArg::const_iterator it ( coords->begin(tileMap) );
#endif
            it != the_end;
            ++it ) {
         
         MC2Coordinate floatCorrected = *it;

         if( m_floatAccuracyCheck ) {
            floatCorrect( floatCorrected );            
         }
         
         m_projection.transformPointInternalCosLat( curPoint.getX(),
                                                    curPoint.getY(),
                                                    floatCorrected );
         if ( curPoint != lastPoint ) {
            m_screenCoords.push_back(curPoint);
            lastPoint = curPoint;
         }
      }
   }
   
   if( feat.getType() != TileFeature::line || halfWorldPeriodPixels == 0 ) {
      return m_screenCoords.size();
   }
         
   CoordsArg::const_iterator the_end ( coords->end(tileMap) );
   VectorProxy<MC2Point>::iterator pIt(m_screenCoords.begin());
   
   for ( CoordsArg::const_iterator it ( coords->begin(tileMap) );
         it != the_end; ++it  )
   {
      MC2Point& curPoint = *pIt;
      
      int dist = curPoint.getX() - centerOfBBox.getX();
      
      if(dist > halfWorldPeriodPixels) {
         curPoint.getX() -= halfWorldPeriodPixels*2;
      }
      else if(dist < -halfWorldPeriodPixels) {
         curPoint.getX() += halfWorldPeriodPixels*2;
      }
      
      pIt++;
   }
      
   return m_screenCoords.size();
}

inline int
TileMapHandler::plotPrimitive( TileMap& curMap,
                               TilePrimitiveFeature& curPrim,
                               const MC2BoundingBox& bbox,
                               const MC2Point& centerOfBBox,
                               int halfWorldPeriodPixels,
                               int pass,
                               int moving )
{
   int curNbrDrawn = 0;

   switch ( curPrim.getType() ) {
      case TilePrimitiveFeature::line:
      case TilePrimitiveFeature::polygon:
         if ( prepareCoordinates( curMap, curPrim, bbox,
                                  centerOfBBox, halfWorldPeriodPixels ) == 0 ) {
            return 0;
         }
         break;
   }
   
   switch ( curPrim.getType() ) {
      case TilePrimitiveFeature::line:
      case TilePrimitiveFeature::polygon:
         curNbrDrawn =
            plotPolygonOrLineFeature( bbox,curMap, curPrim, pass, moving );
         break;
      case TilePrimitiveFeature::bitmap:
         if ( ! checkScale( curPrim ) ) {
            return 0;
         }
         curNbrDrawn = plotBitMapFeature( curMap, curPrim, bbox,
                                          pass, moving  );
         break;
      case TilePrimitiveFeature::circle:
         if ( pass == 1 ) {
            curNbrDrawn += plotCircleFeature( curPrim, bbox,
                                              pass, moving );
         }
      default:
         // Unknown stuff.
         break;
   }
   
   // Mark if the primitive was drawn.
   curPrim.setDrawn( curNbrDrawn > 0 );

   return curNbrDrawn;
}

void
TileMapHandler::drawProgressIndicator()
{
   // Only draw progress indicator if we have a valid pixelbox for the
   // string.
   if ( ! m_progressIndicatorBox.isValid() ) {
      return;
   }
   
   // How many percent of the maps do we have?
   uint32 percentMapsReceived = (*m_tileMapCont).getPercentageMapsReceived();
   
   // Don't draw any progress indicator if everything is completed.
   if ( percentMapsReceived >= 100 ) {
      MC2_ASSERT( percentMapsReceived == 100 );
      return;
   }
      
   // Get font name. 
   const STRING* fontName = m_stringCache->getOrCreateString( 
         m_textHandler->getTextSettings().
            getProgressIndicatorFont().first );
   // And font size.
   int fontSize = 
      m_textHandler->getTextSettings().getProgressIndicatorFont().second;

   m_plotter->setFont( *fontName, fontSize );
   
   char tmpStr[40];
   const char percent = '%';
#ifdef __unix__
   sprintf( tmpStr, "%u%c %u/%u", 
         (unsigned int) percentMapsReceived, percent,
         (unsigned int) (*m_tileMapCont).m_mapsToReqCount.first, 
         (unsigned int) (*m_tileMapCont).m_mapsToReqCount.second );
#else
   sprintf( tmpStr, "%u%c",  
         (unsigned int) percentMapsReceived, percent );
#endif
   STRING* str = m_plotter->createString( tmpStr );
   
#if 0   
   m_plotter->setLineWidth( 1 );
   m_plotter->setFillColor( 231, 221, 187 );
   m_plotter->setPenColor( 231, 221, 187 );
   m_plotter->drawRect( true, m_progressIndicatorBox );
#endif 

   m_plotter->setPenColor(0,0,0);
   m_plotter->drawText( *str, m_progressIndicatorBox.getCenterPoint() );
   
   m_plotter->deleteString( str );
}

void 
TileMapHandler::plotCopyrightString( const char* copyrightString ) 
{
   if ( m_mapFormatDesc == NULL ) {
      return;
   }
   if ( ! m_showCopyright ) {
      return;
   }

   // Get font name. 
   const STRING* fontName = m_stringCache->getOrCreateString( 
         m_textHandler->getTextSettings().
            getCopyrightFont().first );
   // And font size.
   int fontSize = 
      m_textHandler->getTextSettings().getCopyrightFont().second;

   m_plotter->setFont( *fontName, fontSize );

   m_plotter->setPenColor( m_mapFormatDesc->getTextColor() );
   STRING* str = m_plotter->createString( 
         copyrightString );
   
   m_plotter->drawTextAtBaselineLeft( *str, m_copyrightPos );
   m_plotter->deleteString( str );
}

void 
TileMapHandler::plotLastCopyrightString()
{
   char copyrightString[ MAX_COPYRIGHT_STRING_LEN ];
   m_copyrightHandler.getCopyrightString( copyrightString, MAX_COPYRIGHT_STRING_LEN );
   plotCopyrightString( copyrightString );
}

inline void
TileMapHandler::plotWhatWeGotInBBox( const MC2BoundingBox& bbox,
                                     int skipOutlines,
                                     int lowQualityDrawing,
                                     int& nbrFeatures,
                                     int& nbrDrawn )
{
   if ( m_mapFormatDesc == NULL ) {
      return;
   }
       
   m_plotter->enableLowQualityDrawing( lowQualityDrawing );

   m_plotter->setLineWidth( 1 );
   
   m_plotter->setFillColor(255, 0, 0);
   m_plotter->setPenColor(0, 0, 0);

   // Divide the bbox by two to check the removal of features outside
   // the bbox.
   //bbox *= 0.5;
   
   // Paint what we've got.

   // To account for overflow errors when projecting polylines at high
   // zoom levels, we need to use the period to offset the projected
   // points back correctly. We also need to center point of the
   // bounding box to determine if we actually have overflowed.
   // 
   // The period is defined as the width of the "world", i.e. the
   // coordinate span projected to pixel space.
   
   int halfPeriodPixels =
      (int)((MAX_INT32 * GfxConstants::MC2SCALE_TO_METER * m_projection.getCosLat())
            / m_projection.getPixelScale());
   
   // However, since we don't need to do it in cases where no overflow
   // can occur, we see if we can make this "early rejection".
   // This can take place when the size of the period is less than that
   // of the screen resolution (when lines can "wrap around"

   isab::Rectangle r;
   m_plotter->getMapSizePixels(r);

   MC2Point centerOfBoxPx;
   
   // 1.9 instead of 2.0 to leave room for some margin due to other
   // projection errors etc.
   if(r.getWidth() > halfPeriodPixels * 2.1) {
      halfPeriodPixels = 0; // We use a period of 0 to denote that
                            // no correction should happen
   } else {
      m_operationTranslator->transform(centerOfBoxPx, bbox.getCenter());
   }
   
   MapsToDrawIt endIt = (*m_tileMapCont).endMapsToDraw();
   for( int pass = 0; pass < 2; ++pass ) {
      // The system with using passes and levels are not optimal performance wise.
      // If re-designing the the map format (all the way from server to client), then
      // this should be revised so that drawing can be done without so many loops.
      for( int level = m_mapFormatDesc->getMinLevel();
           level <= m_mapFormatDesc->getMaxLevel();
           ++level ) {

         for( MapsToDrawIt mIt = (*m_tileMapCont).beginMapsToDraw();
              mIt != endIt; ++mIt ) {
            TileMap* tileMap = *mIt;
         
            if ( tileMap == NULL ) {
               continue;
            }
            
            if (!isTileMapVisible(tileMap)){
               continue;
            }


            TileMap::primVect_t& prims =
               tileMap->getPrimitives(*m_mapFormatDesc,
                                      level);
            
            TileMap::primVect_t::iterator theEnd = prims.end();
            for ( TileMap::primVect_t::iterator it = prims.begin();
                  it != theEnd;
                  ++it ) {
               clearCoords( *(*it) );
               int curNbrDrawn = plotPrimitive(*tileMap,
                                               *(*it),
                                               bbox,
                                               centerOfBoxPx,
                                               halfPeriodPixels,
                                               pass,
                                               skipOutlines );
               if ( curNbrDrawn > 0 ) {
                  ++nbrFeatures;
               }
               nbrDrawn += curNbrDrawn;
            }
         }
      }
      mc2dbg8 << "[TMH]: Level done. Number features "
              << nbrFeatures << ", Number of points: " << nbrDrawn << endl;

   }
   
   m_plotter->enableLowQualityDrawing( false );
}

void TileMapHandler::plotWhatWeGotUsingPlotter()
{

   if ( m_mapFormatDesc == NULL ) {
      return;
   }
  
   if (!m_plotter->isVisible()) {
      mc2dbg << "[TMH] Skipping drawing since hidden window." << endl;
      return;
   }

   // if(!m_descCRCReceived) {
   //    std::cout << "Plotting even though we have not received CRC from server"
   //              << std::endl;
   // }

   int nbrDrawn = 0;
   int nbrFeatures = 0;
   
   mc2dbg8 << "[TMH]: Plotting STARTS" << endl;

   // Only redraw for outlines if we didn't do it last time.
   bool detailRepaint = m_detailRepaintTimerHasBeenRun &&
      !(m_outlinesDrawnLastTime);

   bool overlayChanged = m_overlayView->getStateChanged();
   
   m_movementHelper.updateIfToDrawOutlines( detailRepaint );

   // Test if it is possible to divide the bounding boxes into
   // smaller parts and draw several times.
   
   // Divide the bounding box into several new ones
   
   MC2BoundingBox bbox = m_projection.getDrawingBBox();

   vector<MC2BoundingBox> bboxVec;
   m_projection.getDrawingBBoxes( bboxVec );
   
   if ( (m_matrixChangedSinceDraw || detailRepaint ) ||
        m_forceRedraws || 
        m_plotter->nbrMaskedExtraBitmaps() < 1 ||
        m_mapArrivedSinceRepaint || overlayChanged)
   {
      m_matrixChangedSinceDraw = false;
      
      m_plotter->prepareDrawing();
      // Set white
      uint32 backColor = m_mapFormatDesc->getDefaultBackgroundColor();
      m_plotter->setBackgroundColor( (backColor >> 16 ) & 0xff,
                                     (backColor >> 8  ) & 0xff,
                                     (backColor >> 0  ) & 0xff );
      m_plotter->clearScreen();

      if ( m_3dOn ) { 
         // If 3d is on, draw a horizon at the top of 
         // the screen.
         isab::Rectangle screenRect;
         getMapSizeFromRenderer( screenRect );
         
         uint32 horizTopColor = m_mapFormatDesc->getHorizonTopColor();
         uint32 horizBottomColor = m_mapFormatDesc->getHorizonBottomColor();

         std::vector<uint32> colors;
         ImageBlender::blendImage( horizTopColor, horizBottomColor, m_horizonHeight, colors );

         std::vector<MC2Point> points;
         points.push_back( MC2Point( 0, 0 ) );
         points.push_back( MC2Point( screenRect.getWidth(), 0) );
         for ( uint32 i = 0; i < colors.size(); ++i ) {
            points[0].getY() = i;
            points[1].getY() = i;
            m_plotter2d->drawPolyLineWithColor( points.begin(), points.end(), colors[i], 1 );
         }
      }

      m_overlapDetector.clear();

      m_scaleIndex = 
         m_mapFormatDesc->getScaleIndexFromScale( 
            uint32(m_projection.getDPICorrectedScale()) );

      // If the detail repaint timer hasn't been run yet, then
      // skip the outlines so that moving can be done faster.
      // If tracking is on, always draw outlines.
      bool drawOutlines =
         // Means that we have already painted the map once without
         // the outlines and the user now has stopped moving the map.
         m_detailRepaintTimerHasBeenRun ||
         // When tracking is on we should always draw the outlines.
         !m_movementHelper.isMoving();
      
      m_movementHelper.updateIfToDrawOutlines( drawOutlines );
      
      // If the map should be rendered in low quality (read fast) mode.
      bool lowQualityDrawing = !drawOutlines;
    
      if ( get3dMode() && !getOutlinesIn3dEnabled() ) {
         drawOutlines = false;
      }

      const bool skipOutlines = ! drawOutlines;

      m_outlinesDrawnLastTime = drawOutlines;

      // Start the text handling if the outlines have been drawn
      if ( drawOutlines ) {
         requestTextHandlerIdle();
      }
      
      // Plot the parts
       uint32 i;   
       for ( i = 0; i < bboxVec.size(); ++i ) {
          
          plotWhatWeGotInBBox( bboxVec[i], skipOutlines,
                               lowQualityDrawing,
                               nbrFeatures, nbrDrawn );
       }
           
      // Request a new detail repaint timer so that we can decide if
      // we should draw the details the next time.
      if ( m_movementHelper.isMoving() ) {

         if ( m_detailRepaintTimerRequested ) {
            m_toolkit->cancelTimer(this, m_detailRepaintTimerRequested);
            m_detailRepaintTimerRequested = 0;
         }
         m_detailRepaintTimerHasBeenRun = false;
         mc2dbg8 << "[TMH]: Requested detail repaint timer" << endl;
         
         if ( m_movementHelper.detectMovementBySelf() ) {
            // Sigh, start a timer to check if the map has stopped moving.
            m_detailRepaintTimerRequested =
               m_toolkit->requestTimer(this,
                                       400);
         }
      }

      
#if 0
      // Plot all the bounding boxes.
      m_plotter->setLineWidth( 2 );
      m_plotter->setPenColor( 0xff00ff );
      for ( i = 0; i < bboxVec.size(); ++i ) {
         plotBBox( bboxVec[i], bbox );
      }
#endif
      
      drawProgressIndicator();
   
      // Add the map count here.
      if ( (*m_tileMapCont).getPercentageMapsReceived() >= 100 ) {     
         mc2dbg8 << "[TMH]: No missing maps - turning off the re-request timer"
                 << endl;
         if ( m_rerequestTimer != 0 ) {
            m_toolkit->cancelTimer( this, m_rerequestTimer );
            m_rerequestTimer = 0;
         }
      }
      
      drawTexts();

      /*
       * We have now finished a first painting and we will now allow draw
       * partial.
       */
      m_firstDrawDone = true;

   }
   
   m_overlayView->draw(m_plotter);

   // m_bitmapMover.reset();
   // m_plotter->makeScreenSnapshot();

   std::vector<MC2BoundingBox> boxes;
   boxes.push_back( bbox );
   
   char copyrightString[ MAX_COPYRIGHT_STRING_LEN ];
   m_copyrightHandler.getCopyrightString( boxes, copyrightString, MAX_COPYRIGHT_STRING_LEN );
   plotCopyrightString( copyrightString );

   // This can be drawn on a separate bitmap
   drawUserDefinedFeatures();
   
   {
      if ( false && m_tmkTimingInfo != NULL ) {
         vector<MC2SimpleString>
            texts;
         char tmpStr[80];
         sprintf( tmpStr, "R: %ld", (long int)m_tmkTimingInfo->m_nbrRunsLeft );
         texts.push_back( tmpStr );
         if( m_nightMode ){
            sprintf( tmpStr, "d: %ld", (long int)m_tmkTimingInfo->m_nbrRunsDone );
         }
         else{
            sprintf( tmpStr, "D: %ld", (long int)m_tmkTimingInfo->m_nbrRunsDone );
         }
         texts.push_back( tmpStr );
         sprintf( tmpStr, "Avg: %lu",
                  (long unsigned int)m_tmkTimingInfo->m_avgTimePerRun );
         texts.push_back( tmpStr );
         sprintf( tmpStr, "Tim: %lu",
                  (long unsigned int)m_tmkTimingInfo->m_elapsedTime );
         texts.push_back( tmpStr );
         sprintf( tmpStr, "L: %lu",
                  (long unsigned int)m_tmkTimingInfo->m_lastTime );
         texts.push_back( tmpStr );
         printDebugStrings( texts );
      }
   }
   
   drawDebugStrings();

   if ( m_showHighlight ) {
      isab::Rectangle tmpRect;
      getMapSizeFromRenderer( tmpRect );
      
      TileMapHandlerClickResult tmpRes;
      if ( m_highlightPoint == MC2Point( -1, -1 ) ) {
         getInfoForFeatureAt( tmpRes, PixelBox(tmpRect).getCenterPoint(),
                              true ); // Only highlightable features wanted.
      } else {
         getInfoForFeatureAt( tmpRes, m_highlightPoint,
                              true ); // Only highlightable features wanted.
      }
      UserDefinedFeature* highlight = tmpRes.getOutlineFeature();
      if ( highlight ) {
         drawOneUserDefinedFeature(*highlight);
      }
   }
   
   mc2dbg8 << "[TMH]: Finished plotting   ENDS Number features "
           << nbrFeatures << ", Number of points: " << nbrDrawn << endl;

#ifdef ENABLE_MAPPERFORMANCE_DEBUGGING
   // Draw fps.
   drawFps();
#endif
   drawTextPlacementInfo();
      
#ifdef ENABLE_INTERPOLATION_DEBUGGING
   drawInterpolationDebugInfo();
#endif

   m_plotter->endFrame();
   
   if(m_mapDrawingCallback) {
      m_mapDrawingCallback->mapIsReadyToBeDrawn();
   } else {
      m_plotter->triggerApplicationRedraw();      
   }
}

void b();

void TileMapHandler::plotWhatWeGotUsingExternal()
{   
   // m_extRenderer->beginFrame();
   // m_extRenderer->endFrame();
   // return;
   
   if(TileMapConfig::PROFILE_LOADING) {
      m_tileMapLoader->profileRun();
   }
   
   int nbrDrawn = 0;
   int nbrFeatures = 0;
   
   mc2dbg8 << "[TMH]: Plotting STARTS" << endl;

   // Only redraw for outlines if we didn't do it last time.
   bool detailRepaint = m_detailRepaintTimerHasBeenRun &&
      !(m_outlinesDrawnLastTime);
   
   m_movementHelper.updateIfToDrawOutlines( detailRepaint );

   // Test if it is possible to divide the bounding boxes into
   // smaller parts and draw several times.
   
   // Divide the bounding box into several new ones

   MC2BoundingBox bbox = m_projection.getDrawingBBox();
   
   vector<MC2BoundingBox> bboxVec;
   m_projection.getDrawingBBoxes( bboxVec );
   
   // MC2BoundingBox bbox = m_projection.getDrawingBBox();

   bool overlayChanged = m_overlayView->getStateChanged();

   // // PRINT_VAL(overlayChanged);
   // // PRINT_VAL(m_matrixChangedSinceDraw);
   // PRINT_VAL(detailRepaint);
   // // PRINT_VAL(m_mapArrivedSinceRepaint);

   bool performedDraw = false;
   
   if ( (m_matrixChangedSinceDraw || detailRepaint ) ||
        m_forceRedraws || 
        m_mapArrivedSinceRepaint || overlayChanged )
   {
      m_matrixChangedSinceDraw = false;

      performedDraw = true;
      
      // Set white
      uint32 backColor = m_mapFormatDesc->getDefaultBackgroundColor();
      m_extRenderer->setBackgroundColor( backColor );

      m_scaleIndex = 
         m_mapFormatDesc->getScaleIndexFromScale( 
            uint32(m_projection.getDPICorrectedScale()) );

      // If the detail repaint timer hasn't been run yet, then
      // skip the outlines so that moving can be done faster.
      // If tracking is on, always draw outlines.
      bool drawOutlines =
         // Means that we have already painted the map once without
         // the outlines and the user now has stopped moving the map.
         m_detailRepaintTimerHasBeenRun ||
         // When tracking is on we should always draw the outlines.
         !m_movementHelper.isMoving();
      
      m_movementHelper.updateIfToDrawOutlines( drawOutlines );     

      m_outlinesDrawnLastTime = drawOutlines;
      // PRINT_VAL(m_outlinesDrawnLastTime);

      // Start the text handling if the outlines have been drawn
      if ( drawOutlines ) {
         requestTextHandlerIdle();
      }

      MapsToDrawIt endIt = m_tileMapCont->endMapsToDraw();

      float scale = m_projection.getDPICorrectedScale();

      uint32 scaleIndex =
         m_mapFormatDesc->getScaleIndexFromScale(uint32(scale));
      
      bool indexChanged = scaleIndex != m_prevLoadingScaleIndex;

      for( MapsToDrawIt mIt = m_tileMapCont->beginMapsToDraw();
           mIt != endIt; ++mIt )
      {
         TileMap* tileMap = *mIt;
         if (tileMap != NULL) {
            if(!tileMap->geometryInitialized()) {
               tileMap->initGeometry(*m_mapFormatDesc,
                                     scaleIndex,
                                     m_projection.getPixelScale(),
                                     m_projection.getDPICorrectedScale(),
                                     m_projection.getDPICorrectionFactor(),
                                     m_projection.getCosLat());
            } else if(scale != m_prevLoadingScale) {
               tileMap->updateGeometry(scaleIndex,
                                       m_projection.getPixelScale(),
                                       m_projection.getDPICorrectedScale(),
                                       m_projection.getDPICorrectionFactor(),
                                       m_projection.getCosLat(),
                                       indexChanged);
            }
         }
      }
      
      m_prevLoadingScaleIndex = scaleIndex;
      m_prevLoadingScale = scale;
      
      // Profiler::EntryHandle secHandle = profiler.startTiming( "plotWhatWeGot" );
      // Profiler::EntryHandle begFrame = profiler.startTiming("beginFrame");
         
      m_extRenderer->beginFrame();
      m_extRenderer->clearScreen();
      // profiler.endTiming( begFrame );
      
      // Plot the parts 

      std::vector<MC2BoundingBox> bboxVec;
      
      m_projection.getDrawingBBoxes(bboxVec);
      

#ifndef DRAW_BITMAP_SAMPLE
      const bool skipOutlines = ! drawOutlines;
      uint32 i;
      for ( i = 0; i < bboxVec.size(); ++i ) {      
         m_extRenderer->drawMap( bboxVec[i],
                                 *m_tileMapCont,
                                 *m_mapFormatDesc,
                                 skipOutlines);
      }
#endif
      
      // Profiler::EntryHandle endFrame = profiler.startTiming("endFrame");

      m_extRenderer->drawTexts( m_textHandler->getTextResults() );

      m_overlayView->draw(m_extRenderer);
      
      m_extRenderer->drawHorizon(
         getMapProjection().getCamera().getHorizonHeight() );

      if ( m_showCopyright && m_mapFormatDesc != NULL ) {
         std::vector<MC2BoundingBox> boxes;
         boxes.push_back( bbox );
         char copyrightString[ MAX_COPYRIGHT_STRING_LEN ];
         m_copyrightHandler.getCopyrightString( boxes, copyrightString, MAX_COPYRIGHT_STRING_LEN );
         m_extRenderer->drawString( copyrightString, m_copyrightPos,
                                    m_mapFormatDesc->getTextColor() );
      }

#ifdef DRAW_BITMAP_SAMPLE
      m_extRenderer->drawImageTileVector(m_imgTiles->getMaps(MC2BoundingBox(),
                                                             m_projection.getPixelScale()));
#endif
      
      m_extRenderer->endFrame();
       
      // profiler.endTiming( endFrame );
      // profiler.endTiming( secHandle );

      // m_lastProfilingResults = profiler.getSummary();

      //std::cout << m_lastProfilingResults << std::endl;

      /*
       * We have now finished a first painting and we will now allow draw
       * partial.
       */
      m_firstDrawDone = true;
   }

   // Request a new detail repaint timer so that we can decide if
   // we should draw the details the next time.
   if ( m_movementHelper.isMoving() ) {

      if ( m_detailRepaintTimerRequested ) {
         m_toolkit->cancelTimer(this, m_detailRepaintTimerRequested);
         m_detailRepaintTimerRequested = 0;
      }
      m_detailRepaintTimerHasBeenRun = false;
      mc2dbg8 << "[TMH]: Requested detail repaint timer" << endl;
         
      if ( m_movementHelper.detectMovementBySelf() ) {
         // Sigh, start a timer to check if the map has stopped moving.
         m_detailRepaintTimerRequested =
            m_toolkit->requestTimer(this,
                                    400);
      }
   }
   
   // Add the map count here.
   if ( (*m_tileMapCont).getPercentageMapsReceived() >= 100 ) {     
      mc2dbg8 << "[TMH]: No missing maps - turning off the re-request timer"
              << endl;
      if ( m_rerequestTimer != 0 ) {
         m_toolkit->cancelTimer( this, m_rerequestTimer );
         m_rerequestTimer = 0;
      }
   }
   
   mc2dbg8 << "[TMH]: Finished plotting   ENDS Number features "
           << nbrFeatures << ", Number of points: " << nbrDrawn << endl;

   if(performedDraw) {
#ifdef ENABLE_MAPPERFORMANCE_DEBUGGING
// Draw fps.
      drawFps();
#endif
      drawTextPlacementInfo();

      if(m_mapDrawingCallback) {
         m_mapDrawingCallback->mapIsReadyToBeDrawn();
      } else {
         m_extRenderer->triggerApplicationRedraw();
      }
   }
   
   // static bool zoomingIn = true;

   // SmoothMapManipulator& manipulator =
   //    m_mapManipulator->getSmoothMapManipulator();

   // bool prevZoomingIn = zoomingIn;
   
   // if(m_projection.getPixelScale() < 0.5) {
   //    zoomingIn = false;
   // } else if(m_projection.getPixelScale() > 60.0) {
   //    zoomingIn = true;
   // }
   
   // if(!manipulator.isWorking() || prevZoomingIn != zoomingIn) {
   //    if(zoomingIn) {
   //       manipulator.attemptZoomIn();
   //    } else {
   //       manipulator.attemptZoomOut();
   //    }
   // }
}

void
TileMapHandler::plotWhatWeGot()
{   
   if ( m_mapFormatDesc == NULL || !m_mapDrawingEnabled) {
      return;
   }

   switch( m_drawingContext ) {
   case DrawingContextProxy::NONE:
      break;
   case DrawingContextProxy::EXTERNAL:
      plotWhatWeGotUsingExternal();
      break;
   case DrawingContextProxy::PLOTTER:
      plotWhatWeGotUsingPlotter();
      break;
   }
   
   if (m_forceRedraws){
      requestRepaint();
   }
}

void TileMapHandler::drawInterpolationDebugInfo()
{
   drawFps();


   std::vector<MC2Point> screenCoords;
   MC2Point curPoint(0,0);

   // /**
   //  *   Draw stored information
   //  */  

   // for( InterpolationDebugVector::iterator iI
   //         = m_interpolationDebugInfo.begin(); iI !=
   //         m_interpolationDebugInfo.end();
   //      iI++ )
   // {
    
   //    transformPointInternalCosLat( curPoint.getX(),
   //                                  curPoint.getY(),
   //                                  iI->coord );
      
   //    screenCoords.push_back( curPoint );
   // }

   // if( !screenCoords.empty() ) {
   //    m_plotter->drawPolyLineWithColor( screenCoords.begin(),
   //                                      screenCoords.end(), 
   //                                      0x20cc20,
   //                                      2 );
   // }

   // char num[64];
   
   // for( InterpolationDebugVector::iterator iI
   //         = m_interpolationDebugInfo.begin(); iI !=
   //         m_interpolationDebugInfo.end();
   //      iI++ )
   // {
    
   //    transformPointInternalCosLat( curPoint.getX(),
   //                                  curPoint.getY(),
   //                                  iI->coord );

   //    sprintf( num, "%d", iI->timeDiffMillis );
   //    drawDebugText( num, curPoint );
   // }
   
   /**
    *   Draw interpolation path.
    */

   std::vector<PositionInterpolator::InterpolationNode> nodes =
      m_posInterpolator->getInterpolationVector();
   
   if ( nodes.empty() ) {
      return;
   }

#ifndef __unix__
   nodes.clear(); // We still want to see them on linux, where we have no route.
#endif
   
   
   for ( vector<PositionInterpolator::InterpolationNode>::const_iterator it =
            nodes.begin(); it != nodes.end(); ++it )
   {
      
      m_projection.transformPointInternalCosLat( curPoint.getX(),
                                                 curPoint.getY(),
                                                 it->coord );
      
      screenCoords.push_back( curPoint );
   }

   if( !screenCoords.empty() ) { 
      m_plotter->drawPolyLineWithColor( screenCoords.begin(),
                                        screenCoords.end(), 
                                        0x20cc20,
                                        2 );      
   }
   
   screenCoords.clear();

   char num[64];
   MC2Point centerPoint = m_operationTranslator->getCenterPoint();
   centerPoint.getX() = 65;
      
   sprintf( num, "Dx: %.02fm", m_posInterpolator->getPositionDeltaMeters() );
   drawDebugText( num, centerPoint );
   centerPoint.getY() += 20;
   
   sprintf( num, "Cl: %.02fkmph",
            m_posInterpolator->getPrevCalcSpeedKMPH() );
   drawDebugText( num, centerPoint );
   centerPoint.getY() += 20;
   
   sprintf( num, "Cr: %.02fkmph",
            m_posInterpolator->getPrevCorrectSpeedKMPH() );
   drawDebugText( num, centerPoint );

   centerPoint.getY() += 20;

   char config = m_posInterpolator->getConfiguration() + 'A';
   
   sprintf( num, "Configuration: %c", config );

   drawDebugText( num, centerPoint );
}

void TileMapHandler::drawDebugText( const char* text,
                                    const MC2Point& position )
{         
   STRING* str = m_plotter->createString( text );
#ifdef __SYMBIAN32__
   STRING* fontName = m_plotter->createString("Swiss");
#elif UNDER_CE
   STRING* fontName = m_plotter->createString("tahoma");
#else
   STRING* fontName =
      m_plotter->createString("-*-times-*-r-*-*-12-*-*-*-*-*-*-*");
#endif
   
   m_plotter->setPenColor( 0, 0, 0 );   
         
   m_plotter->setFont( *fontName, 18 );
   m_plotter->deleteString( fontName );
   m_plotter->drawText( *str, position );
   m_plotter->deleteString( str );
}

// Inlined so that it can be auto-removed in releases.
inline void
TileMapHandler::printDebugStrings(const vector<MC2SimpleString>& strings,
                                  const MC2Point& pos )
{
   if ( strings.empty() ) {
      return;
   }
   // Black text s.v.p.
#ifdef __SYMBIAN32__
   STRING* fontName = m_plotter->createString("Swiss");
#else
   STRING* fontName =
      m_plotter->createString("-*-times-*-r-*-*-12-*-*-*-*-*-*-*");
#endif
   m_plotter->setFont( *fontName, 18 );
   m_plotter->deleteString( fontName );
   
   MC2Point curPoint = pos;

   for ( int i = strings.size() - 1; i >= 0; -- i ) {
      STRING* str = m_plotter->createString(strings[i].c_str());
      m_plotter->setPenColor(255,255,255);
      MC2Point backPoint ( curPoint );
      backPoint.getX()--;
      backPoint.getY()--;
      m_plotter->drawText( *str, backPoint );
      m_plotter->setPenColor(0,0,0);      
      m_plotter->drawText( *str, curPoint );
      // Back up y
      curPoint.getY() -=
         m_plotter->getStringAsRectangle( *str,
                                          curPoint ).getHeight() + 1;
      m_plotter->deleteString(str);
   }
}

inline void
TileMapHandler::printDebugStrings(const vector<MC2SimpleString>& strings )
{
   isab::Rectangle lastScreenSize;
   getMapSizeFromRenderer( lastScreenSize );
   PixelBox pix(lastScreenSize);
   MC2Point curPoint( pix.getCenterPoint() );
   // Start at y = height-24
   curPoint.getY() = lastScreenSize.getHeight() - 24;
   printDebugStrings( strings, curPoint ); 
}

void
TileMapHandler::drawDebugStrings()
{   

#if 0 // Enable this for some debug output.
   while ( m_drawTimes.size() > 20 ) {
      m_drawTimes.pop_front();
   }
   vector<MC2SimpleString> strings;
//   for ( list<pair<char, uint32> >::const_iterator it = m_drawTimes.begin();
//         it != m_drawTimes.end();
//         ++it ) {
//      char* tmpStr = new char[1024];
//      unsigned long int thetime = it->second;
//      sprintf(tmpStr, "%c %ld ms", it->first, thetime);
//
//      strings.push_back(tmpStr);
//      delete [] tmpStr;
//   }

   // Print available memory also.
   char avail[80];
   uint32 biggestBlock;
   sprintf(avail, "Av: %lu",
           (unsigned long int)m_toolkit->availableMemory(biggestBlock));
   strings.push_back( avail );
#ifndef __unix__
 
   
   sprintf(avail, "Ac: %lu",
         (unsigned long int)User::CountAllocCells());
   strings.push_back( avail );

   int allocSize;
   User::AllocSize( allocSize );
   sprintf(avail, "As: %lu", (unsigned long int)allocSize );
   strings.push_back( avail );

   int memFree = 0;
#if !(defined NAV2_CLIENT_SERIES60_V3 || defined NAV2_CLIENT_UIQ3)
   HAL::Get(HALData::EMemoryRAMFree, memFree );

   if ( memFree < 1024*1024 ) {
      User::CompressAllHeaps();
      sprintf( avail, "User::compressAllHeaps() called!" );
      strings.push_back( avail );
   }
#endif
   
   sprintf(avail, "HAL+As: %lu", (unsigned long int)allocSize + memFree );
   strings.push_back( avail );
   
   sprintf(avail, "HAL: %lu", 
           (unsigned long int) memFree);
   strings.push_back( avail );
   sprintf(avail, "G: %lu", 
           (unsigned long int) m_garbage.size());
   strings.push_back( avail );
#endif
   printDebugStrings(strings);
#else
   m_drawTimes.clear();
#endif
}


void
TileMapHandler::drawTexts()
{
   if ( m_3dOn ) {
      return;
   }

   /* 
    * the code below is purly for debug. draws al the overlaytest boxes 
    * so that we can se that they are correct.
    */

         
#if 0
   vector<PixelBox> boxes;
   m_textHandler->getOverlapVector( boxes );
   m_plotter->setLineWidth( 1 );  
   for( int i = 0;
         i < (int)boxes.size();
         i++ ) {
      const PixelBox pb = boxes[ i ];
      const Rectangle rect( 
            pb.getMinLon(), pb.getMinLat(), 
            pb.getWidth(), pb.getHeight() );
      m_plotter->setPenColor( 0, 255, 0 );
      m_plotter->drawRect( false, rect, 1 );
   }
#endif

   drawTextUsingPlotter();
}

void
TileMapHandler::drawTextUsingPlotter()
{
   for( int i = 0;
        i < (int)m_textHandler->getTextResults().size();
        i++ ) {
      const TextResult* tr = m_textHandler->getTextResults()[i];
      const isab::Rectangle rect = tr->getStringAsRect();
      if ( tr->getTypeOfString() == TileMapNameSettings::on_roundrect ) {
         m_plotter->setLineWidth( 1 );
         m_plotter->setFillColor( 31, 31, 255 );
         m_plotter->setPenColor( 31, 31, 255 );
         m_plotter->drawRect( true, rect, 2 );
         m_plotter->setPenColor( 255, 255, 255 );
         m_plotter->drawRect( false, rect, 2 );
      }
      mc2dbg2 << "In TileMapHandler::drawTexts()" << endl;

      m_plotter->setFont( tr->getFontName(), tr->getFontSize() );
      
      unsigned int r;
      unsigned int g;
      unsigned int b;
      tr->getFontColor( r, g, b );
      m_plotter->setPenColor( r, g, b );
      for( int j = 0; j < (int)tr->getTPN().size(); j++ ) {
         TextPlacementNotice tpn = tr->getTPN()[j];
    
         m_plotter->drawText( tr->getString(), tpn.m_point,
                              tpn.m_startIndex, 
                              tpn.m_endIndex - tpn.m_startIndex, 
                              tpn.m_angle );
      }
   }
}
inline void
TileMapHandler::drawOneUserDefinedFeature( UserDefinedFeature& userFeat )
{
   // Note that the screen coordinates are updated inside the switch
   // statement.
   
   // Not drawn until proven guilty.
   userFeat.setDrawn( false );
  
   MapPlotter* plotter;
   if ( userFeat.getAlways2d() ) {
      // If the feature always should be plottered in 2d, use
      // the 2d plotter.
      plotter = m_plotter2d;
   } else {
      // If feature should be in 3d when map is in 3d, use the
      // activated plotter, if in 3d mode the 3d plotter will be used
      // otherwise the 2d plotter will be used.
      plotter = m_plotter;
   }

   switch ( userFeat.getType() ) {
      // Filled or unfilled polygon.
      case UserDefinedFeature::directed_poly: {
         if ( ! userFeat.isVisible() ) {
            return;
         }
         // Update the screen coordinates.
         bool mode3d = m_3dOn;
         m_3dOn = false;
         // Najs.
         userFeat.updateScreenCoords(*m_operationHandler);
         m_3dOn = mode3d;
         DirectedPolygon* poly = static_cast<DirectedPolygon*>(&userFeat);
         plotter->setLineWidth( poly->getPenSize() );
         userFeat.setDrawn( true );
         if ( poly->isFilled() ) {
            plotter->setFillColor( poly->getColor() );
            plotter->drawPolygon( poly->getScreenCoords().begin(),
                                  poly->getScreenCoords().end() );
         } else {
            plotter->setPenColor( poly->getColor() );
            plotter->drawPolyLine( poly->getScreenCoords().begin(),
                                       poly->getScreenCoords().end() );
         }
      }
      break;
      // BitMap
      case UserDefinedFeature::bitmap: {
         // Update the user defined features before checking the BB
         userFeat.updateScreenCoords(*m_operationHandler);
         if ( ! m_projection.getBoundingBox().contains( 
                 userFeat.getCenter().getWorldCoord() ) ) {
            return;
         }
         UserDefinedBitMapFeature* bmp =
            static_cast<UserDefinedBitMapFeature*>(&userFeat);
         if ( bmp->getBitMapName() != NULL ) {
            // Load the bitmap even if the feature is not visible.
            isab::BitMap* bitMap =
               getOrRequestBitMap( bmp->getBitMapName() );
            if ( ! userFeat.isVisible() ) {
               return;
            }
            if ( bitMap != NULL ) {
               // Update the screen coordinates.
               userFeat.updateScreenCoords(*m_operationHandler);
               userFeat.setDrawn( true );
               /* plotter->drawBitMap( bmp->getScreenCoords().front(),
                  bitMap ); */
               
            }
         }
      }
      break;

      case UserDefinedFeature::scale: {
         if(!m_3dOn) {
            // Update the screen coordinates.
            userFeat.updateScreenCoords(*m_operationHandler);
            UserDefinedScaleFeature* scaleFeat =
               static_cast<UserDefinedScaleFeature*>(&userFeat);
            scaleFeat->draw( m_projection, *plotter, 
                             m_mapFormatDesc->getTextColor() );
            userFeat.setDrawn( true );
         }
      }
      break;
       
      default:
         // Do you want to know what it is? No, I'm fine.
         break;
   }
}

void
TileMapHandler::drawUserDefinedFeatures()
{
#ifdef USE_TRACE
   TRACE_FUNC();
#endif
   if ( m_userDefinedFeatures == NULL ) {
      return;
   }
   if ( m_plotter->nbrMaskedExtraBitmaps() >= 1 ) {
      for ( int i = 0; i < 2; ++i ) {
         // Switch to extra bitmap, then to extra bitmap mask
         if ( m_plotter->switchBitmap( 1, i ) ) {
            // Success. Clear the screen.
            m_plotter->clearScreen();
         }
         for ( vector<UserDefinedFeature*>::iterator it =
                  m_userDefinedFeatures->begin();
               it != m_userDefinedFeatures->end();
               ++it ) {
            if ( *it != NULL ) {
               drawOneUserDefinedFeature( **it );
            }
         }
      }

      m_plotter->switchBitmap( 0, false );
   } else {
      // Must draw on the only bitmap
      for ( vector<UserDefinedFeature*>::iterator it =
               m_userDefinedFeatures->begin();
            it != m_userDefinedFeatures->end();
            ++it ) {
         if ( *it != NULL ) {
            drawOneUserDefinedFeature( **it );
         }
      }
   }
}

void
TileMapHandler::updateParams()
{
   mc2dbg8 << "[TMH]: Updateparams 1" << endl;
   // If no mapformatdesc then request it.
   if ( m_mapFormatDesc == NULL ) {
      requestMaps( 1 );
      return;
   }

   if( m_drawingContext == DrawingContextProxy::NONE ) {
      //this could happen if we get a setCenter before the
      //GLDrawer is initialized
      return;
   }
   
   if ( (*m_tileMapCont).updateParams( *m_mapFormatDesc,
                                    m_projection, m_useGzip,
                                    m_lang, m_layersToDisplay, 
                                    m_routeID) ) {
      
      // Cancel the re-request timer so that the time will start
      // anew.
      if ( m_rerequestTimer != 0 ) {
         m_toolkit->cancelTimer( this, m_rerequestTimer );
         m_rerequestTimer = 0;
      }

      m_requester->cancelAll();
      if( m_drawingContext != DrawingContextProxy::NONE ) {
         m_textHandler->mapVectorChanged( (*m_tileMapCont).beginMapsToDraw(),
                                          (*m_tileMapCont).endMapsToDraw(),
                                          !m_movementHelper.isMoving() );
         // m_textHandler->mapProjectionChanged( (*m_tileMapCont).beginMapsToDraw(),
         //                                      (*m_tileMapCont).endMapsToDraw(),
         //                                      !m_movementHelper.isMoving() );
      }
   }
}

const vector<const WFAPI::POICategory*>*
TileMapHandler::getCategories() const
{
   if ( m_mapFormatDesc == NULL ) {
      return NULL;
   } else {
      return & ( m_mapFormatDesc->getCategories() );
   }
}

bool
TileMapHandler::setCategoryVisible( int id, bool visible )
{
   if ( m_mapFormatDesc == NULL ) {
      return false;
   } else {
      bool retVal = m_mapFormatDesc->setCategoryVisible(id, visible);
      if ( retVal ) {
         // Update the list of disabled item types.
         m_disabledParentTypes.clear(); 
         m_mapFormatDesc->getFeaturesDisabledByCategories(
            m_disabledParentTypes);
      }
      return retVal;
   }
}

bool
TileMapHandler::updateLayerInfo( TileMapLayerInfoVector& info )
{
   if ( m_mapFormatDesc == NULL ) {
      // No change
      return false;
   }
   const bool layers_changed = m_mapFormatDesc->updateLayers( info );
   const bool visibility_changed =
      m_mapFormatDesc->updateLayersToDisplay( m_layersToDisplay, m_acpModeEnabled );
   if ( visibility_changed ) {
      updateParams();
      requestRepaint();
   }
   return layers_changed;
}
   
void 
TileMapHandler::setProgressIndicatorBox( const PixelBox& box )
{
   m_progressIndicatorBox = box;
}
   
DBufRequestListener* 
TileMapHandler::getDBufRequestListener() const
{
   return m_dataBufferListener;
}


void
TileMapHandler::setBBoxFromParamStr( const MC2SimpleString& paramString ) {
   if ( m_mapFormatDesc == NULL ) {
      return;
   }

   TileMapParams param( paramString );
   mc2dbg8 << param << endl;
   MC2BoundingBox bbox;
   m_mapFormatDesc->getBBoxFromTileIndex( param.getLayer(), bbox,
                                          param.getDetailLevel(),
                                          param.getTileIndexLat(),
                                          param.getTileIndexLon() );
   // Tilemaps can be off the world
   if ( bbox.getMaxLat() > MAX_INT32/2 ) {
      bbox.setMaxLat( MAX_INT32/2 );
   } 
   if ( bbox.getMinLat() > MAX_INT32/2 ) {
      bbox.setMinLat( MAX_INT32/2 );
   } 
   if ( bbox.getMinLat() < -MAX_INT32/2 ) {
      bbox.setMinLat( -MAX_INT32/2 );
   }
   if ( bbox.getMaxLat() < -MAX_INT32/2 ) {
      bbox.setMaxLat( -MAX_INT32/2 );
   }
   bbox.updateCosLat();
   m_projection.setBoundingBox( bbox );
//   mc2dbg8 << bbox << endl;

   if ( param.getRouteID() != NULL ) {
      mc2dbg8 << " RouteID: X_X";
      setRouteID( *param.getRouteID() );
   }

   mc2dbg8 << endl;
}

   
bool 
TileMapHandler::getNextHighlightPoint( MC2Point& point ) 
{
   const TilePrimitiveFeature* highlightFeature = 
      (*m_tileMapCont).getNextHighlightFeature();
   if ( highlightFeature != NULL ) {
      // Got something to highlight!

      // We "know" that it must be a bitmap.
      MC2_ASSERT( highlightFeature->getType() == TileFeature::bitmap );

      // Get the coordinate.
      const CoordArg* coordArg =
         static_cast<const CoordArg*>
         (highlightFeature->getArg(TileArgNames::coord));
      MC2Coordinate coord;
      coord.lat = coordArg->getCoord().getLat();
      coord.lon = coordArg->getCoord().getLon();

      // And transform it to screen coordinates.
      m_projection.transformPointInternalCosLat( point, coord );

      // Move it to the middle of the poi.
      const StringArg* bitMapName =
         static_cast<const StringArg*>
         (highlightFeature->getArg(TileArgNames::image_name));

      MC2_ASSERT( bitMapName != NULL );
     
      bitMapMap_t::const_iterator it = 
         m_bitMaps.find( bitMapName->getValue() );
      
      if ( it == m_bitMaps.end() ) {
         // Should not happen, but if it does,
         // the cursor will not be centered.
         return true;
      }

      // Fetch bitmap size from the plotter.
      PixelBox box;// = m_plotter->getBitMapAsRectangle( point, it->second );
      // And then move to the center of that.
      point = box.getCenterPoint();      
      
      return true;
   } else {
      // Nothing to highlight.
      return false;
   }
}

void
TileMapHandler::setMoving( bool moving ) 
{
   m_movementHelper.setMoving( moving );
   if ( ! moving ) {
      // Stopped moving, then time to draw the outlines, ey.
      requestRepaint();
   }
}

void 
TileMapHandler::setDetectMovementBySelf( bool detectSelf )
{
   m_movementHelper.setDetectMovementBySelf( detectSelf );
}

void 
TileMapHandler::setCopyrightPos( const MC2Point& pos )
{
   m_copyrightPos = pos;
}

void 
TileMapHandler::showCopyright( bool show )
{
   m_showCopyright = show;
}

bool 
TileMapHandler::canHandleScreenAsBitmap() const
{
   // if( m_drawingContext == DrawingContextProxy::PLOTTER ) {
   //    return m_plotter->snapshotHandlingImplemented();
   // } else if( m_drawingContext == DrawingContextProxy::EXTERNAL ) {
   //    return false;
   // }

   // Even though this is implemented on some platforms,
   // we don't want it to be used at the moment, mainly since it is a slowdown
   // for the winMobilePlotter.
   
   return false;
}

void 
TileMapHandler::moveBitmap(int deltaX, int deltaY)
{
   // m_bitmapMover.move( MC2Point( deltaX, deltaY ) );
   // m_plotter->prepareDrawing();
   // m_plotter->moveScreenSnapshot( m_bitmapMover.getMoveOffset() );
   
   // // Draw the rest of map.
   // plotLastCopyrightString(); 
   // drawUserDefinedFeatures();

   // m_plotter->triggerApplicationRedraw();
}

void 
TileMapHandler::setPointBitmap( const MC2Point& screenPoint )
{
   // m_bitmapMover.moveTo( screenPoint );
   // m_plotter->prepareDrawing();
   // m_plotter->moveScreenSnapshot( m_bitmapMover.getMoveOffset() );
   
   // // Draw the rest of map.
   // plotLastCopyrightString(); 
   // drawUserDefinedFeatures();

   // m_plotter->triggerApplicationRedraw();
}

void
TileMapHandler::setBitmapDragPoint( const MC2Point& dragPoint )
{
   // m_bitmapMover.setDragPoint( dragPoint );
}

void 
TileMapHandler::zoomBitmapAtPoint( double factor, 
                                   const MC2Point& screenPoint )
{
   // m_bitmapMover.zoom( factor ); 
   // m_plotter->prepareDrawing();
   // m_plotter->zoomScreenSnapshot( m_bitmapMover.getZoomFactor(), 
   //                                screenPoint );
   
   // // Draw the rest of map.
   // plotLastCopyrightString(); 
   // drawUserDefinedFeatures();

   // m_plotter->triggerApplicationRedraw();

}

void TileMapHandler::getMapSizeFromRenderer( isab::Rectangle& rect ) const
{
   if( m_drawingContext == DrawingContextProxy::PLOTTER ) {
      m_plotter->getMapSizePixels( rect );   
   } else if( m_drawingContext == DrawingContextProxy::EXTERNAL ) {
      m_extRenderer->getMapSizePixels( rect );
   }
}

void 
TileMapHandler::zoomBitmapAtCenter( double factor )
{
   // isab::Rectangle rect;
   // getMapSizeFromRenderer( rect );

   // zoomBitmapAtPoint( factor, MC2Point( rect.getWidth() / 2, 
   //                                      rect.getHeight() / 2 ) );
}

void 
TileMapHandler::setNightModeL( bool aOn )
{
   if( aOn != m_nightMode ){
      m_nightMode = aOn;

      requestCachedTMFD();

      m_descCRCReceived = false;
      requestMaps( 1 );      
   }
   if(m_drawingContext == DrawingContextProxy::EXTERNAL){
      m_extRenderer->setNightMode(aOn);
   }
}

bool
TileMapHandler::getNightMode()
{
   return m_nightMode;
}


void 
TileMapHandler::set3dMode( bool on )
{

   m_projection.set3dMode( on );
   
   if ( on ) {
      m_plotter = m_plotter3d;
   } else {
      m_plotter = m_plotter2d;
   }
   
   if ( m_3dOn != on ) {
      m_3dOn = on;
      forceRedrawOnNextRepaint();
      requestRepaint();
   }

}

bool 
TileMapHandler::get3dMode( )
{
   return m_3dOn;
}

void
TileMapHandler::setOutlinesIn3dEnabled( bool enabled )
{
   m_outlinesIn3dEnabled = enabled;
}

bool
TileMapHandler::getOutlinesIn3dEnabled( ) const
{
   return m_outlinesIn3dEnabled;
}

void
TileMapHandler::setHorizonHeight( uint32 height )
{
   switch( m_drawingContext ) {
   case DrawingContextProxy::NONE:
      break;
   case DrawingContextProxy::EXTERNAL:
      break;
   case DrawingContextProxy::PLOTTER: {
      m_plotter3d->setHorizonHeight( height );
  
      bool changed = m_horizonHeight != height;
      m_horizonHeight = height;
      if ( changed && get3dMode() ) {
         forceRedrawOnNextRepaint();
         requestRepaint();
      }
   }
      break;
   }
}

// Remove this method once the 3d parameters are tweaked.
Settings3D&
TileMapHandler::getSettings3D()
{
   return m_plotter3d->m_settings;
}

void TileMapHandler::cycleTextPlacementAlgorithm()
{
   // m_textHandler->cycleAlgorithm();
   repaintNow();
}

void
TileMapHandler::drawTextPlacementInfo() {
#ifdef DRAW_TPL_INFO
   if( m_drawingContext == DrawingContextProxy::EXTERNAL ) {

   } else if(m_drawingContext == DrawingContextProxy::PLOTTER) {
      if (!m_plotter) {
         return;
      }

      m_plotter->setPenColor( 0, 0, 0 );
      
      char tmpStr[255];

      sprintf(tmpStr, "* TPL-algorithm: %s", m_textHandler->getAlgorithmName());
      STRING* str = m_plotter->createString( tmpStr );
      m_plotter->drawTextAtBaselineLeft( *str, MC2Point(0,80) );
      m_plotter->deleteString( str );
      sprintf(tmpStr, "* Processing time: %u ms", m_textHandler->getProcessingTimeMs());
      str = m_plotter->createString( tmpStr );
      m_plotter->drawTextAtBaselineLeft( *str, MC2Point(0,95) );
      m_plotter->deleteString( str );
      
      // sprintf(tmpStr, "TPL-algorithm: %s", m_textHandler->getAlgorithmName());
      // str = m_plotter->createString( tmpStr );
      // m_plotter->drawTextAtBaselineLeft( *str, MC2Point(60,70) );
   }
#endif
}
   
void
TileMapHandler::drawFps()
{
   uint32 currTimeStamp = TileMapUtil::currentTimeMillis();
   uint32 millisSinceDraw = currTimeStamp - m_lastDrawTimeStamp;
   m_lastDrawTimeStamp = currTimeStamp;
   double fps = 0;
   if ( millisSinceDraw != 0 ) {
      fps = 1000.0 / millisSinceDraw;
   }

   char tmpStr[40];
   sprintf( tmpStr, "fps: %.2f, s: %.2f", fps,
            m_operationTranslator->getScale()  );
   
   if( m_drawingContext == DrawingContextProxy::EXTERNAL ) {
      // it's kind of bad to use drawString to print the fps, since it will change
      // often. This means it will need to render the string on each run,
      // effectively decreasing the fps by doing so.
      m_extRenderer->drawString( tmpStr, m_copyrightPos,
                                 m_mapFormatDesc->getTextColor() );
   } else if(m_drawingContext == DrawingContextProxy::PLOTTER) {
      if (m_plotter) {
#ifdef __SYMBIAN32__
         STRING* fontName = m_plotter->createString("Swiss");
#elif UNDER_CE
         STRING* fontName = m_plotter->createString("tahoma");
#else
         STRING* fontName =
            m_plotter->createString("-*-times-*-r-*-*-12-*-*-*-*-*-*-*");
#endif

         STRING* str = m_plotter->createString( tmpStr );
         m_plotter->setFont( *fontName, 18 );
         m_plotter->deleteString( fontName );
         m_plotter->setPenColor( 0, 0, 0 );
         m_plotter->drawTextAtBaselineLeft( *str, MC2Point(60,60) );
         m_plotter->deleteString( str );
      }
   }
}

void 
TileMapHandler::updateCopyrightHandler( const TileMapFormatDesc& tmfd ) 
{
   if ( tmfd.getCopyrightHolder() != NULL ) {
      // Set the CopyrightHolder to the CopyrightHandler.
      m_copyrightHandler.setCopyrightHolder( tmfd.getCopyrightHolder() );
   } else {
      // Set the old version of copyright data from TMFD.
      m_copyrightHandler.setStaticCopyrightString( tmfd.getStaticCopyrightString() );
   }
}

bool
TileMapHandler::isInitialized() const
{
   return m_mapFormatDesc != NULL;
}

bool 
TileMapHandler::isACPModeEnabled() const
{
   return m_acpModeEnabled;
}

void 
TileMapHandler::setACPModeEnabled( bool enable )
{
   m_acpModeEnabled = enable;
   if ( m_mapFormatDesc ) {
      // A race can occure here when having no internet access point, entering the map and 
      // imediately goes back and enters settings. This function will be called and the
      // m_mapFormatDesc has not yet been read from the cache and therefore is NULL.
      const bool visibility_changed =
         m_mapFormatDesc->updateLayersToDisplay( m_layersToDisplay, m_acpModeEnabled );
      if ( visibility_changed ) {
         updateParams();
         requestRepaint();
      }
   }
}

MapProjection& TileMapHandler::getMapProjection()
{
   return m_projection;
}

PositionInterpolator* TileMapHandler::getPositionInterpolator()
{
   return m_posInterpolator;
}

void TileMapHandler::setInterpolationCallback( InterpolationCallback* callback )
{
   m_interpolationCallback = callback; 
}

void TileMapHandler::clearInterpolationCallback( InterpolationCallback* callback )
{
   MC2_ASSERT( callback == m_interpolationCallback );
   m_interpolationCallback = NULL;
}


bool TileMapHandler::getInterpolatingPositions() const
{
   return m_interpolatingPositions;
}

void TileMapHandler::setInterpolatingPositions( bool enabled )
{
   m_interpolatingPositions = enabled;
}

void TileMapHandler::handleIncomingBitMap( BitBuffer* dataBuffer,
                                           const MC2SimpleString& descr ) {
   isab::BitMap* bmp = NULL;
   // Remove the 'B'
   MC2SimpleString bmpName( bitMapDescToBitMapName( descr ) );

   if( m_drawingContext == DrawingContextProxy::EXTERNAL ) {
      m_imgLoader->setParameters( dataBuffer->getBufferAddress(),
                                  dataBuffer->getBufferSize() );
            
      TextureBlockManager* texMgr =
         m_extRenderer->getTextureBlockManager();

      RGBA32BitMap bitMap;
         
      m_imgLoader->load( bitMap );

      TextureBlock* tex = texMgr->allocateFromBitMap( bitMap );
      bmp = tex;

      if ( m_bitMaps.find( bmpName ) != m_bitMaps.end() ) {
         delete m_bitMaps[bmpName];
         m_bitMaps[bmpName] = NULL;
      }
   } else if( m_drawingContext == DrawingContextProxy::PLOTTER ) {
      bmp = m_plotter->createBitMap( isab::bitMapType(0),
                                     dataBuffer->getBufferAddress(),
                                     dataBuffer->getBufferSize(),
                                     m_projection.getDPICorrectionFactor() );

      if( bmp ) {
         // Delete old bitmap. (Default should be NULL here, I hope).
         m_plotter->deleteBitMap(m_bitMaps[bmpName]);
      }
   }
   
   if ( bmp != NULL ) {
      // Store the bitmap
      delete m_bitMaps[bmpName];
      m_bitMaps[ bmpName ] = bmp;
      m_neededBitMaps.erase( bmpName );
   }
}

void TileMapHandler::clearLoadedBitMaps()
{
   if( m_drawingContext == DrawingContextProxy::NONE ) {
      return;
   }

   bitMapMap_t::iterator it = m_bitMaps.begin();
   bitMapMap_t::iterator end = m_bitMaps.end();
   
   while( it != end ){
      
      if( m_drawingContext == DrawingContextProxy::PLOTTER ) {
         m_plotter->deleteBitMap( it->second );
      } else if ( m_drawingContext == DrawingContextProxy::EXTERNAL ) {
         delete it->second;
      }
      
      it++;
   }

   m_textHandler->clear();
   // m_textHandler->resetState();
   m_requester->cancelAll();
   m_bitMaps.clear();
   m_neededBitMaps.clear();
   
   {
      if ( m_rerequestTimer != 0 ) {
         m_toolkit->cancelTimer( this, m_rerequestTimer );
         m_rerequestTimer = 0;
      }

      m_requester->cancelAll();
      // m_textHandler->mapProjectionChanged( (*m_tileMapCont).beginMapsToDraw(),
      //                                      (*m_tileMapCont).endMapsToDraw(),
      //                                      !m_movementHelper.isMoving() );
      m_textHandler->mapVectorChanged( (*m_tileMapCont).beginMapsToDraw(),
                                       (*m_tileMapCont).endMapsToDraw(),
                                       !m_movementHelper.isMoving() );
   }
   
   updateParams();
}

void TileMapHandler::setDrawingContext( WFAPI::DrawingContext* context )
{
   clearLoadedBitMaps();


   if(context == NULL) {
      getOverlayView()->getLayerInterface().clearCachingBlocksForLayersItems();
      m_drawingContext = DrawingContextProxy::NONE;
      m_textHandler->setViewPortInterface(NULL);
      m_textHandler->setTextInterface(NULL);
      m_stringCache->setTextInterface(NULL);
      return;
   }
   
   DrawingContextProxy* proxy = static_cast<DrawingContextProxy*>( context );

   if( proxy->type == DrawingContextProxy::EXTERNAL ) {
      ExternalContextConcrete* extCont =
         static_cast<ExternalContextConcrete*>( context );

      m_drawingContext = DrawingContextProxy::EXTERNAL;
      m_projection.setUsingFOVBoundingBox( true );
      
      m_extRenderer = extCont->renderer;
      m_extRenderer->setCamera(&m_projection.getCamera());
      m_extRenderer->setProjection(&m_projection);
      m_extRenderer->setMapHandler( this );
      m_textHandler->setViewPortInterface( extCont->renderer );

      m_textHandler->setTextInterface( extCont->textInterface );
      m_stringCache->setTextInterface( extCont->textInterface );
      m_imgLoader = extCont->imgLoader;
      
#ifdef DRAW_BITMAP_SAMPLE
      initGrid();
#endif
   } else if( proxy->type == DrawingContextProxy::PLOTTER ) {
      m_projection.setUsingFOVBoundingBox( false );
      
      PlotterContextConcrete* plotCont =
         static_cast<PlotterContextConcrete*>( context );
      m_drawingContext = DrawingContextProxy::PLOTTER;
      m_plotter = plotCont->plotter;
      
      m_plotter2d = plotCont->plotter;
      if (m_plotter3d != NULL){
         delete m_plotter3d;
      }
      m_plotter3d = new PlotterAdapter3d( plotCont->plotter );

      m_textHandler->setViewPortInterface( plotCont->plotter );
      m_textHandler->setTextInterface( plotCont->plotter );
      m_stringCache->setTextInterface( plotCont->plotter );
      set3dMode( get3dMode() );
   }

   // Get screensize
   getMapSizeFromRenderer( m_lastScreenSize );

   m_projection.setScreenSize( MC2Point(m_lastScreenSize.getWidth(),
                                        m_lastScreenSize.getHeight() ) );

}

OverlayView* TileMapHandler::getOverlayView()
{
   return m_overlayView; 
}

SelectedMapObjectNotifier* TileMapHandler::getSelectedMapObjectNotifier()
{
   return m_selectedMapObjectNotifier;
}

MapLibSettingsNotifier* TileMapHandler::getMapLibSettingsNotifier()
{
   return m_settingsNotifier;
}

void TileMapHandler::enableAutomaticHighlight(bool enable)
{
   m_overlayView->enableAutomaticHighlight(enable); 
}

MapManipulator& TileMapHandler::getMapManipulator()
{
   return *m_mapManipulator;
}

WFAPI::MapOperationInterface* TileMapHandler::getMapOperationInterface()
{
   return m_operationHandler; 
}

void TileMapHandler::repaintOverlayDialogs()
{

   /*
    * If a first draw has not been done we can not do a partial draw.
    */ 
   if(!m_firstDrawDone) {
      requestRepaint();
      return;
   }
   if( m_drawingContext == DrawingContextProxy::EXTERNAL ) {
      m_overlayView->drawPartialDialog(m_extRenderer);
      m_extRenderer->endFrame();
   } else if( m_drawingContext == DrawingContextProxy::PLOTTER ) {
      m_overlayView->drawPartialDialog(m_plotter);
      m_plotter->triggerApplicationRedraw();
   }
}

void 
TileMapHandler::setStartupListener(WFAPI::MapLibStartupListener* startupListener)
{
   m_startupListener = startupListener;
   if (m_mapFormatDesc != NULL) {
      // Already started.
      m_startupListener->handleMapLibStartupComplete(
            WFAPI::MapLibStartupListener::SUCCESS);
   }
}

DrawingContextProxy::Type TileMapHandler::getDrawingContext()
{
   return m_drawingContext; 
}

MapProjectionAdapter& TileMapHandler::getMapProjectionAdapter()
{
   return *m_operationTranslator; 
}

bool TileMapHandler::forcedRedrawsEnabled() const
{
   return m_forceRedraws; 
}

void TileMapHandler::setForcedRedraws(bool enabled)
{
   m_forceRedraws = enabled;
}

void TileMapHandler::setFormatDesc(TileMapFormatDesc* desc)
{
   // Delete the format description now,
   // there may not be references.
   bool mapLibStarted = false;
   if (m_mapFormatDesc == NULL) {
      mapLibStarted = true;
   }
   delete m_mapFormatDesc;
   
   m_mapFormatDesc = desc;    

   coreprintln_info("TileMapFormatDesc now downloaded");
   
   // Allow for the format description to notify POI category
   // changes.
   m_mapFormatDesc->setNotifier(m_settingsNotifier);
   m_mapFormatDesc->updateLayersToDisplay( m_layersToDisplay, m_acpModeEnabled );
   // Real repaint needed
   m_matrixChangedSinceDraw = true;
   (*m_tileMapCont).setMapDesc( m_mapFormatDesc );

   // Update the copyright handler with data from tmfd.
   updateCopyrightHandler( *m_mapFormatDesc );

   m_tileMapLoader->setFormatDesc(m_mapFormatDesc);

   m_textHandler->setTextColor( m_mapFormatDesc->getTextColor() );
   
   m_textHandler->getTextSettings().setTextColor(
      m_mapFormatDesc->getTextColor());
   
   sendEvent ( TileMapEvent(
                  TileMapEvent::NEW_CATEGORIES_AVAILABLE ) );
            
   // First time the tmfd is set.
   // Notify listeners that maplib is started.
   if (mapLibStarted && m_startupListener) {
      coreprintln_info("MapLib startup complete, issuing notification");
      m_startupListener->handleMapLibStartupComplete( 
        WFAPI::MapLibStartupListener::SUCCESS);
   }
}

ExternalRenderer* TileMapHandler::getExternalRenderer()
{
   return m_extRenderer; 
}

StressTester& TileMapHandler::getStressTester()
{
   return *m_stressTester; 
}

bool TileMapHandler::mapFullyLoaded() const
{
   if ( m_tileMapCont->getPercentageMapsReceived() == 100 && 
        m_mapFormatDesc )
   {
      return true;
   }
   return false; 
}

void TileMapHandler::repaintNow()
{
   CORE_FUNCTION_PROLOGUE();
   repaint(0);
}

void TileMapHandler::setMapDrawingCallback(WFAPI::MapDrawingCallback* callback)
{
   m_mapDrawingCallback = callback; 
}

void TileMapHandler::drawImageSpec(const WFAPI::ImageSpec* spec,
                                   const WFAPI::ScreenPoint& point)
{
   if( m_drawingContext == DrawingContextProxy::EXTERNAL ) {
      m_extRenderer->drawImageSpec(spec, point);
   } else if( m_drawingContext == DrawingContextProxy::PLOTTER ) {
      m_plotter->drawImageSpec(spec, point);
   }   
}

void TileMapHandler::setVariable3dMode(float step)
{
   m_projection.setVariable3dMode(step);
}

bool TileMapHandler::isTileMapVisible(const TileMap* map) const
{
   if ( m_hidePOILayers &&
        (map->getLayer() == TileMapTypes::c_poiLayer ||
         map->getLayer() == TileMapTypes::c_acpLayer)) {
      // Skip this tilemap, since it contains POIs that are currently hidden.
      return false;
   } else {
      return true;
   }
}

void TileMapHandler::hidePOIs(bool hidden)
{
   m_hidePOILayers = hidden; 
}


void TileMapHandler::dumpGeometry(const char* fileName) {
#ifdef ALGOTRIM_TILEMAPDUMP
   printf("Dumping geometry\n");
   
   FILE* fPoly = fopen("/tmp/poly_dump.txt", "w");
   FILE* fLine = fopen("/tmp/line_dump.txt", "w");

   int lineCount = 0;
   int polyCount = 0;
   
   fprintf(fPoly, "COSLAT: %f\n", m_projection.getCosLat());
   fprintf(fLine, "COSLAT: %f\n", m_projection.getCosLat());
   
   if(!fPoly || !fLine) {
      printf("Failed to open files for dumping\n");
      return;
   }
   
   for( int level = m_mapFormatDesc->getMinLevel();
        level <= m_mapFormatDesc->getMaxLevel();
        ++level ) {

      MapsToDrawIt endIt = m_tileMapCont->endMapsToDraw();
      
      for( MapsToDrawIt mIt = (*m_tileMapCont).beginMapsToDraw();
           mIt != endIt; ++mIt )
      {
         TileMap* tileMap = *mIt;
         
         if ( tileMap == NULL ) {
            continue;
         }

         TileMapCoord refCoord = tileMap->getReferenceCoord();
         
         TileMap::primVect_t& prims =
            tileMap->getPrimitives(*m_mapFormatDesc,
                                   level);
            
         TileMap::primVect_t::iterator theEnd = prims.end();
         for ( TileMap::primVect_t::iterator it = prims.begin();
               it != theEnd;
               ++it )
         {
            FILE* fCur = NULL;
            TilePrimitiveFeature& curPrim = *it;

            bool isLine = false;
            
            switch(curPrim.getType()) {
            case TilePrimitiveFeature::line:
               isLine = true;
               fprintf(fLine, "LINE\n");
               fprintf(fLine, "%d\n", lineCount++);
               fCur = fLine;
               break;
            case TilePrimitiveFeature::polygon:
               isLine = false;
               fprintf(fPoly, "POLYGON\n");
               fprintf(fPoly, "%d\n", polyCount++);
               fCur = fPoly;
               break;
            default:
               continue;
            }

            fprintf(fCur, "REF: %d %d\n", refCoord.getLat(), refCoord.getLon());

            const CoordsArg* coords = static_cast<const CoordsArg*>(
               curPrim.getArg(TileArgNames::coords));

            if ( coords == NULL ) {
               continue;
            }
            
            CoordsArg::const_iterator the_end ( coords->end(*tileMap) );
            
            for ( CoordsArg::const_iterator it ( coords->begin(*tileMap) );
                  it != the_end; ++it  )
            {
               MC2Coordinate curCoord = *it;
               fprintf(fCur, "%d %d\n",
                       curCoord.lat - refCoord.getLat(),
                       curCoord.lon - refCoord.getLon());
            }
         }
      }
   }
   
   fclose(fPoly);
   fclose(fLine);
#endif
}

void TileMapHandler::toggleTriangulationStrategy()
{
   TileMap::s_triStrategy =
      (TileMap::s_triStrategy + 1) % TileMap::TRI_NUM_STRATEGIES;

   switch(TileMap::s_triStrategy) {
   case TileMap::TRI_WAYFINDER:
      printf("Now using Wayfinder triangulation strategy.\n");
      break;
   case TileMap::TRI_ALGOTRIM:
      printf("Now using Algotrim triangulation strategy.\n");
      break;
   }
   
   m_prevLoadingScale = 1337;
      
   requestRepaint();
}

isab::BitMap* loadPNGFromFile(const char* file,
                              TextureBlockManager* texMgr)
{
   FILE* f = fopen(file, "rb");

   if(f) {
      // printf("File \"%s\" successfully opened.\n", file);
   } else {
      printf("Failed to open \"%s\".\n", file);
      return NULL;
   }

   unsigned char tmp[255];
   std::vector<unsigned char> fileContents;

   int totalSize = 0;
   
   while(!feof(f)) {
      int numRead = fread(tmp, 1, 255, f);
      if(numRead > 0) {
         totalSize += numRead;
         fileContents.insert(fileContents.end(), tmp, tmp + numRead);
      }
   }

   PNGImageLoader pngLoader;
   pngLoader.setParameters(&fileContents[0], totalSize);
   RGBA32BitMap tmpBmp;

   int err = pngLoader.load(tmpBmp);
   
   if(err == 0) {
//      printf("PNG loading of \"%s\" successful!\n", file);
   } else {
      printf("PNG failed to load, error code = %d\n", err);
   }

   TextureBlock* texture = texMgr->allocateFromBitMap(tmpBmp);
      
   return texture;
      
   fclose(f);
   
   return NULL;
}

void TileMapHandler::initGrid()
{
   // Populate grid storage
#if defined __SYMBIAN32__ 
   const char* gridPath = "c:\\shared\\WFAPITestClient\\grid\\";
#else
   const char* gridPath = "images/grid/";
#endif   
   char tmp[255];

   ZoomRange range(4);

   range.setScaleForLevel(0, 0);
   range.setScaleForLevel(1, 13.5056f);
   range.setScaleForLevel(2, 30.3813f);
   range.setScaleForLevel(3, 75.843f);

   m_imgTiles = new GridStorage<ImageTile>(range);
   
   // Level 0
   sprintf(tmp, "%s9_0_0.png", gridPath);
   isab::BitMap* level0Img = loadPNGFromFile(tmp,
                                             m_extRenderer->getTextureBlockManager());

   MC2Coordinate lowerLeft(582564735, 26843546);
   MC2Coordinate upperRight(583668151, 28521267);

   int32 latDiff = upperRight.lat - lowerLeft.lat;
   int32 lonDiff = upperRight.lon - lowerLeft.lon;
   
   if(level0Img) {
      ImageTile* imgTile = new ImageTile(level0Img,
                                         MC2BoundingBox(lowerLeft, upperRight));
      m_imgTiles->addMap(2, imgTile);
   }
   
   // Level 1
   for(int i = 0; i < 2; i++) {
      for(int j = 0; j < 2; j++) {
         sprintf(tmp, "%s10_%d_%d.png", gridPath, i, j);
         isab::BitMap* img =
            loadPNGFromFile(tmp, m_extRenderer->getTextureBlockManager());

         MC2Coordinate curLL = lowerLeft;
         curLL.lat += j * (latDiff / 2);
         curLL.lon += i * (lonDiff / 2);

         MC2Coordinate curUR = lowerLeft;
         curUR.lat += (j+1) * (latDiff / 2);
         curUR.lon += (i+1) * (lonDiff / 2);

         // printf("Coord: (%d, %d)\n", curLL.lat, curLL.lon);
         // printf("Coord: (%d, %d)\n", curUR.lat, curUR.lon);
         
         ImageTile* imgTile = new ImageTile(img,
                                            MC2BoundingBox(curLL, curUR));
         
         m_imgTiles->addMap(1, imgTile);  
      }
   }
   
   // Level 2
   for(int i = 0; i < 4; i++) {
      for(int j = 0; j < 4; j++) {
         sprintf(tmp, "%s11_%d_%d.png", gridPath, i, j);
         isab::BitMap* img =
            loadPNGFromFile(tmp, m_extRenderer->getTextureBlockManager());

         
         MC2Coordinate curLL = lowerLeft;
         curLL.lat += j * (latDiff / 4);
         curLL.lon += i * (lonDiff / 4);

         MC2Coordinate curUR = lowerLeft;
         curUR.lat += (j+1) * (latDiff / 4);
         curUR.lon += (i+1) * (lonDiff / 4);

         // printf("Coord: (%d, %d)\n", curLL.lat, curLL.lon);
         // printf("Coord: (%d, %d)\n", curUR.lat, curUR.lon);
         
         ImageTile* imgTile = new ImageTile(img, MC2BoundingBox(curLL, curUR));
         
         m_imgTiles->addMap(0, imgTile);  
      }
   }

}
