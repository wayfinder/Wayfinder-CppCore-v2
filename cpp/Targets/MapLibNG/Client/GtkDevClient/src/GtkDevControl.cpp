/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "GtkDevControl.h"
#include "Unix/GtkFactory.h"
#include "Unix/GtkDrawer.h"
#include "Shared/ImageSpec.h"
#include "Shared/ConfigInterface.h"
#include "Shared/DetailedConfigInterface.h"
#include "Shared/ScaleConfigInterface.h"
#include "Shared/ExternalKeyConsumer.h"
#include "Shared/MapLibInitialConfig.h"
#include "Shared/MapLibAPI.h"
#include "Shared/MapLibNetworkContext.h"
#include "Shared/WGS84Coordinate.h"
#include "Shared/ScreenPoint.h"
#include "Shared/MapOperationInterface.h"
#include "Shared/MapDrawingInterface.h"
#include "Shared/MapObjectInfo.h"
#include "Shared/MapObjectInterface.h"
#include "Shared/OverlayItemVisualSpec.h"
#include "Shared/OverlayItemZoomSpec.h"
#include "Shared/OverlayInterface.h"
#include "Shared/OverlayItem.h"
#include "Shared/StackedDialogVisualSpec.h"
#include "Shared/ImageSpec.h"
#include "Shared/WFString.h"
#include "PALGraphics.h"
#include <gdk/gdkkeysyms.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "GtkUtil.h"
#include "OverlayZoomSetup.h"
#include <cairo.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

//%#ifndef EXTERNAL_CUSTOMER
#include "InternalConfig.h"
//%#endif



#ifdef USE_OPENGL_ES
#include "Unix/GtkGLESFactory.h"
#include "Unix/GtkGLESDrawer.h"
#endif

#ifndef TILEMAPSPATH
# define TILEMAPSPATH "/tmp/tilemaps"
#endif

using namespace std;
using namespace WFAPI;

template <typename T>
inline ScreenPoint makePoint( const T* event ) {
   return ScreenPoint( static_cast<wf_int32>( event->x ),
                       static_cast<wf_int32>( event->y ) );
}

GtkDevControl::GtkDevControl(const char* pathToResources, 
                             WFAPI::DBufConnection* connection,
                             WFAPI::ExternalKeyConsumer* keyConsumer) :
   m_zoomMode(false),
   m_isVisible(true),
   m_centerOnMeMarker(true),
   m_mapScaleAdjustmentToggle(false),
   m_testImg(NULL),
   m_testText(NULL)
{
   m_curDrawer = NONE;
   m_zoomSpec = NULL;
   m_window = NULL;
   m_mapLib = NULL;
   m_drawer = NULL;
   m_mapArea = NULL;
   m_running = true;
   m_networkContext = NULL;
   m_itemToBeRemoved = NULL;
   m_externalKeyConsumer = keyConsumer;

   m_currentStreetLabel = NULL;
   m_distanceLabel = NULL;
   m_meMarker = NULL;
   m_meMarkerSnapped = NULL;
   m_mapAreaContainer = NULL;

   m_pathToResources = new char[strlen(pathToResources)+1];
   strcpy(m_pathToResources, pathToResources);
   m_fullscreenMode=0;

   // Initialize key mappings.
   initKeyMap();

   // Initialize Gtk windows.
   initWindow();

   // Initialize MapLib.
   initMapLib(connection);

   // Initialize shared overlay properties
   initOverlay();
   
   // Create the stacked dialogs.
   createStackedDialog();

   // showStockholm();

   m_mapLib->getConfigInterface()->
      getDetailedConfigInterface()->enableAutomaticHighlight(true);

// MapLibKeyInterface* keyInterface = m_mapLib->getKeyInterface();
   
   // keyInterface->handleKeyEvent(MapLibKeyInterface::INTERNAL_TRIGGER_KEY_2,
   //                              MapLibKeyInterface::KEY_DOWN_EVENT);
}

MapLibKeyInterface::kindOfPressType
GtkDevControl::translateKeyType( GdkEventType type )
{
   // Translate native GDK key event types to MapLib key types 
   // (key up, key down, etc).
   switch( type ) {
   case GDK_KEY_PRESS:
      return MapLibKeyInterface::KEY_DOWN_EVENT;
      break;
   case GDK_KEY_RELEASE:
      return MapLibKeyInterface::KEY_UP_EVENT;
      break;
   default:
      return MapLibKeyInterface::KEY_UNKNOWN_EVENT;
      break;
   }
}

