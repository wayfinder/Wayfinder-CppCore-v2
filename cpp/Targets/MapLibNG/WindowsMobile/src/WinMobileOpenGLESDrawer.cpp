/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#define CORE_LOGGING_MODULE_NAME "WinMobileOpenGLESDrawer"
// #define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE
#include "CoreDebugPrint.h"

#include "WinMobileOpenGLESDrawer.h"
#include "EGLContextWrapper.h"
#include "OpenGlEsExternalContext.h"
#include "OpenGlEsTextureBlockManager.h"
#include "OpenGlEsVertexBlockManager.h"
#include "OpenGlEsRenderer.h"
#include "WinMobileNativeTextInterface.h"
#include "PNGImageLoader.h"
#include "SwapListener.h"
#include "CoreFunctionPrologue.h"
#include "WinMobileImageSpec.h"

using namespace WFAPI;

class WinMobileSwapper : public SwapListener {
public:   
   WinMobileSwapper(HWND hWnd)
      : m_hWnd(hWnd)
      {
      
      }
   
   virtual void doSwap() {
      coreprintln("doSwap");
      InvalidateRect(m_hWnd, NULL, false);
   }
private:
   HWND m_hWnd;
};


struct WinMobileOpenGLESDrawer::Impl {
   OpenGlEsExternalContext* context;
   OpenGlEsRenderer* renderer;
   WinMobileSwapper* swapper;
};

WinMobileOpenGLESDrawer::WinMobileOpenGLESDrawer(HWND parentWindow)
{
   CORE_FUNCTION_PROLOGUE();
   
   m_impl = new Impl;
   m_impl->context = NULL;
   m_impl->renderer = NULL;

   m_impl->context = new OpenGlEsExternalContext;
   // HDC hdc = GetWindowDC(parentWindow);
   m_impl->context->eglWrapper =
      new EGLContextWrapper(parentWindow, EGL_DEFAULT_DISPLAY, 16,
      // new EGLContextWrapper(parentWindow, hdc, 16,
                            true);
   
   EGLContextWrapper::ErrorCode err = m_impl->context->eglWrapper->init();

   if ( err == EGLContextWrapper::SUCCESS ) {
      OpenGlEsTextureBlockManager* texManager = new OpenGlEsTextureBlockManager;
      OpenGlEsVertexBlockManager* vertManager = new OpenGlEsVertexBlockManager;
      WinMobileNativeTextInterface* textInterface = new WinMobileNativeTextInterface;
      
      m_impl->context->imgLoader = new PNGImageLoader;
      m_impl->swapper = new WinMobileSwapper(parentWindow);
      m_impl->context->textInterface = textInterface;

      RECT windowRect;
      GetWindowRect(parentWindow, &windowRect);
      int width = windowRect.right - windowRect.left;
      int height = windowRect.bottom - windowRect.top;

      m_impl->renderer = new OpenGlEsRenderer( textInterface, textInterface,
                                               texManager,
                                               vertManager,
                                               width,
                                               height);
      m_impl->renderer->setSwapListener( m_impl->swapper );
      m_impl->context->renderer = m_impl->renderer;
   } else {
      std::cout
         << m_impl->context->eglWrapper->translateError(err)
         << std::endl;
      MC2_ASSERT( !"Error when initializing egl context " );
      
      delete m_impl->context;
      m_impl->context = NULL;
   }
}

WinMobileOpenGLESDrawer::~WinMobileOpenGLESDrawer()
{
   CORE_FUNCTION_PROLOGUE();
   
   delete m_impl;
}

DrawingContext* WFAPI::WinMobileOpenGLESDrawer::getDrawingContext()
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_impl->context; 
}

WFAPI::ImageSpec*
WFAPI::WinMobileOpenGLESDrawer::createImageSpec(const pal::wf_uint8* buffer,
                                                pal::wf_uint32 widthPixels,
                                                pal::wf_uint32 heightPixels)
{
   return m_impl->context->renderer->loadImageSpecFromRGBA(buffer,
                                                           widthPixels,
                                                           heightPixels);
}

void WFAPI::WinMobileOpenGLESDrawer::render()
{
   EGLDisplay display = m_impl->context->eglWrapper->getEGLDisplay();
   EGLSurface surface = m_impl->context->eglWrapper->getEGLSurface();

   // glFlush();
	// glFinish(); //TODO: CHECK IF THIS HINDERS PERFORMANCE
   
   eglSwapBuffers( display, surface );
}



