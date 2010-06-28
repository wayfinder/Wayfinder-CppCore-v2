/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "WinMobileMapControl.h"
#include "WindowsMobile/WinMobileFactory.h"
#include "Shared/MapLibKeyInterface.h"
#include "Shared/MapLibAPI.h"
#include "Shared/Nav2API.h"
#include "Shared/ScreenPoint.h"
#include "Shared/StartupData.h"
#include "Shared/MapLibInitialConfig.h"
#include "WindowsMobile/WindowsMobileNav2API.h"
#include "VoiceLanguage.h"
#include "TextLanguage.h"
#include <iostream>
#include <aygshell.h>
#include <shlobj.h>
#include "Shared/MapDrawingInterface.h"
#include "PALGraphics.h"
#include <cassert>

#define WM_USE_OPENGL_ES

#ifdef WM_USE_OPENGL_ES
#include "WindowsMobile/WinMobileOpenGLESDrawer.h"
#endif

using namespace WFAPI;

static WinMobileMapControl* s_winMobileMapControl = NULL;

LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
   if(s_winMobileMapControl) {
      return s_winMobileMapControl->handleMessage( hWnd, msg, wParam, lParam );
   } else {
      return DefWindowProc(hWnd, msg, wParam, lParam);
   }
}

WinMobileMapControl::WinMobileMapControl( HINSTANCE hInstance )
{
   m_drawerState = UNINITIALIZED;
   
   /**
    *   Initialize the translation map.
    */   
   m_keyMap[VK_UP]     = MapLibKeyInterface::MOVE_UP_KEY;
   m_keyMap[VK_SPACE]  = MapLibKeyInterface::ZOOM_OUT_KEY;
   m_keyMap[VK_TAB]    = MapLibKeyInterface::ZOOM_IN_KEY;
   m_keyMap[VK_DOWN]   = MapLibKeyInterface::MOVE_DOWN_KEY;
   m_keyMap[VK_LEFT]   = MapLibKeyInterface::MOVE_LEFT_KEY;
   m_keyMap[VK_RIGHT]  = MapLibKeyInterface::MOVE_RIGHT_KEY;
   m_keyMap[VK_RETURN] = MapLibKeyInterface::ROTATE_LEFT_KEY;
   m_keyMap[VK_BACK]   = MapLibKeyInterface::ROTATE_RIGHT_KEY;

   m_mapLib = NULL;
   m_nav2 = NULL;
   m_mapLibStarted = false;
   m_nav2Started = false;
   m_hInstance = hInstance;
   m_drawer = NULL;
   
#ifdef WM_USE_OPENGL_ES
   m_glesDrawer = NULL;
#endif
   
   m_shuttingDown = false;
   
   createFullScreenWindow();
   
   s_winMobileMapControl = this;
   m_testImg = NULL;
   m_testText = NULL;
}


void WinMobileMapControl::init()
{
   m_nav2 = new WindowsMobileNav2API;

   wchar_t wideProgramFilesPath[MAX_PATH];
   char programFilesPath[MAX_PATH];
   char dataPath[MAX_PATH];
   char imgPath[MAX_PATH];
   char progPath[MAX_PATH];

   
   memset(imgPath, 0, MAX_PATH);
   memset(dataPath, 0, MAX_PATH);
   memset(wideProgramFilesPath, 0, MAX_PATH * sizeof(wchar_t));
   memset(programFilesPath, 0, MAX_PATH);
   
   if(!SUCCEEDED(SHGetSpecialFolderPath(NULL,
                                        wideProgramFilesPath,
                                        CSIDL_PROGRAM_FILES,
                                        true))) 
   {
      //Uh-oh
   }

   DWORD uniLen = WideCharToMultiByte(CP_OEMCP,
                                      0,
                                      wideProgramFilesPath,
                                      wcslen(wideProgramFilesPath),
                                      programFilesPath,
                                      MAX_PATH,
                                      NULL, NULL);
   
   sprintf(dataPath, "%s\\%s", programFilesPath,
           "WinMobileWFAPITestClient\\nav2data\\");
   sprintf(imgPath, "%s\\%s", programFilesPath,
           "WinMobileWFAPITestClient\\nav2data\\csicons\\");
   sprintf(progPath, "%s\\%s", programFilesPath,
           "WinMobileWFAPITestClient");

   m_progPath = progPath;
   
   WFAPI::StartupData startupData(dataPath,
                                  dataPath,
                                  dataPath,
                                  WFAPI::ENGLISH,
                                  WFAPI::VoiceLanguage::ENGLISH,
                                  imgPath, imgPath);

   AsynchronousStatus status = m_nav2->start(this, &startupData);
   if(status.getStatusCode() != OK){
      std::cout << "Unable to start nav2, Returned error: "
                << status.getStatusCode() << std::endl;
   }
}