bool GtkDevControl::handleKey( GdkEventKey* keyEvent, 
                               const gchar *text, gint length )
{
   if(!m_running) {
      return false;
   }
   
   guint gdkKey;
   GdkEventType gdkKeyType;

   if (keyEvent == NULL) {
      gdkKey = text[0];
      gdkKeyType = GDK_KEY_PRESS;
   } else {
      gdkKey = keyEvent->keyval;
      gdkKeyType = keyEvent->type;
   }

   // Check if an external key consumer is present and let it try first.
   if (m_externalKeyConsumer != NULL &&
       (translateKeyType( gdkKeyType )  == 
        WFAPI::MapLibKeyInterface::KEY_DOWN_EVENT) &&
       m_externalKeyConsumer->handleKey(gdkKey)) {
      return true;
   }
   // Else let MapLib handle it

   // Get the key interface from MapLib.
   MapLibKeyInterface* keyInterface = m_mapLib->getKeyInterface();
   
   // Look up in the key map wich action on the map that should be performed
   // based on the supplied key code (refer to initKeyMap method).
   KeyMap::iterator itr = m_keyMap.find( gdkKey );
   
   if( itr != m_keyMap.end() ) {
      // Key was found in key map and desired action is defined 
      // by the key object.
      MapLibKeyInterface::keyType key = itr->second;
      
      MapLibKeyInterface::kindOfPressType type =
         translateKeyType( gdkKeyType );

      bool consumed = keyInterface->handleKeyEvent( key, type );

      if (consumed){
         // Maplib consumed the key and interacted with the map,
         // so we stop centering on the me marker.
         stopCenteringOnMeMarker();
      }
      
      return consumed;
   } else {
      // Other key handling, if not found in key map. We only
      // respond to up events.

      if(gdkKeyType != GDK_KEY_PRESS) {
         return false;
      }
      
      switch ( gdkKey ) {
      case GDK_2:
         m_mapLib->getConfigInterface()->set3dMode(false);
         break;
      case GDK_3:
         m_mapLib->getConfigInterface()->set3dMode(true);
         break;
       case GDK_a:
      {
         WFAPI::DetailedConfigInterface* detailedConfig =
            m_mapLib->getConfigInterface()->getDetailedConfigInterface();
         WFAPI::POICategories cat = detailedConfig->getPOICategoriesSnapshot();
         if (cat.size() > 0){
            cat[0].setVisible(!cat[0].isVisible());
         }
         detailedConfig->synchronizePOICategories(cat);
         requestRepaint();
      }
      break;
      case GDK_r:
         if (m_itemToBeRemoved) {
            OverlayInterface* overlayInterface =
               m_mapLib->getOverlayInterface();
            
            overlayInterface->removeOverlayItem(m_itemToBeRemoved,
                                                SCREEN_LAYER);
            m_itemToBeRemoved->removeReference();
            m_itemToBeRemoved = NULL;
            requestRepaint();
         }
         break;
      case GDK_T:
         if (m_itemToBeRemoved) {
            OverlayInterface* overlayInterface =
               m_mapLib->getOverlayInterface();
            
            overlayInterface->clearLayer(SCREEN_LAYER);
            m_itemToBeRemoved->removeReference();
            m_itemToBeRemoved = NULL;
            requestRepaint();
         }
         break;
      case GDK_i:
         toggleLayerVisibility(FRIENDS_LAYER);
         requestRepaint();
         break;
      // case GDK_l:
      //    toggleLayerVisibility(LOCATIONS_LAYER);
      //    requestRepaint();
      //    break;
      case GDK_m:
      case GDK_F7:
         m_mapLib->getMapOperationInterface()->zoom(1/1.25);
         requestRepaint();
         break;
      case GDK_n:
      case GDK_F8:
         m_mapLib->getMapOperationInterface()->zoom(1.25);
         requestRepaint();
         break;
      case GDK_l:
      {
         MapOperationInterface* i =
            m_mapLib->getMapOperationInterface();
         
         WGS84Coordinate coord = i->getCenter();

         std::cout << i->getZoomLevel() << std::endl;
         std::cout << coord.latDeg << " " << coord.lonDeg << std::endl;
      }
      break;
      case GDK_s:
         // Example of how to center map around Stockholm, Sweden.
         showStockholm();
         break;
      case GDK_V:
         // Hide.
         if (m_isVisible){
            std::cout << "The map is set to hidden mode" << std::endl;
            m_isVisible = false;
            g_object_ref(m_mapArea);
            gtk_container_remove(GTK_CONTAINER(m_mapAreaContainer),
                                 m_mapArea );
            //         gtk_widget_hide(m_mapArea);
         }
         break;
      case GDK_B:
         // Unhide.
         if (!m_isVisible){
            std::cout << "The map is set to visible mode." << std::endl;
            m_isVisible = true;
//         gtk_widget_show(m_mapArea);
            gtk_container_add(GTK_CONTAINER(m_mapAreaContainer), m_mapArea);
            g_object_unref(m_mapArea);
         }
         break;
      case GDK_w:
         // Force repaint.
         std::cout << "Repaint" << std::endl;
         m_mapLib->getMapDrawingInterface()->requestRepaint();
         break;

      case '.':
         std::cout << "Stop key actions." << std::endl;
         m_mapLib->getKeyInterface()->stop();
         break; 
      case GDK_e:
         // Quit app.
         m_running = false;
         gtk_main_quit();
         break;

      case GDK_b: {
         //%#ifndef EXTERNAL_CUSTOMER
         InternalConfig* config = m_mapLib->getInternalConfig();
         config->cycleTextPlacementPosition();
         //%#endif
         } break;
      case GDK_v: {
         //%#ifndef EXTERNAL_CUSTOMER
         InternalConfig* config = m_mapLib->getInternalConfig();
         config->cycleTextPlacementAlgorithm();
         //%#endif
      } break;
      case GDK_F6:
      case GDK_6:
         // Make app fullscreen/unfullscreen.
         if( m_fullscreenMode ) {
            gtk_window_unfullscreen(GTK_WINDOW(m_window));
            m_fullscreenMode = 0;
            requestRepaint();
         }
         else {
            gtk_window_fullscreen(GTK_WINDOW(m_window));
            m_fullscreenMode = 1;
            requestRepaint();
         }
         break;

      }
   
      return true;          
   }
}

void
GtkDevControl::quitApp()
{
   m_running = false;
   gtk_main_quit();
}

gboolean keyEventCallback( GtkWidget* widget,
                           GdkEventKey* event,
                           GtkDevControl* app )
{
   // Connect the keyhandling call back from GTK to the handleKey method. 
   return app->handleKey( event, NULL, 0 );
}

void destroy_event( GtkWidget *widget,
                    gpointer   data )
{
   gtk_main_quit();
}

static bool inTracking = false;

void buttonPress(GtkWidget* widget,
                 GdkEventButton* event,
                 GtkDevControl* app) 
{
   inTracking = true;
   
   app->startDrag( event->button, makePoint( event ) );
   
}

void buttonRelease( GtkWidget* widget,
                    GdkEventButton* event,
                    GtkDevControl* app  ) 
{
   inTracking = false;
   
   app->releaseButton();
   app->endDrag( event->button, makePoint( event ) );
}

void buttonMotion( GtkWidget* widget,
                   GdkEventMotion* event,
                   GtkDevControl* app) 
{
   ScreenPoint pos( 0, 0 );
   GdkModifierType state;

   if ( event->is_hint ) {
      int x, y;
      gdk_window_get_pointer( event->window, &x, &y, &state );
      pos = ScreenPoint( x, y );
   } else {
      pos = makePoint( event );
      state = static_cast<GdkModifierType>( event->state );
   }

   if(state == 0) {
      // If this happens, we should abort tracking until a button down has
      // appeared again.
      inTracking = false;
   } else if ( state & GDK_BUTTON3_MASK ) {
      app->zoomRectangle( pos );
   } else if ( state & GDK_BUTTON1_MASK ) {
      if(inTracking) {
         app->drag( 1, pos ) ;        
      }
//       updateInfoLabels( getTopLevelWidget(),
//                         *getDrawingArea()->getTileMapHandler() );
   }
}

void buttonScroll( GtkWidget* widget,
                   GdkEventScroll* event,
                   GtkDevControl* app ) 
{
   if ( event->direction != GDK_SCROLL_UP &&
        event->direction != GDK_SCROLL_DOWN ) {
      return;
   }

   
   if ( event->direction == GDK_SCROLL_DOWN ) {
      app->zoom( false );
   } else {      
      app->zoomToPoint( makePoint( event) );
   }
   app->stopCenteringOnMeMarker();

   app->requestRepaint();
}

