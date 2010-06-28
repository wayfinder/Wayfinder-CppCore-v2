/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "config.h"
#include "MapLib.h"
#include "MapOperationInterface.h"
#include "MC2SimpleString.h"
#include "InternalConfig.h"
#include "TileMapHandler.h"
#include "DBufConnection.h"
#include "DraggingMapManipulator.h"
#include "FileHandler.h"
#include "MapPlotter.h"
#include "MemTracker.h"
#include "MemoryDBufRequester.h"
#include "MultiFileDBufRequester.h"
#include "OverlayView.h"
#include "NullCursor.h"
#include "PositionInterpolator.h"
#include "SmoothMapManipulator.h"
#include "SharedBuffer.h"
#include "SharedDBufRequester.h"
#include "SingleFileDBufRequester.h"
#include "TileMapEvent.h"
#include "WFDRMUtil.h"
#include <utility>
#include "MapLibInitialConfig.h"

#include "MapLibKeyInterface.h"

//TODO: Move out hardcoding of fonts per platform.

#if defined __SYMBIAN32__
   #if defined(NAV2_CLIENT_SERIES60_V3)
      #define SCALE_FONT "LatinBold12";
      #define SCALE_FONT_SIZE 12
   #else   
      #define SCALE_FONT "LatinPlain12";
      #define SCALE_FONT_SIZE 12
   #endif
#elif (defined __linux__) || defined (IPHONE)
   #define SCALE_FONT "-*-times-*-r-*-*-12-*-*-*-*-*-*-*";
   #define SCALE_FONT_SIZE 18
#elif defined _WIN32_WCE
#define SCALE_FONT "Arial";
#define SCALE_FONT_SIZE 18
#else
   #error "Must set legal font name for platform!"
#endif

class MapLibKeyHandler;
class TileMapToolkit;

// This file is a bit strange since it tries to hide so much.

using namespace std;

//    namespace {

/**
 *   Uses the functions of MapLib to be able to create a
 *   multifile dbufrequester.
 */
class MapLibMultiFileDBufRequester : public MultiFileDBufRequester {
public:
   /**
    *   Constructor.
    */
   MapLibMultiFileDBufRequester( MapLib* maplib,
                                 DBufRequester* parent,
                                 const char* basePath,
                                 MemTracker* memTracker,
                                 WFAPI::wf_uint32 maxSize,
                                 int read_only = false ) :
      MultiFileDBufRequester( parent,
                              basePath,
                              memTracker,
                              maxSize,
                              -1, -1,
                              read_only ) {
      m_mapLib = maplib;
      init();
   }

   /// Creates a file handler.
   FileHandler* createFileHandler( const char* fileName,
                                   bool createIfNotExists,
                                   bool initNow = true ) {
      bool readOnly = false; 
      return m_mapLib->createFileHandler( fileName,
                                          readOnly,
                                          createIfNotExists,
                                          initNow );
   }

   /// Returns the path separator
   const char* getPathSeparator() const {
      return m_mapLib->getPathSeparator();
   }

   /**
    *   Pointer to the MapLib that can be used to create the
    *   filehandlers.
    */      
   MapLib* m_mapLib;
      
};
   
class MapLibInternal : public SFDHeaderLoadedListener {
                 
public:
   class ExampleCacheInfo : public MapLib::CacheInfo {
   public:

      virtual ~ExampleCacheInfo() {}
         
      const char* getPathUTF8() const {
         return m_names.first.c_str();
      }

      const char* getNameUTF8() const {
         return m_names.second.c_str();
      }
      
      bool isValid() const {
         return true;
      }

      typedef pair<MC2SimpleString, MC2SimpleString> namePair_t;
      namePair_t m_names;
         
   };
      
