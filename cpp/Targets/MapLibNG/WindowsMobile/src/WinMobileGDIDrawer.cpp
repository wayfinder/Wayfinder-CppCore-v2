/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#undef WIN32_NO_OLE
#include "WinMobileGDIDrawer.h"
#include "WinMobilePlotterContextConcrete.h"
#include "DxDraw.h"
#include "WinMapPlotter.h"

namespace WFAPI {

struct WinMobileGDIDrawer::Impl {
   WinMobilePlotterContextConcrete* context;
   WinMapPlotter* plotter;
   HWND hwnd;
};

WinMobileGDIDrawer::WinMobileGDIDrawer(HWND hWnd)
{
   m_impl = new Impl;
   m_impl->hwnd = hWnd;
   m_impl->context = NULL;
   
   createContext();
}

WinMobileGDIDrawer::~WinMobileGDIDrawer()
{
   delete m_impl; 
}

void WinMobileGDIDrawer::createContext()
{
   RECT windowRect;
   GetWindowRect(m_impl->hwnd,&windowRect);
   int width = windowRect.right;// - windowRect.left;
   int height = windowRect.bottom;// - windowRect.top;
   
   DxDraw::getInstance()->Init( m_impl->hwnd );

   m_impl->plotter =
      isab::WinMapPlotter::allocate( width, height);

   WinMobilePlotterContextConcrete* context =
      new WinMobilePlotterContextConcrete;
   
   context->setHWND( m_impl->hwnd );
   m_impl->plotter->setWinPaintCallback( context );
   context->plotter = m_impl->plotter;

   m_impl->context = context;
}

DrawingContext* WinMobileGDIDrawer::getDrawingContext()
{
   return m_impl->context; 
}

void WinMobileGDIDrawer::changeWindow(HWND newWindow)
{
   m_impl->context->setHWND(newWindow);
}

void WinMobileGDIDrawer::doSwap(HDC destDC)
{
   //arg 2 and 3 not used atm.
   m_impl->plotter->blitTo(destDC, 0, 0);
}

HDC WFAPI::WinMobileGDIDrawer::getBackBuffer()
{
   return m_impl->plotter->getBuffer();   
}

} // End of namespace WFAPI