void GtkDevControl::initKeyMap()
{
   // Initialize which key codes that should result in which 
   // operations on the map.
   m_keyMap[GDK_x] = MapLibKeyInterface::ZOOM_OUT_KEY;
   m_keyMap[GDK_z] = MapLibKeyInterface::ZOOM_IN_KEY;
   m_keyMap[GDK_KP_Subtract] = MapLibKeyInterface::ZOOM_OUT_KEY;
   m_keyMap[GDK_KP_Add] = MapLibKeyInterface::ZOOM_IN_KEY;
   m_keyMap[GDK_minus] = MapLibKeyInterface::ZOOM_OUT_KEY;
   m_keyMap[GDK_plus] = MapLibKeyInterface::ZOOM_IN_KEY;
   m_keyMap[GDK_Up] = MapLibKeyInterface::MOVE_UP_KEY;
   m_keyMap[GDK_Down] = MapLibKeyInterface::MOVE_DOWN_KEY;
   m_keyMap[GDK_Left] = MapLibKeyInterface::MOVE_LEFT_KEY;
   m_keyMap[GDK_Right] = MapLibKeyInterface::MOVE_RIGHT_KEY;
   m_keyMap[GDK_space] = MapLibKeyInterface::RESET_ROTATION_KEY;
   m_keyMap[GDK_Page_Up] = MapLibKeyInterface::ROTATE_LEFT_KEY;
   m_keyMap[GDK_Page_Down] = MapLibKeyInterface::ROTATE_RIGHT_KEY;
}


void GtkDevControl::mapIsReadyToBeDrawn()
{   
   GdkWindow* window = m_mapArea->window;
   
   GdkRegion* region = gdk_drawable_get_visible_region( window );

   gdk_window_invalidate_region( window, region, true );
   
   gdk_window_process_all_updates();
}

void exposeFunc( GtkWidget* widget, GdkEventExpose* event, GtkDevControl* control ) {
   control->expose( event );   
}

void GtkDevControl::expose( GdkEventExpose* event )
{
   if ( m_curDrawer == NATIVE_GTK ) {
      // Create a gc to draw upon. 
      GdkGC *gc = gdk_gc_new( m_mapArea->window );
    
      drawInternalOverlay();
    

      // Render the map component to the supplied gc.
      if(m_drawer != NULL){
         m_drawer->renderTo( gc, &event->area );
      }
      
      // Draw any additional static overlay to the supplied gc. 
      drawOverlay( gc );
    
      // Release GC. 
      // Note that this part of the code is highly unoptimized.
      // It should not be necessary to re-allocate the gc each time the
      // map is drawn.
      g_object_unref( gc );
   }
#ifdef USE_OPENGL_ES
   else if(m_curDrawer == OPENGL_ES) {

      // if(m_testImg) {
      //    m_mapLib->getMapDrawingInterface()->drawImageSpec(
      //       m_testImg, WFAPI::ScreenPoint(50, 50) );

      //    m_mapLib->getMapDrawingInterface()->drawImageSpec(
      //       m_testText, WFAPI::ScreenPoint(72, 55) );
      // }
      
      m_glesDrawer->render();      
   }
#endif
}

void GtkDevControl::drawOverlay( GdkGC* gc )
{

   // Retrieve center of map.
   // gint width = m_mapArea->allocation.width;
   // gint height = m_mapArea->allocation.height;

   // Draws a black rectangle, 10x10 pixels, 
   // in the middle of the map
   // as an example how to draw static overlay on top of the map.
   // gdk_draw_rectangle( m_mapArea->window, gc, 
   //                     true,  // filled
   //                     width / 2, height / 2, 10, 10 );   

}

void GtkDevControl::drawInternalOverlay()
{

}

  

static gboolean
ZoomIn_Callback (GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
	GtkDevControl* app = (GtkDevControl*)user_data;
   app->zoom(true);
   return true;
}

static gboolean
ZoomOut_Callback (GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
	GtkDevControl* app = (GtkDevControl*)user_data;
   app->zoom(false);
   
   return true;
}

static gboolean
Button3_CB (GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
	cout << "Button3_CB is invoked" << endl;
	GtkDevControl* app = (GtkDevControl*)user_data;
   app->toggle3dMode();
   
   return true;
}

static gboolean
Button4_CB (GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
	cout << "Button4_CB is invoked" << endl;
	GtkDevControl* app = (GtkDevControl*)user_data;
   app->toggleNightMode();

   return true;
}

static gboolean
Button5_CB (GtkWidget* widget, GdkEventButton* event, gpointer user_data)
{
	GtkDevControl* app = (GtkDevControl*)user_data;
   app->quitApp();
   
   return true;
}


