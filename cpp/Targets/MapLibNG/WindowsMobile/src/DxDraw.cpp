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
#include <windows.h>

// i hope this never will be defined
//#define DD_USE_TRUE_FULLSCREEN


//-----------------------------------------------------------------------------
// Name: EnumFunction()
// Desc: Enumeration callback for surfaces in flipping chain. We expect this
//          function to be called once with the surface interface pointer of
//          our back buffer (we only ask for a single back buffer.)
//-----------------------------------------------------------------------------
static HRESULT PASCAL
EnumFunction(LPDIRECTDRAWSURFACE pSurface,
             LPDDSURFACEDESC lpSurfaceDesc,
             LPVOID  lpContext)
{
   static BOOL bCalled = FALSE;

   if (!bCalled) {

      *((LPDIRECTDRAWSURFACE *)lpContext) = pSurface;
      bCalled = TRUE;
      return DDENUMRET_OK;
   }
   else {

      OutputDebugString(L"DDEX1: Enumerated more than surface?");
      pSurface->Release();
      return DDENUMRET_CANCEL;
   }
}

//==============================================================================

DxDraw::DxDraw(){
   memset(this, 0, sizeof(*this)); // null all
}

//==============================================================================

/*
*	initialise the Draw object hwnd is the hwnd to the window that it is a part of.
*   width and height is the with and height of the front and back buffers.
*/

void DxDraw::Init(HWND hWnd)
{
   if (lpdd != NULL) return;

   RECT rc_window;

   lpddsBackbuffer = lpddsPrimary = NULL;
//    m_bb_in_dd_mode = true;

   m_windowedMode = true;	// Placed here for testing, desides if we will go windowed or fullscreen

   // create the DirectDraw objektet.	
   HRESULT resultat = DirectDrawCreate(NULL, &lpdd, NULL);

   HRESULT r = lpdd->CreateClipper(0, &m_ownerClipper, 0);
   
   // some fail checks
   if (resultat != DD_OK) {

   } else {

   }

   DDCAPS ddCaps, ddHelCaps;

   // Check what the device can handle with hardware.
   lpdd->GetCaps(&ddCaps, &ddHelCaps);

   // if the device can not handle a flip chain or accellerated backbuffers we will have to do
   // it our selfs.
   if (!(ddCaps.ddsCaps.dwCaps & DDSCAPS_BACKBUFFER) || !(ddCaps.ddsCaps.dwCaps & DDSCAPS_FLIP)) 
   {
//       m_singelbuffer = TRUE;
   }
	
   if (m_windowedMode) // windowed mode, as in not fullscreen
   {
      resultat = lpdd->SetCooperativeLevel(hWnd,DDSCL_NORMAL); // normal mode, exclusive do not work with windowed
      
      if (resultat == DD_OK){
         GetWindowRect(hWnd,&rc_window);	// the dimetions of the rect that we will draw in
      }
   } 
   else
   {
      resultat = lpdd->SetCooperativeLevel(hWnd,DDSCL_FULLSCREEN); // the fullscreen flag gives some problems due to nothing will be handeld by itself
   }
	
	
   ////////////////////////////////////////////

   if (m_windowedMode)
   {
      recreateBackBuffer(rc_window.right - rc_window.left,
                         rc_window.bottom - rc_window.top);
   } 
   else	// we are in fullscreen
   {
      // Note: can't create surface with createSurfaceInternal
      // Create the primary surface with 1 back buffer
      //
      // but now this code is not used

#ifdef DD_USE_TRUE_FULLSCREEN
#pragma warning("not implemented propertly - will not work")
      
      DDSURFACEDESC ddsd = {0};
      memset(&ddsd, 0, sizeof(ddsd));
      ddsd.dwSize = sizeof(ddsd);
      ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;	// we want a hardware enabled backbuffer
      ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP;	// we want a flipchain
      ddsd.dwBackBufferCount = 1;		// Set number of backbuffers to 1 can have more for trippel buffering

      resultat = lpdd->CreateSurface(&ddsd, &lpddsPrimary, NULL);	// creates the primary surface with the info in ddsd
      if(resultat == DD_OK) {
         Logger::LogInfo("DxDraw", "Init", "Create surface == OK\n");
      } else {
         Logger::LogInfo("DxDraw", "Init", getErrorName(resultat));
      }

      // Get a pointer to the back buffer by enumerating it
      resultat = lpddsPrimary->EnumAttachedSurfaces(&lpddsBackbuffer, EnumFunction);
		
      if (resultat != DD_OK)
      {
         Logger::LogInfo("DxDraw", "Init", "enum failed\n");
      }

      lpddsCachebuffer = createSurfaceInternal(rc_window.right,rc_window.bottom// +42,
                                               false, 0
                                       );	// create a offscreen surface later used for caching the backbuffer
#else
      ASSERT(false);
#endif
   }

//    m_clippingRegionDataBuffer = (RGNDATA *) new char[sizeof(RGNDATA) + sizeof(RECT)*MAX_RECTS_IN_CLIPPING_RGN];
//    m_clippingRegionDataBuffer->rdh.dwSize = sizeof(RGNDATAHEADER);
   
//    if (lpddsPrimary){
//       DDSURFACEDESC ddsd;
//       lpddsPrimary->GetSurfaceDesc(&ddsd);      // get info about the primary screen
//       Pitch = ddsd.lPitch;                      // save the pitch
//    }
	
   //   lpddsCachebuffer = createSurface(rc_window.right,rc_window.bottom// +42
   //                                    );	// create a offscreen surface later used for caching the backbuffer
}

