/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "WinMobileNativeTextInterface.h"
#include <cstring>
#include <cstdlib>
#include <math.h>
#include <tchar.h>
#include "MC2Point.h"
#include "WinBitmap.h"
#include "WinTileMapToolkit.h"
#include "TextConv.h"
#include "WinMobileCompatibility.h"
#include "BitMapUtil.h"

#define MAX_STR_LEN 512


/* rotates a point around another by the specified angle */
void rotate2D(int x, int y,
              float rotAngle, 
              int* outx, int* outy,
              int cx = 0, int cy = 0)
{
   #define DEG2RAD(ang) (0.01745329252*ang)

   float sine = sin(DEG2RAD(rotAngle));
   float cosine = cos(DEG2RAD(rotAngle));

   /* calculate the points and translate back to given centre */
   *outx = int( float(y)*sine   + float(x)*cosine ) + cx;
   *outy = int( float(y)*cosine - float(x)*sine ) + cy;

   return;
}

/* gets the rotated bounding rectangle for the specified angle and points */
isab::Rectangle WinMobileNativeTextInterface::rotatedRect( int x1, int y1,
                                                           int x2, int y2, 
                                                           float angle )
{
   // This method doesn't work very well.

   /* max\min bounds of the final rectangle */
   POINT minBound, maxBound;

   /* the points of the rectangle */
   POINT rotVerts[4];

   /* get half dimensions of the rectangle */
   int halfW = (x2-x1) >> 1;
   int halfH = (y2-y1) >> 1;

   /* get the center point */
   int cx = x1 + halfW;
   int cy = y1 + halfH;

   /* rotate the points */
   rotate2D( -halfW, -halfH, angle,
             (int*)&rotVerts[0].x,
             (int*)&rotVerts[0].y,
             cx, cx);
   
   rotate2D( halfW, -halfH, angle,
             (int*)&rotVerts[1].x,
             (int*)&rotVerts[1].y,
             cx, cx);
   
   rotate2D( halfW,  halfH, angle,
             (int*)&rotVerts[2].x,
             (int*)&rotVerts[2].y,
             cx, cx);
   
   rotate2D( -halfW,  halfH, angle,
             (int*)&rotVerts[3].x,
             (int*)&rotVerts[3].y,
             cx, cx);

   /* get the minimum and maximum bound points */
   minBound.x = min( rotVerts[0].x,
                     min( min( rotVerts[1].x,
                               rotVerts[2].x),
                          rotVerts[3].x));
   
   minBound.y = min( rotVerts[0].y,
                     min( min( rotVerts[1].y,
                               rotVerts[2].y),
                          rotVerts[3].y));
   
   maxBound.x = max( rotVerts[0].x,
                     max( max( rotVerts[1].x,
                               rotVerts[2].x),
                          rotVerts[3].x));
   
   maxBound.y = max( rotVerts[0].y,
                     max( max( rotVerts[1].y,
                               rotVerts[2].y),
                          rotVerts[3].y));

   /* get the dimensions of the rectangle */
   int w = maxBound.x - minBound.x;
   int h = maxBound.y - minBound.y;

   /* return the rotated rectangle */
   return( isab::Rectangle(minBound.x, minBound.y, w, h) );
}

WinMobileNativeTextInterface::WinMobileNativeTextInterface()
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

   /* set the default font */
   setFont(STRING("Lucida Console"), 12 );

}

int
WinMobileNativeTextInterface::utf8ToUnicode( const STRING& text,
                              int startIdx,
                              int nbrChars,
                              wchar_t* uniStr,
                              int uniStrLen )
{
   // Convert the full string to unicode first.
   wchar_t fullUniStr[MAX_STR_LEN];
   int fullUniLen = TextConverter::UTF2Unicode( text.c_str(), 
                                                fullUniStr, 
                                                MAX_STR_LEN );

   if( nbrChars == -1 ) {
      // Get length of string from startIdx to end. 
      nbrChars = fullUniLen - startIdx;
   }
 
   // Extract the portion of it that we are interested in.
   wcsncpy( uniStr, fullUniStr + startIdx, nbrChars );
   int uniLen = nbrChars;    
   uniStr[ uniLen ] = 0; 

   return uniLen;
}

