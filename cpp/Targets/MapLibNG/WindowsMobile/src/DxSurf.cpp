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

#include "DxDraw.h"
#include <ddraw.h>

#include <memory.h>
#include <memory>

#include "DxSurf.h"

//#define TIMINGS

DxSurf::DxSurf(){
   m_can_be_released = true;
   m_dd_surf = 0;
   m_locked = false;
   m_ptr = 0;
   m_dc = 0;
   m_y_shift = 0;
   m_x_shift = 0;

}

//==============================================================================

void DxSurf::setSurf(IDirectDrawSurface *a_dd_surf, bool a_can_be_released){
   // assume setting unlocked surface !!
   m_dc = 0;
   m_ptr = 0;
   m_locked = false;

   m_dd_surf = a_dd_surf;
   m_can_be_released = a_can_be_released;
   if (a_dd_surf){
      RECT rect;      
      getSurfaceRect(a_dd_surf, rect);
      m_size.cx = rect.right;
      m_size.cy = rect.bottom;
   }
}

//==============================================================================

bool DxSurf::doBlit( DxSurf& src, int xpos, int ypos, bool keysrc){
   ASSERT((!m_locked) && src.m_dd_surf && (!src.m_locked));
   if (m_locked || (!src.m_dd_surf) || src.m_locked || (!m_dd_surf)) return false;
//    if (!m_enabled) return false;
#ifdef TIMINGS
   DWORD starttime = GetTickCount();
#endif
      

   RECT srcRect;
   
   if ( getSurfaceRect(src.m_dd_surf, srcRect) ){
      
      if (m_y_shift) ypos += m_y_shift;
      if (m_x_shift) xpos += m_x_shift;
      
      RECT destRect = {xpos,  ypos, xpos + srcRect.right, ypos + srcRect.bottom };

      // fix destination rect
      if ( fixDestSrcRect(destRect, srcRect) ){
         // setup flags
         DWORD flags = DDBLT_WAITNOTBUSY;
         if (keysrc) flags |= DDBLT_KEYSRC;

         // Blit!   
         bool retval = false;
         if (m_dd_surf){
            retval = m_dd_surf->Blt( &destRect, src.m_dd_surf, &srcRect, flags, 0 ) == DD_OK;
         }

#ifdef TIMINGS
         DWORD endtime = GetTickCount();
         FILE *bltfile = fopen("/blttime.txt", "ab");
         if (bltfile){
            fprintf(bltfile, "image %dx%d blitted to %dx%d in %d ms (keysrc %d)\n",
                    srcRect.right, srcRect.bottom,
                    destRect.right, destRect.bottom,
                    endtime - starttime,
                    keysrc);
            fclose(bltfile);
         }
#endif
         return retval;

      }
   }

   return false;
}

//==============================================================================

bool DxSurf::getSurfaceRect(IDirectDrawSurface *a_surface, RECT & a_rect){
   // get source sufrace size
   if (!a_surface) return false;
   DDSURFACEDESC ddsd = { sizeof( DDSURFACEDESC ), 0 };
   if (a_surface->GetSurfaceDesc( &ddsd ) != DD_OK ) return false;
   
   a_rect.top = 0;
   a_rect.left = 0;
   a_rect.right = ddsd.dwWidth;
   a_rect.bottom = ddsd.dwHeight;
   return true;
}

//==============================================================================

// fix source rect to fit destination by clipping it's parts
bool DxSurf::fixDestSrcRect(RECT & a_destrect, RECT & a_srcrect){
   if (!m_dd_surf) return false;
   
   if (a_destrect.top < 0){
      a_srcrect.top -= a_destrect.top;
      a_destrect.top = 0;
   }

   if (a_destrect.left < 0){
      a_srcrect.left -= a_destrect.left;
      a_destrect.left = 0;
   }

   if (a_destrect.right > m_size.cx){
      a_srcrect.right -= a_destrect.right - m_size.cx;
      a_destrect.right = m_size.cx;
   }

   if (a_destrect.bottom > m_size.cy){
      a_srcrect.bottom -= a_destrect.bottom - m_size.cy;
      a_destrect.bottom = m_size.cy;
   }
   return true;
}


//==============================================================================