//==============================================================================

bool DxDraw::recreateBackBuffer(int width, int height){
//    ASSERT(m_bb_in_dd_mode); // only in DD mode!!
   SAFE_RELEASE_DX_SURFACE(lpddsBackbuffer);
   SAFE_RELEASE_DX_SURFACE(lpddsPrimary);

   lpddsPrimary = createSurfaceInternal(0, 0, false, DDSCAPS_PRIMARYSURFACE);
   lpddsBackbuffer = createSurfaceInternal(width, height, false, 0);
   
   if ((!lpddsBackbuffer)
       || (!lpddsPrimary)){
      SAFE_RELEASE_DX_SURFACE(lpddsBackbuffer);
      SAFE_RELEASE_DX_SURFACE(lpddsPrimary);
      return false;
   }

   m_primary_wrapper.setSurf(lpddsPrimary, false);
   m_backbuffer_wrapper.setSurf(lpddsBackbuffer, false);

   if (m_owner_window){
      HWND temp = m_owner_window;
      m_owner_window = NULL;
      SetOwner(temp);
   }

   m_size.x = width;
   m_size.y = height;

   return true;
}

//==============================================================================

/*
*  sets the Colorkey for the surface.
*/
void DxDraw::setColorKey( IDirectDrawSurface* src, DWORD color )
{
   DDCOLORKEY ddck = {0};
   ddck.dwColorSpaceLowValue = color;
   ddck.dwColorSpaceHighValue = color;
   src->SetColorKey(DDCKEY_SRCBLT, &ddck);
}

//==============================================================================

/*
* Destructor, not complete.
*/
DxDraw::~DxDraw()
{
   if (m_ownerClipper){
      SetOwner(0);
      m_ownerClipper->Release();
      m_ownerClipper = 0;
   }

//    delete[] (char *)m_clippingRegionDataBuffer;

   // do unlock before release
   m_primary_wrapper.unlock();
   m_backbuffer_wrapper.unlock();
   
   SAFE_RELEASE_DX_SURFACE(lpddsBackbuffer);
   SAFE_RELEASE_DX_SURFACE(lpddsPrimary);


   if( lpdd )
      lpdd->Release();

}

