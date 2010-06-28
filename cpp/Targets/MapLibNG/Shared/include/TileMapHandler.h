/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/



#ifndef TILE_MAPHANDLER_H
#define TILE_MAPHANDLER_H

#define MAX_COPYRIGHT_STRING_LEN 80

#include "MC2SimpleString.h"
#include "TileMapConfig.h"
#include "TileMapHandlerTypes.h"
#include "TileMapLoader.h"

#include <map>
#include <set>
#include <vector>
#include <list>

#include "TileMapToolkit.h"
#include "MapPlotter.h"
#include "MC2BoundingBox.h"
#include "MapProjection/MapProjection.h"
#include "LangTypes.h"

#include "PixelBox.h"
#include "OverlapDetector.h"
#include "MovementHelper.h"
#include "TileMap.h"
#include "MapDrawingInterfaceInternal.h"
#include "BitmapMovementHelper.h"
#include "CopyrightHandler.h"
#include "DrawingContextProxy.h"
#include "ClickInfo.h"
#include "Grid/GridStorage.h"
#include "ImageMap/ImageTile.h"

namespace WFAPI {
class ScreenPoint;
class ImageSpec;
class MapLibStartupListener;
class MapDrawingCallback;
}

class InterpolationDebugInfo {
public:
   MC2Coordinate coord;
   int timeDiffMillis;
};

typedef std::vector<InterpolationDebugInfo> InterpolationDebugVector;

using namespace isab;

namespace isab {
class PlotterAdapter3d;
}

class StressTester;
class SelectedMapObjectNotifier;
class MapLibSettingsNotifier;
class MapManipulator;
class PlatformImageLoader;
class DBufRequester;
class BitBuffer;
class TileMapFormatDesc;
class ClickInfo;
class CoordsArg;
class TileMapUtil;
class TileMap;
class TileMapParams;
class TileMapCoord;
class TilePrimitiveFeature;
class DBufRequestListener;
class TransformMatrix;
class FileDBufRequester;
class RouteID;
class MC2Coordinate;
class MC2Point;
class PositionInterpolator;
class TileMapTextHandler;
class UserDefinedFeature;
class DirectedPolygon;
class StringCache;
class TileMapEventListener;
class TileMapTextSettings;
class TileMapLayerInfoVector;
class TileMapContainer;
class TileCategory;
class TMKTimingInfo;
class TileMapHandlerReleaseChecker;
class TileMapInfoCallback;
class TileMapEvent;

namespace isab {
class PlotterAdapter3d;
}

class InterpolationCallback;
struct Settings3D;
class ExternalRenderer;
class OverlayView;
class MapLibOperationHandler;
class MapProjectionAdapter;

/**
 *    Class for returning information about clicked items.
 */
class TileMapHandlerClickResult : public ClickInfo {
public:
   TileMapHandlerClickResult() : m_clickedFeature(NULL),
                                 m_selectionOutline(NULL),
                                 m_distance(MAX_UINT32) {}

   /**
    *   Returns the name of the clicked feature, if a
    *   feature in the map has been clicked.
    */
   inline const char* getName() const {
      return m_name.c_str();
   }

   inline const MC2SimpleString& getNameAsString() const {
      return m_name;
   }

   /**
    *   Returns the server string to use when getting more information
    *   about an item when a feature in the map has been clicked.
    */
   inline const char* getServerString() const {
      return m_serverString.c_str();
   }

   /**
    *   Returns the feature clicked, if any. Probably already owned
    *   by the caller of the method. NULL if a feature in the map
    *   was clicked.
    */
   inline UserDefinedFeature* getClickedUserFeature() const {
      return m_clickedFeature;
   }

   /// Returns the outline feature. Must not be deleted.
   inline UserDefinedFeature* getOutlineFeature() const {
      return m_selectionOutline;
   }

   inline int32 getDistance() const {
      return m_distance;
   }
   
   inline bool shouldHighlight() const {
      return getOutlineFeature() != NULL;
   }
private:
   friend class TileMapHandler;
   
   MC2SimpleString m_name;
   MC2SimpleString m_serverString;
   UserDefinedFeature* m_clickedFeature;
   UserDefinedFeature* m_selectionOutline;
   uint32 m_distance;
};


#include "TileMapGarbage.h"

#include "TileFeature.h"

/**
 *    Cross-platform TileMap getter and drawer.
 *    No platform dependand code should be here, only
 *    in the drivers, e.g. XXXMapPlotter.
 */
