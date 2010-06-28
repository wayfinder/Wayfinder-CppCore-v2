/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef DXDRAWHEADER
#define DXDRAWHEADER

#include "PolyLine.h"
#include <ddraw.h>
//#include "winuserm.h"


#include "DxSurf.h"

#define RGB_TO_565(r,g,b) (WORD) ((r & 0xf8 )<<8) | ((g&0xfc)<<3) | ((b&0xf8)>>3) // All color should be defied with this macro

#define SAFE_RELEASE_DX_SURFACE(surf)           \
   if (surf){                                   \
      surf->Release();                          \
      surf = 0;                                 \
   }

class DxDraw
{
   static DxDraw *m_DxDraw;

   // don't think there will be more
   enum {MAX_RECTS_IN_CLIPPING_RGN = 150 };
public:


   static DxDraw* getInstance()
   {
      if (!m_DxDraw) m_DxDraw = new DxDraw();
      return m_DxDraw;
   }

   void Init(HWND hwnd);
   
   static void Destroy();

   ~DxDraw();


   void flip();

   // partially flip function
   void flip( const RECT * ); 

   void clear( DWORD col);            // Clears the backbuffer .

   bool IsLost(); // if focus is lost we need to restore order.

   bool createSurface(DxSurf& holder, int width, int height );//Creates a offscreen surface
   // here is overloaded methods, each creates surface from bitmap
   // image copied to surface, so HBITMAP's can be freed after surface creation
   bool createSurface(DxSurf& holder, HBITMAP hbm );
   // next two methods creates alpha-enabled surfaces, first from colorkey (use macro RGB(r, g, b)),
   // second from bitmap mask.
   // note - white - is opaque, black - is transparent (inverted if inverted == true). Gradients can be used aswell.
   bool createSurface(DxSurf& holder, HBITMAP hbm, DWORD key); 
   bool createSurface(DxSurf& holder, HBITMAP hbm, HBITMAP key, bool inverted = false);

   // used for resizing and layout switch
   bool recreateBackBuffer(int width, int height);

   void SetOwner(HWND );

//    void enableDrawing(bool = true);
//    bool isDrawingEnabled();

   // deprecated, will be removed
   void setColorKey( IDirectDrawSurface* src, DWORD color );// Set colorkey on a surface

   int getWidth(){return m_size.x;}
   int getHeight(){return m_size.y;}

   static const TCHAR *getErrorName(HRESULT errcode);

   DxSurf &primSurf();
   DxSurf &backSurf();
   
private:

   bool checkAndClipMenuZone();
   
   // this is crutch to deny access to frontbuffer while overall
   // drawing is disabled. Consider redesign of all wf-internal painting mechanism to remove this
   // crutch
   DxSurf m_primary_wrapper;
   DxSurf m_backbuffer_wrapper;
   
   IDirectDrawSurface *lpddsBackbuffer;

   // owner window - window, from what will be taken clipping zone,
   // so drawing will look like drawing in this window
   IDirectDrawClipper *m_ownerClipper;
   HWND m_owner_window;

   IDirectDrawSurface *createSurfaceInternal(int a_xsize, int a_ysize, bool a_alpha_enabled = false,
                                             DWORD a_capabilities = 0);

   struct _bitmap_data{
      unsigned char *ptr;
      SIZE size;
      int shift; // this is scanline complement      

      // theese members needed, primarly, to cleanup correctly
      HBITMAP bmp;
      HDC container_dc;
      HBITMAP old_container_bmp;
   };

   _bitmap_data *getBitmapData(HDC a_dc, HBITMAP a_bitmap);
   void freeBitmapData(_bitmap_data *);

   DxDraw();
   DxDraw( DxDraw & ){};
   HWND hwnd;
   LPDIRECTDRAW lpdd;

   IDirectDrawSurface *lpddsPrimary;
   
   bool m_windowedMode;         // now unused, but can be useful in future?
   
   POINT m_size;

   bool m_clip_menu;
   RECT m_rcWindow;
   
   RECT m_menuClipRect;

   bool m_bb_in_dd_mode;
};

// #define ENABLE_DIRECT_DRAW_OUTPUT {       \
//       DxDraw *dd = DxDraw::getInstance(); \
//       if (dd) dd->enableDrawing(true);    \
//    }

// #define DISABLE_DIRECT_DRAW_OUTPUT {       \
//       DxDraw *dd = DxDraw::getInstance();  \
//       if (dd) dd->enableDrawing(false);    \
//    }


#define DDRAW_ENABLED

#define DDRAW (*DxDraw::getInstance())

#endif