void GtkDevControl::initWindow()
{   
   // Create our GDK components.   
   m_mapArea = gtk_drawing_area_new();
   
   g_set_application_name("GtkSampleApp");

#if ((defined MAEMO4) || (defined MAEMO5))
   m_program = HILDON_PROGRAM(hildon_program_get_instance());
   m_window = HILDON_WINDOW(hildon_window_new());
   hildon_program_add_window(m_program, m_window);
   gtk_window_set_default_size(GTK_WINDOW(m_window), 800, 480);
#else
   m_window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
   gtk_window_set_title(GTK_WINDOW( m_window ), "GtkSampleApp");
   gtk_window_set_default_size(GTK_WINDOW(m_window), 480, 800);
#endif

   gtk_window_set_position(GTK_WINDOW(m_window), GTK_WIN_POS_CENTER);

  
   GtkWidget* vbox = gtk_vbox_new(0, 0);

   GtkWidget* table = gtk_table_new(1,2, true);
   m_currentStreetLabel = gtk_label_new("Current street");
   m_distanceLabel = gtk_label_new("Distance");

   gtk_table_attach(GTK_TABLE(table), m_currentStreetLabel, 0, 1, 0, 1, 
                    GTK_EXPAND, GTK_SHRINK, 0, 0);
   gtk_table_attach(GTK_TABLE(table), m_distanceLabel, 1, 2, 0, 1, 
                    GTK_EXPAND, GTK_SHRINK, 0, 0);
   gtk_table_set_row_spacings(GTK_TABLE(table), 5);
   gtk_table_set_col_spacings(GTK_TABLE(table), 5);

#if ((defined MAEMO4) || (defined MAEMO5))
   gtk_widget_set_size_request(table, 800, 20);
#else
   gtk_widget_set_size_request(table, 480, 100);
#endif



	GtkWidget *option_button;
	
	GtkWidget* fixed_bg = gtk_fixed_new();

	GtkWidget * hbox1 = gtk_hbox_new(FALSE, 0);

#if ((defined MAEMO4) || (defined MAEMO5))
	gtk_widget_set_size_request( hbox1, 800, 40);
#else
	gtk_widget_set_size_request( hbox1, 480, 80);
#endif
	///TODO: The name of the button can be changed. eg. Just replace "one" with the text that you want.
	option_button = gtk_button_new_with_label("Zoom In");
	gtk_widget_set_size_request( option_button, 95, 100);
	gtk_box_pack_start(GTK_BOX(hbox1), option_button, FALSE, FALSE, 0);
	gtk_widget_show(option_button);

	g_signal_connect(GTK_OBJECT(option_button), "button-release-event",
			G_CALLBACK(ZoomIn_Callback), this);

	option_button = gtk_button_new_with_label("Zoom Out");
	gtk_widget_set_size_request( option_button, 90, 100);
	gtk_box_pack_start(GTK_BOX(hbox1), option_button, FALSE, FALSE, 0);
	gtk_widget_show(option_button);

	g_signal_connect(GTK_OBJECT(option_button), "button-release-event",
			G_CALLBACK(ZoomOut_Callback), this);

	option_button = gtk_button_new_with_label("2D/3D");
	gtk_widget_set_size_request( option_button, 90, 100);
	gtk_box_pack_start(GTK_BOX(hbox1), option_button, FALSE, FALSE, 0);
	gtk_widget_show(option_button);

	g_signal_connect(GTK_OBJECT(option_button), "button-release-event",
			G_CALLBACK(Button3_CB), this);

	option_button = gtk_button_new_with_label("Day/Night");
	gtk_widget_set_size_request( option_button, 90, 100);
	gtk_box_pack_start(GTK_BOX(hbox1), option_button, FALSE, FALSE, 0);
	gtk_widget_show(option_button);

	g_signal_connect(GTK_OBJECT(option_button), "button-release-event",
			G_CALLBACK(Button4_CB), this);

	option_button = gtk_button_new_with_label("Quit");
	gtk_widget_set_size_request( option_button, 90, 100);
	gtk_box_pack_start(GTK_BOX(hbox1), option_button, FALSE, FALSE, 0);
	gtk_widget_show(option_button);

	g_signal_connect(GTK_OBJECT(option_button), "button-release-event",
			G_CALLBACK(Button5_CB), this);

	gtk_fixed_put((GtkFixed*)(fixed_bg), hbox1, 0, 0);
	gtk_widget_show_all(hbox1);
	gtk_widget_show_all(fixed_bg);


	m_mapAreaContainer = gtk_vbox_new(0,0);
   gtk_container_add(GTK_CONTAINER(m_mapAreaContainer), m_mapArea );

   gtk_container_add(GTK_CONTAINER(m_window), vbox);
   gtk_container_add(GTK_CONTAINER(vbox), table );
   gtk_container_add(GTK_CONTAINER(vbox), fixed_bg );
   gtk_container_add(GTK_CONTAINER(vbox), m_mapAreaContainer );

 
   


#if ((defined MAEMO4) || (defined MAEMO5))
   gtk_widget_set_size_request(m_mapArea, 800, 390);
#else 
   gtk_widget_set_size_request(m_mapArea, 480, 620);
#endif

/* how we did it before we had the top bar:
   gtk_container_add(GTK_CONTAINER(m_window), m_mapArea );
*/

   gtk_widget_set_events(m_mapArea, 
                         GDK_EXPOSURE_MASK |
                         GDK_LEAVE_NOTIFY_MASK |
                         GDK_BUTTON_PRESS_MASK |
                         GDK_BUTTON_RELEASE_MASK |
                         GDK_POINTER_MOTION_MASK |
                         GDK_POINTER_MOTION_HINT_MASK);
   
   gtk_widget_show_all(GTK_WIDGET(m_window));

   
   // Setup callback for destroy event
   g_signal_connect( G_OBJECT( m_window ), "destroy",
                     G_CALLBACK( destroy_event ), NULL );

   // Setup callbacks for key presses and expose events
   g_signal_connect( G_OBJECT( m_window ), "key_press_event",
                     G_CALLBACK( keyEventCallback ), this );

   // Setup callbacks for key presses and expose events
   g_signal_connect( G_OBJECT( m_window ), "key_release_event",
                     G_CALLBACK( keyEventCallback ), this );
 
   g_signal_connect( G_OBJECT( m_mapArea ), "expose_event",
                     G_CALLBACK( exposeFunc ), this );

   // Setup callbacks for mouse press, drag event and scrolling
   g_signal_connect( G_OBJECT( m_mapArea ), "button_press_event",
                     G_CALLBACK( buttonPress ), this );

   g_signal_connect( G_OBJECT( m_mapArea ), "button_release_event",
                     G_CALLBACK( buttonRelease ), this );

   g_signal_connect( G_OBJECT( m_mapArea ), "motion_notify_event",
                     G_CALLBACK( buttonMotion ), this);

   g_signal_connect( G_OBJECT( m_window ), "scroll_event",
                     G_CALLBACK( buttonScroll ), this );

//   // Hide it.
//   gtk_widget_hide(m_mapArea);
}

void GtkDevControl::toggleLayerVisibility(LayerId id)
{
   OverlayInterface* overlayInterface = m_mapLib->getOverlayInterface();

   bool isVisible = overlayInterface->getLayerVisible(id);

   if(isVisible) {
      overlayInterface->hideLayer(id);
   } else {
      overlayInterface->showLayer(id);
   }
}

void GtkDevControl::initOverlay()
{
   // Create the zoom specification we will use for our overlay items.
   m_zoomSpec = OverlayZoomSetup::createZoomSpec(m_pathToResources);

   // // Initialize friends layer
   initFriendsLayer();

   // Initialize locations layer
   initLocationsLayer();
   
   // Initialize the "Me"-layer.
   initMeLayer();

   // Initialize our screen components
   initScreenComponents();
}