//==============================================================================

/*
* Changes the frontbuffer to the backbuffer
*/
void DxDraw::flip()
{
   // 01-09-2008, 12:44, q-alo: here, actually, can be used this:
   RECT fullrect = {0, 0, m_size.x, m_size.y};
   return flip(&fullrect);
}

//==============================================================================

void DxDraw::flip( const RECT * rect )  
{
   if (! checkAndClipMenuZone() ) return;
   
   // ensure input values in sane
   ASSERT(rect);
   ASSERT(rect->top < rect->bottom );
   ASSERT(rect->left < rect->right );

   if (!rect) return;
   if (rect->top >= rect->bottom) return;
   if (rect->left >= rect->right) return;

   RECT rct = *rect;


   // fix rect, if it overrect or underrect
   if (rct.top < 0) rct.top = 0;
   if (rct.left < 0) rct.left = 0;
   if (rct.right > m_size.x) rct.right = m_size.x;
   if (rct.bottom > m_size.y) rct.bottom = m_size.y;

   // prepare source rect
   RECT srcrect = { rct.left,
                    rct.top,
                    rct.right,
                    rct.bottom};

   int shiftx = m_primary_wrapper.BlitShiftX();
   int shifty = m_primary_wrapper.BlitShiftY();
   
   //and dest rect
   RECT destRect = {rct.left + shiftx, rct.top +  shifty,
                    rct.right + shiftx, rct.bottom + shifty};

   // 01-07-2008, 17:57, q-alo: here is easier to partial blit here, than adding new (i think not
   // needed) method to DxSurf
   // 
   // 01-09-2008, 12:44, q-alo: here we need to check, if we can just do blitting, or need to calc
   // rectangles to avoid menu overpainting

   if (m_clip_menu){
      POINT shift = {srcrect.left - destRect.left,
                     srcrect.top - destRect.top};
      
      // dance with the regions
      HRGN rgn = CreateRectRgn(destRect.left, destRect.top,
                               destRect.right, destRect.bottom);
      
      HRGN menu_rgn = CreateRectRgn(m_menuClipRect.left, m_menuClipRect.top,
                                    m_menuClipRect.right, m_menuClipRect.bottom);

      CombineRgn(rgn, rgn, menu_rgn, RGN_DIFF);
      DeleteObject(menu_rgn);

      DWORD rgndata_size = GetRegionData(rgn, 0, 0); // obtain needed size
      RGNDATA *rgndata = (RGNDATA *)new unsigned char[rgndata_size];
      GetRegionData(rgn, rgndata_size, rgndata);

      RECT *paintrect = (RECT *)rgndata->Buffer;
      for(int a = 0; a < rgndata->rdh.nCount; ++a){

         RECT sr = {paintrect->left + shift.x,
                    paintrect->top + shift.y,
                    paintrect->right + shift.x,
                    paintrect->bottom + shift.y};
         
         lpddsPrimary->Blt(paintrect, lpddsBackbuffer, &sr , DDBLT_WAITNOTBUSY, 0 );
         
         ++paintrect;
      }
      

      DeleteObject(rgn);
      delete[] (unsigned char *)rgndata; // created as char, deleted as char

   }else{
      // here is simply blit
      HRESULT hres;   
      hres = lpddsPrimary->Blt(&destRect, lpddsBackbuffer, &srcrect , DDBLT_WAITNOTBUSY, 0 );
   }
}

//==============================================================================

void DxDraw::clear( DWORD col )            // clear the backbuffer to the color col
{
   DDBLTFX ddbltfx = {0};
   ddbltfx.dwSize = sizeof DDBLTFX;
   ddbltfx.dwFillColor = col;

   lpddsBackbuffer->Blt( 0, 0, 0, DDBLT_COLORFILL /*| DDBLT_WAITNOTBUSY*/, &ddbltfx );
}

//==============================================================================