void WinMobileMapControl::shutdown()
{
   m_shuttingDown = true;
   m_nav2->stop();
   m_mapLib->stop();
}

void WinMobileMapControl::createFullScreenWindow()
{
   WNDCLASS wc = { 0L, MsgProc, 0L, 0L, 
                   m_hInstance, NULL, NULL, NULL, NULL,
                   TEXT( "WinMobileMapClient Application" ) };

   RegisterClass( &wc );

   int iScreenWidth = GetSystemMetrics( SM_CXSCREEN );
   int iScreenHeight = GetSystemMetrics( SM_CYSCREEN );

   // Create the application's window
   m_hWnd = CreateWindow( TEXT( "WinMobileMapClient Application" ), 
                          TEXT( "WinMobileMapClient App" ), 
                          WS_VISIBLE, 
                          0, 0, iScreenWidth, iScreenHeight,
                          NULL, NULL, wc.hInstance, NULL );
   
   SHFullScreen( m_hWnd, SHFS_HIDESIPBUTTON | SHFS_HIDETASKBAR );
}

LRESULT WinMobileMapControl::handleMessage( HWND hWnd, unsigned int msg,
                                            WPARAM wParam, LPARAM lParam )
{
   static bool repeat = false;

   HDC hdc;
   PAINTSTRUCT ps;
   static SHACTIVATEINFO s_sai;
   
   switch( msg ) {
      case WM_ACTIVATE:
         // Notify shell of our activate message
         SHHandleWMActivate(hWnd, wParam, lParam, &s_sai, FALSE);
         break;
      case WM_MOUSEMOVE:
      case WM_LBUTTONDOWN:
      case WM_LBUTTONUP: 
         handleMouseEvent( msg, wParam, lParam );
         break;
      case WM_KEYDOWN:
         handleKey( hWnd, msg, wParam, lParam, repeat );
         repeat = false;
         break;
      case WM_KEYUP:         
         repeat = false;
         handleKey( hWnd, msg, wParam, lParam, repeat );
         break;
      case WM_PAINT: {
         if(m_drawerState == GDI_DRAWER)  {

            hdc = BeginPaint(hWnd, &ps);

            
            if(m_drawer && !m_shuttingDown) {

//#define TEST_GET_BACK_BUFFER
#ifdef TEST_GET_BACK_BUFFER               
               HDC backDc = m_drawer->getBackBuffer();

               // Create an array of TRIVERTEX structures that describe 
               // positional and color values for each vertex. For a rectangle, 
               // only two vertices need to be defined: upper-left and lower-right. 
               TRIVERTEX vertex[2] ;
               vertex[0].x     = 0;
               vertex[0].y     = 0;
               vertex[0].Red   = 0x0000;
               vertex[0].Green = 0x8000;
               vertex[0].Blue  = 0x8000;
               vertex[0].Alpha = 0x0000;

               vertex[1].x     = 300;
               vertex[1].y     = 80; 
               vertex[1].Red   = 0x0000;
               vertex[1].Green = 0xd000;
               vertex[1].Blue  = 0xd000;
               vertex[1].Alpha = 0x0000;

               // Create a GRADIENT_RECT structure that 
               // references the TRIVERTEX vertices. 
               GRADIENT_RECT gRect;
               gRect.UpperLeft  = 0;
               gRect.LowerRight = 1;

               // Draw a shaded rectangle. 
               GradientFill(backDc, vertex, 2, &gRect, 1, GRADIENT_FILL_RECT_H);
#endif

               m_drawer->doSwap( hdc );
            }
            
            EndPaint(hWnd, &ps);
         } else if(m_drawerState == OPENGL_ES_DRAWER) {
            return DefWindowProc(hWnd, msg, wParam, lParam); 
         } else {
            return DefWindowProc(hWnd, msg, wParam, lParam);
         }
         break;
      }
      default:
         return DefWindowProc(hWnd, msg, wParam, lParam);
   }
   
   return 0;
}