void GtkDevControl::initLocationsLayer() {
   // Holder for the name and the additional description 
   struct Location {
      const char* name;
      WGS84Coordinate coord;
   };

   static Location locations[] = {
      { "Möllev.", WGS84Coordinate(55.5921, 13.008) },
      { "Möll. K.", WGS84Coordinate(55.5924, 13.0092) },
      { "Metro Bar", WGS84Coordinate(55.5925, 13.0092) },
      { "Krua Thai", WGS84Coordinate(55.5919, 13.0085) },
      { "Nobel square där lilla gossen hampus bor", WGS84Coordinate(55.5918, 13.0197) },
      { "Pub 25", WGS84Coordinate(55.591, 13.0127) },
   };
   
   // Create the images for the item
   ImageSpec* overlayImgLevel1 =
      GtkUtil::createImageSpec(m_pathToResources, "overlay_icon_location0.png");
   ImageSpec* overlayImgLevel2 =
      GtkUtil::createImageSpec(m_pathToResources, "overlay_icon_location1.png");
   ImageSpec* overlayImgLevel3 =
      GtkUtil::createImageSpec(m_pathToResources, "overlay_icon_location2.png");
   ImageSpec* highlightIcon =
      GtkUtil::createImageSpec(m_pathToResources, 
                               "highlight_icon_location.png");

   // Get the overlay interface that we'll be using to add the items 
   OverlayInterface* overlayInterface = m_mapLib->getOverlayInterface();

   static const int numLocations = sizeof(locations) / sizeof(Location);

   for(int i = 0; i < numLocations; i++) {
      MapObjectInfo objInfo(locations[i].name, 
                            MapObjectInfo::OTHER_TYPE, 100 + i);

      // Create the item itself
      OverlayItem* item = OverlayItem::allocate(m_zoomSpec,
                                                objInfo,
                                                locations[i].coord);
      
      // Add the images.
      item->addImageSpec(overlayImgLevel1);
      item->addImageSpec(overlayImgLevel2);
      item->addImageSpec(overlayImgLevel3);
      item->addImageSpec(highlightIcon);

      overlayInterface->addOverlayItem(item, LOCATIONS_LAYER);
      
      item->removeReference();
   }
   
   overlayImgLevel1->removeReference();
   overlayImgLevel2->removeReference();
   overlayImgLevel3->removeReference();
   highlightIcon->removeReference();
}

void GtkDevControl::initFriendsLayer()
{
   // Holder for the name and the additional description 
   struct NamePair {
      const char* name;
   };

   // Set of names that we will use for our friends layer
   static NamePair names[] = {
      { "John D." },
      { "Foo B." },
      { "John S." },
      { "Joe R." },
      { "Jane D." },
      { "Martin S." },
      { "Henrik F." },
   };

   // Create the images for the item
   ImageSpec* overlayImgLevel1 =
      GtkUtil::createImageSpec(m_pathToResources, "overlay_icon0.png");
   ImageSpec* overlayImgLevel2 =
      GtkUtil::createImageSpec(m_pathToResources, "overlay_icon1.png");
   ImageSpec* overlayImgLevel3 =
      GtkUtil::createImageSpec(m_pathToResources, "overlay_icon2.png");
   ImageSpec* highlightIcon =
      GtkUtil::createImageSpec(m_pathToResources, "highlight_icon.png");

   // Get the overlay interface that we'll be using
   OverlayInterface* overlayInterface = m_mapLib->getOverlayInterface();
   
   static const int numNames = sizeof(names) / sizeof(NamePair);

   // The range that our random positions will occur in
   static const float maxDiffLon = 0.003;
   static const float maxDiffLat = 0.004;
   
   // Add all of our friends
   for(int i = 0; i < numNames; i++) {
      MapObjectInfo objInfo(names[i].name, 
                            MapObjectInfo::OTHER_TYPE, i);

      // Create normalized random numbers (0..1) to use as
      // offset for our coordinates
      float rLat = rand() / float(RAND_MAX);
      float rLon = rand() / float(RAND_MAX);
      
      // Create the item itself
      OverlayItem* item =
         OverlayItem::allocate(m_zoomSpec, objInfo,
                               WGS84Coordinate(55.59 + rLat * maxDiffLat,
                                               13.0138 + rLon * maxDiffLon));
      // Add the images.
      item->addImageSpec(overlayImgLevel1);
      item->addImageSpec(overlayImgLevel2);
      item->addImageSpec(overlayImgLevel3);
      item->addImageSpec(highlightIcon);

      overlayInterface->addOverlayItem(item, FRIENDS_LAYER);

      item->removeReference();
   }   

   overlayImgLevel1->removeReference();
   overlayImgLevel2->removeReference();
   overlayImgLevel3->removeReference();
   highlightIcon->removeReference();
}

void GtkDevControl::initMeLayer() 
{
   // Create the images for the item
   ImageSpec* overlayImgLevel1 =
      GtkUtil::createImageSpec(m_pathToResources, "overlay_me_icon0.png");
   ImageSpec* overlayImgLevel2 =
      GtkUtil::createImageSpec(m_pathToResources, "overlay_me_icon1.png");
   ImageSpec* overlayImgLevel3 =
      GtkUtil::createImageSpec(m_pathToResources, "overlay_me_icon2.png");
   ImageSpec* highlightIcon =
      GtkUtil::createImageSpec(m_pathToResources, "highlight_me_icon.png");

   // Get the overlay interface that we'll be using
   OverlayInterface* overlayInterface = m_mapLib->getOverlayInterface();
   
   MapObjectInfo objInfo("Me", 
                         MapObjectInfo::OTHER_TYPE, 0);

   // Create normalized random numbers (0..1) to use as
   // offset for our coordinates
   float rLat = rand() / float(RAND_MAX);
   float rLon = rand() / float(RAND_MAX);
   static const float maxDiffLon = 0.003;
   static const float maxDiffLat = 0.004;
      
   // Create the item itself
   OverlayItem* item =
      OverlayItem::allocate(m_zoomSpec, objInfo,
                            WGS84Coordinate(55.59 + rLat * maxDiffLat,
                                            13.0138 + rLon * maxDiffLon));

   // Add the images.
   item->addImageSpec(overlayImgLevel1);
   item->addImageSpec(overlayImgLevel2);
   item->addImageSpec(overlayImgLevel3);
   item->addImageSpec(highlightIcon);

   m_meMarker = item;
   
   overlayInterface->addOverlayItem(item, ME_LAYER);

   // Set the Me-marker to be unstackable
   item->setStackable(false);
   
   overlayImgLevel1->removeReference();
   overlayImgLevel2->removeReference();
   overlayImgLevel3->removeReference();
   highlightIcon->removeReference();

   // Create the images for the snapped me marker
   ImageSpec* snappedOverlayImgLevel1 =
      GtkUtil::createImageSpec(m_pathToResources,
                               "overlay_me_icon0_snapped.png");
   ImageSpec* snappedOverlayImgLevel2 =
      GtkUtil::createImageSpec(m_pathToResources,
                               "overlay_me_icon1_snapped.png");
   ImageSpec* snappedOverlayImgLevel3 =
      GtkUtil::createImageSpec(m_pathToResources,
                               "overlay_me_icon2_snapped.png");
   ImageSpec* snappedHighlightIcon =
      GtkUtil::createImageSpec(m_pathToResources,
                               "highlight_me_icon_snapped.png");

   MapObjectInfo snappedObjInfo("Snapped", 
                         MapObjectInfo::OTHER_TYPE, 1);

   // Create the snapped item itself
   OverlayItem* snappedItem =
      OverlayItem::allocate(m_zoomSpec, snappedObjInfo,
                            WGS84Coordinate(55.61, 13.0141));

   // Add the images.
   snappedItem->addImageSpec(snappedOverlayImgLevel1);
   snappedItem->addImageSpec(snappedOverlayImgLevel2);
   snappedItem->addImageSpec(snappedOverlayImgLevel3);
   snappedItem->addImageSpec(snappedHighlightIcon);
   
   m_meMarkerSnapped = snappedItem;

   overlayInterface->addOverlayItem(snappedItem, ME_LAYER);

   // Set the Me-marker to be unstackable
   snappedItem->setStackable(false);
   
   snappedOverlayImgLevel1->removeReference();
   snappedOverlayImgLevel2->removeReference();
   snappedOverlayImgLevel3->removeReference();
   snappedHighlightIcon->removeReference();
}

