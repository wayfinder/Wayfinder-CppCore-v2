/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "CoreFunctionPrologue.h"
#include "config.h"
#include "GtkGLESDrawer.h"
#include <GLES/egl.h>
#include <GLES/gl.h>
#include <iostream>
#include <gdk/gdkx.h>
#include "DrawingContextsConcrete.h"
#include "PangoTextInterface.h"
#include "EGLContextWrapper.h"
#include "OpenGlEsTextureBlockManager.h"
#include "OpenGlEsVertexBlockManager.h"
#include "PNGImageLoader.h"
#include "SwapListener.h"
#include "OpenGlEsExternalContext.h"
#include "OpenGlEsRenderer.h"
#include "SwapListener.h"

#include "Graphics/Text/PALTextInterface.h"

class GtkGLESSwapper : public SwapListener {
public:
   GtkGLESSwapper( GdkWindow* window ) :
      m_window( window )
      {
         
      }

   virtual void doSwap() {
      GdkRegion* region = gdk_drawable_get_visible_region( m_window );
      gdk_window_invalidate_region( m_window, region, true );
      gdk_window_process_all_updates();
   }
   
private:
   GdkWindow* m_window;
};

struct WFAPI::GtkGLESDrawer::Impl {
   GtkWidget* widget;
   OpenGlEsExternalContext* context;
   SwapListener* swapper;
};

namespace WFAPI {


GtkGLESDrawer::GtkGLESDrawer( GtkWidget* widget )
{
   CORE_FUNCTION_PROLOGUE();
   
   m_impl = new Impl;
   m_impl->widget = widget;
   m_impl->context = NULL;
   m_impl->swapper = NULL;
}


bool GtkGLESDrawer::init() 
{
   CORE_FUNCTION_PROLOGUE();
   
   /**
    *  We need to disable the double buffering so that it does not
    *  interfere with the OpenGL ES swapping.
    */
   
   gtk_widget_set_double_buffered( m_impl->widget, false );

   Display* x11Display = gdk_x11_drawable_get_xdisplay( m_impl->widget->window );
   Window x11Window = GDK_WINDOW_XID( m_impl->widget->window );

   EGLContextWrapper* eglWrapper =
      new EGLContextWrapper( (NativeWindowType) x11Window,
                             (NativeDisplayType) x11Display,
                             gdk_drawable_get_depth( m_impl->widget->window ),
                             true);

#if defined OPENGL_PAL_TEXT_RENDERING
   PALTextInterface* textInterface = new PALTextInterface;
#else
   PangoTextInterface* textInterface = new PangoTextInterface;
#endif

   BitMapTextLoader* textLoader = textInterface;

   EGLContextWrapper::ErrorCode err = eglWrapper->init();

   if ( err != EGLContextWrapper::SUCCESS ) {
      mc2dbg << "Error when initializing egl context: "
             << eglWrapper->translateError( err ) << endl;
      return false;
   }

   OpenGlEsTextureBlockManager* texManager =
      new OpenGlEsTextureBlockManager;

   OpenGlEsVertexBlockManager* vertManager=
      new OpenGlEsVertexBlockManager;
   gint width, height;
   gdk_drawable_get_size( m_impl->widget->window, &width, &height );

   ExternalRenderer* renderer =
      new OpenGlEsRenderer( textLoader,
                            textInterface,
                            texManager,
                            vertManager,
                            width, height );

   m_impl->swapper = new GtkGLESSwapper( m_impl->widget->window );
   renderer->setSwapListener( m_impl->swapper );
   
   m_impl->context = new OpenGlEsExternalContext;
   
   m_impl->context->imgLoader = new PNGImageLoader;
   m_impl->context->renderer = renderer;
   m_impl->context->textInterface = textInterface;
   m_impl->context->eglWrapper = eglWrapper;
  
   return true;
}

void GtkGLESDrawer::render()
{
   CORE_FUNCTION_PROLOGUE();
   
   // glFlush();
	// glFinish(); 
   
   eglSwapBuffers( m_impl->context->eglWrapper->getEGLDisplay(),
                   m_impl->context->eglWrapper->getEGLSurface() ) ;  
}

WFAPI::DrawingContext* GtkGLESDrawer::getDrawingContext()
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_impl->context; 
}

GtkGLESDrawer::~GtkGLESDrawer()
{
   CORE_FUNCTION_PROLOGUE();
   
   delete m_impl->context;
   delete m_impl->swapper;
   delete m_impl;
}

ImageSpec* WFAPI::GtkGLESDrawer::createImageSpec(const pal::wf_uint8* buffer,
                                                 pal::wf_uint32 widthPixels,
                                                 pal::wf_uint32 heightPixels)
{
   return m_impl->context->renderer->loadImageSpecFromRGBA(buffer,
                                                           widthPixels,
                                                           heightPixels); 
}

ImageSpec* WFAPI::GtkGLESDrawer::createTextSpec(const char* utf8Sring)
{
   return NULL;
}


} // End of namespace WFAPI