bool DxDraw::IsLost()
{
   if (FAILED(lpddsPrimary->IsLost())) 
   {
      lpddsPrimary->Restore();
      lpdd->RestoreAllSurfaces();
      return true;

   }
   
   return false;
}

//==============================================================================

// void DxDraw::FillRect( int x1, int y1, int x2, int y2, DWORD col ) // fill the rectangle with the color col
// // till backbuffern
// {
//    Logger::LogDebug("DxDraw", "FillRect", "Rect = top : %d. left : %d. bottom : %d. right : %d \n",y1,x1,y2,x2);

//    DDBLTFX ddbltfx = {0};
//    ddbltfx.dwSize = sizeof (DDBLTFX);
//    ddbltfx.dwFillColor = col;
//    //FIX ME, harcoded values should not be used or needed
//    RECT r = {rc_window.left+x1, rc_window.top// -25
//              +y1, rc_window.left+x2, rc_window.top// -25
//              +y2};
//    lpddsBackbuffer->Blt( &r, 0, 0, DDBLT_COLORFILL, &ddbltfx );
//    Logger::LogDebug("DxDraw", "FillRect", "leaving (note - this must not be called at all)\n");
// }

//==============================================================================

IDirectDrawSurface *DxDraw::createSurfaceInternal(int a_xsize, int a_ysize,
                                                  bool a_alpha_enabled,
                                                  DWORD a_capabilities){
   IDirectDrawSurface* retval = 0;
   
   DDSURFACEDESC surf_desc = {0};
   memset(&surf_desc, 0, sizeof (surf_desc));
   surf_desc.dwSize = sizeof (surf_desc);
   if (a_xsize){
      surf_desc.dwWidth = a_xsize;      
      surf_desc.dwFlags |= DDSD_WIDTH;
   }
   
   if (a_ysize){
      surf_desc.dwFlags |= DDSD_HEIGHT;
      surf_desc.dwHeight = a_ysize;      
   }

   if (a_alpha_enabled){
      // adding alpha pixel information support it surface
      surf_desc.dwFlags |= DDSD_PIXELFORMAT;
      surf_desc.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
      surf_desc.ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS | DDPF_ALPHAPREMULT;
      surf_desc.ddpfPixelFormat.dwRGBBitCount = 32;
      surf_desc.ddpfPixelFormat.dwRBitMask = 0x00FF0000;
      surf_desc.ddpfPixelFormat.dwGBitMask = 0x0000FF00;
      surf_desc.ddpfPixelFormat.dwBBitMask = 0x000000FF;
      surf_desc.ddpfPixelFormat.dwRGBAlphaBitMask = 0xFF000000;
   }

   if (a_capabilities){
      surf_desc.dwFlags |= DDSD_CAPS;
      surf_desc.ddsCaps.dwCaps = a_capabilities;
   }

   HRESULT res = lpdd->CreateSurface( &surf_desc, &retval, NULL );
   if(res == DD_OK) {
      return retval;
   }
   
   return NULL;
}


//==============================================================================

// Create an offscreen surface with the height a width
bool DxDraw::createSurface( DxSurf &holder, int width, int height) {
   IDirectDrawSurface* dds = createSurfaceInternal(width, height, false, 0);
   
   if (dds){
      
      setColorKey(dds,RGB_TO_565(255,0,255)); // magic pink is still invisible
      holder.unlock();
      
      if (holder.m_can_be_released){
         holder.Release();
      }
      holder.setSurf(dds, true);
      
      return true;
   }

   return false;
}

//==============================================================================

