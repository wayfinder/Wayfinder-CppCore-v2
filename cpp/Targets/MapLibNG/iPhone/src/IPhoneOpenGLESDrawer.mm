/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#define CORE_LOGGING_MODULE_NAME "IPhoneOpenGLESDrawer"
// #define CORE_LOGGING_ENABLE_MODULE

#include "CoreDebugPrint.h"
#import "IPhoneOpenGLESDrawer.h"
#include "DrawingContextsConcrete.h"
#include "OpenGlEsExternalContext.h"
#include "OpenGlEsTextureBlockManager.h"
#include "OpenGlEsVertexBlockManager.h"
#include "OpenGlEsRenderer.h"
#include "SwapListener.h"
#include "PreDrawListener.h"
#include "PNGImageLoader.h"
#include "IPhoneNativeTextInterface.h"
#include "CoreFunctionPrologue.h"

namespace WFAPI {

class IPhoneOpenGLState : public SwapListener,
                          public PreDrawListener {
public:   
IPhoneOpenGLState( UIView* view,
                   GLuint viewRenderBuffer,
                   GLuint viewFrameBuffer,
                   EAGLContext* context)
   :  m_view( view ),
      m_context(context),
      m_backWidth(0),
      m_backHeight(0)
   {
      m_viewRenderBuffer = viewRenderBuffer;
      m_viewFrameBuffer = viewFrameBuffer;
      
      glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES,
                                      GL_RENDERBUFFER_WIDTH_OES,
                                      &m_backWidth);
      
      glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES,
                                      GL_RENDERBUFFER_HEIGHT_OES,
                                      &m_backHeight);
   }

   unsigned int getWidth() const {
      return m_backWidth;
   }

   unsigned int getHeight() const {
      return m_backHeight;
   }
							  
	IPhoneOpenGLState() : 
	m_view( NULL ),
	m_context(0),
	m_backWidth(100),
	m_backHeight(100)
	{
	   m_view = NULL;
	}
							  

   /**
    * We don't do the actual swap here, instead we allow the
    * application to do some drawing first. Once the application is
    * done, it needs to present the buffer. See the EAGLView sample
    * application from the SDK for an example.
    *
    */ 
   virtual void doSwap() {
      if(!m_view) {
         return;
      }
      
      coreprintln("Triggering redraw");
      [m_view drawRect:[m_view bounds]];
   }

   /**
    * Initializes the platform specific drawing so that MapLib can
    * do it's magic.
    */ 
   virtual void initializeDrawing() {
      coreprintln("initializeDrawing");

      [EAGLContext setCurrentContext:m_context];
    
      glBindFramebufferOES(GL_FRAMEBUFFER_OES, m_viewFrameBuffer);
      
      coreprintln("glViewport(0, 0, %d, %d);",
                  m_backWidth, m_backHeight);
   }
   


   void updateContext(GLuint viewRenderBuffer,
                      GLuint viewFrameBuffer,
                      EAGLContext* context)
   {
      m_viewRenderBuffer = viewRenderBuffer;
      m_viewFrameBuffer = viewFrameBuffer;

      glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES,
                                      GL_RENDERBUFFER_WIDTH_OES,
                                      &m_backWidth);
      
      glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES,
                                      GL_RENDERBUFFER_HEIGHT_OES,
                                      &m_backHeight);
      
      m_context = context;
   }
private:
   /// The view that the drawing takes place in
   UIView* m_view;
   
   /// The EAGLContext that the view uses, Apples equivalent to EGL
   EAGLContext* m_context;

   /// The rendering buffer we use for drawing
   GLuint m_viewRenderBuffer;

   /// The frame buffer we use for drawing
   GLuint m_viewFrameBuffer;

   /// Width of the rendering buffer in pixels
   GLint m_backWidth;

   /// Height of the rendering buffer in pixels
   GLint m_backHeight;
};

