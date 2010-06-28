/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef __DXSURF_H_INCLUDED__
#define __DXSURF_H_INCLUDED__

#include <ddraw.h>

class DxDraw;

class DxSurf{
   friend class DxDraw;
   
   void setSurf(IDirectDrawSurface *, bool);

   // this object can't be copied
   DxSurf(const DxSurf& ) {};
   DxSurf &operator=(const DxSurf&) {};
   
public:
   enum { INVALID_SHIFT = INT_MAX };
   
   DxSurf();
   
   // blitting - very fast
   bool doBlit( DxSurf& src, int xpos = 0, int ypos = 0, bool keysrc = false);
   bool doAlphaBlit(DxSurf& src, int xpos = 0, int ypos = 0, int trans = 255);
   bool doStretchBlit(DxSurf& src, int xpos, int ypos , int xsize, int ysize, bool keysrc = false);

   bool clear(COLORREF color);
   bool fill(COLORREF color, int x, int y, int sizex, int sizey);
   operator bool();

   // direct access
   void *lock();
   int pitch();

   // gdi acces (!!!!!! SLOW !!!!!!)
   HDC dc();

   // release any lock
   void unlock();

   int width();
   int height();
   
   int BlitShiftY(int = INVALID_SHIFT);
   int BlitShiftX(int = INVALID_SHIFT);

   void Release();
   ~DxSurf();
private:

   DWORD fixColor(DWORD);
   DWORD colorToMask(DWORD color, DWORD mask);

   bool getSurfaceRect(IDirectDrawSurface *a_surface, RECT & a_rect);
   bool fixDestSrcRect(RECT &a_destrect, RECT & a_srcrect);
   
   bool m_can_be_released;
   IDirectDrawSurface *m_dd_surf;
   SIZE m_size;
   bool m_locked;
   HDC m_dc;
   void *m_ptr;
   int m_pitch;
   
   int m_y_shift;
   int m_x_shift;

   bool m_enabled;
};

//==============================================================================
// some inline functions:

inline int DxSurf::width(){
   return m_size.cx;   
}

inline int DxSurf::height(){
   return m_size.cy;
}

inline DxSurf::operator bool(){
   return m_dd_surf != 0;
}

//==============================================================================

#endif 	// __DXSURF_H_INCLUDED__