// Creates an offscreen surface with the image on it (same size as the bitmap)   
bool DxDraw::createSurface( DxSurf & holder, HBITMAP hbm, DWORD key) 
{

   //   HDC hdcImage;
   _bitmap_data *bmpdata = getBitmapData(NULL, hbm);
   if (!bmpdata) return false;
   
   LPDIRECTDRAWSURFACE pdds = createSurfaceInternal(bmpdata->size.cx, bmpdata->size.cy, true, 0);

   if (pdds){
      DDSURFACEDESC ddsd;
      ddsd.dwSize = sizeof( ddsd );
      if (pdds->Lock(NULL, &ddsd, DDLOCK_WRITEONLY | DDLOCK_WAITNOTBUSY, 0) == DD_OK){
         // we already know - one pixel - 32 bits, so can use long
         unsigned long *target_buffer = (unsigned long *)ddsd.lpSurface;
         unsigned char *source_buffer = (unsigned char *)bmpdata->ptr;
         int scanline = bmpdata->shift + (bmpdata->size.cx * 3);
         
         unsigned char *workptr = source_buffer + (scanline * (bmpdata->size.cy - 1));

         for(int y = 0; y < bmpdata->size.cy; ++y){
            for (int x = 0; x < bmpdata->size.cx; ++x){
               unsigned long target_px = 0;
               unsigned long alpha = 0xff;
               unsigned long src_px = (((long)workptr[2]) << 16) | (((long)workptr[1]) << 8) | ((long)workptr[0]);

               if (key != src_px){
                  target_px = (alpha << 24) | src_px;
               } else{
                  target_px = 0;
               }
               
               *target_buffer = target_px;
               ++target_buffer;
               workptr += 3;
            }
            workptr -= 2*scanline - bmpdata->shift;
         }
         
         pdds->Unlock(NULL);
      }
   }
   
   freeBitmapData(bmpdata);
   if (pdds){
      holder.unlock();
      
      if (holder.m_can_be_released){
         holder.Release();
      }
      holder.setSurf(pdds, true);
      return true;
   }

   return false;
}

//==============================================================================

bool DxDraw::createSurface( DxSurf &holder, HBITMAP hbm, HBITMAP key, bool a_inverted) {
   LPDIRECTDRAWSURFACE pdds = 0;
   //   HDC hdcImage;
   _bitmap_data *bmpdata = getBitmapData(NULL, hbm);
   _bitmap_data *keybitmapdata = getBitmapData(NULL, key);

   if (bmpdata && keybitmapdata){

      // Q-ALO: note - if mask bitmap is bigger than bitmap - this is error, but not fatal
      // if mask smaller than bitmap - this fatal for now
      if ((bmpdata->size.cx <= keybitmapdata->size.cx)
          && (bmpdata->size.cy <= keybitmapdata->size.cy)){
         
         pdds = createSurfaceInternal(bmpdata->size.cx, bmpdata->size.cy, true, 0);

         if (pdds){
            DDSURFACEDESC ddsd;
            ddsd.dwSize = sizeof( ddsd );
            if (pdds->Lock(NULL, &ddsd, DDLOCK_WRITEONLY | DDLOCK_WAITNOTBUSY, 0) == DD_OK){
               // we already know - one pixel - 32 bits, so can use long
               unsigned long *target_buffer = (unsigned long *)ddsd.lpSurface;
               unsigned char *source_buffer = (unsigned char *)bmpdata->ptr;
               int scanline = bmpdata->shift + (bmpdata->size.cx * 3);
         
               unsigned char *workptr = source_buffer + (scanline * (bmpdata->size.cy - 1));
               unsigned char *keyptr = (unsigned char *)keybitmapdata->ptr + (scanline * (bmpdata->size.cy - 1));

               for(int y = 0; y < bmpdata->size.cy; ++y){
                  for (int x = 0; x < bmpdata->size.cx; ++x){
                     unsigned long target_px = 0;
               
                     unsigned long alpha = (((long)keyptr[2]) + ((long)keyptr[1]) + ((long)keyptr[0]))/3;
                     if (a_inverted){
                        alpha = 255 - alpha;
                     }
                     unsigned long src_px =
                        (((((long)workptr[2])*alpha)/255) << 16)
                        | (((((long)workptr[1])*alpha)/255) << 8)
                        | ((((long)workptr[0])*alpha)/255);
               
                     target_px = (alpha << 24) | src_px;
               
                     *target_buffer = target_px;
                     ++target_buffer;
                     workptr += 3;
                     keyptr += 3;
                  }
                  workptr -= 2*scanline - bmpdata->shift;
                  keyptr -= 2*scanline - bmpdata->shift;
               }
         
               pdds->Unlock(NULL);
            }
         }
      }
   }
   
   freeBitmapData(keybitmapdata);
   freeBitmapData(bmpdata);

   if (pdds){
      holder.unlock();
      
      if (holder.m_can_be_released){
         holder.Release();
      }
      holder.setSurf(pdds, true);
      return true;
   }

   return false;
}