/**
 *   Puts the bounding boxes of each character in str
 *   into the vector boxes.
 *   <br />
 *   Default implementation does nothing and returns 0.
 *   @param boxes  Vector to add the boxes to.
 *   @see drawText
 *   @return Number of characters in str.
 */
isab::Rectangle
WinMobileNativeTextInterface::getStringAsRectangle( const STRING& text,
                                                    const MC2Point& point,
                                                    int startIdx,
                                                    int nbrChars,
                                                    float angle )
{

   // Extract the portion of it that we are interested in.
   wchar_t uniStr[MAX_STR_LEN];

   int uniLen = utf8ToUnicode( text, startIdx, nbrChars, uniStr, MAX_STR_LEN);

   /* calculate the size of the string */
   SIZE strSize;
   GetTextExtentPoint32W(m_dc, uniStr, uniLen, &strSize);

   /* the rectangle's dimensions are based on the center point and the 
      font height and the total string width */
   int rectX = point.getX() - (strSize.cx >> 1);
   int rectY = point.getY() - (strSize.cy >> 1);

   isab::Rectangle rect;
   
   /* zero angle specified */
   if(angle == 0.0f) {
      /* return the new rectangle */
      rect = isab::Rectangle(rectX, rectY, strSize.cx, strSize.cy);
   } else {
      // This probably doesn't work very well.
      /* angle specified, calculate the rotated rectangle */
      rect = rotatedRect(rectX, rectY, 
                         rectX+strSize.cx, rectY+strSize.cy, 
                         angle);
   }


   return rect;
}


/**
 *   Puts the bounding boxes of each character in str
 *   into the vector boxes.
 *   <br />
 *   Default implementation does nothing and returns 0.
 *   @param boxes  Vector to add the boxes to.
 *   @see drawText
 *   @return Number of characters in str.
 */
int WinMobileNativeTextInterface::getStringAsRectangles( vector<isab::Rectangle>& boxes,
                                                         const STRING& text,
                                                         const MC2Point& point,
                                                         int startIdx,
                                                         int nbrChars,
                                                         float angle )
{
   
   // Extract the portion of it that we are interested in.
   wchar_t uniStr[MAX_STR_LEN];

   int uniLen = utf8ToUnicode( text, startIdx, nbrChars, uniStr, MAX_STR_LEN);
   
   /* select our font */
   HFONT newFont = createFont((int)angle);
   if(newFont == NULL) return 0;
   /* select the new font into our DC */
   HFONT oldFont = (HFONT)SelectObject(m_dc, newFont);
   
   /* calculate the size of the string */
   SIZE strSize;

   GetTextExtentPoint32W(m_dc, uniStr, uniLen, &strSize);

   /* delete our font */
   SelectObject(m_dc, oldFont);
   DeleteObject(newFont);
   
   // End of new stuff.
   
   /* counter variable */
   int i;   
   
   /* Loop thru the text and set the bounding rectangle for each character 
      Each rectangle holds absolute coords for the begininning of 
      each character and their width and height */

   /* height info */
   int textH = strSize.cy;
   int halfTextH = textH >> 1;

   /* number of characters to be printed */
   int maxTextLen = strSize.cx;
   int halfTextLen = maxTextLen >> 1;

   /* get the center points of all the characters */
   vector<MC2Point> origArray;

   int startX = point.getX() - halfTextLen;
   // Lets guess that all chars are approx. equally wide.
   int textW = strSize.cx / nbrChars;

   for(i = 0; i < nbrChars; i++) {

      /* get the relative coord for the center */
      int origX = (startX + (textW >> 1)) - point.getX();
      int origY = 0;

      /* get the rotated center */
      int rotOrigX, rotOrigY;
      rotate2D(origX, origY, 
            angle, 
            &rotOrigX, &rotOrigY,
            point.getX(), point.getY());

      /* add to list */
      origArray.push_back(MC2Point(rotOrigX,
               rotOrigY));

      /* move the x-axis forward */
      startX += textW;
   }

   /* get the rectangles */
   int curOrig = 0;
   for(i = 0; i < nbrChars; i++) {
      // If Varun's rotatedRect method would work 
      // then it should be used.
      // However it doesn't work, so instead we don't rotate the boxes.
      // The nonrotated boxes may cause incorrect collision detection,
      // however it will not happen very often.

      //         isab::Rectangle rotBox = rotatedRect(origArray[curOrig].getX(),
      //                                        origArray[curOrig].getY(),
      //                                        origArray[curOrig].getX()+textW,
      //                                        origArray[curOrig].getY()+textH, 
      //                                        angle);
      isab::Rectangle rotBox = isab::Rectangle(origArray[curOrig].getX() - textW/2,
            origArray[curOrig].getY() - textH/2,
            textW,
            textH );

      boxes.push_back(rotBox);
      curOrig++;
   }
   /* clear origin vector */
   origArray.clear();

   /* returns the number of characters processed */
   return(nbrChars);
}
STRING* WinMobileNativeTextInterface::createString(const char* text)
{
   STRING* newStr = new STRING(text);
   return(newStr);
}