MapLibKeyInterface::kindOfPressType getPressType( unsigned int msg,
                                                  bool repeat )
{
   if( msg == WM_KEYUP ) {
      return MapLibKeyInterface::KEY_UP_EVENT;
   } else {
      // if( repeat ) {
      //    return MapLibKeyInterface::KEY_REPEAT_EVENT;
      // } else {
         return MapLibKeyInterface::KEY_DOWN_EVENT;
      // }
   }
}

bool WinMobileMapControl::handleKey( HWND hWnd, 
                                     unsigned int msg,
                                     WPARAM wParam,
                                     LPARAM lParam,
                                     bool repeat )
{
   MapLibKeyInterface::kindOfPressType type = getPressType( msg, repeat );
   
   /**
    *   Attempt to find the entered key in the translation map.
    */
   
   KeyMap::iterator kIt = m_keyMap.find( wParam );
   
   if( kIt != m_keyMap.end() ) {
      
      /**
       *   If the entered key is found in the translation map,
       *   we attempt to handle the key using our key handler.
       */
      int key = kIt->second;
      
      bool handled = m_mapLib->getKeyInterface()->handleKeyEvent(
         MapLibKeyInterface::keyType( key ), type);
      
      return handled;
   } else {
      switch(wParam) {
         case 'Q':
            shutdown();
         default:
            return false;
      }
   }
}

bool WinMobileMapControl::handleMouseEvent( unsigned int msg,
                                            WPARAM wParam,
                                            LPARAM lParam )
{
   static bool mouseDrag = false;
   ScreenPoint pos = ScreenPoint( LOWORD(lParam), HIWORD(lParam) ) ;

   switch( msg ) {
      case WM_LBUTTONDOWN:
         startDrag( 1, pos );
         mouseDrag = true;
         break;
      case WM_LBUTTONUP:
         endDrag( 1, pos );
         mouseDrag = false;
         break;
      case WM_MOUSEMOVE:
         if( mouseDrag ){
            drag( 1, pos );
         }
         break;
   }
   return true;
}

void WinMobileMapControl::startDrag( int button, const ScreenPoint& pos )
{
   dragAction( button, MapLibKeyInterface::POINTER_DOWN_EVENT, pos );
}

void WinMobileMapControl::endDrag( int button, const ScreenPoint& pos )
{
   dragAction( button, MapLibKeyInterface::POINTER_UP_EVENT, pos );
}

void WinMobileMapControl::drag( int button, const ScreenPoint& pos )
{
   dragAction( button, MapLibKeyInterface::POINTER_UPDATE_EVENT, pos );
}

void WinMobileMapControl::dragAction( int button,
                                      int kindOfPress,
                                      const ScreenPoint& pos )
{
   if ( button == 1 ) {
      m_mapLib->getKeyInterface()->handlePointerEvent(
         MapLibKeyInterface::DRAG_TO,
         static_cast<MapLibKeyInterface::kindOfPointerType>( kindOfPress ),
         pos);
   }
}

void WinMobileMapControl::mapIsReadyToBeDrawn() {
   // Draw any custom overlay here before we invalidate the window.            
   if(m_testImg) {
      m_mapLib->getMapDrawingInterface()->drawImageSpec(
         m_testImg, WFAPI::ScreenPoint(50, 50) );

      m_mapLib->getMapDrawingInterface()->drawImageSpec(
         m_testText, WFAPI::ScreenPoint(72, 55) );
   }

#ifdef WM_USE_OPENGL_ES   
   if(m_drawerState == OPENGL_ES_DRAWER) {
      m_glesDrawer->render();      
   }
#endif
   
   InvalidateRect(m_hWnd, NULL, false);
}