//==============================================================================

DxDraw::_bitmap_data *DxDraw::getBitmapData(HDC a_dc, HBITMAP a_bitmap){
   if (!a_bitmap) return NULL;
   
   BITMAP bm = {0};
   GetObject(a_bitmap, sizeof(bm), &bm);
   
   _bitmap_data *bmpdata = new _bitmap_data;
   
   bmpdata->size.cx = bm.bmWidth;
   bmpdata->size.cy = bm.bmHeight;

   std::auto_ptr<BITMAPINFO> bitmapInfo((LPBITMAPINFO)(new char[sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD)]));
   bitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
   bitmapInfo->bmiHeader.biPlanes = 1;
   bitmapInfo->bmiHeader.biBitCount = 24;
   bitmapInfo->bmiHeader.biCompression = BI_RGB;
   bitmapInfo->bmiHeader.biHeight = bm.bmHeight;
   bitmapInfo->bmiHeader.biWidth = bm.bmWidth;

   bmpdata->container_dc = CreateCompatibleDC(a_dc);
   
   bmpdata->bmp = CreateDIBSection( bmpdata->container_dc, (const BITMAPINFO*)bitmapInfo.get(),
                                    DIB_RGB_COLORS, (void**)&(bmpdata->ptr), NULL, 0);

   HDC paint_dc                 = CreateCompatibleDC(a_dc);
   
   bmpdata->old_container_bmp   = (HBITMAP)SelectObject(bmpdata->container_dc, bmpdata->bmp);
   HBITMAP paint_oldbmp         = (HBITMAP)SelectObject(paint_dc, a_bitmap);
   
   BitBlt(bmpdata->container_dc, 0, 0, bm.bmWidth, bm.bmHeight, paint_dc, 0, 0, SRCCOPY);

   // scanlines aligned on 4 byte boundary
   bmpdata->shift = (bm.bmWidth*3)%4;
   if (bmpdata->shift != 0){
      bmpdata->shift = 4 - bmpdata->shift;
   }

   SelectObject(paint_dc, paint_oldbmp);
   DeleteDC(paint_dc);
   
   return bmpdata;
}

//==============================================================================

void DxDraw::freeBitmapData(_bitmap_data *a_data){

   if (a_data){
      if (a_data->container_dc){
         SelectObject(a_data->container_dc, a_data->old_container_bmp);
         DeleteDC(a_data->container_dc);         
      }
      if (a_data->bmp){
         DeleteObject(a_data->bmp);
      }
      delete a_data;
   }
}

//==============================================================================