   MapLibInternal( TileMapHandler* handler,
                   TileMapToolkit* toolkit,
                   MapPlotter* plotter = NULL )
      : m_memTracker( 1024*1024, // Mem for pool.
                      1024*1024) // Min mem free.
      {            
         m_toolkit = toolkit;
         m_internalConfig = new WFAPI::InternalConfig( handler );
         m_plotter = plotter;
         m_memTracker.setToolkit( toolkit );
         // Create warez buffer.
         static const char tmpstr2[] = "ueaag4ha58op0q4yX897t478ufjuf";
         SharedBuffer tmpBuf2( (byte*)tmpstr2, strlen(tmpstr2));
         m_warezBuffer = WFDRMUtil::createXorKey( tmpBuf2 );

         m_handler = handler;
         
         m_baseKeyHandler =
            new MapLibKeyHandler(m_handler->getMapManipulator(),
                                 handler,
                                 handler->getSelectedMapObjectNotifier());

         
         
         m_handler->getMapProjection().addListener(m_baseKeyHandler);

         m_nbrExampleCacheInfo = 3;
         m_exampleCacheInfo = new MapLib::CacheInfo*[m_nbrExampleCacheInfo];
         for ( int i = 0; i < m_nbrExampleCacheInfo; ++i ) {
            ExampleCacheInfo* cur = new ExampleCacheInfo;
            m_exampleCacheInfo[i] = cur;
            typedef ExampleCacheInfo::namePair_t namePair_t;
            switch ( i ) {
               case 0:
                  cur->m_names = namePair_t( "a:\\DOS\\COMMAND.COM",
                                             "Stockholm" );
                  break;
               case 1:
                  cur->m_names = namePair_t( "/usr/local/bin/ls",
                                             "krya på dig!" );
                  break;
               case 2:
                  cur->m_names = namePair_t( "df0:c/list",
                                             "Malmö" );
                  break;
            }
         }

         // Must be done
         updateMemReq();
      }
      
   virtual ~MapLibInternal() {
      delete m_handler;
      for ( int i = 0; i < m_nbrExampleCacheInfo; ++i ) {
         delete m_exampleCacheInfo[i];
      }
      delete [] m_exampleCacheInfo;
         
      // Note that it is possible that some of these things weren't
      // deleted in the old implementation.
      delete m_warezBuffer;
      for ( xorVect_t::iterator it = m_xorBuffers.begin();
            it != m_xorBuffers.end();
            ++it ) {
         delete it->second;
      }
      delete m_plotter;
      // delete m_keyHandler;
      delete m_toolkit;
      delete m_internalConfig;
      delete m_baseKeyHandler;
   }

   /**
    *    Real get cache info function.
    */
   int getCacheInfo( MapLib::const_cacheinfo_p_p& infos ) {
      m_cacheInfoVector.clear();
      // Put all requesters into the vector
      m_handler->getAllRequesters( m_cacheInfoVector );
      // Remove the ones with no names
      for ( cacheInfoVector_t::iterator it = m_cacheInfoVector.begin();
            it != m_cacheInfoVector.end(); ) {
         if ( (*it)->getNameUTF8()[0] == '\0' ) {
            // empty name
            it = m_cacheInfoVector.erase( it );
         } else {
            ++it;
         }            
      }
      // Done - set return values.
      infos = & ( m_cacheInfoVector.front() );
      return m_cacheInfoVector.size();
   }

   /**
    *   Returns the example cache info. Should be removed.
    */
   int getExampleCacheInfo( MapLib::const_cacheinfo_p_p& infos ) {
      infos = m_exampleCacheInfo;
      return m_nbrExampleCacheInfo;
   }

   /// Called by the SFD when the header is loaded.
   void headerLoaded( SingleFileDBufRequester* /*requester*/ ) {
      mc2dbg << "[MLI]: headerLoaded" << endl;
      m_handler->sendEvent( TileMapEvent::UPDATE_CACHE_INFO );
   }    

   /**
    *    Gets the xor buffer for the supplied uid.
    *    If uid is NULL, the warezbuffer is returned.
    */
   SharedBuffer* getUINXorBuffer( const char* uid ) {
      if ( uid == NULL ) {
         // No name, use the warez buffer.
         return m_warezBuffer;
      }
      // Linear search.
      for ( xorVect_t::const_iterator it = m_xorBuffers.begin();
            it != m_xorBuffers.end();
            ++it ) {
         if ( it->first == uid ) {
            return it->second;
         }
      }
      // Not found - make new one
      SharedBuffer tmpBuf( const_cast<uint8*>
                           (reinterpret_cast<const uint8*>( uid )),
                           strlen( uid ) );
      m_xorBuffers.push_back(
         xorVect_t::value_type( uid, WFDRMUtil::createXorKey( tmpBuf ) ) );
         
      // Now it should be in the vector
      return getUINXorBuffer( uid );
   }
      
