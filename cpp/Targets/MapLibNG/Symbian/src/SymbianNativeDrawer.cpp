/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "DrawingContextsConcrete.h"
#include <coemain.h>
#include <gdi.h>
#include <coecntrl.h>
#include "MapRenderer.h"
#include "SymbianNativeDrawer.h"
#include "CoreFunctionPrologue.h"

using namespace WFAPI;

struct SymbianNativeDrawer::Impl {
   PlotterContextConcrete* context;
   isab::MapRenderer* renderer;
};

SymbianNativeDrawer::SymbianNativeDrawer( CCoeControl& parent )
{
   CORE_FUNCTION_PROLOGUE();
   
   m_impl = new Impl;
   
   /* get the devices color deopth */
   int colorDepth = 12;
   
   TDisplayMode nativeMode = 
      CCoeEnv::Static()->ScreenDevice()->DisplayMode();
   
   switch ( nativeMode ) {
   case EColor16:
      colorDepth = 4;
      break;
   case EColor256:
      colorDepth = 8;
      break;
   case EColor4K:
      colorDepth = 12;
      break;
   case EColor64K:
      colorDepth = 16;
      break;
   default:
      colorDepth = 12;
      break;
   }
   
   m_impl->context = new PlotterContextConcrete;
   
   m_impl->renderer = isab::MapRenderer::NewL(parent, 
                                              parent.Rect().Width(),
                                              parent.Rect().Height(), 
                                              colorDepth);
   
   m_impl->context->plotter = m_impl->renderer;
}

SymbianNativeDrawer::~SymbianNativeDrawer()
{
   CORE_FUNCTION_PROLOGUE();
   
   delete m_impl->context;
   delete m_impl;
}


void SymbianNativeDrawer::renderTo( CWindowGc& destGc, 
                                    const TPoint& point, 
                                    TRect rect ) 
{
   CORE_FUNCTION_PROLOGUE();
   
   m_impl->renderer->RenderTo( destGc, point, rect );
}

void SymbianNativeDrawer::resizeBuffer( int newWidth, int newHeight )
{
   CORE_FUNCTION_PROLOGUE();
   
   m_impl->renderer->resizeBuffer( newWidth, newHeight );
}

DrawingContext* SymbianNativeDrawer::getDrawingContext()
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_impl->context; 
}