bool DxDraw::createSurface( DxSurf& holder, HBITMAP hbm ) // Creates an offscreen surface with the image on it (same size as the bitmap)   
{
   HDC hdcImage;
   //	HDC hdc;
   HRESULT hr = E_FAIL;
   DDSURFACEDESC ddsd;
   // FIX ME this part is used to get alpha channel info in the surface, have not gotten it to work

   LPDIRECTDRAWSURFACE pdds;

   // Get image size, and create surface of that size
   BITMAP bm = {0};
   GetObject(hbm, sizeof(bm), &bm);
	
   pdds = createSurfaceInternal(bm.bmWidth, bm.bmHeight, false, 0);
   if (pdds){
      //
      // Select bitmap into a memoryDC so we can use it.
      //
      hdcImage = CreateCompatibleDC(NULL);
      SelectObject(hdcImage, hbm);

      // Get surface DC
      HDC hdc = {0};
      if ((hr = pdds->GetDC(&hdc)) == DD_OK) {
         
         if (!BitBlt( hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcImage, 0, 0, SRCCOPY)) hr = E_FAIL;
         pdds->ReleaseDC(hdc);
      }		
      DeleteDC(hdcImage);
   
      setColorKey(pdds,RGB_TO_565(255,0,255));
   }
   if (hr == DD_OK){
      holder.unlock();
      
      if (holder.m_can_be_released){
         holder.Release();
      }
      holder.setSurf(pdds, true);
      return true;
   }
   return false;
}

//==============================================================================

void DxDraw::SetOwner(HWND a_owner){
   // do it only if owner have been changed
   if (m_owner_window != a_owner){
      if (a_owner){
         m_ownerClipper->SetHWnd(0, a_owner);
         lpddsPrimary->SetClipper(m_ownerClipper);
      }else{
         lpddsPrimary->SetClipper(0);
         m_ownerClipper->SetHWnd(0, 0);
      }
   }
   m_owner_window = a_owner;   
}

//==============================================================================

// Q-ALO: helper macro
#define CHECK_AND_RETURN_DDERR_IN_STRING(x, err)        \
   if (x == err){                                       \
      return L#err L"\n";                               \
   }

//--------------------------------------------------