   DBufRequester* getRequesterOfExactType( WFAPI::wf_uint32 typeMask ) {
      vector<DBufRequester*> requesters;
      m_handler->getAllRequesters( requesters );
      for ( vector<DBufRequester*>::const_iterator it = requesters.begin();
            it != requesters.end();
            ++it ) {
         if ( WFAPI::wf_uint32( (*it)->getType() ) == typeMask ) {
            return *it;
         }
      }
      return NULL;
   }

   void updateMemReq( ) {
      // Check for memory cache
      // Get all the requesters from the TileMapHandler.
      if ( getRequesterOfExactType( DBufRequester::MEMORY ) ) {
         mc2dbg << "[MapLib]: Memory buffer already present" << endl;
      } else {
         DBufRequester* tmhRequester = m_handler->getInternalRequester();
         DBufRequester* oldParent = tmhRequester->getParent();
         // Insert memory DBufRequester with 100k bytes default mem.
         MemoryDBufRequester* memReq = new MemoryDBufRequester( oldParent,
                                                                100*1024 );
         tmhRequester->setParent( memReq );
         mc2dbg << "[MapLib]: Added memory requester" << endl;
      }
   }


   /// The behind-the-scenes keyhandler that the composite keyhandler calls
   MapLibKeyHandler* m_baseKeyHandler;
   /// The internal config member.
   WFAPI::InternalConfig* m_internalConfig;
   /// The TileMapHandler
   TileMapHandler* m_handler;
   /// The toolkit associated with the handler
   TileMapToolkit* m_toolkit;
   /// The cache info.
   MapLib::CacheInfo ** m_exampleCacheInfo;
   /// Keeps track of memory usage across objects.
   MemTracker m_memTracker;
   int m_nbrExampleCacheInfo;
   typedef vector<MapLib::CacheInfo*> cacheInfoVector_t;
   cacheInfoVector_t m_cacheInfoVector;
   SharedBuffer* m_warezBuffer;
   /// Type of the vector of xor buffers.
   typedef vector<pair<MC2SimpleString, SharedBuffer*> > xorVect_t;
   /// Xor buffer vector.
   xorVect_t m_xorBuffers;
   /// Map plotter
   MapPlotter* m_plotter;


   // MapLibCompositeKeyHandler* m_keyHandler;

};

//    }

MapLib::MapLib( TileMapHandler* handler )
{
   m_storage = new MapLibInternal(  handler, handler->m_toolkit );

   m_keyInterface = new WFAPI::MapLibKeyInterface(this);
   
   m_scaleFeature = NULL;
   m_featureVec = NULL;
}

MapLib::MapLib( WFAPI::DrawingContext* drawingContext,
                TileMapToolkit* toolkit,
                WFAPI::DBufConnection* bufferConnection,
                const WFAPI::MapLibInitialConfig& initialConfig )
{
   m_initialConfig = initialConfig;

   DBufRequester* requester = NULL;
      
   if(bufferConnection) {
      requester = new SharedDBufRequester( bufferConnection );
   } else {
      requester = new DBufRequester;
   }
   
   Init( drawingContext, toolkit, requester );
}

MapLib::MapLib( WFAPI::DrawingContext* drawingContext,
                TileMapToolkit* toolkit,
                DBufRequester* dbufrequester )
{
   Init( drawingContext, toolkit, dbufrequester );
}

void MapLib::Init( WFAPI::DrawingContext* drawingContext,
                   TileMapToolkit* toolkit,
                   DBufRequester* bufferRequester )
{
   
   TileMapHandler* tmh =
      new TileMapHandler( bufferRequester, toolkit );
   if ( drawingContext ) {
      tmh->setDrawingContext( drawingContext );
   }
   
   m_storage = new MapLibInternal( tmh, toolkit );
   m_keyInterface = new WFAPI::MapLibKeyInterface(this);
}

