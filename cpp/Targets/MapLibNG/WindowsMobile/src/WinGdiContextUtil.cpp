/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "WinGdiContextUtil.h"
#include "DrawingContextsConcrete.h"
#include "DxDraw.h"
#include "WinMapPlotter.h"
#include "config.h"
#include "WinPaintCallback.h"

DrawingContext*
WinGdiContextUtil::createContext( HWND hWnd )
{
   RECT windowRect;
   GetWindowRect(hWnd,&windowRect);
   int width = windowRect.right - windowRect.left;
   int height = windowRect.bottom - windowRect.top;
   
   DxDraw::getInstance()->Init( hWnd );
   
   isab::WinMapPlotter* plotter =
      isab::WinMapPlotter::allocate( width, height );

   WinPlotterContextConcrete* context = new WinPlotterContextConcrete;
   
   context->setHWND( hWnd );
   plotter->setWinPaintCallback( context );
   plotter->setVisible(true);
   context->plotter = plotter;
   
   return context;
}

void
WinGdiContextUtil::changeWindow( DrawingContext* context,
                                     HWND newWindow )
{
    WinPlotterContextConcrete* winPlotterContext =
       static_cast<WinPlotterContextConcrete*> ( context );
    MC2_ASSERT( winPlotterContext );
    winPlotterContext->setHWND( newWindow );
    
}

void WinGdiContextUtil::destroyContext( DrawingContext* context )
{
   MC2_ASSERT( context );
   delete context;
}