void GtkDevControl::initMapLib(WFAPI::DBufConnection* connection)
{
   if (connection == NULL) {
      // No supplied connection. Create our own instead.

      // Create the communication channel for Maplib to use to retrieve 
      // maps from.
      // In this example a HttpContext is created, i.e. 
      // the maps will be fetched using Http directly.


      m_networkContext = WFAPI::GtkFactory::createHTTPContext();


      connection = m_networkContext->getConnection();
   }

   // Create the drawing classes and connecting them to the window.
   m_drawer = GtkFactory::createGtkDrawer(m_mapArea);

#ifdef USE_OPENGL_ES
   m_glesDrawer = WFAPI::GtkGLESFactory::createGtkGLESDrawer( m_mapArea );
#endif
   
   MapLibInitialConfig initialConfig;
   initialConfig.setDiskCacheSettings(TILEMAPSPATH, 10*1024*1024);

#ifdef USE_OPENGL_ES
   m_curDrawer = OPENGL_ES;
   assert(m_glesDrawer != NULL);
   m_mapLib = GtkFactory::createMapLib(connection,
                                       m_glesDrawer->getDrawingContext(),
                                       initialConfig);
#else
   m_curDrawer = NATIVE_GTK;
   assert(m_drawer != NULL);
   m_mapLib = GtkFactory::createMapLib(connection,
                                       m_drawer->getDrawingContext(),
                                       initialConfig);
#endif
   
   WFAPI::MapObjectInterface* objInterface =
      m_mapLib->getMapObjectInterface();
   
   // Set listener to get notified when map objects have been
   // selected in the map.
   objInterface->addSelectedMapObjectListener(this);

   // Set listener to get notified when MapLib settings has changed.
   m_mapLib->getConfigInterface()->addSettingsListener(this);

   // Enable magnetic zoom.
   m_mapLib->getConfigInterface()->
      getDetailedConfigInterface()->enableAutomaticHighlight(true);
   // Update zoom level.
   m_mapLib->getMapOperationInterface()->setZoomLevel(7.0);

   

   
   m_mapLib->getMapDrawingInterface()->setMapDrawingCallback(this);
}

GtkDevControl::~GtkDevControl()
{
   m_mapLib->getMapDrawingInterface()->setMapDrawingCallback(NULL);
   
   m_mapLib->getConfigInterface()->removeSettingsListener(this);
   m_zoomSpec->removeReference();
   if (m_meMarker) {
      m_meMarker->removeReference();
   }
   if (m_meMarkerSnapped) {
      m_meMarkerSnapped->removeReference();
   }
   delete m_mapLib;
   delete m_drawer;
   
#ifdef USE_OPENGL_ES
   delete m_glesDrawer;
#endif
   
   delete m_networkContext;
   delete[] m_pathToResources;
}

void GtkDevControl::showStockholm()
{

   // Example of how to center the map to show a certain place,
   // in this example Stockholm, Sweden.

   // Set the new center of the map.
   
   MapOperationInterface* operationInterface =
      m_mapLib->getMapOperationInterface();

   operationInterface->setCenter( WGS84Coordinate( 55.5971, 13.009 ) );
   // operationInterface->setZoomLevel(0.780376);

   // operationInterface->setCenter( WGS84Coordinate( 55.5878, 12.9576 ) );

   requestRepaint();
}

void GtkDevControl::dragAction( int button,
                                int upordown,
                                const ScreenPoint& pos ) 
{
   if ( button == 1 ) {
      // Get the key interface from MapLib.
      MapLibKeyInterface* keyHandler = m_mapLib->getKeyInterface();
      bool consumed =
         keyHandler->handlePointerEvent(
            MapLibKeyInterface::DRAG_TO, 
            static_cast<MapLibKeyInterface::kindOfPointerType>
            ( upordown ), pos );
      
      if (consumed) {
         // The dragging action was consumed, so a dragging interaction
         // with the map has been done. This means that we should stop
         // centering on the me marker. 
         stopCenteringOnMeMarker();
      }
   }
}

void GtkDevControl::startDrag( int button, const ScreenPoint& pos ) 
{
   dragAction( button, MapLibKeyInterface::POINTER_DOWN_EVENT, pos );
}

void GtkDevControl::endDrag( int button, const ScreenPoint& pos ) 
{
   m_mapLib->getKeyInterface()->stop();
   dragAction( button, MapLibKeyInterface::POINTER_UP_EVENT, pos );
}

void GtkDevControl::drag( int button, const ScreenPoint& pos ) 
{
   dragAction( button, MapLibKeyInterface::POINTER_UPDATE_EVENT, pos );
}

void GtkDevControl::releaseButton() 
{
   if ( m_zoomMode ) {
      // disable zoom mode on button release
      m_zoomMode = false;
      // Get the moving interface which is used for zooming
      MapOperationInterface* operationInterface =
         m_mapLib->getMapOperationInterface();

      operationInterface->setScreenBox( m_zoomRect.getTopLeft(),
                                        m_zoomRect.getBottomRight() );

      MapDrawingInterface* drawingInterface =
         m_mapLib->getMapDrawingInterface();

      drawingInterface->repaintNow();
   }
}

void GtkDevControl::zoom( bool zoomIn ) 
{
   const float level = 1.1;
   //Get the moving interface which is used for zooming

   MapOperationInterface* movingInterface =
      m_mapLib->getMapOperationInterface();
   
   if ( zoomIn ) {
      movingInterface->zoom( 1.0 / level );
   } else {
      movingInterface->zoom( level );
   }
}

void
GtkDevControl::toggle3dMode()
{
   if(m_mapLib->getConfigInterface()->get3dMode()){
      m_mapLib->getConfigInterface()->set3dMode(false);
   } else {
      m_mapLib->getConfigInterface()->set3dMode(true);
   }
}

