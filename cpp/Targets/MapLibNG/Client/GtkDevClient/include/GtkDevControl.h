/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _GTKDEVCONTROL_H_
#define _GTKDEVCONTROL_H_

// Forward declarations.
namespace WFAPI {
class MapLibAPI;
class GtkDrawer;
#ifdef USE_OPENGL_ES
class GtkGLESDrawer;
#endif
class MapLibNetworkContext;
class ImageSpec;
class OverlayItemZoomSpec;
class OverlayItem;
class DBufConnection;
class ExternalKeyConsumer;
class ScreenPoint;
class WGS84Coordinate;
}

#if ((defined MAEMO4) || (defined MAEMO5))
 #include <hildon/hildon-window.h>
 #include <hildon/hildon-program.h>
#endif

#include "MapDrawingCallback.h"
#include "PALMachineTypes.h"
#include <gtk/gtk.h>
#include "Shared/MapLibKeyInterface.h"
#include "Shared/ScreenPoint.h"
#include "Shared/SelectedMapObjectListener.h"
#include "Shared/MapLibSettingsListener.h"
#include <map>

/**
 *   GtkDevControl is an example class which shows how to integrate the
 *   MapLib into an application. For the real application, a similar
 *   class like this one must be created.
 *
 *   This class is implemented using the GDK interface
 *   to MapLib. Handles key events and draws the map to a GDK
 *   window.
 *
 *   Will create GDK windows and instantiate MapLib.
 *   
 */ 
