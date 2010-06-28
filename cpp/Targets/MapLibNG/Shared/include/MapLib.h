/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef MAPLIB_H
#define MAPLIB_H

#include "PALMachineTypes.h"
#include <vector>
#include "MapLibKeyHandler.h"
#include "MapLibInitialConfig.h"
#include "LangTypes.h"
#include "UserDefinedFeature.h"
#include "UserDefinedScaleFeature.h"

//    namespace {
   class MapLibInternal;
//    }

namespace isab {
   class MapPlotter;
}

class OverlayView;
class DBufRequester;
class FileHandler;
class MC2Point;
class RouteID;
class TileMapEventListener;
class TileMapHandler;
class TileMapToolkit;
class SelectedMapObjectNotifier;
class MapLibSettingsNotifier;
class OverlayViewInputInterface;
class TileMapToolkit;


/*__________________ External api design____________*/



namespace WFAPI {
class MapLibKeyInterface;
class MapDrawingInterface;
class InternalConfig;
class POICategory;
class DrawingContext;
class DBufConnection;
class MapOperationInterface;
class MapLibCallback;
class MapLibStartupListener;
class MapLibInitialConfig;
}

/*__________________/External api design____________*/

class InterpolationHintConsumer;
class InterpolationCallback;

/**
 *   Interface class for MapLib that should have been created
 *   a long time ago.
 *   <b>All strings are in utf-8 and zero terminated.</b>
 */
class MapLib {
public:

   /// Return value for get cache info.
   class CacheInfo {
   public:
      virtual ~CacheInfo() {}
      /// Returns the path of the cache in question.
      virtual const char* getPathUTF8() const = 0;
      /// Returns the user displayable name of the cache
      virtual const char* getNameUTF8() const = 0;
      /// Returns true if the cache seems valid.
      virtual bool isValid() const = 0;
   };

   typedef const CacheInfo* const_cacheinfo_p;
   typedef const const_cacheinfo_p* const_cacheinfo_p_p;
   
   /**
    *    Returns the filenames of the currently active caches.
    *    Should not be deleted and is invalidated when adding/removing
    *    caches.
    *    @param infoArray Is set to a pointer to the internal array.
    *    @return The size of the array.
    */
   int getCacheInfo( const_cacheinfo_p_p& infos );

   /**
    *    Add a cache file to the cache stack.
    *    Will invalidate the CacheInfo.
    *    @param fileName Full path to the file.
    *    @param uin      UIN of the user.
    *    @return -1 if failed.
    */
   int addSingleFileCache( const char* fileName,
                           const char* uin );

   /**
    *    Sets the size of the memory cache.
    */
   void setMemoryCacheSize( WFAPI::wf_uint32 nbrBytes );

   /**
    *    Clears the memory cache.
    */
   void clearMemoryCache();

   /**
    *    Creates the read/write cache with the supplied max size.
    *    @param path    The path to the cache in utf-8.
    *    @param maxSize The maximum size of the cache.
    */
   int addDiskCache( const char* path,
                     WFAPI::wf_uint32 maxSize );

   /**
    *    Sets the max size of the disk cache if it exists.
    *    @param nbrBytes The maximum size of the R/W cache in bytes.
    */
   int setDiskCacheSize( WFAPI::wf_uint32 nbrBytes );

   /**
    *    Clears the R/W disk cache.
    */
   int clearDiskCache();
   
   /**
    *    Adds a multifile read only cache with the specified path.
    *    @param path The path to the directory where the multi file
    *                cache is located.
    *    @param uin  The uin for decryption if downloaded cache. <br /> 
    *                NULL for warez.
    */
   int addMultiFileCache( const char* path,
                          const char* uin );

   /**
    *    Disconnects the cache with the corresponding info.
    *    Currently only works for single file caches.
    *    The CacheInfo can never be used again.
    */
   int disconnectCache( const CacheInfo* info );

   /**
    *     Start maplib if not yet started.
    */
   void start();

   /**
    *    Adds an event listener to be called when new categories
    *    are available and if cache info needs update.
    */
   void addEventListener( TileMapEventListener* listener );

   /**
    *    Removes an event listener.
    */
   void removeEventListener( TileMapEventListener* listener );

   /**
    *    Sets the route id of MapLib.
    */
   void setRouteID( const RouteID& routeID );

   /**
    *    Gets the route id of MapLib.
    */
   const RouteID* getRouteID() const;

   /**
    *    Clears the routeID.
    */
   void clearRouteID();

   /**
    *   Set the language using the specified iso639 string.   
    *   The string can consist of two or three letters and must be
    *   NULL terminated. 
    *   @param isoStr The string, containing two or three letters.
    *   @return   The language as iso639 that was set to MapLib.
    */
   const char* setLanguageAsISO639( const char* isoStr );

