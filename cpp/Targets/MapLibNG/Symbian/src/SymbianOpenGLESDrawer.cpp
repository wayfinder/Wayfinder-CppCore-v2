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
#include "SymbianOpenGLESDrawer.h"

#include "DrawingContextsConcrete.h"
#include "EGLContextWrapper.h"
#include "OpenGlEsExternalContext.h"
#include <e32std.h>
#include <eikapp.h>
#include <eikdoc.h>
#include <eikappui.h>
#include <coecntrl.h>
#include "OpenGlEsTextureBlockManager.h"
#include "OpenGlEsVertexBlockManager.h"
#include "OpenGlEsRenderer.h"
#include "MifImageLoader.h"
#include "SymbianNativeTextInterface.h"
#include "SwapListener.h"
#include "CoreFunctionPrologue.h"

using namespace WFAPI;

class CCoeControlSwapper : public SwapListener {
public:   
   CCoeControlSwapper( CCoeControl& control,
                       EGLContextWrapper* wrapper)
      : m_control( control ),
        m_wrapper(wrapper)
   {
      
   }
   
   virtual void doSwap() {
      EGLDisplay display = m_wrapper->getEGLDisplay();
      EGLSurface surface = m_wrapper->getEGLSurface();

      eglSwapBuffers( display, surface );
      
      // m_control.DrawNow();
   }
private:
   CCoeControl& m_control;
   EGLContextWrapper* m_wrapper;
};

struct SymbianOpenGLESDrawer::Impl {
   OpenGlEsExternalContext* context;
   OpenGlEsRenderer* renderer;
   CCoeControlSwapper* swapper;
};

TInt getBufferColorDepth( RWindow* window ) {
   TDisplayMode DMode = window->DisplayMode();
   TInt BufferSize = 0;

   switch ( DMode )
   {
   case(EColor4K):
      BufferSize = 12;
      break;
   case(EColor64K):
      BufferSize = 16;
      break;
   case(EColor16M):
      BufferSize = 24;
      break;
   case(EColor16MU):
   case(EColor16MA):
      BufferSize = 32;
      break;
   default:
      _LIT(KDModeError,       "unsupported displaymode");
      User::Panic( KDModeError, 0 );
      break;
   }   

   return BufferSize;
}

SymbianOpenGLESDrawer::SymbianOpenGLESDrawer( CCoeControl& parent,
                                              RWindow* window )
{
   CORE_FUNCTION_PROLOGUE();
   
   m_impl = new Impl;
   m_impl->context = NULL;
   m_impl->renderer = NULL;
   
   TInt colorDepth = getBufferColorDepth( window );
   
   m_impl->context = new OpenGlEsExternalContext;
   
   m_impl->context->eglWrapper = 
      new EGLContextWrapper( window, EGL_DEFAULT_DISPLAY, colorDepth,
                             true);

   EGLContextWrapper::ErrorCode err = m_impl->context->eglWrapper->init();

   if ( err == EGLContextWrapper::SUCCESS ) {
      SymbianNativeTextInterface* textInterface =
         new SymbianNativeTextInterface( parent, colorDepth );
   
      OpenGlEsTextureBlockManager* texManager = new OpenGlEsTextureBlockManager;
      OpenGlEsVertexBlockManager* vertManager = new OpenGlEsVertexBlockManager;

      int width = parent.Rect().Width();
      int height = parent.Rect().Height();

      m_impl->context->imgLoader = new MifImageLoader;
      m_impl->context->textInterface = textInterface;
      m_impl->swapper = new CCoeControlSwapper( parent,
                                                m_impl->context->eglWrapper);
   
      m_impl->renderer = new OpenGlEsRenderer( textInterface,
                                               textInterface,
                                               texManager,
                                               vertManager,
                                               width,
                                               height );

      m_impl->renderer->setSwapListener( m_impl->swapper );
   
      m_impl->context->renderer = m_impl->renderer;      
   } else {
      mc2dbg << "Error when initializing egl context: "
             << m_impl->context->eglWrapper->translateError( err ) << endl;
      MC2_ASSERT( !"Error when initializing egl context " );
      m_impl->context = NULL;
   }
}

SymbianOpenGLESDrawer::~SymbianOpenGLESDrawer()
{
   CORE_FUNCTION_PROLOGUE();
   
   MC2_ASSERT( m_impl->context );
   EGLContextWrapper::ErrorCode err =
      m_impl->context->eglWrapper->destroyContext();
   MC2_ASSERT( err == EGLContextWrapper::SUCCESS );
   delete m_impl->context;
   delete m_impl->swapper;
   delete m_impl;
}

DrawingContext* SymbianOpenGLESDrawer::getDrawingContext()
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_impl->context; 
}

void SymbianOpenGLESDrawer::resizeBuffer( int newWidth, int newHeight )
{
   CORE_FUNCTION_PROLOGUE();
   
   m_impl->context->renderer->setScreenSize( newWidth, newHeight ); 
}

void SymbianOpenGLESDrawer::render()
{
   CORE_FUNCTION_PROLOGUE();   

   m_impl->swapper->doSwap();
}
