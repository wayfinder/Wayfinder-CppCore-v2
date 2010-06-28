/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "Window/GtkAppWindow.h"
#include "WindowEventListener.h"
#include <gdk/gdkkeysyms.h>
#include "GtkGLESFactory.h"
#include "GtkGLESDrawer.h"
#include "DrawingContext.h"

#include "MapLibKeyHandler.h"
#include "MapLibKeyInterface.h"
#include "ScreenPoint.h"
#include "PointerEvent.h"
#include "TileMapUtil.h"

using namespace WFAPI;

template <typename T>
inline ScreenPoint makePoint( const T* event ) {
   return ScreenPoint( static_cast<wf_int32>( event->x ),
                       static_cast<wf_int32>( event->y ) );
}

void GtkAppWindow::initKeyMap()
{
   // Initialize which key codes that should result in which 
   // operations on the map.
   m_keyMap[GDK_x] = KeyEvent::ML_x;
   m_keyMap[GDK_z] = KeyEvent::ML_z;
   m_keyMap[GDK_KP_Subtract] = KeyEvent::ML_Minus;
   m_keyMap[GDK_KP_Add] = KeyEvent::ML_Plus;
   m_keyMap[GDK_minus] = KeyEvent::ML_Minus;
   m_keyMap[GDK_plus] = KeyEvent::ML_Plus;
   m_keyMap[GDK_Up] = KeyEvent::ML_Up;
   m_keyMap[GDK_Down] = KeyEvent::ML_Down;
   m_keyMap[GDK_Left] = KeyEvent::ML_Left;
   m_keyMap[GDK_Right] = KeyEvent::ML_Right;
   m_keyMap[GDK_space] = KeyEvent::ML_Space;
   m_keyMap[GDK_Page_Up] = KeyEvent::ML_PageUp;
   m_keyMap[GDK_Page_Down] = KeyEvent::ML_PageDown;
   m_keyMap[GDK_F1] = KeyEvent::ML_F1;
   m_keyMap[GDK_F2] = KeyEvent::ML_F2;
   m_keyMap[GDK_F3] = KeyEvent::ML_F3;
   m_keyMap[GDK_F4] = KeyEvent::ML_F4;
   m_keyMap[GDK_F5] = KeyEvent::ML_F5;
   m_keyMap[GDK_F6] = KeyEvent::ML_F6;
   m_keyMap[GDK_F7] = KeyEvent::ML_F7;
   m_keyMap[GDK_F8] = KeyEvent::ML_F8;
   m_keyMap[GDK_F9] = KeyEvent::ML_F9;
   m_keyMap[GDK_a] = KeyEvent::ML_a;
   m_keyMap[GDK_b] = KeyEvent::ML_b;
   m_keyMap[GDK_c] = KeyEvent::ML_c;
   m_keyMap[GDK_d] = KeyEvent::ML_d;
   m_keyMap[GDK_e] = KeyEvent::ML_e;
   m_keyMap[GDK_f] = KeyEvent::ML_f;
   m_keyMap[GDK_g] = KeyEvent::ML_g;
   m_keyMap[GDK_h] = KeyEvent::ML_h;
   m_keyMap[GDK_i] = KeyEvent::ML_i;
   m_keyMap[GDK_j] = KeyEvent::ML_j;
   m_keyMap[GDK_k] = KeyEvent::ML_k;
   m_keyMap[GDK_l] = KeyEvent::ML_l;
   m_keyMap[GDK_m] = KeyEvent::ML_m;
   m_keyMap[GDK_n] = KeyEvent::ML_n;
   m_keyMap[GDK_o] = KeyEvent::ML_o;
   m_keyMap[GDK_p] = KeyEvent::ML_p;
   m_keyMap[GDK_q] = KeyEvent::ML_q;
   m_keyMap[GDK_r] = KeyEvent::ML_r;
   m_keyMap[GDK_s] = KeyEvent::ML_s;
   m_keyMap[GDK_t] = KeyEvent::ML_t;
   m_keyMap[GDK_u] = KeyEvent::ML_u;
   m_keyMap[GDK_v] = KeyEvent::ML_v;
   m_keyMap[GDK_w] = KeyEvent::ML_w;
   m_keyMap[GDK_x] = KeyEvent::ML_x;
   m_keyMap[GDK_y] = KeyEvent::ML_y;
   m_keyMap[GDK_z] = KeyEvent::ML_z;
}

bool GtkAppWindow::handleKey(GtkWidget* widget, GdkEventKey* keyEvent)
{
   if(!m_listener) {
      return false;
   }
   
   guint gdkKey;
   GdkEventType gdkKeyType;

   gdkKey = keyEvent->keyval;
   gdkKeyType = keyEvent->type;

   // Look up in the key map wich action on the map that should be performed
   // based on the supplied key code (refer to initKeyMap method).
   KeyMap::iterator itr = m_keyMap.find( gdkKey );

   KeyEvent k;
   
   if(gdkKeyType == GDK_KEY_PRESS) {
      k.type = KeyEvent::KEY_DOWN;
   } else if(gdkKeyType == GDK_KEY_RELEASE) {
      k.type = KeyEvent::KEY_UP;
   }
   
   if( itr != m_keyMap.end()) {
      // Key was found in key map and desired action is defined 
      // by the key object.
      KeyEvent::Code code = itr->second;
      k.code = code;

      if(k.type == KeyEvent::KEY_UP) {
         m_listener->onKeyUp(k);         
      } else {
         m_listener->onKeyDown(k);
      }

      
      return true;
   } else {
      return false;
   }
}