bool DxSurf::doAlphaBlit(DxSurf& src, int xpos, int ypos, int trans){
   ASSERT((!m_locked) && src.m_dd_surf && (!src.m_locked));
   if (m_locked || (!src.m_dd_surf) || src.m_locked || (!m_dd_surf)) return false;
//    if (!m_enabled) return false;


#ifdef TIMINGS
   DWORD starttime = GetTickCount();
#endif

   RECT srcRect;
   
   if (getSurfaceRect(src.m_dd_surf, srcRect)){
      // prepare DDALPHABLTFX structure
      DDALPHABLTFX ddfx = {0};
      ddfx.dwSize = sizeof( DDALPHABLTFX);
      ddfx.ddargbScaleFactors.alpha = trans; // trans from args
      ddfx.ddargbScaleFactors.red = 255;
      ddfx.ddargbScaleFactors.green = 255;
      ddfx.ddargbScaleFactors.blue = 255;

      if (m_y_shift) ypos += m_y_shift;
      if (m_x_shift) xpos += m_x_shift;

      // Calc rects
      RECT destRect = { xpos, ypos, xpos + srcRect.right, ypos + srcRect.bottom };

      if (fixDestSrcRect(destRect, srcRect)){
         
         bool retval = false;
         if (m_dd_surf){
            retval = m_dd_surf->AlphaBlt(&destRect, src.m_dd_surf, &srcRect, DDBLT_WAITNOTBUSY,  &ddfx) == DD_OK;
         }

#ifdef TIMINGS
         DWORD endtime = GetTickCount();
         FILE *bltfile = fopen("/blttime.txt", "ab");
         if (bltfile){
            fprintf(bltfile, "image %dx%d alphablitted to %dx%d in %d ms (trans: %d)\n",
                    srcRect.right, srcRect.bottom,
                    destRect.right, destRect.bottom,
                    endtime - starttime, trans);
            fclose(bltfile);
         }
#endif
         return retval;
      }
   }

   return true;
}

//==============================================================================

bool DxSurf::doStretchBlit(DxSurf& src, int xpos, int ypos , int xsize, int ysize, bool keysrc){
   ASSERT((!m_locked) && src.m_dd_surf && (!src.m_locked));
   if (m_locked || (!src.m_dd_surf) || src.m_locked || (!m_dd_surf)) return false;
//    if ( !m_enabled) return false;

   
#ifdef TIMINGS
   DWORD starttime = GetTickCount();
#endif

   RECT srcRect;
   if (getSurfaceRect(src.m_dd_surf, srcRect)){
      
      if (m_y_shift) ypos += m_y_shift;
      if (m_x_shift) xpos += m_x_shift;

      RECT destRect = {xpos,  ypos, xpos+xsize, ypos+ysize };

      // fix destination rect 
      if (fixDestSrcRect(destRect, srcRect)){

         // setup flags
         DWORD flags = DDBLT_WAITNOTBUSY;
         if (keysrc) flags |= DDBLT_KEYSRC;
         
         // Blit!
         bool retval = false;
         if (m_dd_surf){
            retval =  m_dd_surf->Blt( &destRect, src.m_dd_surf, &srcRect, flags, 0 );
         }

         
#ifdef TIMINGS
         DWORD endtime = GetTickCount();
         FILE *bltfile = fopen("/blttime.txt", "ab");
         if (bltfile){
            fprintf(bltfile, "image %dx%d stretchblitted to %dx%d in %d ms (keysrc %d)\n",
                    srcRect.right, srcRect.bottom,
                    destRect.right, destRect.bottom,
                    endtime - starttime, keysrc);
            fclose(bltfile);
         }
#endif
         return retval;
      }
   }

   return true;
}

//==============================================================================

void DxSurf::Release(){
   ASSERT(m_can_be_released); // if not releaseable - will not be released!!!
   if (m_dd_surf && m_can_be_released){
      unlock();
      m_dd_surf->Release();
      m_dd_surf = 0;
   }
}

//==============================================================================

void *DxSurf::lock(){
   if (! m_dd_surf) return 0;
   if (m_locked){
      if( m_ptr) return m_ptr;
      // if already locked, but not for direct access - fail
      return 0;
   }
   
   DDSURFACEDESC ddsd = {0};
   ddsd.dwSize = sizeof( DDSURFACEDESC );
   // Lock primary surface
   if ( m_dd_surf->Lock( NULL, &ddsd, DDLOCK_WAITNOTBUSY, NULL ) == DD_OK ){
      m_locked = true;
      
      // Get pointer to video memory
      // TODO: redo this!
      m_ptr = ddsd.lpSurface;
      m_pitch = ddsd.lPitch;
      return m_ptr;
   }
   return 0;
}