class TileMapHandler : public TileMapTimerListener,
                       public TileMapIdleListener,
                       public MapProjectionListener,
                       public MapDrawingInterfaceInternal,
                       public TileMapLoadingListener
{
public:

   // Not the most beautiful piece of code, but better than the alternative
   friend class MapProjectionAdapter;

   /**
    *   Creates a new TileMapHandler plotting on the supplied
    *   MapPlotter and requesting from the supplied requester.
    *   @param plotter       The MapPlotter.
    *   @param requester     DBufRequester to use.
    *   @param platform      The toolkit for the platform.
    *   @param eventListener 
    */
   TileMapHandler( DBufRequester* requester,
                   TileMapToolkit* platform,
                   TileMapEventListener* eventListener = NULL );

   /**
    *   The destructor.
    */
   virtual ~TileMapHandler();
   
   /**
    * Start maplib if not yet started.
    */
   void start();

   /**
    *   Tells the TileMapHandler that it is time to redraw
    *   the screen.
    */
   void repaint(uint32 maxTimeMillis);
   
   /**
    *   Called when a timer expires.
    */
   void timerExpired(uint32 id);

   /**
    *   Called when the system is a bit idle.
    */
   void runIdleTask(uint32 id);

   /**
    *   Sets automatic selection highlighting on/off.
    */
   void setHighlight( bool onoff );

   /**
    *   Sets highlight point. MC2Point(-1, -1) means auto-center.
    */
   void setHighlightPoint( const MC2Point& hightlightPoint );

   /**
    *   Sets new text settings in the text handler.
    */
   void setTextSettings( const TileMapTextSettings& settings );
   
   /**
    *   Sets the current route id to show in the map.
    *   @param routeID Route to show.
    */
   void setRouteID(const RouteID& routeID);

   /**
    *   Gets the current route id that is shown in the map.
    *   @return Current route id
    */
   const RouteID* getRouteID() const;

   /**
    * @return the overlay view associated with this handler.
    */ 
   OverlayView* getOverlayView();

   /**
    * @return The map manipulator used to handle map movements,
    *         animations etc.
    */
   MapManipulator& getMapManipulator();

   /**
    * @return Returns the interface used to set the map position,
    *         angle, zoom etc.
    */
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
    * @return the notifier for selected map objects.
    */ 
   SelectedMapObjectNotifier* getSelectedMapObjectNotifier();

   /**
    * @return the notifier for changed MapLib settings.
    */
   MapLibSettingsNotifier* getMapLibSettingsNotifier();
   
   /**
    *   Dumps all of the geometry to be rendered
    *   into location.
    */ 
   
   void dumpGeometry(const char* location);

   void toggleTriangulationStrategy();

   
   /**
    *   Changes the drawing context.
    *
    *   @param context       A context that should be used for drawing.   
    */
   
   void setDrawingContext( WFAPI::DrawingContext* context );
   
   /**
    *   Removes the route id from the TileMapHandler.
    *   @see setRouteID.
    */
   void clearRouteID();

   /**
    * Returns the internal map projection member.
    * 
    * @return Reference to map projection
    */ 
   
   MapProjection& getMapProjection();

   MapProjectionAdapter& getMapProjectionAdapter();

   /**
    * @return The stress tester associated with the TileMapHandler.
    */ 
   StressTester& getStressTester();
   
   /**
    *    Set the language for the maps 
    *    using the LangTypes::language_t type.
    */   
   void setLanguage( LangTypes::language_t lang );

   /**
    *    Returns the lanugage.
    */
   LangTypes::language_t getLanguage() const { return m_lang; }
   
   /**
    *    Set the language for the maps using the nav2 lang type.
    */   
   void setNav2Language( uint32 nav2Lang );

   /**
    *   Set the language using the specified iso639 string.   
    *   The string can consist of two or three letters and must be
    *   NULL terminated. 
    *   @param isoStr The string, containing two or three letters.
    *   @return   The language as iso639 that was set to MapLib.
    */
   const char* setLanguageAsISO639( const char* isoStr );

   /**
    *   Returns all information possible at the same time.
    *   More efficient than calling getNameForFeatureAt followed
    *   by getSelectionOutline etc.
    *
    *   @param highlightable  [Optional] If set to true, only
    *                         highlightable features will be
    *                         processed. Default is that
    *                         all feature types are processed.
    *   @param infoCallback   [Optional] Callback that will be called
    *                         when a missing string map is received.
    */
   void getInfoForFeatureAt( TileMapHandlerClickResult& res,
                             const MC2Point& point,
                             bool highlightable = false,
                             TileMapInfoCallback* infoCallback= NULL );
   
   /**
    *   Returns the name for an item at the specified point.
    *   @deprecated Use getInfoForFeatureAt instead.
    *   @param point    The coordinate to look at.
    *   @param distance Set to the distance to the found item in pixels
    *                   if not NULL.
    */
   const char* getNameForFeatureAt(const MC2Point& point,
                                   uint32* distance = NULL );
   
   /**
    *   Puts most probably only one id string to identify the
    *   object at the given coordinate.
    *   @deprecated Use getInfoForFeatureAt instead.
    *   @param strings The strings are put here. Send them to the server.
    *   @return The number of items found.
    */
   int getServerIDStringForFeaturesAt( std::vector<MC2SimpleString>& strings,
                                       const MC2Point& point );

   /**
    *   Set the user defined features that should be drawn after
    *   the map.
    */
   void setUserDefinedFeatures( std::vector<UserDefinedFeature*>* features );

   /**
    * Get the vector of user defined features.
    * 
    * @return The vector with all user defined features.
    */
   std::vector<UserDefinedFeature*>* getUserDefinedFeatures();

   /**
    *   Returns the poi categories or NULL if not loaded yet.
    *   Must _not_ be saved since the vector can disappear
    *   anytime after you have left your calling method.
    */
   const std::vector<const WFAPI::POICategory*>* getCategories() const;

   /**
    *   Synchronizes the layer info between the local copy
    *   and the copy of the ui.
    *   @return True if the info was updated and may need to be 
    *           re-displayed.
    */
   bool updateLayerInfo( TileMapLayerInfoVector& info );

   /**
    *   Show or hide a category with the specified id.
    *   Will not work correctly until getCategories returns non-NULL.
    *   @param id      The id of the parameter to disable/enable.
    *   @param visible True/false.
    *   @return True if the category id was found.
    */
   bool setCategoryVisible( int id, bool visible );

   /**
    *   Sets tracking mode on or off.
    *   On (true) means that all of the map is drawn at once, including the
    *   edges of the streets and that the texts are moved with the map
    *   to be updated correctly later.
    */
   //void setTrackingMode( bool on );
   
   /**
    *   Returns the layer id:s and descriptions. If the vector is NULL
    *   it means that the layers and descriptions have not arrived
    *   yet, try again later. The ids are the ids to use when
    *   choosing which layers to display.
    */
   const TileMapLayerInfoVector* getLayerIDsAndDescriptions() const;

   /**
    *   Returns the detail level for the layer.
    *   FOR DEBUG!
    */
   uint32 getDetailLevelForLayer(uint32 layerID) const;

   void setInterpolationDebugInfo( InterpolationDebugVector v );
   
   /**
    *   Returns the total map size for the maps loaded so far.
    *   Only for DEBUG!.
    */
   uint32 getTotalMapSize() const;

   void drawPoint( MC2Coordinate center,
                   unsigned int color,
                   unsigned int size);

   void drawPoint( MC2Point center,
                   unsigned int color,
                   unsigned int size);


   /**
    *   Toggles float accuracy check.
    *
    *   Only for DEBUG!
    */ 

   void toggleFloatAccuracyCheck();
   
   /**
    *   Dups the sizes of the maps to mc2dbg.
    *   Only for DEBUG!. 
    */
   void dumpMapSizes() const;

   /**
    *   Requests that the screen should be repainted soon.
    */
   void requestRepaint();

   /**
    *   Requests that the display should be repainted now.
    */
   void repaintNow();

   /**
    * Request that the map should not be redrawn ever, even if it thinks it
    * needs to redraw itself.
    * Typically used if the maps is in a background view and one want to make
    * sure it's not redrawn, that is make sure it does not use ui thread cpu
    * cycles.
    * The default state for this is enabled, until the user changes it.
    *
    * @param mapDrawingEnabled set to true if map should be drawn, false to disable drawing.
    */
   void setMapDrawingEnabled(bool mapDrawingEnabled);
   
   void repaintOverlayDialogs();

   void setMapDrawingCallback(WFAPI::MapDrawingCallback* callback);

   /**
    *  @see MapDrawingInterface::drawImageSpec
    */
   void drawImageSpec(const WFAPI::ImageSpec* spec,
                      const WFAPI::ScreenPoint& point);
   
   /**
    *   Sends the signal that we are ready to move to an
    *   interpolated position, if available. Typically called
    *   after drawing.
    */
   void requestPositionInterpolation();

   /**
    *   Called when the position interpolation request timer has
    *   expired.
    */ 

   void positionInterpolationTimerExpired();
   
   /**
    *   Puts the dimensions of the displayed map in the
    *   variables.
    */
   PixelBox getMapSizePixels() const;

   /**
    * Set the position of the copyright string of the map. 
    * @param   pos   The left baseline position of the copyright string.
    */
   void setCopyrightPos( const MC2Point& pos );
   
   /**
    * Set if to show copyright string.
    */
   void showCopyright( bool show );

   template<class VECTOR> void getAllRequesters( VECTOR& dest ) {
      if ( m_requester ) {
         m_requester->getAllRequesters( dest );
      }
   }

   /// @return If MapLib is initialized or not.
   bool isInitialized() const;
   
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

   bool canHandleScreenAsBitmap() const;
   void moveBitmap(int deltaX, int deltaY);
   void setPointBitmap( const MC2Point& screenPoint );
   void setBitmapDragPoint( const MC2Point& dragPoint );
   void zoomBitmapAtPoint( double factor, 
                           const MC2Point& screenPoint );
   void zoomBitmapAtCenter( double factor );

   /**
    * Set the startup listener.
    * 
    * @param startupListener Listener that will be called 
    * when maplib is started.
    */
   void setStartupListener(WFAPI::MapLibStartupListener* startupListener);

protected:
   /**
    *   Called by MapProjection to tell TileMapHandler that
    *   the matrix has been updated.
    */
   
   virtual void projectionUpdated(projectionEvent trigger);
public:
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
    *    Sends a TileMapEvent to all listeners.
    */
   void sendEvent( const TileMapEvent& event );
   
   /**
    * Returns true if the specified layer is already visible,
    * else false
    */
   bool isLayerVisible(int layerNo);

   /**
    *   sets the specified layer for display if it is not getting displayed,
    *   else sets it to not be displayed.
    */
   void toggleLayerToDisplay(int layerNo);

   /**
    * Set if a specific layer show be visiable or not.
    *
    * @param layerNo The layer to change.
    * @param visible If the layer should be shown or not.
    */
   void setLayerVisible(WFAPI::wf_uint32 layerNo, bool visible);

   /**
    * Set the update period for a layer.
    *
    * @param layerNo The layer to change.
    * @param updateIntervalMinutes The new update time for the layer in
    *                              minutes.
    */
   void setUpdatePeriodMinutes(WFAPI::wf_uint32 layerNo, 
                               WFAPI::wf_uint32 updateIntervalMinutes);

   /**
    *   Get's a reference to the screen coords for the feature.
    *   To be used by the textplacement.
    */
   VectorProxy<MC2Point>& getScreenCoords( TileMap& tileMap,
                                           TilePrimitiveFeature& feature );

   /**
    *    Get the outer pixel width of the polyline.
    */
   uint32 getPolylineOuterPixelWidth( 
                              const TilePrimitiveFeature& prim ) const;

   /**
    *    Set the pixelbox for the progress indicator text.
    */
   void setProgressIndicatorBox( const PixelBox& box );

   /**
    *    @return The DBufRequestListener.
    */
   DBufRequestListener* getDBufRequestListener() const;

   /**
    *   @return the interpolation hint consumer, which in our case
    *   is the position interpolator.
    */ 

   PositionInterpolator* getPositionInterpolator();

   /**
    *   Sets the callback that will receive new position related
    *   information from the interpolation, when available.
    */
   
   void setInterpolationCallback( InterpolationCallback* callback );

   /**
    *   Clears the callback previously set with setInterpolationCallback.
    */
   
   void clearInterpolationCallback( InterpolationCallback* callback );

   /**
    *   True if we are currently interpolating positions
    */ 

   bool getInterpolatingPositions() const;

   /**
    *   Sets if we should interpolate positions or not.
    *
    */ 
   
   void setInterpolatingPositions( bool enabled );
   
   /**
    *    Set timing info to display or null.
    */
   void setTMKTimingInfo( const TMKTimingInfo* info ) {
      m_tmkTimingInfo = info;
   }

   /**
    * Sets the bbox and sets routeID if route param str.
    *
    * @param paramString The tilemap param string.
    */
   void setBBoxFromParamStr( const MC2SimpleString& paramString );


   /**
    *    Get the next point that contains a feature that could be
    *    highlighted.
    *    @param   point [IN] The point to be set to the next highlightable
    *                   feature.
    *    @return  True if a highlightable feature was found and thus
    *             if the point was updated. False otherwise.
    */
   bool getNextHighlightPoint( MC2Point& point );

   /**
    *    Set if the map is moving or not.
    */
   void setMoving( bool moving );

   /**
    *    Set if to detect movement by self, or by someone using the
    *    setMoving method.
    */
   void setDetectMovementBySelf( bool detectSelf );

   /**
    * Turns the night mode on or off.
    **/
   void setNightModeL( bool aOn );

   /**
    * Returns whether the night mode is on (true) or off (false)
    *
    * @return True of night mode is on.
    */
   bool getNightMode();


   /// Temporary method that returns the 3D settings for modification.
   Settings3D& getSettings3D();

   /**
    * Turns 3d mode on or off.
    */
   void set3dMode( bool on );

   /**
    * @see ConfigInterface::setVariable3dMode
    */
   void setVariable3dMode(float step);

   /**
    * Turns outlines on in 3d mode.
    */
   void setOutlinesIn3dEnabled( bool enabled );

   /**
    * True if in 3d mode, false if not.
    */
   bool get3dMode( );

   bool forcedRedrawsEnabled() const;

   void setForcedRedraws(bool enabled);

   void cycleTextPlacementAlgorithm();
   
   /**
    * Returns the active drawing context that TMH uses to draw maps.
    */ 
   DrawingContextProxy::Type getDrawingContext();

   /**
    * True if in outlines are enabled in 3d mode, false if not.
    */
   bool getOutlinesIn3dEnabled( ) const;

   /**
    * Sets the horizon height that is displayed
    * when map is displayed in 3d. 
    * @param height The height of the horizon.
    */
   void setHorizonHeight( uint32 height );

   /**
    *  Get the ACP mode setting. True if ACP enabled, false if not.
    */
   bool isACPModeEnabled() const;

   /**
    * Set the ACP mode setting. True if ACP enabled, false if not.
    */
   void setACPModeEnabled( bool enable );

   const MC2SimpleString& getLatestProfilingResults();


   ExternalRenderer* getExternalRenderer();

   /**
    *   @return True if the map has been completely downloaded.
    */  
   bool mapFullyLoaded() const;

   /**
    * @return True if the tileMap is drawn, false otherwise.
    */
   bool isTileMapVisible(const TileMap* map) const;

   void hidePOIs(bool hidden);
   
private:
   
   friend class TileMapHandlerReleaseChecker;

   // REMOVE THIS LATER ON.
   friend class ExternalRenderer;
   
   /// Easiest way to solve this right now
   friend class MapLib;

   /// Easiest way to solve this right now
   friend class MapLibInternal;

   /// For evaluation of image based maps
   void initGrid();
   
   /// For use in MapLib.
   DBufRequester* getInternalRequester() {
      return m_requester;
   } 


   void updateTileMapLoader();

   
   void setFormatDesc(TileMapFormatDesc* desc);
   
   // Queries the renderer for the map size.
   
   void getMapSizeFromRenderer( isab::Rectangle& rect ) const;
   
   /**
    *   Converts a bit map desc from the server to a bitmap name.
    */
   MC2SimpleString bitMapDescToBitMapName(const MC2SimpleString& desc);

   /**
    *   Converts a bit map name to a desc to use when requesting from
    *   the server.
    */
   MC2SimpleString bitMapNameToDesc( const MC2SimpleString& name );

   /**
    *   Gets a bitmap from the internal storage and returns it if
    *   it is there. If it is not there, the function will add it to
    *   the queue of buffers to be fetched from the server and return
    *   NULL.
    */
   isab::BitMap* getOrRequestBitMap( const MC2SimpleString& name );

   /**
    *   Handles the loading and storing of an incoming bitmap.
    *
    *   @param   dataBuffer   The buffer containing the bitmap data.
    *   @param   descr        The bitmap description string.
    */
   void handleIncomingBitMap( BitBuffer* dataBuffer,
                              const MC2SimpleString& descr );

   /**
    *   Clears already loaded bitmaps. Uses m_drawingContext to
    *   determine which bitmap unloader to use.
    */
   void clearLoadedBitMaps();
   
   /**
    *   Request the cached TMFDs ([Dd]XXX, [Dd]YYY).
    */
   void requestCachedTMFD();

   /**
    *   Requests the desc from cache or by requesting it from
    *   the requester.
    *   @param desc     DataBuffer to send for.
    *   @param reqType  The type of source that the desc may be requested
    *                   from. Default is both cache or internet.
    *   @return True if the buffer was found in the cache.
    */
   bool requestFromCacheOrForReal(
                        const MC2SimpleString& desc,
                        const CachePolicy& reqType =
                                    CachePolicy::cacheOrInternet );

   /**
    *    Clear coordinates for the feature.
    */
   inline void clearCoords( TilePrimitiveFeature& feat );
   
   /**
    *   Prepares the coordinates of the feature to work with
    *   the current screen.
    */
   inline int prepareCoordinates(TileMap& tileMap,
                                 TilePrimitiveFeature& feat,
                                 const MC2BoundingBox& bbox,
                                 const MC2Point& centerOfBBox,
                                 int halfWorldPeriodPixels);
   
   /**
    *   Plots a feature containing polyline stuff.
    *   @param color     The color in rrggbb.
    *   @param lineWidth Line width in pixels.
    *   @return Number of coordinates drawn.    
    */
   inline int plotPolyLines(const TilePrimitiveFeature& feat,
                            uint32 color,
                            int lineWidth );

   /**
    *   Plots a polygon.
    *   @return Number of coordinates drawn.
    */
   inline int plotPolygon(const MC2BoundingBox& bbox,
                           TileMap& tilemap,
                          const TilePrimitiveFeature& feat,
                          uint32 color );
   
   /**
    *   Called by dataBufferReceived when the BitBuffer
    *   contains a tileMap.
    *    @param   removeFromCache   [OUT] Descs that are to be removed
    *                                     from the cache.
    *    @return  True if everything was ok. False if the map contained
    *             unknown features and therefore the mapdesc should be
    *             rerequested.
    */
   inline bool tileMapReceived(const MC2SimpleString& descr,
                               const TileMapParams& params,
                               TileMap* tileMap,
                               std::vector<TileMapParams>& removeFromCache );

   void tileMapLoaded(const MC2SimpleString& descr,
                      const TileMapParams& params,
                      TileMap* tileMap,
                      BitBuffer* buffer);
   /**
    *   Get which maps to plot.
    *   @param maps     [Out] The maps to plot.
    *   @return   True if the outparameter contains the maps to plot.
    *             False if m_mapVector should be used instead.
    */
   bool getWhichMapsToPlot( std::vector<TileMap*>& maps );

   /**
    *   @return If the feature is within the current scale.
    */
   int checkScale( const TilePrimitiveFeature& feature ) const;

   /**
    *    Draw the progess indicator.
    */
   void drawProgressIndicator();
   
   /**
    *   Plots the maps that we have already.
    */
   void plotWhatWeGot( );

   /**
    *   Plots the maps using the plotter paradigm of renderers.
    *
    */ 
   void plotWhatWeGotUsingPlotter();
 
   /**
    *   Plots the maps using the external paradigm of renderers.
    *
    */ 
   void plotWhatWeGotUsingExternal();
   
   /**
    *   Plots the maps in a certain bounding box only.
    */
   void plotWhatWeGotInBBox( const MC2BoundingBox& bbox,
                             int skipOutLines,
                             int lowQualityDrawing,
                             int& nbrFeatures,
                             int& nbrDrawn );


   /**
    *   Plots the texts using the internal MapPlotter.
    */
   
   void drawTextUsingPlotter();

   /**
    *   Draws the debug strings if enabled.
    */
   void drawDebugStrings();

   
   void drawDebugText( const char* text,
                       const MC2Point& position );

   /**
    *   Draws interpolation related information.
    */

   void drawInterpolationDebugInfo();
   
   /**
    *   Get the inner pixel width of the polyline. 
    */
   inline uint32 getPolylineInnerPixelWidth( 
                              const TilePrimitiveFeature& prim ) const;
   
   /**
    *   Plots a polygon or line feature.
    *   @return The number of coordinates plotted.
    */
   inline int plotPolygonOrLineFeature( const MC2BoundingBox& bbox,
                                        TileMap& tilemap,
                                        const TilePrimitiveFeature& prim,
                                        int pass,
                                        int moving );

   /**
    *   Converts the coordinates in the CoordArg of primWithCoordArg
    *   to screen coordinates.
    *   @return True if there were coordinates in the feature and they
    *           were inside the bbox.
    */
   inline bool getXYFromCoordArg( 
                         const TilePrimitiveFeature& primWithCoordArg,
                         const MC2BoundingBox& bbox,
                         MC2Point& p ) const;
   
   /**
    *   Plots a bitmap feature. Must have one coordinate.
    */
   inline int plotBitMapFeature( TileMap& curMap,
                                 TilePrimitiveFeature& prim,
                                 const MC2BoundingBox& bbox,
                                 int pass,
                                 int moving );
   /**
    *   Plots a circle feature. Must have one coordinate.
    */
   inline int plotCircleFeature( TilePrimitiveFeature& prim,
                                 const MC2BoundingBox& bbox,
                                 int pass,
                                 int moving );

   int plotBBox( const MC2BoundingBox& bbox,
                 const MC2BoundingBox& screenBBox );
   
   /**
    *   Plots a feature.
    */
   inline int plotPrimitive( TileMap& curMap,
                             TilePrimitiveFeature& prim,
                             const MC2BoundingBox& bbox,
                             const MC2Point& centerOfBBox,
                             int halfWorldPeriodPixels,
                             int pass,
                             int moving);
   
   /**
    *   Returns true if one of the coordinates in coords
    *   is inside the boundingbox.
    */
   inline static bool coordsInside(const CoordsArg& coords,
                                   const MC2BoundingBox& bbox);
     
   /**
    *   Returns true if the map with the given description
    *   is loaded.
    */
   inline bool haveMap(const MC2SimpleString& desc) const;

   /**
    *   Returns a pointer to the map with the given description
    *   or NULL if not loaded yet.
    */
   inline TileMap* getMap(const MC2SimpleString& desc) const;

   
   /**
    *   Returns true if new parameters have been created.
    *   @param oldParamsUpdated  [Out] If the old params also 
    *                            were updated.
    */
   bool getNewParams( bool& oldParamsUpdated );

   /**
    *   Updates the parameters so that they are ok for the
    *   current display etc.
    */
   void updateParams();

   /**
    *    Debug method. Dumps the params.
    */
   void dumpParams();
   
   /**
    *   Checks the cache for the existance of the descr
    *   and then calls dataBufferReceived if the requested
    *   data was in a cache.
    *   @param descr Descr to request.
    *   @return True if the buffer was in cache.
    */
   bool requestFromCacheAndCreate(const MC2SimpleString& descr);
   
   /**
    *   Requests maxNbr maps or other buffers.
    */
   void requestMaps(int maxNbr);

   // -- Geometry
   
   /**
    *    Returns the distance from the coordinate coord to the
    *    line described by the coordinates linefirst and
    *    lineSecond.
    */
   static int64 sqDistanceCoordToLine(const TileMapCoord& coord,
                                      const TileMapCoord& lineFirst,
                                      const TileMapCoord& lineSecond,
                                      float cosF );

   /**
    *    Returns the minimum distance from a coordinate to
    *    a primitive.
    */
   static int64 getMinSQDist(TileMap& tileMap,
                             const TileMapCoord& coord,
                             const TilePrimitiveFeature& prim,
                             float cosLat);

   /**
    *    Returns the closest feature and map for the given point.
    *    @param primType   [Optional] Default (MAX_INT32) is that
    *                      all primitive types are processed, 
    *                      but setting the param to a specific primitive
    *                      type (e.g. TilePrimitiveFeature::bitmap)
    *                      will lead to that only that primitive
    *                      type will be processed.
    */
   std::pair<const TileMap*, const TilePrimitiveFeature*>
      getFeatureAt(const MC2Point& points, uint32* distance = NULL,
                   int32 primType = MAX_INT32 );
   
   /**
    *   Get name for feature.
    *
    *   @param theMap         The map.
    *   @param theFeature     The feature to get the name of.
    *   @param infoCallback   [Optional] Callback that will be called
    *                         when the possibly missing string map is 
    *                         received.
    */
   const char* getNameForFeature( 
                               const TileMap& theMap,
                               const TilePrimitiveFeature& theFeature,
                               TileMapInfoCallback* infoCallback = NULL );
   
   /**
    *    Returns the first primitive that is inside a polygon.
    *    Goes through the levels top down.
    */
   std::pair<const TileMap*, const TilePrimitiveFeature*> getFirstInside(
      const TileMapCoord& coord) const;

   /**
    *    Get the bitmap's pixelbox. The method is not forgiving if
    *    submitting a non bitmap feature.
    */
   PixelBox getPixelBoxForBitMap( 
            const TilePrimitiveFeature& bitmap ) const; 
   
   /**
    *    Returns the closest feature or null.
    *    @param primType   MAX_INT32 means that
    *                      all primitive types are processed, 
    *                      but setting the param to a specific primitive
    *                      type (e.g. TilePrimitiveFeature::bitmap)
    *                      will lead to that only that primitive
    *                      type will be processed.
    */   
   inline const TilePrimitiveFeature*
      getClosest(TileMap& tileMap,
                 const MC2Point& point,
                 const MC2Coordinate& coord,
                 TileMap::primVect_t::const_iterator begin,
                 TileMap::primVect_t::const_iterator end,
                 int64& mindist,
                 int primType ) const;
  
   /// Plot the copyright string.
   void plotCopyrightString( const char* copyrightString );

   /// Plot the last copyright string.
   void plotLastCopyrightString();
      
   /// Force that the map is really redrawn from scratch on next repaint.   
   void forceRedrawOnNextRepaint();
   
   // -- Variables
   
   /**
    *   The MapPlotter can plot stuff, e.g. maps.
    */
   MapPlotter* m_plotter;

   /**
    * Points to the 2d plotter, normal map plotter.
    */
   MapPlotter* m_plotter2d;

   /**
    * Pointer to the 3d plotter.
    */
   PlotterAdapter3d* m_plotter3d;

   /**
    *   Pointer to an external renderer, perhaps an
    *   OpenGL ES-based module.
    */
   ExternalRenderer* m_extRenderer;

   /**
    *   Pointer to a platform-dependant image loader that
    *   can be used in conjunction with any external
    *   renderer.
    */ 
   PlatformImageLoader* m_imgLoader;
   
   /**
    *   The active drawing context.
    */
   DrawingContextProxy::Type m_drawingContext;

   /**
    *   The current projection, which determines how the
    *   world should be drawn (according to perspective etc)
    */
   
   MapProjection m_projection;
   
   /**
    *   The DBufRequester can request BitBuffers from
    *   server or e.g. a file using a string as key.
    */
   DBufRequester* m_requester;
   
   /**
    *    The current platform.
    */
   TileMapToolkit* m_toolkit;

   /**
    *   The MapFormatDescription.
    */
   TileMapFormatDesc* m_mapFormatDesc;
   
   /**
    *    The current databufferlistener.
    */
   DBufRequestListener* m_dataBufferListener;

   /**
    *    The text handler.
    */
   TileMapTextHandler* m_textHandler;
   
   /**
    *    The callback that is used when new map graphics
    *    are ready to be drawn to a surface.
    */ 
   WFAPI::MapDrawingCallback* m_mapDrawingCallback;
   
   /**
    *    The id of the timer of the textHandler.
    */
   uint32 m_textHandlerIdleID;
   
   /**
    *    If == 0 we are not painting.
    */
   int m_painting;
   
   /**
    *   Callback to use when databuffer is received.
    *   Will release the buffer back to the requester.
    *   @param descr       The received descr.
    *   @param dataBuffer  The received dataBuffer.
    *   @param requestMore True if another buffer should be reuqested. 
    */
   void dataBufferReceived(const MC2SimpleString& descr,
                           BitBuffer* dataBuffer,
                           bool requestMore = true);

   void tileMapBufferReceived(const MC2SimpleString& descr,
                              BitBuffer* dataBuffer,
                              bool requestMore = true);


   /**
    *
    *   
    *   Requests a idle object if the text handler wants one
    *   @return True if the text handler wants to run.
    */
   bool requestTextHandlerIdle();

   /// Draws the texts from the TileMapTextHandler.
   void drawTexts();

   /// Draws one UserDefinedFeature and updates the coords of it
   inline void drawOneUserDefinedFeature( UserDefinedFeature& userFeat );
   
   /// Draws the user defined features if there are any.
   void drawUserDefinedFeatures();
   
   /// Plots some debug on the screen.
   void printDebugStrings(const std::vector<MC2SimpleString>& strings,
                          const MC2Point& pos );
   void printDebugStrings(const std::vector<MC2SimpleString>& strings);

   /// Draw fps debug info.
   void drawFps();

   /// Draw debug information for text placement
   void drawTextPlacementInfo();
   
   /// Update the copyright handler with data from tmfd.
   void updateCopyrightHandler( const TileMapFormatDesc& tmfd ); 
   
   /// Pointer to the outline feature
   DirectedPolygon* m_outlinePolygon;

   /**
    *    Time when the stuff was repainted last time.
    */
   uint32 m_lastRepaintTime;
   
   /// Type of the map where we store our bitmaps.
   typedef std::map<MC2SimpleString, isab::BitMap*> bitMapMap_t;
   
   /**
    *   Map containing the bitmaps for e.g. pois.
    *   The leading 'B' has been removed and also the extension.
    */
   bitMapMap_t m_bitMaps;

   /**
    *   Map containing the bitmaps needed.
    *   The leading 'B' has been removed.
    */
   std::set<MC2SimpleString> m_neededBitMaps;
   
   /**
    *    True if gzipped maps should be used.
    */
   bool m_useGzip;

   /* Only for debug */
   
   bool m_floatAccuracyCheck;

   /* The maximum number of points loaded */
   uint32 m_maxNumPoints;
   
   /**
    *    The id of the repaint timer if a repaint timer
    *    has been requested. Else 0.
    */
   uint32 m_repaintTimerRequested;
   
   /**
    *    The id of the detail repaint timer if a detail repaint timer
    *    has been requested. Else 0. Used for filling in missing details
    *    after a while of non-movement.
    */
   uint32 m_detailRepaintTimerRequested;

   /**
    *    The id of the position interpolation request timer.
    */

   uint32 m_positionInterpolationRequested;

   /**
    *  The id of the tilemap logic driven loader.
    */ 
   uint32 m_tileMapLoaderTimerRequested;
   
   /**
    *    The callback that will receive new information from
    *    the interpolator, when available.
    */

   InterpolationCallback* m_interpolationCallback;

   /**
    * The overlay view containing layers of overlay items.
    */ 
   OverlayView* m_overlayView;

   /**
    * Our notifier class for selected map objects
    */ 
   SelectedMapObjectNotifier* m_selectedMapObjectNotifier;

   /**
    * Our notifier class for selected map objects
    */ 
   MapLibSettingsNotifier* m_settingsNotifier;

   /**
    *    True if there has been time for the detail repaint timer
    *    to run.
    */
   int m_detailRepaintTimerHasBeenRun;

   /**
    *    The id of a timer that will repaint the screen
    *    after the last received map in a batch.
    */
   uint32 m_repaintAfterMapReceptionTimer;

   /// Timer that handles re-requests of maps that have not arrived.
   uint32 m_rerequestTimer;
   /// Time for the re-request timer
   uint32 m_rerequestTimerValue;
   enum {
      /// Minimum value for the re-request timer
      c_minRerequestTimerValue = 7500,
      /// Maximum value for the re-requsst timer.If above, it will be shut down
      c_maxRerequestTimerValue = 5*60*1000,
   };
   
   /// True if the transformmatrix has changed since the last draw.
   int m_matrixChangedSinceDraw;
   
   /**
    *    The current detaillevel.
    */
   int m_detailLevel;

   /// Set of layers to display (e.g. map, route, pois)
   std::set<int> m_layersToDisplay;

   /// The route id or NULL.
   RouteID* m_routeID;

   /// The current language
   LangTypes::language_t m_lang;  

   /// The overlap detector for bitmaps.
   OverlapDetector<PixelBox> m_overlapDetector;

   /// The current scale index.
   int m_scaleIndex;

   /// Random characters to put in request for DXXX
   MC2SimpleString m_randChars;

   /// List of debug printouts, a character in first and a time in second.
   std::list<std::pair<char, uint32> > m_drawTimes;

   /// Pointer to the list of user defined features.
   std::vector<UserDefinedFeature*>* m_userDefinedFeatures;

   MC2Coordinate m_lastInterpolatedCenter;
   
   /// The garbage collector.
   TileMapGarbage<TileMap> m_garbage;

   /// The maps.
   TileMapContainer* m_tileMapCont;

   /// True if tracking mode is on
   int m_trackingModeOn;

   /// Set of features that are disabled for drawing.
   std::set<int32> m_disabledParentTypes;

   /// Event listener to inform e.g. when DXXX has arrived
   std::vector<TileMapEventListener*> m_eventListeners;

   /// True if highlight should be shown.
   int m_showHighlight;
   
   /// Highlight point
   MC2Point m_highlightPoint;
   
   /// This class should be able to call the Handler when DB is recv.
   friend class TileMapHandlerDefaultDBListener;
   /// This class needs the m_mapFormatDesc
   friend class TileMapHandlerDBBufRequester;
   
   /// Screen coords for the currently drawn feature.
   std::vector<MC2Point> m_realScreenCoords;
   VectorProxy<MC2Point> m_screenCoords;

   /// The bitbuffer of the last loaded DXXX ( or null )
   BitBuffer* m_dxxBuffer;
   /// True if the crc for the DXXX has been received
   int m_descCRCReceived;
   /// Server's version of the crc
   uint32 m_serverDescCRC;

   /// The pixel box that the progress indicator text should be drawn into.
   PixelBox m_progressIndicatorBox;

   /// Current timing info from TileMapKeyHandler
   const TMKTimingInfo* m_tmkTimingInfo;

   /// True if a map has arrived since the map was repainted
   bool m_mapArrivedSinceRepaint;
   
   /// True if the outlines were drawn last time
   bool m_outlinesDrawnLastTime;
   
   /// Screen size the last time the screen was drawn
   isab::Rectangle m_lastScreenSize;
   
   /// Movement helper.
   MovementHelper m_movementHelper;

   /// Copyright string position.
   MC2Point m_copyrightPos;
   
   /// Release checker
   TileMapHandlerReleaseChecker* m_releaseChecker;
   
   /// If to show copyright.
   bool m_showCopyright;
   
   /// If in night mode.
   bool m_nightMode;
   
   /// If in 3d mode
   bool m_3dOn;

   /// If we should interpolate positions
   bool m_interpolatingPositions;
   
   /// If true, draw outlines in 3d mode.
   bool m_outlinesIn3dEnabled;

   /// The height of the horizon
   uint32 m_horizonHeight;

   /// Clickinfo to return.
   TileMapHandlerClickResult* m_clickInfo;

   /**
    *    Keeps track if we requested a string map due to a call of
    *    getNameForFeature.
    * 
    *    first is the string map desc that we are waiting for.
    *    second is the callback class that should be called once this desc
    *    is received. If NULL then we are not waiting for any info strings.
    */
   std::pair<MC2SimpleString, TileMapInfoCallback*> m_waitingForInfoString;

   /**
    *    The bitmap mover helper class.
    */
   BitmapMovementHelper m_bitmapMover;
 
   /**
    *    The copyright handler.
    */
    CopyrightHandler m_copyrightHandler;

   /**
    *    How many times the requesting maps parts of requestMaps() 
    *    have been entered.
    */
   int m_inRequestingMaps;
   
   /// Used for getCenter() which returns a Coord reference.
   MC2Coordinate m_tmpCoord;

   /// Time since last redraw, used to calculate fps.
   uint32 m_lastDrawTimeStamp;

   /**
    * The ACP mode setting. True if ACP enabled, false if not.
    */
   bool m_acpModeEnabled;

   /**
    * If this is true, redraws will happen unconditionally.
    */ 
   bool m_forceRedraws;
   
   /**
    *  An interpolator that can be used to smooth out position
    *  changes.
    */
   
   PositionInterpolator* m_posInterpolator;
   
   MapLibOperationHandler* m_operationHandler;

   /// Our translator of map operations. Should be the last stop of any
   /// map operation calling.
   MapProjectionAdapter* m_operationTranslator;
   
   /// The startup listener.
   WFAPI::MapLibStartupListener* m_startupListener;

   /// The manipulator that external parties can use to control the view of
   /// the map.
   MapManipulator* m_mapManipulator;
   /// Used to offload the loading of maps
   TileMapLoader* m_tileMapLoader;

   /// Used to stress the map drawing system
   StressTester* m_stressTester;

   /// Used to avoid heap allocating platform specific strings all the time
   StringCache* m_stringCache;
   
   /// The scale index that was used during the last TileMap updating run
   uint32 m_prevLoadingScaleIndex;
   float m_prevLoadingScale;

   // True when the first draw has been done.
   bool m_firstDrawDone;

   // True if the POI layer (and acp layer) should not be drawn.
   bool m_hidePOILayers;

   // True if the map is allowed to redraw itself
   bool m_mapDrawingEnabled;

   GridStorage<ImageTile>* m_imgTiles;
};

#endif // TILE_MAPHANDLER_H
