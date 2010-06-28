/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "PALGraphics.h"
#include <cassert>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>
#include "stb_image.cpp"
#include <windows.h>
#include <vector>

#define TEXT_LIMIT 255

#define DBG "[PALGraphicsWM]: "
#define MS_RGB(r,g,b) ((COLORREF)((BYTE)(r)|((BYTE)(g) << 8)|((BYTE)(b) << 16)))
 
namespace pal {

struct PALGraphicsWMContext {
   PALGraphicsWMContext()  :
      m_metricBuf(),
      m_fontRgb(0xFFFFFFFF)
   {
      m_dc = CreateCompatibleDC( GetDC( NULL ) );
      ::memset(&m_fontData, 0, sizeof(LOGFONT));
      m_fontData.lfWidth = 0;
      m_fontData.lfWeight = FW_MEDIUM;
      m_fontData.lfItalic = FALSE;
      m_fontData.lfStrikeOut = FALSE;
      m_fontData.lfUnderline = FALSE;
      m_fontData.lfCharSet = OEM_CHARSET;
      m_fontData.lfOutPrecision = OUT_DEFAULT_PRECIS;
      m_fontData.lfClipPrecision = CLIP_DEFAULT_PRECIS;
      m_fontData.lfQuality = CLEARTYPE_QUALITY;
      m_fontData.lfPitchAndFamily = FIXED_PITCH | FF_DONTCARE;

      setFontForContext(this, "Verdana", "Normal", 10);
   }

   ~PALGraphicsWMContext() {
      
   }