/**
 *   Deletes a text string.
 *   Default implementation does nothing.
 */   
void WinMobileNativeTextInterface::deleteString(STRING* text)
{
   delete text;
   text = NULL;
   return;
}

/**
 *   @return The number of characters in the string.
 */
int WinMobileNativeTextInterface::getStringLength( const STRING& text )
{
   // Unfortunately neither _mbslen or _mbstrlen seem to work in 
   // windows. However MultiByteToWideChar will return the correct
   // number of characters. Not very efficient though.

   wchar_t uniStr[MAX_STR_LEN];
   int uniLen = 0;
   return MultiByteToWideChar(CP_UTF8,
                              0,
                              text.c_str(),
                              strlen(text.c_str()), // nbr bytes in str.
                              uniStr,
                              MAX_STR_LEN);
}

struct GDI_FontInfo {
   HBITMAP bitmap;
   int bmpWidth;
   int bmpHeight;
   LPSTR bmpBits;
};

#define WIDTHBYTES(i) ((i+31)/32*4)
#define PALETTE_SIZE 2

int WinMobileNativeTextInterface::load( RGBA32BitMap& bitMap ) {

   // Extract the portion of it that we are interested in.
   wchar_t uniStr[MAX_STR_LEN];

   const STRING& text = *m_string;
   
   int uniLen = utf8ToUnicode( text, m_startIndex,
                               m_endIndex - m_startIndex,
                               uniStr, MAX_STR_LEN);

   GDI_FontInfo fontInfo;

   LPBITMAPINFO bitMapInfo;
   SIZE size;

   bitMapInfo = (LPBITMAPINFO) LocalAlloc(LPTR, sizeof(BITMAPINFO) +
                                    PALETTE_SIZE * sizeof(RGBQUAD));   

   
// Select font, and get the text dimensions
   SelectObject(m_dc, m_curFont);
   GetTextExtentPoint32W(m_dc, uniStr, uniLen, &size);

// Set bitmap dimensions
   fontInfo.bmpWidth = size.cx + 2;
   fontInfo.bmpHeight = size.cy + 2;
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
   fontInfo.bitmap = CreateDIBSection(m_dc, bitMapInfo,
                                      DIB_RGB_COLORS, (void **)
                                      &fontInfo.bmpBits,
                                      NULL, 0);
   LocalFree( bitMapInfo );
   
   if (fontInfo.bitmap == NULL)
   {
      DeleteDC(m_dc);
      return NULL;
   }

// Set up our memory DC with the font and bitmap
   HGDIOBJ prevObj = SelectObject( m_dc, fontInfo.bitmap );

   DeleteObject( prevObj );
   
   SetBkColor( m_dc, MS_RGB(0, 0, 0) );
   SetTextColor( m_dc, MS_RGB(255, 255, 255) );

// Output text to our memory DC (the bits end up in our DIB section)
   PatBlt(m_dc, 0, 0, fontInfo.bmpWidth, fontInfo.bmpHeight,
          BLACKNESS);

   ExtTextOutW( m_dc, 
                1, 1,
                ETO_OPAQUE, NULL,
                uniStr, uniLen, NULL);

   int widthAdjust = 0;
   int heightAdjust = 0;
   int extraWidthAdjust = 0;
   
   if(m_hasBackgroundBox) {
      // this is an offset to center the text into the roadsign box:
      widthAdjust = 8;
      heightAdjust = 2;
      extraWidthAdjust = 1;
   }

   bitMap.imgWidth = bitMap.memWidth = fontInfo.bmpWidth+widthAdjust+extraWidthAdjust;
   bitMap.imgHeight = bitMap.memHeight = fontInfo.bmpHeight+heightAdjust;
   
   // Make room for the entire buffer
   bitMap.data.resize(bitMap.imgWidth * bitMap.imgHeight);
   
   int i=0;
   if(m_hasBackgroundBox) {
      BitMapUtil::applyRoadsignBackground(bitMap);
      i=bitMap.imgWidth*heightAdjust/2+widthAdjust/2;
   }
   
   uint32 pixel = 0;
   
   LPSTR lpsrc = fontInfo.bmpBits;
   //loop over the pixels in the image containing the text
   for (int ii = 0; ii < fontInfo.bmpHeight; ii++) {
      for (int jj = 0; jj < fontInfo.bmpWidth; jj++) {

         // If lpsrc[j] is 0, then it's a black pixel (opaque)
         // otherwise it's white (transparent)
         if (lpsrc[jj]) {
            pixel = 0xFF000000 | ( m_b << 16) | ( m_g <<  8) | ( m_r <<  0);
         }
         else {
            pixel = 0;
         }
            
         if(m_hasBackgroundBox && pixel) {
            // if we are drawing upon a roadsign image,
            // only draw the actual characters
            bitMap.data[i] = FRAMECOLOR;
         }
         if(!m_hasBackgroundBox) {
            // this is an ordinary text string, ie small road names
            bitMap.data[i] = pixel;
         }
         i++;
      }
      i += widthAdjust+extraWidthAdjust;
      lpsrc += WIDTHBYTES(fontInfo.bmpWidth * 8);
   }
   
   return 0;
}