   /**
    * Set the language for the maps.
    *
    * @param lang The language to set.
    */
   void setLanguage( LangTypes::language_t lang );
   
   /**
    * Set the position of the copyright string of the map.
    * 
    * @param   pos   The left baseline position of the copyright string.
    */
   void setCopyrightPos( const MC2Point& pos );
   
   /**
    * Set if to show copyright string.
    * Some versions of the library binary file allows you to change this setting,
    * others don't. 
    * 
    * @return Returns true if you are allowed to change the default setting, otherwise false
    * 
    */
   bool showCopyright( bool show );

   /**
    *    Returns the mapdrawinginteface
    *    Valid during the lifetime of the MapLib.
    */
   WFAPI::MapDrawingInterface* getMapDrawingInterface() const;

   /**
    * Get the overlay views input handler
    *
    * @return The overlay views input handler.
    */
   OverlayViewInputInterface* getOverlayInputHandler() const;

   /**
    * Get MapLibs input handler
    *
    * @return MapLibs input handler. 
    */
   MapLibKeyHandler* getMapLibInputHandler() const;

   /**
    * Get the internal config interface.
    *
    * @return The internal config interface.
    */
   WFAPI::InternalConfig* getInternalConfig() const;

   /**
    * Get the toolkit
    *
    * @return The toolkit associated with the maplib context.
    */
   TileMapToolkit* getToolkit() const;

   /**
    * Set if a specific layer show be visiable or not.
    *
    * @param layerNo The layer to change.
    * @param visible If the layer should be shown or not.
    */
   void setLayerVisible(int layerNo, bool visible);

   /**
    * Set the update period for a layer.
    *
    * @param layerNo The layer to change.
    * @param updateIntervalMinutes The new update time for the layer in
    *                              minutes.
    */
   void setUpdatePeriodMinutes(int layerNo, 
                               WFAPI::wf_uint32 updateIntervalMinutes);

//// BEGIN:  TO BE MOVED TO SEPARATE INTERPOLATION INTERFACE
   /**
    *    Returns the interpolation hint conusmer.
    *    Can be used to provide hints to MapLib regarding
    *    where the position should be between real position
    *    updates.
    */ 

   InterpolationHintConsumer* getInterpolationHintConsumer();

   void setInterpolationCallback( InterpolationCallback* callback );

   void clearInterpolationCallback( InterpolationCallback* callback );

   void setInterpolationEnabled( bool enabled );

   void requestImmediatePositionInterpolation();
   
//// END:  TO BE MOVED TO SEPARATE INTERPOLATION INTERFACE
   
   /**
    *    Set the DPI correction factor. The factor 1 corresponds
    *    a value suitable for old phone models, such as Nokia 6600
    *    and similar. For new devices with displays with higher
    *    DPI, the factor should be correspond with how many times
    *    greater the DPI is compared to the reference display
    *    (e.g. Nokia 6600).
    */
   void setDPICorrectionFactor( WFAPI::wf_uint32 factor );

   /**
    * Get the DPI correction scale
    *
    * @return The DPI correction scale
    */
   double getDPICorrectedScale() const;

   /**
    * Set the DPI correction scale.
    *
    * @param scale The DPI corrected scale to change to.
    */
   void setDPICorrectedScale( double scale );

   /**
    *   Changes the context to an external context.
    *
    *   @param context       An external context that should
    *                        be used for drawing.   
    */
   
   void setDrawingContext( WFAPI::DrawingContext* context );
   
   /**
    * Turns 3d mode on or off.
    */
   void set3dMode( bool on );

   void setVariable3dMode(float step);

   /**
    * Turns outlines on in 3d mode.
    */
   void setOutlinesIn3dEnabled( bool enabled );

   /**
    * True if in 3d mode, false if not.
    */
   bool get3dMode( );

   /**
    * True if in outlines are enabled in 3d mode, false if not.
    */
   bool getOutlinesIn3dEnabled( ) const;

   
   std::vector<UserDefinedFeature*>* getUserDefinedFeatures();


   /**
    * Sets the horizon height that is displayed
    * when map is displayed in 3d. 
    * @param height The height of the horizon.
    */
   void setHorizonHeight( WFAPI::wf_uint32 height );

   /**
    *    Sets night mode on or off. 
    */
   void setNightMode( bool on );

   /**
    * Returns whether the night mode is on (true) or off (false).
    *
    * @return True if night mode is on.
    */
   bool getNightMode();