const TCHAR* DxDraw::getErrorName(HRESULT a_errcode){
   CHECK_AND_RETURN_DDERR_IN_STRING(a_errcode, DDERR_DIRECTDRAWALREADYCREATED);
   CHECK_AND_RETURN_DDERR_IN_STRING(a_errcode, DDERR_EXCLUSIVEMODEALREADYSET);
   CHECK_AND_RETURN_DDERR_IN_STRING(a_errcode, DDERR_GENERIC);
   CHECK_AND_RETURN_DDERR_IN_STRING(a_errcode, DDERR_INCOMPATIBLEPRIMARY);
   CHECK_AND_RETURN_DDERR_IN_STRING(a_errcode, DDERR_INVALIDCAPS);
   CHECK_AND_RETURN_DDERR_IN_STRING(a_errcode, DDERR_INVALIDCLIPLIST);
   CHECK_AND_RETURN_DDERR_IN_STRING(a_errcode, DDERR_INVALIDOBJECT);
   CHECK_AND_RETURN_DDERR_IN_STRING(a_errcode, DDERR_INVALIDPARAMS);
   CHECK_AND_RETURN_DDERR_IN_STRING(a_errcode, DDERR_INVALIDPIXELFORMAT);
   CHECK_AND_RETURN_DDERR_IN_STRING(a_errcode, DDERR_INVALIDRECT);
   CHECK_AND_RETURN_DDERR_IN_STRING(a_errcode, DDERR_NOALPHAHW);
   CHECK_AND_RETURN_DDERR_IN_STRING(a_errcode, DDERR_NOBLTHW);
   CHECK_AND_RETURN_DDERR_IN_STRING(a_errcode, DDERR_NOCLIPLIST);
   CHECK_AND_RETURN_DDERR_IN_STRING(a_errcode, DDERR_NOCOOPERATIVELEVELSET);
   CHECK_AND_RETURN_DDERR_IN_STRING(a_errcode, DDERR_NOFLIPHW);
   CHECK_AND_RETURN_DDERR_IN_STRING(a_errcode, DDERR_NOOVERLAYHW);
   CHECK_AND_RETURN_DDERR_IN_STRING(a_errcode, DDERR_NORASTEROPHW);
   CHECK_AND_RETURN_DDERR_IN_STRING(a_errcode, DDERR_NOSTRETCHHW);
   CHECK_AND_RETURN_DDERR_IN_STRING(a_errcode, DDERR_OUTOFMEMORY);
   CHECK_AND_RETURN_DDERR_IN_STRING(a_errcode, DDERR_OUTOFVIDEOMEMORY);
   CHECK_AND_RETURN_DDERR_IN_STRING(a_errcode, DDERR_PRIMARYSURFACEALREADYEXISTS);
   CHECK_AND_RETURN_DDERR_IN_STRING(a_errcode, DDERR_SURFACEBUSY);
   CHECK_AND_RETURN_DDERR_IN_STRING(a_errcode, DDERR_SURFACELOST);
   CHECK_AND_RETURN_DDERR_IN_STRING(a_errcode, DDERR_UNSUPPORTED);
   CHECK_AND_RETURN_DDERR_IN_STRING(a_errcode, DDERR_WASSTILLDRAWING);
   CHECK_AND_RETURN_DDERR_IN_STRING(a_errcode, DDERR_GENERIC);
   CHECK_AND_RETURN_DDERR_IN_STRING(a_errcode, DDERR_NOPALETTEATTACHED );
   CHECK_AND_RETURN_DDERR_IN_STRING(a_errcode, DDERR_INVALIDOBJECT);
   CHECK_AND_RETURN_DDERR_IN_STRING(a_errcode, DDERR_NOSTRETCHHW);
   CHECK_AND_RETURN_DDERR_IN_STRING(a_errcode, DDERR_INVALIDPARAMS);
   CHECK_AND_RETURN_DDERR_IN_STRING(a_errcode, DDERR_OVERLAPPINGRECTS );
   CHECK_AND_RETURN_DDERR_IN_STRING(a_errcode, DDERR_INVALIDRECT);
   CHECK_AND_RETURN_DDERR_IN_STRING(a_errcode, DDERR_SURFACEBUSY);
   CHECK_AND_RETURN_DDERR_IN_STRING(a_errcode, DDERR_NOALPHAHW);
   CHECK_AND_RETURN_DDERR_IN_STRING(a_errcode, DDERR_SURFACELOST);
   CHECK_AND_RETURN_DDERR_IN_STRING(a_errcode, DDERR_NOBLTHW);
   CHECK_AND_RETURN_DDERR_IN_STRING(a_errcode, DDERR_UNSUPPORTED);
   return L"unknown error code";
}

//==============================================================================

DxSurf &DxDraw::primSurf(){
   return m_primary_wrapper;
}

//==============================================================================

DxSurf &DxDraw::backSurf(){
   return m_backbuffer_wrapper;
}

//==============================================================================

bool DxDraw::checkAndClipMenuZone(){
   if (! m_owner_window) return false;
//    if (! m_primary_wrapper.isEnabled()) return false;

   // 12-08-2008, 17:46, q-alo: this is a trick to catch menu window position and clip it out of the
   // painting area
   HWND mnu = FindWindow(L"MNU", NULL);

   if ((mnu != NULL) && (m_owner_window != mnu)){
      m_clip_menu = true;
      GetWindowRect(mnu, &m_menuClipRect);
   }else{
      m_clip_menu = false;      // no menu - no clip
   }

   return true;
}

//==============================================================================

// void DxDraw::enableDrawing(bool enable){
//    m_primary_wrapper.enable(enable);
// }

// //==============================================================================

// bool DxDraw::isDrawingEnabled(){
//    return m_primary_wrapper.isEnabled();
// }

//==============================================================================

DxDraw* DxDraw::m_DxDraw = NULL;

void DxDraw::Destroy(){
   if (m_DxDraw){
      delete m_DxDraw;
      m_DxDraw = NULL;
   }
}
//==============================================================================
// eof