   HDC m_dc;
   LOGFONT m_fontData;
   TEXTMETRIC m_textMetrics;
   HFONT m_curFont;
   pal::wf_uint32 m_fontRgb;
   std::vector<ABC> m_metricBuf;
};



pstatus loadImageFromMemory(GraphicsContext context,
                            const wf_uint8* source,
                            wf_uint32 sourceSize,
                            ImageType type,
                            BufFormat format,
                            wf_uint32 desiredWidthPixels,
                            wf_uint32 desiredHeightPixels,
                            wf_uint32* widthPixels,
                            wf_uint32* heightPixels,
                            ResultBuffer* buffer)
{
   if(type != PNG) {
      return PAL_ERR_IMG_TYPE_NOT_SUPPORTED;
   }

   if(format != RGBA8) {
      return PAL_ERR_BUFFER_FORMAT_NOT_SUPPORTED;
   }

   assert(sizeof(stbi_uc) == sizeof(wf_uint8));

   int reqComp  = 4; // Always read RGBA
   int comp     = 0;

   pal::wf_int32 width = 0;
   pal::wf_int32 height = 0;
   
   wf_uint8* buf =
      stbi_load_from_memory(source,
                            sourceSize,
                            &width,
                            &height,
                            &comp,
                            reqComp);

   assert(width >= 0);
   assert(height >= 0);
   
   *widthPixels = width;
   *heightPixels = height;
   
   assert(comp == reqComp);

   buffer->init(buf, (*widthPixels) * (*heightPixels) * 4, free);
   
   return PAL_OK;
}

struct GDI_FontInfo {
   HBITMAP bitmap;
   int bmpWidth;
   int bmpHeight;
   LPSTR bmpBits;
};

#define WIDTHBYTES(i) ((i+31)/32*4)
#define PALETTE_SIZE 2

pstatus loadImageFromFile(GraphicsContext context,
                          FILE* source,
                          ImageType type,
                          BufFormat format,
                          wf_uint32 desiredWidthPixels,
                          wf_uint32 desiredHeightPixels,
                          wf_uint32* widthPixels,
                          wf_uint32* heightPixels,
                          ResultBuffer* buffer)
{
   if(type != PNG) {
      return PAL_ERR_IMG_TYPE_NOT_SUPPORTED;
   }

   if(format != RGBA8) {
      return PAL_ERR_BUFFER_FORMAT_NOT_SUPPORTED;
   }

   assert(sizeof(stbi_uc) == sizeof(wf_uint8));
   
   int reqComp  = 4; // Always read RGBA
   int comp     = 0;

   pal::wf_int32 width = 0;
   pal::wf_int32 height = 0;
   
   unsigned char* buf =
      stbi_load_from_file(source, &width, &height, &comp, reqComp);

   *widthPixels = width;
   *heightPixels = height;
   
   assert(comp == reqComp);
   
   buffer->init(buf, (*widthPixels) * (*heightPixels) * 4, free);
   
   return PAL_OK;
}

void utf2Unicode(const char* utf8Text,
                 unsigned int pos,
                 unsigned int length,
                 std::wstring& fullUniStr)
{
   unsigned int uniLength = strlen(utf8Text);
   unsigned int wideBufSize = strlen(utf8Text) * 2;

   fullUniStr.resize(wideBufSize, L'A');

   /* convert the UTF8 text to Unicode */
   int uniLen = 0;
   uniLen = MultiByteToWideChar(CP_UTF8,
                                0,
                                utf8Text,
                                strlen(utf8Text), // nbr bytes in str.
                                &fullUniStr[0],
                                wideBufSize);

   // Also null terminate.
   if ( uniLen < (int)wideBufSize ) {
      fullUniStr.resize(uniLen);
   } else if ( wideBufSize > 0 ) {
      fullUniStr.resize(wideBufSize);
   }
   
   if(pos != 0 || length != -1) {
      fullUniStr = fullUniStr.substr(pos, length);
   }
}

pstatus getTextDimensions(GraphicsContext extContext,
                          const std::wstring& str,
                          wf_uint32* widthPixels,
                          wf_uint32* heightPixels)
{
   
   PALGraphicsWMContext* context = ((PALGraphicsWMContext*)extContext);

   // First, start by determining the length of the string up to the
   // last character. We do this using GetTextExtentPoint32W instead of
   // GetCharABCWidths. We could use GetCharABCWidths for the entire
   // string, but we would then have to preallocate a ABC for each
   // and every unique unicode character in the string. At most, this
   // buffer would contain 65536 entries. Since the bounding boxes for
   // the individual characters are technically correct with respect
   // to the offset of the next character, this will work (essentially,
   // the last character, which is covered by GetCharABCWidths, will
   // start at size.cx).

   SIZE size;
   
   if(str.size() > 1) {
      GetTextExtentPoint32W(context->m_dc, &str[0], str.size() - 1, &size);      
   } else {
      GetTextExtentPoint32W(context->m_dc, &str[0], str.size(), &size);
      size.cx = 0;
   }

   // We then find the unicode value of the last character in the string
   unsigned lastCharUnicodeVal = (unsigned int)(*str.rbegin());

   // Just preallocate a single ABC entry since the span of the range is 1
   ABC tmp;
   
   if (GetCharABCWidths( context->m_dc,
                         lastCharUnicodeVal,
                         lastCharUnicodeVal,
                         &tmp ))
   {
      // Don't take into account the negative displacement of the C
      // part, since that is the adjustment for the following character,
      // and could be negative. We are only interested in the bounding box.
      *widthPixels  = size.cx + tmp.abcA + tmp.abcB + std::abs(tmp.abcC);
      *heightPixels = size.cy;
   } else {
      // If we for some reason fail, we resort to using the incorrect
      // GetTextExtentPoint32W value
      GetTextExtentPoint32W(context->m_dc, &str[0], str.size(), &size);
      *widthPixels  = size.cx;
      *heightPixels = size.cy;
   }
   
   return PAL_OK;
}

pstatus drawTextLineToBuffer(GraphicsContext extContext,
                             const wf_char* utf8Text,
                             wf_int32 charPosition,
                             wf_int32 length,
                             BufFormat format,
                             wf_uint32* widthPixels,
                             wf_uint32* heightPixels,
                             ResultBuffer* buffer)
{
   PALGraphicsWMContext* context = ((PALGraphicsWMContext*)extContext);

   GDI_FontInfo fontInfo;

   LPBITMAPINFO bitMapInfo;

   bitMapInfo = (LPBITMAPINFO) LocalAlloc(LPTR, sizeof(BITMAPINFO) +
                                          PALETTE_SIZE * sizeof(RGBQUAD));   

   std::wstring tmpStr;

   utf2Unicode(utf8Text, charPosition, length, tmpStr);
   
   // Select font, and get the text dimensions
   SelectObject(context->m_dc, context->m_curFont);
   
   pstatus stat = getTextDimensions(extContext,
                                    tmpStr,
                                    widthPixels,
                                    heightPixels);
   assert(stat == PAL_OK);

   // Set bitmap dimensions
   fontInfo.bmpWidth = *widthPixels;
   fontInfo.bmpHeight = *heightPixels;

   //
   bitMapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
   bitMapInfo->bmiHeader.biWidth = fontInfo.bmpWidth;
   
   // negative height = top-down
   bitMapInfo->bmiHeader.biHeight = -1 * fontInfo.bmpHeight;
   bitMapInfo->bmiHeader.biPlanes = 1;
   bitMapInfo->bmiHeader.biBitCount = 8; // 8bpp makes it easy to get data

   bitMapInfo->bmiHeader.biCompression = BI_RGB;
   bitMapInfo->bmiHeader.biXPelsPerMeter = 0;
   bitMapInfo->bmiHeader.biYPelsPerMeter = 0;
   bitMapInfo->bmiHeader.biClrUsed = PALETTE_SIZE;
   bitMapInfo->bmiHeader.biClrImportant = PALETTE_SIZE;

   bitMapInfo->bmiHeader.biSizeImage = WIDTHBYTES(fontInfo.bmpWidth * 8) *
      fontInfo.bmpHeight;

// Just a plain monochrome palette
   bitMapInfo->bmiColors[0].rgbRed = 0;
   bitMapInfo->bmiColors[0].rgbGreen = 0;
   bitMapInfo->bmiColors[0].rgbBlue = 0;
   bitMapInfo->bmiColors[1].rgbRed = 255;
   bitMapInfo->bmiColors[1].rgbGreen = 255;
   bitMapInfo->bmiColors[1].rgbBlue = 255;

// Create a DIB section that we can use to read the font bits out of
   fontInfo.bitmap = CreateDIBSection(context->m_dc, bitMapInfo,
                                      DIB_RGB_COLORS, (void **)
                                      &fontInfo.bmpBits,
                                      NULL, 0);
   LocalFree( bitMapInfo );
   
   if (fontInfo.bitmap == NULL)
   {
      DeleteDC(context->m_dc);
      return PAL_ERR_FONT_SURFACE_ERROR;
   }

// Set up our memory DC with the font and bitmap
   HGDIOBJ prevObj = SelectObject( context->m_dc, fontInfo.bitmap );

   DeleteObject( prevObj );
   
   SetBkColor( context->m_dc, MS_RGB(0, 0, 0) );
   SetTextColor( context->m_dc, MS_RGB(255, 255, 255) );

// Output text to our memory DC (the bits end up in our DIB section)
   PatBlt(context->m_dc, 0, 0, fontInfo.bmpWidth, fontInfo.bmpHeight,
          BLACKNESS);

   ExtTextOutW( context->m_dc, 
                1, 1,
                ETO_OPAQUE, NULL,
                &tmpStr[0], tmpStr.size(), NULL);

   DWORD width = fontInfo.bmpWidth;
   DWORD height = fontInfo.bmpHeight;
   
   wf_uint8* buf = (wf_uint8*)malloc(width * height * 4);
   buffer->init(buf, width * height * 4, free);

   wf_uint32* asInt = reinterpret_cast<wf_uint32*>(buf);

   *widthPixels = width;
   *heightPixels = height;
   
   LPSTR lpsrc = fontInfo.bmpBits;
   
   for (int ii = 0; ii < fontInfo.bmpHeight; ii++) {
      for (int jj = 0; jj < fontInfo.bmpWidth; jj++) {

         // If lpsrc[j] is 0, then it's a black pixel (opaque)
         // otherwise it's white (transparent)
         
         if (lpsrc[jj]) {
            *asInt++ = context->m_fontRgb;
         } else {
            *asInt++ = 0;
         }
      }
      lpsrc += WIDTHBYTES(fontInfo.bmpWidth * 8);
   }
   
   // bitMap.imgWidth = bitMap.memWidth = fontInfo.bmpWidth;
   // bitMap.imgHeight = bitMap.memHeight = fontInfo.bmpHeight;
   
   return PAL_OK;
}


pstatus createGraphicsContext(GraphicsContext* context) {
   //TODO: Change to malloc
   *context = new PALGraphicsWMContext;
   return PAL_OK;
}

pstatus destroyGraphicsContext(GraphicsContext context) {
   //TODO: Change to free
   delete ((PALGraphicsWMContext*)context);
   return PAL_OK;
}

pstatus setFontForContext(GraphicsContext extContext,
                          const pal::wf_char* fontFamily,
                          const pal::wf_char* fontStyle,
                          pal::wf_uint32 fontSize)
{
   PALGraphicsWMContext* context = ((PALGraphicsWMContext*)extContext);

   // Start by setting up the font style
   if(strcmp(fontStyle, "Regular") == 0) {
      context->m_fontData.lfWeight = FW_NORMAL;
      context->m_fontData.lfItalic = false;
   } else if(strcmp(fontStyle, "Bold") == 0) {
      context->m_fontData.lfWeight = FW_BOLD;
      context->m_fontData.lfItalic = false;
   } else if(strcmp(fontStyle, "Italic") == 0) {
      context->m_fontData.lfWeight = FW_NORMAL;
      context->m_fontData.lfItalic = true;
   } else if(strcmp(fontStyle, "Bold Italic") == 0) {
      context->m_fontData.lfWeight = FW_BOLD;
      context->m_fontData.lfItalic = true;
   } else {
      return PAL_ERR_TEXT_FONT_STYLE_INVALID;
   }
   
   // printf("[PALGraphicsWM]: Setting font to: %s %s @ %d\n",
   //        fontFamily, fontStyle, fontSize);
   
   std::wstring tmpStr;
   
   utf2Unicode(fontFamily, 0, -1, tmpStr);

   /* add the fontface & size to the font data */
   ::lstrcpy(context->m_fontData.lfFaceName, &tmpStr[0]);
   context->m_fontData.lfHeight = fontSize;
   context->m_fontData.lfPitchAndFamily = VARIABLE_PITCH;
   
   /* try to create the font */
   context->m_curFont = ::CreateFontIndirect(&context->m_fontData);
   
   return PAL_OK;
}

pstatus setFontColorForContext(GraphicsContext extContext,
                               pal::wf_uint8 r,
                               pal::wf_uint8 g,
                               pal::wf_uint8 b)
{
   PALGraphicsWMContext* context = ((PALGraphicsWMContext*)extContext);

   context->m_fontRgb =
      ((wf_uint32)(r)) |
      ((wf_uint32)(g) << 8) |
      ((wf_uint32)(b) << 16) |
      0xFF000000;
   
   return PAL_OK;
}

// TEST_STR2 is equal to "abc (de)" where abcde are random chinese
// characters
#define TEST_STR1 "Test String"
#define TEST_STR2 "\xE4\xB8\xAD\xE5\x9B\xBD\xE8\xAA\x9E (\xE7\xB9\x81\xE4\xBD\x93)"
#define TEST_RES1 L"Test String"
#define TEST_RES2 L"Te"
#define TEST_RES3 L"st S"
#define TEST_RES4 L"st String"

pstatus runGraphicsTests(GraphicsContext context) {
   // Test the internal utf2Unicode functionality.

   std::wstring fullUniStr;

   utf2Unicode(TEST_STR1, 0, -1, fullUniStr);
   assert(wcscmp(TEST_RES1, fullUniStr.c_str()) == 0);
   utf2Unicode(TEST_STR1, 0, 2, fullUniStr);
   assert(wcscmp(TEST_RES2, fullUniStr.c_str()) == 0);
   utf2Unicode(TEST_STR1, 2, 4, fullUniStr);
   assert(wcscmp(TEST_RES3, fullUniStr.c_str()) == 0);
   utf2Unicode(TEST_STR1, 2, -1, fullUniStr);
   assert(wcscmp(TEST_RES4, fullUniStr.c_str()) == 0);
   utf2Unicode(TEST_STR2, 0, -1, fullUniStr);
   
   return PAL_OK;
}

} // End of namespace pal


