void
MapLib::initDefaultSettings()
{
   setDPICorrectionFactor(2);
}

void 
MapLib::initializeSettings()
{
   initDefaultSettings();
   if (m_initialConfig.getCachePath() != "") {
      mc2dbg << "Setting cache : " 
           << m_initialConfig.getCachePath()
           << ", size : "
           << m_initialConfig.getCacheMaxSize()
           << endl;

      addDiskCache( m_initialConfig.getCachePath().c_str(),
                    m_initialConfig.getCacheMaxSize());
   }

   const char* fontName = SCALE_FONT;
   int fontSize = SCALE_FONT_SIZE;
   
   m_scaleFeature =
      new UserDefinedScaleFeature(fontName,
                                  fontSize,
                                  UserDefinedScaleFeature::getMeterSettings(),
                                  2);
   
   m_scaleFeature->setPoint(MC2Point(14,92));
#ifdef VOLANS
   m_scaleFeature->setScaleMaxWidth(110);
#else
   m_scaleFeature->setScaleMaxWidth(146);
#endif
   m_scaleFeature->setAlignment(UserDefinedScaleFeature::LEFT_ALIGNMENT);
   m_scaleFeature->rescalePoints();
   m_featureVec =
      new std::vector<UserDefinedFeature*>;
   m_featureVec->push_back(m_scaleFeature);
   m_storage->m_handler->setUserDefinedFeatures(m_featureVec);   
}

MapLib::~MapLib()
{
   delete m_storage;
   delete m_scaleFeature;
   delete m_featureVec;
   delete m_keyInterface;
}

int
MapLib::getCacheInfo( MapLib::const_cacheinfo_p_p& infos )
{
   return m_storage->getCacheInfo( infos );
}

// Strange name for a function, I know.
static vector<DBufRequester*>::iterator
getCacheBefore( vector<DBufRequester*>::iterator begin,
                vector<DBufRequester*>::iterator end,
                WFAPI::wf_uint32 types )
{
   vector<DBufRequester*>::iterator childit = end;
   for ( vector<DBufRequester*>::iterator it = begin;
         it != end;
         ++it ) {
      const DBufRequester* curReq = *it;
      if ( curReq->getType() & types ) {
         // Now child should be the mem cache.
         break;
      }
      childit = it;
   }
   return childit;
}

int
MapLib::addSingleFileCache( const char* fileName,
                            const char* /*uin*/ )
{
   // Always put these quick caches just after the memory cache.
   
   // Get all the requesters from the TileMapHandler.
   vector<DBufRequester*> requesters;
   m_storage->m_handler->getAllRequesters( requesters );

   // Find the memory cache, i.e. the first one that isn't
   // PERMANENT or EXTERNAL
   vector<DBufRequester*>::iterator childit  =
      getCacheBefore( requesters.begin(),
                      requesters.end(),
                      DBufRequester::PERMANENT | DBufRequester::EXTERNAL );

   if ( childit == requesters.end() ) {
      // Not found, unpossible.
      return -1;
   }
   
   // Start by creating the platform dependent file handler.
   FileHandler* f_handler = createFileHandler( fileName,
                                               true,        // R/O
                                               false,       // createfile
                                               true         // initNow.
                                               ); 
   
   // Put it into the platform-independent requester.
   SingleFileDBufRequester* sfd =
      new SingleFileDBufRequester( (*childit)->getParent(),
                                   f_handler,
                                   m_storage);
   
   // Set the parent of the mem cache to sfd.
   (*childit)->setParent( sfd );

   // Set the language of the sfd (and the parents)
   sfd->setPreferredLang( m_storage->m_handler->getLanguage() );

   return 0;
}

void
MapLib::setMemoryCacheSize( WFAPI::wf_uint32 nbrBytes )
{
   DBufRequester* req =
      m_storage->getRequesterOfExactType( DBufRequester::MEMORY );
   if ( req != NULL ) {
      static_cast<MemoryDBufRequester*>(req)->setMaxSize( nbrBytes );
   } else {
      mc2dbg << "[MapLib::setMaxCacheSize]: Couldn't find mem cache" << endl;
   }
}