//==============================================================================

int DxSurf::pitch(){
   return m_pitch;
}

//==============================================================================

void DxSurf::unlock(){
   if (m_dd_surf && m_locked){
      if (m_ptr){
         m_dd_surf->Unlock(NULL);
         m_ptr = 0;
      }else if(m_dc){
         m_dd_surf->ReleaseDC(m_dc);
         m_dc = 0;
      }
      m_locked = false;      
   }
}

//==============================================================================

HDC DxSurf::dc(){
   if (! m_dd_surf) return 0;
   if (m_locked){
      if( m_dc) return m_dc;
      // if already locked, but not for gdi access - fail
      return 0;
   }
   
   DDSURFACEDESC ddsd = {0};
   ddsd.dwSize = sizeof( DDSURFACEDESC );
   // Lock primary surface
   HDC hdc = {0};
   
   if ( m_dd_surf->GetDC(&hdc) == DD_OK ){
      m_locked = true;
      
      // Get pointer to video memory
      m_dc = hdc;
      return m_dc;
   }
   return 0;
}

//==============================================================================

bool DxSurf::clear(COLORREF color){
   ASSERT(!m_locked);

   if((!m_locked)
      && m_dd_surf){
      DDBLTFX ddbltfx = {0};
      ddbltfx.dwSize = sizeof DDBLTFX;
      ddbltfx.dwFillColor = fixColor(color);
      
      return m_dd_surf->Blt( 0, 0, 0, DDBLT_COLORFILL | DDBLT_WAITNOTBUSY, &ddbltfx ) == DD_OK;
   }
   
   return false;
}

//==============================================================================

bool DxSurf::fill(COLORREF color, int x, int y, int sizex, int sizey){
   ASSERT(!m_locked);
   

   if((!m_locked)
      && m_dd_surf){
      DDBLTFX ddbltfx = {0};
      ddbltfx.dwSize = sizeof DDBLTFX;
      ddbltfx.dwFillColor = fixColor(color);

      RECT target_rect = {x, y, x + sizex, y + sizey};
      
      return m_dd_surf->Blt( &target_rect, 0, 0, DDBLT_COLORFILL | DDBLT_WAITNOTBUSY, &ddbltfx ) == DD_OK;
   }
   
   return false;
}

//==============================================================================

int DxSurf::BlitShiftY(int new_shift){
   if (new_shift != INVALID_SHIFT) m_y_shift = new_shift;
   return m_y_shift;
}

//==============================================================================

int DxSurf::BlitShiftX(int new_shift){
   if (new_shift != INVALID_SHIFT) m_x_shift = new_shift;
   return m_x_shift;
}

//==============================================================================

DxSurf::~DxSurf(){
   if (m_can_be_released){
      Release();
   }
}

//==============================================================================

DWORD DxSurf::fixColor(DWORD color){
   ASSERT(m_dd_surf); // don't sure this can every happen
   if (!m_dd_surf) return 0;
   
   int b = (color & 0xff0000) >> 16,
      g = (color & 0xff00) >> 8,
      r = (color & 0xff);

   DDPIXELFORMAT pf;
   ZeroMemory(&pf,sizeof(pf));
   pf.dwSize=sizeof(pf);

   HRESULT res = m_dd_surf->GetPixelFormat(&pf);
   
   ASSERT(res == DD_OK);
   if ( res != DD_OK ) return 0;
   
   return colorToMask(r, pf.dwRBitMask)
      | colorToMask(b, pf.dwBBitMask)
      | colorToMask(g, pf.dwGBitMask);
}

//==============================================================================
// assume color expressed in 8 bit value
DWORD DxSurf::colorToMask(DWORD color, DWORD mask){
   int maskstart = 32;
   // 0x80000000 is 10000000000000000000000000000000 in bits   
   int masktester = 0x80000000;
   for (int a = 0; a < 32; ++a){
      if (mask & masktester){
         maskstart -= a;
         break;
      }
      masktester >>= 1;
   }
   int shift = (maskstart - 8);
   if (shift > 0){
      return (color << shift) & mask;
   }else{
      return (color >> (-shift)) & mask;
   }
}

//==============================================================================
// eof