   /**
    *  Get the ACP mode setting. True if ACP enabled, false if not.
    */
   bool isACPModeEnabled() const;

   /**
    * Set the ACP mode setting. True if ACP enabled, false if not.
    */
   void setACPModeEnabled( bool enable );

   
   /**
    *    Destructor.
    */
   virtual ~MapLib();

   /**
    *   Use. Implement.
    */
   virtual FileHandler* createFileHandler( const char* filename,
                                           bool readOnly,
                                           bool createFile,
                                           bool initNow ) = 0;

   /**
    *   Implement.
    *   Return the path separator, e.g. "/" for unix and "\\" for
    *   Symbian/Windows.
    */
   virtual const char* getPathSeparator() const = 0;

   /**
    *   @return If MapLib is initialized.
    */
   bool isInitialized() const;

   /**
    * Initialize settings. Should be after construction of MapLib.
    */
   void initializeSettings();

   /**
    * @return The overlay view associated with this MapLib session.
    */ 
   OverlayView* getOverlayView();

   const std::vector<const WFAPI::POICategory*>* getPOICategories() const;

   /**
    *   Show or hide a category with the specified id.
    *   Will not work correctly until getCategories returns non-NULL.
    *   @param id      The id of the parameter to disable/enable.
    *   @param visible True/false.
    *   @return True if the category id was found.
    */
   bool setPOICategoryVisible( int id, bool visible );

   /**
    * @return The selected map object notifier used to delegate
    *         selection events to listeners.
    */
   SelectedMapObjectNotifier* getSelectedMapObjectNotifier();

   /**
    * @return The settings notifier used to delegate
    *         setting change events to listeners.
    */
   MapLibSettingsNotifier* getMapLibSettingsNotifier();

/*________________________________EXTERNAL API DESIGN___________________*/


   /**
    *   Enables an image of a cursor to be displayed on the
    *   screen.  If it is static, the cursor is fixed to the
    *   center of the screen. Note that the caller will still
    *   retain ownership of the widget, but that the internal
    *   key handling will change its position as key events occur.
    *
    */ 
   
   // virtual void setCursorWidget( MapWidget* cursor ) = 0;
   
   WFAPI::MapLibKeyInterface* getKeyInterface();

   WFAPI::MapOperationInterface* getMapOperationInterface();

   /**
    * Set if automatic highlight should be enabled or not.
    * Automatic highlight means that features in the center of the
    * map area will automatically be highlighted.
    * @see DetailedConfigInterface.h
    *
    * @param enable True if automatic highlight should be enabled,
    *               false otherwise if disabled.
    */
   void enableAutomaticHighlight(bool enable);

   /**
    * Sets up the trigger for triggerCallbacks calls.
    */
   void initCallbackTrigger();

   /**
    * Set up for a callback.
    * 
    * @param callback The object to call back.
    */
   void triggerCallbacks(WFAPI::MapLibCallback* callback);
   
   /**
    * Set the startup listener.
    * 
    * @param startupListener Listener that will be called 
    * when maplib is started.
    */
   void setStartupListener(WFAPI::MapLibStartupListener* startupListener);

   /**
    * To invalidate the map to make it repaint. 
    */
   void invalidateMap();

   /**
    * @see DetailedConfigInterface::hidePOIs
    */
   void hidePOIs(bool hidden);
   
protected:

   /**
    *   One of the constructors to use when porting MapLib
    *   to another platform. Ownership of the objets will
    *   be taken over by MapLib.
    */
   MapLib( WFAPI::DrawingContext* drawingContext,
           TileMapToolkit* toolkit,
           DBufRequester* bufferRequester );

   /**
    *   One of the constructors to use when porting MapLib
    *   to another platform. Ownership of the objets will
    *   be taken over by MapLib.
    */
   MapLib( WFAPI::DrawingContext* drawingContext,
           TileMapToolkit* toolkit,
           WFAPI::DBufConnection* bufferConnection,
           const WFAPI::MapLibInitialConfig& initialConfig );

   void Init( WFAPI::DrawingContext* drawingContext,
              TileMapToolkit* toolkit,
              DBufRequester* bufferRequester );

   /**
    *   Don't use.
    */
   MapLib( TileMapHandler* handler );

   /**
    * Initialize default settings.
    */
   void initDefaultSettings();

private:
   MapLibInternal* m_storage;

   /// The scale for the map
   UserDefinedScaleFeature* m_scaleFeature;
   std::vector<UserDefinedFeature*>* m_featureVec;

   /// The initial settings.
   WFAPI::MapLibInitialConfig m_initialConfig;

   /// The key interface
   WFAPI::MapLibKeyInterface* m_keyInterface;
};

#endif
