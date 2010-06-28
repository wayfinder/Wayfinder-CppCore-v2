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
#include "GtkDrawer.h"
#include "DrawingContextsConcrete.h"
#include "GtkMapPlotter.h"
#include "GtkMapLibImage.h"


namespace WFAPI {

struct GtkDrawer::Impl {
   PlotterContextConcrete* context;
   GtkMapPlotter* plotter;
};

GtkDrawer::GtkDrawer( GtkWidget* widget )
{
   CORE_FUNCTION_PROLOGUE();
   
   m_impl = new Impl;

   m_impl->plotter = new GtkMapPlotter( widget );
   m_impl->context = new PlotterContextConcrete;
   m_impl->context->plotter = m_impl->plotter;
}

GtkDrawer::~GtkDrawer()
{
   CORE_FUNCTION_PROLOGUE();
   
   delete m_impl->context;
   delete m_impl;
}

DrawingContext* GtkDrawer::getDrawingContext()
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_impl->context; 
}

void GtkDrawer::renderTo( GdkGC* gc, GdkRectangle* rect )
{
   CORE_FUNCTION_PROLOGUE();
   
   m_impl->plotter->renderTo( gc, rect );
}

} // End of namespace WFAPI


/**

 *  Creates a new image which can used with widgets.
 */
//MapLibImage* GtkDrawer::createImageFromPNG( const unsigned char* rgba32,
//                                            unsigned int size )
//{
//   isab::BitMap* bmp =
//      m_impl->plotter->createBitMap( isab::png, rgba32, size, 1 );
//   
//   isab::Rectangle rect;
//
//   m_impl->plotter->getBitMapAsRectangle( rect, MC2Point( 0, 0 ), bmp );
//
//   return new GtkMapLibImage( bmp, rect.getWidth(), rect.getHeight() );
//}

/**
 *   Destroys an image created using createImage. No
 *   widgets can refer to this image after this call.
 */
//void GtkDrawer::destroyImage( MapLibImage* image )
//{
//   
//}
