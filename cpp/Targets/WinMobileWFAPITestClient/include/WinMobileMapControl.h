/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _WINMOBILEMAPCONTROL_H_
#define _WINMOBILEMAPCONTROL_H_

#include <windows.h>
#include <map>
#include "Shared/Nav2StatusListener.h"
#include "WindowsMobile/WinMobileGDIDrawer.h"
#include "MapDrawingCallback.h"

namespace WFAPI {
class ScreenPoint;
class MapLibAPI;
class Nav2API;
class WinMobileGDIDrawer;
class WinMobileOpenGLESDrawer;
class ImageSpec;
}

/**
 *   Windows specific class which deals with common
 *   functionality expected from a map application.
 *   Provides event handling for key input, dragging
 *   and drawing delegation.
 */ 
class WinMobileMapControl : public WFAPI::Nav2StatusListener, public WFAPI::MapDrawingCallback {
public:

   /**
    *   Creates a new map control. The only external MapLib interfaces
    *   it needs are related to key handling and drawing.
    *
    *   @param   keyHandler         The key handler for MapLib.
    *   @param   drawingInterface   The drawing interface for MapLib.
    */ 
   
   WinMobileMapControl( HINSTANCE hInstance );

   /**
    *   Initializes the map control, triggers Nav2 and MapLib to
    *   initiate the startup phase.
    */ 
   void init();

   /**
    *   Initializes the shutdown phase.
    */ 
   void shutdown();
   
   /**
    *   Attempts to handle a standard Windows message. If succesful,
    *   the method will return true.
    *
    *   @param    hWnd     The window the message relates to.
    *   @param    msg      The actual window message.
    *   @param    wParam   The first message parameter.
    *   @param    lParam   The second message parameter.
    *   
    *   @return   True if the control could handle the message,
    *             false otherwise.
    */ 
   
   LRESULT  handleMessage( HWND hWnd, unsigned int msg,
                           WPARAM wParam, LPARAM lParam );

   /**
    *   @from Nav2StatusListener
    */
   virtual void startupComplete();

   /**
    *   @from Nav2StatusListener
    */
   virtual void mapLibStartupComplete();

   /**
    *   @from Nav2StatusListener
    */
   virtual void stopComplete();

   /**
    *   @from Listener
    */
   virtual void error(const WFAPI::AsynchronousStatus& status);

   /**
    *  When this is called, we will trigger the expose event.
    */
   virtual void mapIsReadyToBeDrawn();
private:   

   /**
    *   Creates a new full screen window and ties it to this control.
    */ 
   void createFullScreenWindow();
   
   /**
    *   Handles a key event.
    *
    *   @param    hWnd     The window the message relates to.
    *   @param    msg      The actual window message ( WM_KEYDOWN or
    *                      WM_KEYUP)
    *   @param    wParam   The first message parameter.
    *   @param    lParam   The second message parameter.
    *   @param    repeat   Indicates whether the key is to be
    *                      treated as a repeat event.
    *
    *   @return   True if the control was able to process event.
    */ 

   bool handleKey( HWND hWnd,
                   unsigned int msg,
                   WPARAM wParam,
                   LPARAM lParam,
                   bool repeat );

   /**
    *   Handles a mouse event.
    *   
    *   @param    msg      The actual window message ( WM_MOUSEMOVE,
    *                      WM_LBUTTONDOWN or WM_LBUTTONUP ).
    *                      
    *   @param    wParam   The first message parameter.
    *   @param    lParam   The second message parameter.
    */ 
   
   bool handleMouseEvent( unsigned int msg,
                          WPARAM wParam,
                          LPARAM lParam );

   /**
    *   Indicates that a drag has started.
    *
    *   @param   button   The button for which the drag started.
    *   @param   pos      The position where the drag started.
    *
    */
   
   void startDrag( int button, const WFAPI::ScreenPoint& pos );

   /**
    *   Indicates the end of a drag.
    *
    *   @param   button   The button for which the drag has ended.
    *   @param   pos      The position where the drag ended.
    *   
    */
   
   void endDrag( int button, const WFAPI::ScreenPoint& pos );

   /**
    *   Indicates that a drag action is being performed.
    *
    *   @param   button   The button for which the drag is performed.
    *   @param   pos      The current position for the drag action.
    *   
    */
   
   void drag( int button, const WFAPI::ScreenPoint& pos );

   /**
    *   Delegates the appropriate drag call to the key handler.
    *   Used by startDrag, endDrag and drag.
    *
    *   @param   button        The button for which the drag is performed.
    *   @param   kindOfPress   Key handler specific press code.
    *   @param   pos           The position of the drag action.
    */

   void dragAction( int button, int kindOfPress, const WFAPI::ScreenPoint& pos );
   
   /**
    *   The translation map used to translate between
    *   windows specific keys and MapLib platform independent
    *   action keys.
    */
   
   typedef std::map<int, int> KeyMap;

   /**
    *   The instance of the translation key map.
    */
   
   KeyMap m_keyMap;

   /**
    *   Toggles the drawer between OpenGL ES and GDI.
    */ 
   void toggleDrawer();

   /**
    *   Initializes our sample screen components that we'll draw
    *   on top of the map.
    */ 
   void initializeScreenComponents();

   /**
    *   Converts an error code from Win32 to a readable string and
    *   prints it. 
    */ 
   void printError(DWORD err);


   
   enum DrawerState { UNINITIALIZED, GDI_DRAWER, OPENGL_ES_DRAWER};

   DrawerState m_drawerState;
   
   bool m_mapLibStarted;

   bool m_nav2Started;

   bool m_shuttingDown;
   
   HWND m_hWnd;

   HINSTANCE m_hInstance;

   WFAPI::MapLibAPI* m_mapLib;
   
   WFAPI::Nav2API* m_nav2;

   WFAPI::WinMobileGDIDrawer* m_drawer;

   WFAPI::WinMobileOpenGLESDrawer* m_glesDrawer;

   WFAPI::ImageSpec* m_sampleImage;

   std::string m_progPath;

   /// Our sample image specification.
   WFAPI::ImageSpec* m_testImg;

   /// Our sample image specification.
   WFAPI::ImageSpec* m_testText;
};


#endif /* _WINMOBILEMAPCONTROL_H_ */