void
GtkDevControl::toggleNightMode()
{
   if(m_mapLib->getConfigInterface()->getNightMode()){
      m_mapLib->getConfigInterface()->setNightMode(false);
   } else {
      m_mapLib->getConfigInterface()->setNightMode(true);
   }
}

void
GtkDevControl::deleteDrawer()
{
   m_mapLib->setDrawingContext(NULL);

   delete m_drawer;
   m_drawer = NULL;
}

void
GtkDevControl::changeMapScaleAdjustment()
{
   if(!m_mapScaleAdjustmentToggle){
      m_mapLib->getConfigInterface()->setMapScaleAdjustment(1.0);
      std::cout << "MSA = 1 " << std::endl;
      m_mapScaleAdjustmentToggle = true;
   } else {
      m_mapLib->getConfigInterface()->setMapScaleAdjustment(2.0);
      std::cout << "MSA = 2 " << std::endl;
      m_mapScaleAdjustmentToggle = false;
   }
}

void GtkDevControl::zoomRectangle( const ScreenPoint& point ) 
{
   if ( ! m_zoomMode ) {
      m_zoomRect.setTopLeft( point );
      m_zoomMode = true;
   } else {
      m_zoomRect.setBottomRight( point );
   }

   drawZoomRectangle();
}

void GtkDevControl::zoomToPoint( const ScreenPoint& pos )
{
   const float level = 1.1;
   // Get the moving interface 
   MapOperationInterface* movingInterface =
      m_mapLib->getMapOperationInterface();   

   // We need the world coordinate for the screen position
   // to zoom in on.
   WGS84Coordinate coord( 0, 0 );
   movingInterface->screenToWorld( coord, pos );

   // Now zoom with the correct world coord locked to the
   // corresponding screen point.
   movingInterface->zoom(1.0 / level, coord, pos);
}

void GtkDevControl::requestRepaint()
{
   MapDrawingInterface* drawingInterface =
      m_mapLib->getMapDrawingInterface();

   drawingInterface->requestRepaint();   
}

void GtkDevControl::drawZoomRectangle() 
{
   
}

void GtkDevControl::createStackedDialog()
{

   // Create the visual spec and images for 2 stacked items
   WFAPI::ImageSpec* frame =
      GtkUtil::createImageSpec(m_pathToResources, "10_pm5_xl.png");
   WFAPI::ImageSpec* tappedFrame =
      GtkUtil::createImageSpec(m_pathToResources, "10_pm5_xl_tapped.png");

   WFAPI::OverlayItemVisualSpec* visualSpec =
      WFAPI::OverlayItemVisualSpec::allocate(frame,
                                             WFAPI::ScreenPoint(16, 63),
                                             WFAPI::ScreenPoint(51, 48),
                                             0);
   WFAPI::OverlayItemVisualSpec* tappedVisualSpec =
      WFAPI::OverlayItemVisualSpec::allocate(tappedFrame,
                                             WFAPI::ScreenPoint(16, 63),
                                             WFAPI::ScreenPoint(51, 48),
                                             0);

   FontSpec* nameFont = FontSpec::allocate("DejaVu LGC Sans", 16);
   
   visualSpec->setTextProperties(
      WFAPI::ScreenPoint(6, 94),
      WFAPI::ScreenPoint(97, 106),
      WFAPI::OverlayItemVisualSpec::TextAlignment(),
      WFAPI::OverlayItemVisualSpec::RGB(0, 0, 0),
      nameFont);
   
   tappedVisualSpec->setTextProperties(
      WFAPI::ScreenPoint(6, 94),
      WFAPI::ScreenPoint(97, 106),
      WFAPI::OverlayItemVisualSpec::TextAlignment(),
      WFAPI::OverlayItemVisualSpec::RGB(0, 0, 0),
      nameFont);

   nameFont->removeReference();
   
   WFAPI::ImageSpec* bg2 =
      GtkUtil::createImageSpec(m_pathToResources, "10_pm5_xl_gallery_2.png");
      
   // Specific to the dialog image
   WFAPI::ScreenPoint leftArrowPos2(0, 0);
   WFAPI::ScreenPoint rightArrowPos2(0, 0);
   
   WFAPI::ScreenPoint upperLeft2(14, 8);
   WFAPI::ScreenPoint lowerRight2(239, 141);
   
   WFAPI::ScreenPoint dialogFocusPoint2(120, 163);

   WFAPI::StackedDialogVisualSpec* stackedDialogSpec2 =
      WFAPI::StackedDialogVisualSpec::allocate(bg2,
                                               dialogFocusPoint2,
                                               upperLeft2,
                                               lowerRight2,
                                               NULL,
                                               leftArrowPos2,
                                               NULL,
                                               rightArrowPos2,
                                               visualSpec,
                                               tappedVisualSpec);
   
   WFAPI::ImageSpec* bg3 =
      GtkUtil::createImageSpec(m_pathToResources, "10_pm5_xl_gallery_3.png");
   WFAPI::ImageSpec* left3 =
      GtkUtil::createImageSpec(m_pathToResources, "dialog_arrow_left.png");
   WFAPI::ImageSpec* right3 =
      GtkUtil::createImageSpec(m_pathToResources, "dialog_arrow_right.png");
      
   // Specific to the dialog image
   WFAPI::ScreenPoint leftArrowPos3(-25, 73);
   WFAPI::ScreenPoint rightArrowPos3(382, 73);
   
   WFAPI::ScreenPoint upperLeft3(22, 17);
   WFAPI::ScreenPoint lowerRight3(335, 130);   

   WFAPI::ScreenPoint dialogFocusPoint3(178, 164);

   WFAPI::StackedDialogVisualSpec* stackedDialogSpec3 =
      WFAPI::StackedDialogVisualSpec::allocate(bg3,
                                               dialogFocusPoint3,
                                               upperLeft3,
                                               lowerRight3,
                                               left3,
                                               leftArrowPos3,
                                               right3,
                                               rightArrowPos3,
                                               visualSpec,
                                               tappedVisualSpec);


   WFAPI::DetailedConfigInterface* detailedConfig =
      m_mapLib->getConfigInterface()->getDetailedConfigInterface();
   detailedConfig->setStackedDialogVisualSpecs(stackedDialogSpec2,
                                               stackedDialogSpec3);
   
   stackedDialogSpec2->removeReference();
   stackedDialogSpec3->removeReference();
   bg2->removeReference();
   bg3->removeReference();
   left3->removeReference();
   right3->removeReference();
   frame->removeReference();
   visualSpec->removeReference();
   tappedFrame->removeReference();
   tappedVisualSpec->removeReference();
}