void
MapLib::clearMemoryCache()
{
   DBufRequester* req =
      m_storage->getRequesterOfExactType( DBufRequester::MEMORY );
   if ( req != NULL ) {
      static_cast<MemoryDBufRequester*>(req)->clearCache();
   } else {
      mc2dbg << "[MapLib::clearMemoryCache]: Couldn't find mem cache" << endl;
   }
}

int
MapLib::setDiskCacheSize( WFAPI::wf_uint32 nbrBytes )
{
   // Get requester that is permanent, but not READ_ONLY
   DBufRequester* req =
      m_storage->getRequesterOfExactType( DBufRequester::PERMANENT );
   if ( req != NULL ) {
      static_cast<MultiFileDBufRequester*>(req)->setMaxSize( nbrBytes );
      return 0;
   } else {
      mc2dbg << "[MapLib::setDiskCacheSize]: Couldn't find dsk cache" << endl;
      return -1;
   }
}

int
MapLib::clearDiskCache( )
{
   // Get requester that is permanent, but not READ_ONLY
   DBufRequester* req =
      m_storage->getRequesterOfExactType( DBufRequester::PERMANENT );
   if ( req != NULL ) {
      static_cast<MultiFileDBufRequester*>(req)->clearCache();
      return 0;
   } else {
      mc2dbg << "[MapLib::setDiskCacheSize]: Couldn't find dsk cache" << endl;
      return -1;
   }
}

TileMapToolkit*
MapLib::getToolkit() const
{
   return m_storage->m_handler->m_toolkit;
}

static int addMultiFileCache( MapLibInternal* storage,
                              MapLib* mapLib,
                              const char* path,
                              const char* uin,
                              WFAPI::wf_uint32 maxSize,
                              int read_only )
{
   // These caches are slower than the SingleFile caches and should
   // be put after them, i.e. just before the internet cache.

   // The order should be (from the bottom)
   // * Internet
   // * Read/write multifile cache
   // * Read-only  multifile caches
   // * Single file caches
   // * Memory cache
   // * Internal requester in TileMapHandler.
   
   vector<DBufRequester*> requesters;
   storage->m_handler->getAllRequesters( requesters );

   // There should be two requesters when we come to this point
   // The Internet and internal ( and probably memory too )
   if ( requesters.size() < 2 ) {
      // Unpossible to add the requester.
      MC2_ASSERT( false );
      return -1;
   }

   DBufRequester* childReq = NULL;
   
   // The internet cache should be the last one.
   vector<DBufRequester*>::reverse_iterator rit = requesters.rbegin();
   ++rit;
   // Now we have the last non-internet cache.
   if ( ! read_only ) {
      // The non read only cache should be the last one before the internet
      // cache.
      childReq = *rit;
   } else {
      // Insert the read only cache before the internet cache if there
      // is no read/write cache. Insert it before the read/write cache if
      // there is one.
      // The internet cache should be the last one.
      // Now we have the last non-internet cache in rit
      if ( ! ( (*rit)->getType() & DBufRequester::READ_ONLY ) ) {
         // Not read only. Take one more step
         ++rit;
      }
      if ( rit != requesters.rend() ) {
         childReq = *rit;
      }         
   }

   MC2_ASSERT( childReq != NULL );
   
   if ( childReq == NULL ) {
      return -1;
   }

   // Create new MultifileDBufRequester with the parent of the
   // new child as parent.
   MultiFileDBufRequester* mfdbr =
      new MapLibMultiFileDBufRequester( mapLib,
                                        childReq->getParent(),
                                        path,
                                        & ( storage->m_memTracker ),
                                        maxSize,
                                        read_only );

   if ( read_only ) {
      // Use warez buffer or ordinary buffer.
      mfdbr->setXorBuffer( storage->getUINXorBuffer( uin ) );
   }
   
   // Set the parent of the child cache to mfdbr.
   childReq->setParent( mfdbr );
   return 0;
}

int
MapLib::addDiskCache( const char* path,
                      WFAPI::wf_uint32 maxSize )
{
   return ::addMultiFileCache( m_storage, this,
                               path, NULL, maxSize, false );
}