gboolean keyEventCallback( GtkWidget* widget,
                           GdkEventKey* event,
                           GtkAppWindow* app )
{
   return app->handleKey(widget, event);
}

static bool inTracking = false;

void buttonPress(GtkWidget* widget,
                 GdkEventButton* event,
                 GtkAppWindow* app) 
{
   inTracking = true;
   
   app->startDrag( event->button, makePoint( event ) );
}

void buttonRelease( GtkWidget* widget,
                    GdkEventButton* event,
                    GtkAppWindow* app  ) 
{
   inTracking = false;
   
   app->endDrag( event->button, makePoint( event ) );
}

void buttonMotion( GtkWidget* widget,
                   GdkEventMotion* event,
                   GtkAppWindow* app) 
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
   }
   
   if ( state & GDK_BUTTON1_MASK && inTracking) {
      app->drag( 1, pos ) ;        
   }
}

void buttonScroll( GtkWidget* widget,
                   GdkEventScroll* event,
                   GtkAppWindow* app ) 
{
}

void exposeFunc( GtkWidget* widget, GdkEventExpose* event, GtkAppWindow* control ) {
   // printf("exposeFunc\n");
   control->expose( event );   
}

GtkAppWindow::GtkAppWindow()
{
   m_listener = NULL;
   m_glesDrawer = NULL;
   initWindow();
   initDrawer();
   initKeyMap();
}

void GtkAppWindow::initWindow()
{
   
   // Create our GDK components.   
   m_mapArea = gtk_drawing_area_new();
   
   g_set_application_name("GtkTestApp");

   m_window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
   gtk_window_set_title(GTK_WINDOW( m_window ), "GtkTestApp");
   gtk_window_set_default_size(GTK_WINDOW(m_window), 480, 800);

   m_prevScreenWidth = 480;
   m_prevScreenHeight = 800;
   
   gtk_window_set_position(GTK_WINDOW(m_window), GTK_WIN_POS_CENTER);
   
	m_mapAreaContainer = gtk_vbox_new(0,0);
   gtk_container_add(GTK_CONTAINER(m_mapAreaContainer), m_mapArea );

   gtk_container_add(GTK_CONTAINER(m_window), m_mapAreaContainer);

   gtk_widget_set_size_request(m_mapArea, 480, 620);

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

   
   // // Setup callback for destroy event
   // g_signal_connect( G_OBJECT( m_window ), "destroy",
   //                   G_CALLBACK( destroy_event ), NULL );

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

GtkAppWindow::~GtkAppWindow()
{
   gtk_widget_destroy(m_mapArea);
   gtk_widget_destroy(m_mapAreaContainer);
   gtk_widget_destroy(m_window);
#ifdef USE_OPENGL_ES
   delete m_glesDrawer;
#endif
}

void GtkAppWindow::initDrawer()
{
#ifdef USE_OPENGL_ES
   m_glesDrawer = WFAPI::GtkGLESFactory::createGtkGLESDrawer( m_mapArea );
#endif
}

WFAPI::DrawingContext* GtkAppWindow::getDrawingContext()
{
#ifdef USE_OPENGL_ES
   return m_glesDrawer->getDrawingContext();
#else
   return NULL;
#endif
}

void GtkAppWindow::expose( GdkEventExpose* event )
{
   if(m_listener) {
      m_listener->onRedraw();      
   }
#ifdef USE_OPENGL_ES
   m_glesDrawer->render();
#endif
   gint width, height;
   gdk_drawable_get_size( m_window->window, &width, &height );

   if(width != m_prevScreenWidth || height != m_prevScreenHeight) {
      m_prevScreenWidth = width;
      m_prevScreenHeight = height;

      if(m_listener) {
         m_listener->onWindowResize(width, height);
      }
   }
   
}

void GtkAppWindow::startDrag( int button, const ScreenPoint& pos ) 
{
   if(m_listener) {
      PointerEvent p;
      PointerEvent::State s = {0,
                               PointerEvent::LEFT_MOUSE_BUTTON,
                               PointerEvent::POINTER_DOWN,
                               1,
                               pos,
                               TileMapUtil::currentTimeMillis()};
      p.allStates.push_back(s);
      p.modStates = p.allStates;
      
      m_listener->onPointerDown(p);
   }
}

void GtkAppWindow::endDrag( int button, const ScreenPoint& pos ) 
{
   if(m_listener) {
      PointerEvent p;
      PointerEvent::State s = {0,
                               PointerEvent::LEFT_MOUSE_BUTTON,
                               PointerEvent::POINTER_UP,
                               1,
                               pos,
                               TileMapUtil::currentTimeMillis()};
      p.allStates.push_back(s);
      p.modStates = p.allStates;
      
      m_listener->onPointerUp(p);
   }
}

void GtkAppWindow::drag( int button, const ScreenPoint& pos ) 
{
   if(m_listener) {
      PointerEvent p;
      PointerEvent::State s = {0,
                               PointerEvent::LEFT_MOUSE_BUTTON,
                               PointerEvent::POINTER_MOVE,
                               1,
                               pos,
                               TileMapUtil::currentTimeMillis()};
      p.allStates.push_back(s);
      p.modStates = p.allStates;
      
      m_listener->onPointerMove(p);
   }
}

void GtkAppWindow::requestDraw()
{
   GdkWindow* window = m_mapArea->window;
   
   GdkRegion* region = gdk_drawable_get_visible_region( window );

   gdk_window_invalidate_region( window, region, true );
   
   // gdk_window_process_all_updates();
}

unsigned int GtkAppWindow::getWidthPixels()
{
   return m_prevScreenWidth;
}

unsigned int GtkAppWindow::getHeightPixels()
{
   return m_prevScreenHeight;
}