void GtkDevControl::handleStackedDialogOpened() {
   cout << "A stacked dialog was opened." << endl;
}

void GtkDevControl::handleStackedDialogClosed() {
   cout << "A stacked dialog was closed." << endl;
}

void GtkDevControl::handleSelectedMapObject(const MapObjectInfo& mapObjectInfo,
                                            OverlayItem* overlayItem,
                                            const WGS84Coordinate& coord,
                                            bool longPress)
{
   const WFString& name = mapObjectInfo.getName();
   
   if(longPress) {
      std::cout << "Map object selected with long press: ";
   } else {
      std::cout << "Map object selected: ";
   }
   
   std::cout << name.c_str() << std::endl;

   std::cout << "Map object coord: " << coord << std::endl;
    
}

void GtkDevControl::handleUpdatedPOICategories()
{
   // Handle the updated POI Categories.
}

WFAPI::MapLibAPI*
GtkDevControl::getMapLibAPI()
{
   return m_mapLib;
}

void
GtkDevControl::setCurrentStreet(const char* turn)
{
   gtk_label_set_text(GTK_LABEL(m_currentStreetLabel), turn);
}

void
GtkDevControl::setDistance(const char* distance)
{
   gtk_label_set_text(GTK_LABEL(m_distanceLabel), distance);
}

void
GtkDevControl::setMeMarkerPosition(const WFAPI::WGS84Coordinate& coord,
                                   const WFAPI::ScreenPoint& screenPos,
                                   int heading,
                                   const WFAPI::WGS84Coordinate& routeCoord,
                                   int routeHeading,
                                   bool updateZoomLevel,
                                   double zoomLevel)
{
   if (m_meMarker != NULL) {
      m_meMarker->setPosition(coord);
   }

   if (m_meMarkerSnapped != NULL) {
      m_meMarkerSnapped->setPosition(routeCoord);
   }

   if (m_centerOnMeMarker){
      // We are currently tracking center to the me marker position.
      MapOperationInterface* movingInterface =
         m_mapLib->getMapOperationInterface();
      movingInterface->setPoint( coord, screenPos);
      if (routeHeading <= 360) {
         movingInterface->setAngle(routeHeading, screenPos);
      } else if (heading <= 360) {
         movingInterface->setAngle(heading, screenPos);
      }

      if (updateZoomLevel) {
         movingInterface->setZoomLevel(zoomLevel);
      }
      
   }

   m_mapLib->getMapDrawingInterface()->requestRepaint();
}


GtkWindow*
GtkDevControl::getWindow() {
   return GTK_WINDOW(m_window);
}

void GtkDevControl::centerOnMeMarker()
{
   // When someone calls setMeMarkerPosition, we will be centered on the
   // me marker.
   m_centerOnMeMarker = true;
}

void GtkDevControl::stopCenteringOnMeMarker()
{
   m_centerOnMeMarker = false;
}

void GtkDevControl::toggleDrawer()
{
#ifdef USE_OPENGL_ES
   if ( m_curDrawer == NATIVE_GTK ) {
      /**
       *  We need to disable the double buffering so that it does not
       *  interfere with the OpenGL ES swapping.
       */
   
      gtk_widget_set_double_buffered( m_mapArea, false );

      m_mapLib->setDrawingContext( m_glesDrawer->getDrawingContext() );
      m_curDrawer = OPENGL_ES;
   } else {

      /**
       *  Since we will use the native GTK drawer, the double buffering
       *  can be enabled.
       */ 
      gtk_widget_set_double_buffered( m_mapArea, true );
      m_mapLib->setDrawingContext( m_drawer->getDrawingContext() );
      m_curDrawer = NATIVE_GTK;
   }
#endif
   
   // Request that the map should be redrawn.
   MapDrawingInterface* drawingInterface =
      m_mapLib->getMapDrawingInterface();
   
   drawingInterface->requestRepaint();
}

void GtkDevControl::initScreenComponents()
{
#if defined USE_OPENGL_ES && defined OPENGL_PAL_TEXT_RENDERING
   // The graphics context used to load images and draw text
   pal::GraphicsContext context;

   // These will contain the dimensions of the generated textures
   pal::wf_uint32 widthPixels, heightPixels;

   // This type encapsulates the generated textures. Will manage
   // the resources returned by PAL.
   pal::ResultBuffer resBuffer;

   // Variable used to check the result of PAL calls
   pal::pstatus status = pal::PAL_OK;
   
   // Initialize the graphics context
   status = pal::createGraphicsContext(&context);
   assert(status == pal::PAL_OK);
   
   // The path to our test image
   std::string imgPath =
      m_pathToResources + std::string("/dialog_bg.png");

   // Open the file containing the image
   FILE* imgFile = fopen(imgPath.c_str(), "rb");
   
   // Attempt to load the image to our buffer
   status = pal::loadImageFromFile(context,
                                   imgFile,
                                   pal::PNG,
                                   pal::RGBA8,
                                   0, 0,
                                   &widthPixels,
                                   &heightPixels,
                                   &resBuffer);
   assert(status == pal::PAL_OK);
   
   // Create the image specification that can be drawn later on.
   m_testImg = m_glesDrawer->createImageSpec(resBuffer.getData(),
                                             widthPixels,
                                             heightPixels);

   // Close the opened file.
   fclose(imgFile);

   const char* fontFile = "/usr/share/fonts/dejavu/DejaVuSans.ttf";
   FILE* f = fopen(fontFile, "r");
   
   if(f) {
      printf("Font file %s loaded\n", fontFile);
      fclose(f);
   } else {
      printf("Font file %s failed to load\n", fontFile);
   }

   status = pal::setFontFaceForContext(context, fontFile);
   assert(status == pal::PAL_OK);
   
   // Set the desired font to use when generating texture
   status = pal::setFontForContext(context, "DejaVu LGC Sans", "Regular", 16);
   assert(status == pal::PAL_OK);
   
   // Set the desired font color to use when generating texture
   status = pal::setFontColorForContext(context, 255, 255, 255);
   assert(status == pal::PAL_OK);

   pal::BitmapMetrics metrics;
   
   // Draw the actual font 
   status = pal::drawTextLineToBuffer(context,
                                      "Test string with ÅÄÖ",
                                      0, -1,
                                      pal::RGBA8,
                                      &metrics,
                                      &resBuffer);
   assert(status == pal::PAL_OK);
   
   // Create the image specification that can be drawn later on.
   m_testText = m_glesDrawer->createImageSpec(resBuffer.getData(),
                                              metrics.m_Width,
                                              metrics.m_Height);

   // Release all the graphics resources.
   status = pal::destroyGraphicsContext(context);
   
   assert(status == pal::PAL_OK);
   
#endif
}