class GtkDevControl : public WFAPI::SelectedMapObjectListener,
                      public WFAPI::MapLibSettingsListener,
                      public WFAPI::MapDrawingCallback {
public:   
   /**
    *  Constructor.
    *
    *  @param pathToResources Path to resource files.
    *  @param connection Optional DBufConnection to supply to MapLib.
    *                    May come from Nav2API.
    *  @param keyConsumer Optional external key consumer which
    *                     can handle certain key values.
    */
   GtkDevControl(const char* pathToResources = "images",
                 WFAPI::DBufConnection* connection = NULL,
                 WFAPI::ExternalKeyConsumer* keyConsumer = NULL);

   /**
    *  Destructor.
    */
   virtual ~GtkDevControl();

   /**
    *   Handles GDK key events and performs actions on
    *   the map if so defined.
    *
    *   @param key   The key event to handle. Can be NULL.
    *   @param text  In case key event is NULL, 
    *                then first char in text should be handled.
    *                Will be NULL if key is non NULL.
    *   @param length Length of text. 0 If text is NULL.
    *   @return If the key was consumed.
    */
   bool handleKey( GdkEventKey* key,
                   const gchar *text, gint length );

   /**
    *  When this is called, we will trigger the expose event.
    */
   virtual void mapIsReadyToBeDrawn();
   
   /**
    *   Called whenever the application needs to redraw the
    *   map area.
    */
   void expose( GdkEventExpose* event );

   /**
    *   Called when the GDK environment is set up.
    */ 
   void realize();
   
   /**
    * Start drag map mode..
    * @param button of the drag device.
    * @param pos position of the drag device.
    */
   void startDrag( int button, const WFAPI::ScreenPoint& pos );

   /**
    * End drag map mode.
    * @param button of the drag device.
    * @param pos position of the drag device.
    */
   void endDrag( int button, const WFAPI::ScreenPoint& pos );

   /**
    * Continue drag map to position
    * @param button button of the drag device 
    * @param pos position of the drag device
    */
   void drag( int button, const WFAPI::ScreenPoint& pos );

   /**
    * Zooms into the rect that was drawn by the user using
    * the mouse.
    * @param point if starting to zoom this will be top left of zoom rect,
    *              if already zooming this point will be bottom right.
    */
   void zoomRectangle( const WFAPI::ScreenPoint& point );

   /**
    * Zooms in and out
    * @param in true if zoom in else zoom out
    */
   void zoom( bool zoomIn );

   /**
    * Toggles between 2d and 3d mode.
    */
   void toggle3dMode();

   /**
    * Toggles between day and night mode.
    */
   void toggleNightMode();

   /**
    * To close down application
    */
   void quitApp();

   /**
    * To set drawing context to NONE and delete the drawer.
    */
   void deleteDrawer();

   /**
    * Toggle between different map scale adjustments levels.
    */
   void changeMapScaleAdjustment();

   /**
    * Zooms to the given point.
    * @param pos the new center point
    */ 
   void zoomToPoint( const WFAPI::ScreenPoint& pos );

   /**
    * Simple factory function that requests a reapaint
    * of the map.
    */
   void requestRepaint();

   /**
    * Called when a button was released. In zoom mode
    * the pixel pox is set to the zoom rects top left and bottom 
    * right and the map is redrawn.
    */
   void releaseButton();
   
   /**
    * Called whenever a map object has been selected.
   *
    * @see WFAPI::SelectedMapObjectListener::handleSelectedMapObject
    */ 
   virtual void handleSelectedMapObject(
      const WFAPI::MapObjectInfo& mapObjectInfo,
      WFAPI::OverlayItem* overlayItem,
      const WFAPI::WGS84Coordinate& coord,
      bool longPress);

   /**
    * Called whenever a stacked dialog has been opened.
    *
    * @see WFAPI::SelectedMapObjectListener::handleStackedDialogOpened
    */
   virtual void handleStackedDialogOpened();

   /**
    * Called whenever a stacked dialog has been closed.
    *
    * @see WFAPI::SelectedMapObjectListener::handleStackedDialogClosed
    */
   virtual void handleStackedDialogClosed();

   /**
    * Called when MapLib settings have changed.
    *
    * @see WFAPI::MapLibSettingsListener::handleUpdatedPOICategories
    */
   virtual void handleUpdatedPOICategories();

   /**
    * Get the MapLib API.
    */
   WFAPI::MapLibAPI* getMapLibAPI();

   /**
    * Set the current street during navigation.
    *
    * @param info The current street.
    */
   void setCurrentStreet(const char* info);

   bool isRunning() const;
   /**
    * Set the distance to next turn during navigation.
    *
    * @param info The distance to next turn.
    */
   void setDistance(const char* info);

   /**
    * Update the position of the me marker on the map.
    *
    * @param coord The world coordinate to set the me marker to.
    * @param screenPos The screen position that the me marker should
    *                  be shown at when tracking to the me marker.
    * @param heading The heading of the gps (in degrees). 
    *                Will rotate the map to this heading if < 360 and
    *                if tracking to the me marker.
    * @param routeCoord The world coordinate to set the snapped me marker to.
    * @param routeHeading The heading of the gps (in degrees). 
    *                     Will rotate the map to this heading if < 360 and
    *                     if tracking to the snapped me marker.
    * @param updateZoomLevel If the zoomlevel should be updated to the
    *                        zoomlevel parameter. Will only be done when
    *                        tracking to the me marker.
    * @param zoomLevel The zoomlevel to update to (if updateZoomLevel == true
    *                  and m_centerOnMeMarker == true).
    */
   void setMeMarkerPosition(const WFAPI::WGS84Coordinate& coord,
                            const WFAPI::ScreenPoint& screenPos,
                            int heading,
                            const WFAPI::WGS84Coordinate& routeCoord,
                            int routeHeading,
                            bool updateZoomLevel,
                            double zoomLevel);
   
   /**
    * Centers the map on the me marker.
    * When the setMeMarkerPosition is called, the map will continue to
    * be centered on the me marker according to the description supplied
    * to setMeMarkerPosition. The map will automatically center on the me
    * marker until the user moves around. This function will then need to
    * be called again.
    */
   void centerOnMeMarker();

   /**
    * Stops the centering of the map on the me marker.
    */
   void stopCenteringOnMeMarker();
   
   /**
    * Get the current GtkWindow we're in
    *
    * @return our GtkWindow
    *
    */
   GtkWindow* getWindow();


   
private:
      
   enum LayerId { FRIENDS_LAYER = 0,
                  LOCATIONS_LAYER,
                  SCREEN_LAYER,
                  ME_LAYER};

   enum DrawerType {
      NONE,
      NATIVE_GTK,
#ifdef USE_OPENGL_ES
      OPENGL_ES
#endif
   };
   
   /**
    * Toggles the visibility status of a layer.
    *
    * @param id The layer to toggle.
    */ 
   void toggleLayerVisibility(LayerId id);

   /**
    * Initializes shared overlay resources, such as zoom settings.
    */ 
   void initOverlay();
   
   /**
    * Initializes our layer with friends.
    */ 
   void initFriendsLayer();

   /**
    * Initializes our layer with locations.
    */ 
   void initLocationsLayer();

   /**
    * Initializes the layer with the Me-marker.
    */
   void initMeLayer();

   /**
    * Initializes raw screen components (not overlay items).
    */ 
   void initScreenComponents();
   
   /**
    *   Example function that draws static overlay components, 
    *   fixed on the screen, on top of the map.
    *   This example draws a crosshair on the screen.
    *
    *   @param  gc   The graphics context to draw upon.
    */
   void drawOverlay( GdkGC* gc );
   
   /**
    *   Example function that uses MapLib interfaces to set the 
    *   current map position to Stockholm, Sweden.
    */
   void showStockholm();

   /**
    * Creates the visual specification for stacked dialogs.
    */
   void createStackedDialog();

   /**
    *   Example function that draws ImageSpecs.
    */ 
   void drawInternalOverlay();
   
   /**
    *   Initializes our GDK window and map component.
    */
   void initWindow();

   /**
    *   Helper function that switches the current drawertype.
    */
   void toggleDrawer();
   
   /**
    *   Initializes MapLib with a drawing context and a buffer
    *   connection.
    *
    *   @param connection Optional DBufConnection to supply to MapLib.
    *                     May come from Nav2API.
    */ 
   void initMapLib(WFAPI::DBufConnection* connection = NULL);

   /**
    *   Initializes the key map which is used to translate native
    *   GDK key events to MapLib action events.
    */
   void initKeyMap();

   /**
    *   Translates a GDK key type to a MapLib key type.
    *
    *   @param   type   The key event 
    */ 
   WFAPI::MapLibKeyInterface::kindOfPressType translateKeyType( GdkEventType type );

   /**
    * Do the actual drag action.
    * @param button the button of the drag event.
    * @param upordown up, down or repeat mode.
    * @param pos position of the drag event.
    */
   void dragAction( int button, int upordown, const WFAPI::ScreenPoint& pos );

   /**
    * Prints the zoom rect to the screen
    */
   void drawZoomRectangle();

   /**
    * Private helper struct used for zooming to a rectangle.
    */
   struct ZoomRectangle {
      ZoomRectangle():
         m_topLeft( 0, 0 ),
         m_bottomLeft( 0, 0 ) { 
      }

      void setTopLeft( const WFAPI::ScreenPoint& pos ) {
         m_topLeft = pos;
      }
      void setBottomRight( const WFAPI::ScreenPoint& pos ) {
         m_bottomLeft = pos;
      }

      const WFAPI::ScreenPoint& getTopLeft() const {
         return m_topLeft;
      }

      const WFAPI::ScreenPoint& getBottomRight() const {
         return m_bottomLeft;
      }

   private:
      WFAPI::ScreenPoint m_topLeft;
      WFAPI::ScreenPoint m_bottomLeft;
   } m_zoomRect; /// the rectangle to zoom to when exiting zoom mode

   /// whether if we are in zoom mode or not
   bool m_zoomMode;
   
   /// The map that stores the lookup information for the key
   /// translation.
   typedef std::map<int, WFAPI::MapLibKeyInterface::keyType> KeyMap;
   KeyMap m_keyMap;
   /// Our two GDK components. The window consists of a map display which
   /// covers the entire window.
   GtkWidget* m_mapArea;
#if ((defined MAEMO4) || (defined MAEMO5))
   HildonProgram* m_program;
   HildonWindow* m_window;
#else
   GtkWidget* m_window;
#endif
   /// fullscreen toggle bool
   bool m_fullscreenMode;
   bool m_running;

   /// Bool indication if the map is visible.
   bool m_isVisible;

   /// Bool indicating if the map should be centered on the me marker
   /// when setMeMarkerPosition is called.
   bool m_centerOnMeMarker;
   
   /// The MapLibAPI class.
   WFAPI::MapLibAPI* m_mapLib;
   /// Drawing interface to maplib using Gtk.
   WFAPI::GtkDrawer* m_drawer;

#ifdef USE_OPENGL_ES
   /// Drawing interface to maplib using OpenGL ES
   WFAPI::GtkGLESDrawer* m_glesDrawer;
#endif
   
   /// The drawertype currently used.
   DrawerType m_curDrawer;
   
   
   /// The network connection interface to maplib 
   /// (from where maps are fetched).
   WFAPI::MapLibNetworkContext* m_networkContext;

   /// The zoom specification used for our overlay items
   WFAPI::OverlayItemZoomSpec* m_zoomSpec;

   /// A sample overlay item that will be removed.
   WFAPI::OverlayItem* m_itemToBeRemoved;
   
   /// The external key consumer. May be NULL.
   WFAPI::ExternalKeyConsumer* m_externalKeyConsumer;

   /// Path to resources.
   char* m_pathToResources;

   /// The turn label.
   GtkWidget* m_currentStreetLabel;

   /// The distance to next turn label.
   GtkWidget* m_distanceLabel;
   
   /// Containing area for the mapArea.
   GtkWidget* m_mapAreaContainer;

   /// The me marker overlay item.
   WFAPI::OverlayItem* m_meMarker;

   /// The me marker overlay item, showing the snapped position.
   WFAPI::OverlayItem* m_meMarkerSnapped;

   /// Bool for toggle between different map scale adjustment settings.
   bool m_mapScaleAdjustmentToggle;

   /// Our sample image specification.
   WFAPI::ImageSpec* m_testImg;

   /// Our sample image specification.
   WFAPI::ImageSpec* m_testText;
};

#endif /* _GTKMAPCONTROL_H_ */