struct IPhoneOpenGLESDrawer::Impl {

   /// The OpenGL ES context MapLib will use internally for drawing
   OpenGlEsExternalContext* context;

   /// The EAGLContext that the view uses, Apples equivalent to EGL
   EAGLContext* eaglContext;

   /// Our internal wrapper class that we use to synchronize drawing
   IPhoneOpenGLState* glState;
   
};

IPhoneOpenGLESDrawer::IPhoneOpenGLESDrawer() 
{
   CORE_FUNCTION_PROLOGUE();
   
   m_impl = new Impl;
   m_impl->context = NULL;
   m_impl->eaglContext = 0;
   m_impl->context = new OpenGlEsExternalContext;
   m_impl->glState = new IPhoneOpenGLState();

   m_impl->context->imgLoader = new PNGImageLoader;
   
   IPhoneNativeTextInterface* textInterface = new IPhoneNativeTextInterface;
   OpenGlEsTextureBlockManager* texManager = new OpenGlEsTextureBlockManager;
   OpenGlEsVertexBlockManager* vertManager = new OpenGlEsVertexBlockManager;

   m_impl->context->textInterface = textInterface;
   
   m_impl->context->renderer =
      new OpenGlEsRenderer(textInterface,
                           textInterface,
                           texManager,
                           vertManager,
                           m_impl->glState->getWidth(),
                           m_impl->glState->getHeight());

   // Now we tie in our platform specific wrapper to the general
   // OpenGL ES renderer
   
   m_impl->context->renderer->setSwapListener( m_impl->glState );
   m_impl->context->renderer->setPreDrawListener( m_impl->glState );
}


IPhoneOpenGLESDrawer::IPhoneOpenGLESDrawer(UIView* view,
                                           GLuint viewRenderBuffer,
                                           GLuint viewFrameBuffer,
                                           EAGLContext* eaglContext) 
{
   CORE_FUNCTION_PROLOGUE();
   
   m_impl = new Impl;
   m_impl->context = NULL;
   m_impl->eaglContext = eaglContext;
   m_impl->context = new OpenGlEsExternalContext;
   m_impl->glState = new IPhoneOpenGLState(view,
                                           viewRenderBuffer,
                                           viewFrameBuffer,
                                           eaglContext);

   m_impl->context->imgLoader = new PNGImageLoader;
   
   IPhoneNativeTextInterface* textInterface = new IPhoneNativeTextInterface;
   OpenGlEsTextureBlockManager* texManager = new OpenGlEsTextureBlockManager;
   OpenGlEsVertexBlockManager* vertManager = new OpenGlEsVertexBlockManager;

   m_impl->context->textInterface = textInterface;
   
   m_impl->context->renderer =
      new OpenGlEsRenderer(textInterface,
                           textInterface,
                           texManager,
                           vertManager,
                           m_impl->glState->getWidth(),
                           m_impl->glState->getHeight());

   // Now we tie in our platform specific wrapper to the general
   // OpenGL ES renderer
   
   m_impl->context->renderer->setSwapListener( m_impl->glState );
   
   m_impl->context->renderer->setPreDrawListener( m_impl->glState );
}

DrawingContext* IPhoneOpenGLESDrawer::getDrawingContext()
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_impl->context; 
}

void IPhoneOpenGLESDrawer::updateContext(GLuint viewRenderBuffer,
                                         GLuint viewFrameBuffer,
                                         EAGLContext* context)
{
   CORE_FUNCTION_PROLOGUE();
   
   m_impl->glState->updateContext(viewRenderBuffer,
                                  viewFrameBuffer,
                                  context);
}

void IPhoneOpenGLESDrawer::setScreenSize(unsigned int newWidth,
                                         unsigned int newHeight)
{
   CORE_FUNCTION_PROLOGUE();
   
   m_impl->context->renderer->setScreenSize(newWidth, newHeight);
}

} // End of namespace WFAPI