int
MapLib::addMultiFileCache( const char* path,
                           const char* uin )
{
   // Add as read only cache.
   return ::addMultiFileCache( m_storage, this,
                               path, uin, MAX_UINT32, true );
}

int
MapLib::disconnectCache( const CacheInfo* info )
{
   vector<DBufRequester*> infos;
   m_storage->m_handler->getAllRequesters( infos );
   vector<DBufRequester*>::iterator childit = infos.end();
   for ( vector<DBufRequester*>::iterator it = infos.begin();
         it != infos.end();
         ++it ) {
      if ( (*it)->getParent() == info ) {
         childit = it;
         // Found the one to be removed.
         break;
      }
   }

   if ( childit == infos.end() ) {
      return -1;
   }
   
   DBufRequester* toRemove = (*childit)->getParent();
   (*childit)->setParent( (*childit)->getParent()->getParent() );
   
   // Prevent parents from being deleted
   toRemove->setParent( NULL );
   delete toRemove;

   // Send an event because an update is needed.
   m_storage->m_handler->sendEvent(
      TileMapEvent( TileMapEvent::UPDATE_CACHE_INFO ) );
   
   // Done
   return 0;
}

void
MapLib::start()
{
   m_storage->m_handler->start();
}

void
MapLib::addEventListener( TileMapEventListener* listener )
{
   m_storage->m_handler->addEventListener( listener );
}

void
MapLib::removeEventListener( TileMapEventListener* listener )
{
   m_storage->m_handler->removeEventListener( listener );
}

void
MapLib::setRouteID( const RouteID& routeID )
{
   m_storage->m_handler->setRouteID( routeID );
}

const RouteID* 
MapLib::getRouteID() const
{
   return m_storage->m_handler->getRouteID();
}

void
MapLib::clearRouteID()
{
   m_storage->m_handler->clearRouteID();
}

const char*
MapLib::setLanguageAsISO639( const char* isoStr )
{
   return m_storage->m_handler->setLanguageAsISO639( isoStr );
}

void
MapLib::setLanguage(LangTypes::language_t lang)
{
   m_storage->m_handler->setLanguage(lang);
}

void
MapLib::setCopyrightPos( const MC2Point& pos )
{
   m_storage->m_handler->setCopyrightPos( pos );
}

bool
MapLib::showCopyright( bool show )
{
//This define should be set in compile scripts if wanted:   
#ifdef ALLOW_MAPLIB_COPYRIGHT_DISABLING
   m_storage->m_handler->showCopyright( show );
   return true;
#else
   return false;
#endif
}

WFAPI::MapDrawingInterface*
MapLib::getMapDrawingInterface() const
{
   return m_storage->m_handler;
}

WFAPI::MapOperationInterface*
MapLib::getMapOperationInterface()
{
   return m_storage->m_handler->getMapOperationInterface();
}

InterpolationHintConsumer* MapLib::getInterpolationHintConsumer()
{
   return m_storage->m_handler->getPositionInterpolator();
}

void 
MapLib::setDPICorrectionFactor( WFAPI::wf_uint32 factor )
{
   m_storage->m_handler->getMapProjection().setDPICorrectionFactor( factor );   
}

double 
MapLib::getDPICorrectedScale() const
{
   return m_storage->m_handler->getMapProjection().getDPICorrectedScale();
}

void
MapLib::setDPICorrectedScale( double scale )
{
   m_storage->m_handler->getMapProjection().setDPICorrectedScale(scale);
}

void
MapLib::set3dMode( bool on )
{
   m_storage->m_handler->set3dMode( on );
}

void
MapLib::setOutlinesIn3dEnabled( bool enabled )
{
   m_storage->m_handler->setOutlinesIn3dEnabled( enabled );
}

bool 
MapLib::get3dMode( )
{
   return m_storage->m_handler->get3dMode( );
}

std::vector<UserDefinedFeature*>*
MapLib::getUserDefinedFeatures()
{
   return m_storage->m_handler->getUserDefinedFeatures();
}

bool
MapLib::getOutlinesIn3dEnabled( ) const
{
   return m_storage->m_handler->getOutlinesIn3dEnabled();
}

void
MapLib::setHorizonHeight( WFAPI::wf_uint32 height )
{
   m_storage->m_handler->setHorizonHeight( height );
}

