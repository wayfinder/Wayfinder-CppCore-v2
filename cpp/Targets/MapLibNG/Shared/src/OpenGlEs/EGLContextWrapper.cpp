/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef IPHONE

#include "config.h"
#include "EGLContextWrapper.h"
#include <cstdlib>
#include "Utility.h"


const char* EGLContextWrapper::translateError( ErrorCode error ) {
   switch( error ) {
   case SUCCESS:
      return "SUCCESS";
      break;
   case NOT_INITIALIZED:
      return "NOT_INITIALIZE";
      break;
   case BAD_ACCESS:
      return "BAD_ACCESS";
      break;
   case BAD_ALLOC:
      return "BAD_ALLOC";
      break;
   case BAD_ATTRIBUTE:
      return "BAD_ATTRIBUTE";
      break;
   case BAD_CONFIG:
      return "BAD_CONFIG";
      break;
   case BAD_CONTEXT:
      return "BAD_CONTEXT";
      break;
   case BAD_CURRENT_SURFACE:
      return "BAD_CURRENT_SURFACE";
      break;
   case BAD_DISPLAY:
      return "BAD_DISPLAY";
      break;
   case BAD_MATCH:
      return "BAD_MATCH";
      break;
   case BAD_NATIVE_PIXMAP:
      return "BAD_NATIVE_PIXMAP";
      break;
   case BAD_NATIVE_WINDOW:
      return "BAD_NATIVE_WINDOW";
      break;
   case BAD_PARAMETER:
      return "BAD_PARAMETER";
      break;
   case BAD_SURFACE:
      return "BAD_SURFACE";
      break;
   case CONTEXT_LOST:
      return "CONTEXT_LOST";
   case NO_CONFIGURATIONS:
      return "NO_CONFIGURATIONS";
      break;
   default:
      return "";
      break;
   }
}

EGLContextWrapper::EGLContextWrapper( NativeWindowType nativeWindow,
                                      NativeDisplayType nativeDisplay,
                                      EGLint colorDepth,
                                      bool vSync) :
   m_display( 0 ),
   m_context( 0 ),
   m_surface( 0 ),
   m_config( 0 ),
   m_nativeWindow( nativeWindow ),
   m_nativeDisplay( nativeDisplay ),
   m_colorDepth( colorDepth ),
   m_vSync(vSync)
{
   m_curPosition = NULL;
}

#define CHECK_EGL_STATUS(position)              \
   setPosition( (position) );                   \
   if( eglGetError() != EGL_SUCCESS) {          \
      return ErrorCode( eglGetError() ); }

EGLContextWrapper::ErrorCode EGLContextWrapper::init()
{

   EGLConfig* configList = NULL;
   EGLint numOfConfigs = 0;
   EGLint configSize = 0;

   const EGLint attribList[] =
      {
#ifdef MAEMO5
         EGL_SAMPLES, 4,
#endif
         EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
         EGL_BUFFER_SIZE, m_colorDepth,
         EGL_NONE
      };

   // Get the default display from platform.
   m_display = eglGetDisplay( m_nativeDisplay );

   CHECK_EGL_STATUS( "eglGetDisplay" );

   // const char* eglVendor = eglQueryString( m_display, 
   //                                         EGL_VENDOR );

   // const char* eglExtensions = eglQueryString( m_display, 
   //                                         EGL_EXTENSIONS );

   // const char* eglVersion = eglQueryString( m_display, 
   //                                          EGL_EXTENSIONS );
   
   // PRINT_STRING( eglVendor );
   // PRINT_STRING( eglExtensions );
   // PRINT_STRING( eglVersion );

   // Initialize the display
   eglInitialize( m_display, NULL, NULL );
   CHECK_EGL_STATUS( "eglInitialize" );


   // Query the available configurations on the platform
   eglGetConfigs( m_display,
                  configList,
                  configSize,
                  &numOfConfigs );
   
   CHECK_EGL_STATUS( "eglGetConfigs" );

   // Allocate a list which will hold the configurations in order
   // of suitability
   configSize = numOfConfigs;
   configList = ( EGLConfig* ) malloc( sizeof( EGLConfig ) * configSize );

   // Create and sort this list
   eglChooseConfig( m_display, attribList,
                    configList, configSize, &numOfConfigs );
   
   CHECK_EGL_STATUS( "eglChooseConfig" );

   // If we cannot find any suitable configuration, we have failed
   if ( numOfConfigs == 0 ) {
      return NO_CONFIGURATIONS;
   }

   // The returned config list is sorted in best matching order so
   // our config will be the first one
   m_config = configList[ 0 ];

   // const char* vendor = eglQueryString( m_display,
   //                                      EGL_VENDOR );
   // const char* version = eglQueryString( m_display,
   //                                       EGL_VERSION );
   // EGLint currAttribVal = 0;
   // eglGetConfigAttrib( m_display, m_config,
   //                     EGL_BUFFER_SIZE, &currAttribVal );
   // // print currAttribVal
   
   // eglGetConfigAttrib( m_display, m_config,
   //                     EGL_NATIVE_RENDERABLE, &currAttribVal );
   // // printf currAttribVal
   // // eglGetConfigAttrib( m_display, m_config,
   // //                     EGL_RENDERABLE_TYPE, &currAttribVal );


   free( configList );

   // Create the surface which we will draw upon, it is tied to the native window
   m_surface = eglCreateWindowSurface( m_display, m_config, m_nativeWindow, NULL );
   CHECK_EGL_STATUS( "eglCreateWindowSurface" );

   // Create our rendering context
   m_context = eglCreateContext( m_display, m_config, EGL_NO_CONTEXT, NULL );
   CHECK_EGL_STATUS( "eglCreateContext" );

   // Tie our rendering contex, surface and display together
   eglMakeCurrent( m_display, m_surface, m_surface, m_context );
   CHECK_EGL_STATUS( "eglMakeCurrent" );

#ifdef GL_OES_VERSION_1_1
   if(m_vSync) {
      eglSwapInterval(m_display, 0);
   }
#endif
   
   return SUCCESS;
}

EGLContextWrapper::ErrorCode EGLContextWrapper::doSwap()
{
   eglSwapBuffers( m_display, m_surface );
   CHECK_EGL_STATUS( "eglSwapBuffers" );
   return SUCCESS;
}


EGLContextWrapper::ErrorCode EGLContextWrapper::destroyContext()
{
   eglDestroySurface( m_display, m_surface );
   CHECK_EGL_STATUS( "eglDestroySurface" );
   
   eglDestroyContext( m_display, m_context );
   CHECK_EGL_STATUS( "eglDestroyContext" );
   
   eglTerminate( m_display );
   CHECK_EGL_STATUS( "eglTerminate" );
   
   return SUCCESS; 
}
#undef CHECK_EGL_STATUS

void EGLContextWrapper::setPosition( const char* position )
{
   m_curPosition = position;
}

const char* EGLContextWrapper::getPosition()
{
   return m_curPosition;
}

EGLDisplay EGLContextWrapper::getEGLDisplay()
{
   return m_display; 
}

EGLSurface EGLContextWrapper::getEGLSurface()
{
   return m_surface; 
}

#endif //IPHONE