void WinMobileMapControl::startupComplete() 
{
   std::cout << "startupComplete()" << std::endl;
   
   m_nav2Started = true;

   MapLibInitialConfig initialConfig;

   WFAPI::DrawingContext* drawContext = NULL;
   
   m_drawer = WinMobileFactory::createGDIDrawer(m_hWnd);

#ifdef WM_USE_OPENGL_ES
   m_glesDrawer = WinMobileFactory::createOpenGlEsDrawer(m_hWnd);
   m_drawerState = OPENGL_ES_DRAWER;
   drawContext = m_glesDrawer->getDrawingContext();
#else
   m_drawerState = GDI_DRAWER;
   drawContext = m_drawer->getDrawingContext();
#endif
   
   m_mapLib = WinMobileFactory::createMapLib(
      m_nav2->getDBufConnection(),
      drawContext,
      initialConfig,
      0, 0);
   
   m_mapLib->getMapDrawingInterface()->setMapDrawingCallback(this);

   m_nav2->connectMapLib(m_mapLib);
}

void WinMobileMapControl::mapLibStartupComplete() 
{
    std::cout << "mapLibStartupComplete()" << std::endl;

    initializeScreenComponents();
   
    m_mapLibStarted = true;
}

void WinMobileMapControl::stopComplete() 
{
   std::cout << "stopComplete()" << std::endl;
   delete m_nav2;
   delete m_mapLib;
   PostQuitMessage( 0 );   
}


void WinMobileMapControl::error(const WFAPI::AsynchronousStatus& status)
{

}

void WinMobileMapControl::toggleDrawer()
{
#ifdef WM_USE_OPENGL_ES
   if(m_drawerState == GDI_DRAWER)  {
      m_drawerState = OPENGL_ES_DRAWER;
      m_mapLib->setDrawingContext(m_glesDrawer->getDrawingContext());
   } else if(m_drawerState == OPENGL_ES_DRAWER) {
      m_drawerState = GDI_DRAWER;
      m_mapLib->setDrawingContext(m_drawer->getDrawingContext());
   }
#endif
}

void WinMobileMapControl::initializeScreenComponents()
{
#ifdef WM_USE_OPENGL_ES
   

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

   status = pal::runGraphicsTests(context);
   assert(status == pal::PAL_OK);
   
   // The path to our test image
   std::string imgPath(m_progPath);
   
   imgPath += "\\images\\image1.png";
   
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
   
   // Set the desired font to use when generating texture
   status = pal::setFontForContext(context, "Times New Roman", "Regular", 48);
   assert(status == pal::PAL_OK);
   
   // Set the desired font color to use when generating texture
   status = pal::setFontColorForContext(context, 0, 0, 255);
   assert(status == pal::PAL_OK);
   
   // Draw the actual font 
   status = pal::drawTextLineToBuffer(context,
                                  "Test string with åäö",
                                  0, -1,
                                  pal::RGBA8,
                                  &widthPixels,
                                  &heightPixels,
                                  &resBuffer);
   assert(status == pal::PAL_OK);
   
   // Create the image specification that can be drawn later on.
   m_testText = m_glesDrawer->createImageSpec(resBuffer.getData(),
                                              widthPixels, heightPixels);

   // Release all the graphics resources.
   status = pal::destroyGraphicsContext(context);
   
   assert(status == pal::PAL_OK);
      
#endif
}

void WinMobileMapControl::printError(DWORD err)
{
   LPVOID lpMsgBuf;

   FormatMessage(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | 
      FORMAT_MESSAGE_FROM_SYSTEM |
      FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL,
      err,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      (LPTSTR) &lpMsgBuf,
      0, NULL );

   wprintf(L"Error: %s", (wchar_t*)(lpMsgBuf));
   LocalFree(lpMsgBuf);
}