void 
MapLib::setNightMode( bool on )
{
   m_storage->m_handler->setNightModeL( on );
}

bool MapLib::getNightMode()
{
   return m_storage->m_handler->getNightMode();
}


bool 
MapLib::isACPModeEnabled() const
{
   return m_storage->m_handler->isACPModeEnabled();
}

void 
MapLib::setACPModeEnabled( bool enable )
{
   m_storage->m_handler->setACPModeEnabled( enable );
}

bool
MapLib::isInitialized() const
{
   bool retVal = false;
   if ( m_storage !=NULL &&
        m_storage->m_handler != NULL && 
        m_storage->m_handler->isInitialized() ) {
      retVal = true;
   }
   return retVal;
}

void
MapLib::setLayerVisible(int layerNo, bool visible)
{
   m_storage->m_handler->setLayerVisible(layerNo, visible);
}

void
MapLib::setUpdatePeriodMinutes(int layerNo, 
                               WFAPI::wf_uint32 updateIntervalMinutes)
{
   m_storage->m_handler->setUpdatePeriodMinutes(
      layerNo, updateIntervalMinutes);
}

void MapLib::setInterpolationCallback( InterpolationCallback* callback )
{
   m_storage->m_handler->setInterpolationCallback( callback ); 
}

void MapLib::setDrawingContext( WFAPI::DrawingContext* context )
{
   m_storage->m_handler->setDrawingContext( context ); 
}

void MapLib::clearInterpolationCallback( InterpolationCallback* callback )
{
   m_storage->m_handler->clearInterpolationCallback( callback ); 
}


/*________________________________EXTERNAL API DESIGN___________________*/

WFAPI::MapLibKeyInterface* MapLib::getKeyInterface() {
   return m_keyInterface;
}

void MapLib::setInterpolationEnabled( bool enabled )
{
   m_storage->m_handler->setInterpolatingPositions( enabled );
}

void MapLib::requestImmediatePositionInterpolation()
{
   m_storage->m_handler->positionInterpolationTimerExpired();
}

OverlayView* MapLib::getOverlayView()
{
   return m_storage->m_handler->getOverlayView();
}

const vector<const WFAPI::POICategory*>* MapLib::getPOICategories() const
{
   return m_storage->m_handler->getCategories(); 
}

bool MapLib::setPOICategoryVisible(int id, bool visible)
{
   return m_storage->m_handler->setCategoryVisible(id, visible);
}

SelectedMapObjectNotifier* MapLib::getSelectedMapObjectNotifier()
{
   return m_storage->m_handler->getSelectedMapObjectNotifier(); 
}


MapLibSettingsNotifier* MapLib::getMapLibSettingsNotifier()
{
   return m_storage->m_handler->getMapLibSettingsNotifier();
}

void MapLib::enableAutomaticHighlight(bool enable)
{
   m_storage->m_handler->enableAutomaticHighlight(enable); 
}

void
MapLib::initCallbackTrigger()
{
   m_storage->m_handler->m_toolkit->initCallbackTrigger();
}

void
MapLib::triggerCallbacks(WFAPI::MapLibCallback* callback)
{
   m_storage->m_handler->m_toolkit->triggerCallbacks(callback);
}

   
void 
MapLib::setStartupListener(WFAPI::MapLibStartupListener* startupListener)
{
   m_storage->m_handler->setStartupListener(startupListener);
}

OverlayViewInputInterface* MapLib::getOverlayInputHandler() const
{
   return &m_storage->m_handler->getOverlayView()->getInputInterface();
}

MapLibKeyHandler* MapLib::getMapLibInputHandler() const
{
   return m_storage->m_baseKeyHandler;
}

WFAPI::InternalConfig* MapLib::getInternalConfig() const
{
   return m_storage->m_internalConfig;
}

void MapLib::invalidateMap()
{
   m_storage->m_handler->repaintNow();
}

void MapLib::setVariable3dMode(float step)
{
   m_storage->m_handler->setVariable3dMode( step );
}

void MapLib::hidePOIs(bool hidden)
{
   m_storage->m_handler->hidePOIs(hidden);
}