/**
 *   Sets font. The description format in the server
 *   and the MapPlotter must know what needs to be put
 *   here.
 *   @param fontName Name of the font in client-specific format.
 *   @param size     Size of the font in UNIT.
 */
void WinMobileNativeTextInterface::setFont( const STRING& fontName, int size )
{
   /* check if the current font is the same as the one requested */
   if( (m_curFontName == fontName) && (m_curFontSize == size) ) return;

   WCHAR buf[32];
   TextConverter::UTF2Unicode(fontName.c_str(), buf, 32);

   /* add the fontface & size to the font data */
   ::lstrcpy(m_fontData.lfFaceName, buf);
   m_fontData.lfHeight = size;
   m_fontData.lfPitchAndFamily = VARIABLE_PITCH;

   /* set the current font variables */
   m_curFontName = fontName;
   m_curFontSize = size;

   /* get the Text Metrics for the font */
#ifdef USE_GDI_DRAWING
   GetTextMetrics(DC(), &m_textMetrics);
#endif
   m_fontHeight = m_textMetrics.tmHeight;
   m_fontWidth = m_textMetrics.tmMaxCharWidth;

   return;
}

HFONT WinMobileNativeTextInterface::createFont(float angle)
{
   HFONT newFont = NULL;

   /* add the font angle to the font data */
   m_fontData.lfEscapement = m_fontData.lfOrientation = int(angle*10.0f);

   /* try to create the font */
   newFont = ::CreateFontIndirect(&m_fontData);

   /* success, return the created font */
   return(newFont);
}

isab::Font* WinMobileNativeTextInterface::createFont(WFAPI::FontSpec* fontSpec)
{
   return NULL;
}

void WinMobileNativeTextInterface::deleteFont(isab::Font* font)
{
   
}
