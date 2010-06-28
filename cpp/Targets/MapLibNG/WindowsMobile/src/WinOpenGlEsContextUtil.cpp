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
#include "WinOpenGlEsContextUtil.h"
#include "EGLContextWrapper.h"
#include "DrawingContextsConcrete.h"
#include "OpenGlEsTextureBlockManager.h"
#include "OpenGlEsVertexBlockManager.h"
#include "OpenGlEsRenderer.h"
#include "WinMobileNativeTextInterface.h"
#include "PNGImageLoader.h"
#include "OpenGlEsExternalContext.h"
#include <iostream>

/**
 *   Shared context initialization code.
 */

DrawingContext* createContextInternal( EGLDisplay display,
                                       EGLSurface surface,
                                       OpenGlEsExternalContext* context );

DrawingContext*
WinOpenGlEsContextUtil::createContext( EGLDisplay display,
                                       EGLSurface surface )
{
   OpenGlEsExternalContext* ret = new OpenGlEsExternalContext;
   ret->eglWrapper = NULL;
   return createContextInternal( display, surface, ret );
}

DrawingContext* WinOpenGlEsContextUtil::createContext( HWND hWnd )
{
   OpenGlEsExternalContext* ret = new OpenGlEsExternalContext;
   ret->eglWrapper = new EGLContextWrapper( hWnd, 16 );

   EGLContextWrapper::ErrorCode err = ret->eglWrapper->init();

   if ( err != EGLContextWrapper::SUCCESS ) {
      mc2dbg << "Error when initializing egl context: "
             << ret->eglWrapper->translateError( err ) << endl;
      MC2_ASSERT( !"Error when initializing egl context " );
      return NULL;
   }
   
   return createContextInternal( ret->eglWrapper->getEGLDisplay(),
                                 ret->eglWrapper->getEGLSurface(),
                                 ret );
}

DrawingContext* createContextInternal( EGLDisplay display,
                                       EGLSurface surface,
                                       OpenGlEsExternalContext* context )
{   
   OpenGlEsTextureBlockManager* texManager = new OpenGlEsTextureBlockManager;
   OpenGlEsVertexBlockManager* vertManager= new OpenGlEsVertexBlockManager;
   WinMobileNativeTextInterface* textInterface =
      new WinMobileNativeTextInterface;
   EGLint eglWidth, eglHeight;

   EGLBoolean ret = eglQuerySurface( display,
                                     surface,
                                     EGL_WIDTH,
                                     &eglWidth ) ;
   MC2_ASSERT( ret );

   ret = eglQuerySurface( display,
                          surface,
                          EGL_HEIGHT,
                          &eglHeight );
   MC2_ASSERT( ret );
   
   context->imgLoader = new PNGImageLoader;
   context->textInterface = textInterface;

   context->renderer = new OpenGlEsRenderer( display,
                                             surface,
                                             textInterface,
                                             textInterface,
                                             texManager,
                                             vertManager,
                                             eglWidth,
                                             eglHeight );

   return context;
}

void WinOpenGlEsContextUtil::destroyContext( DrawingContext* context )
{
   MC2_ASSERT( context );
   OpenGlEsExternalContext* glesContext =
      static_cast<OpenGlEsExternalContext*> ( context );
   EGLContextWrapper::ErrorCode err =
      glesContext->eglWrapper->destroyContext();
   MC2_ASSERT( err == EGLContextWrapper::SUCCESS );
   delete glesContext;
}
