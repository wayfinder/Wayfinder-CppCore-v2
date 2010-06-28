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

#include "config.h"
#include "WinMapPlotter.h"
#include "MC2Point.h"
#include "WinBitmap.h"
#include "WinTileMapToolkit.h"
#include "TextConv.h"
#include <cstring>
#include <cstdlib>
#include <math.h>
#include <tchar.h>
#include "LogFile.h"
#include "WinMobileCompatibility.h"

using namespace WFAPI;

// The max length of the strings supported by the WinMapPlotter.
#define MAX_STR_LEN 512

#define ELLIPSE_SIZE_INFLATE_RATIO 1.5
#define MAX_SCALE_FOR_FLOATED_ELLIPSES 1.5

#define MAX_ZOOM_TO_DRAW_ELLIPSES 90
#define MAX_ZOOM_TO_DRAW_ENDCAPS_ALWAYS 4
#define MIN_PEN_WIDTH_TO_DRAW_AS_POLYGON 10

#define USE_GDI_DRAWING

//#define TIMINGS
// #define SNAPSHOT_MOVING_TIMINGS

//#define DRAWING_COMPLETE_TIMINGS
//#define BLITTO_TIMINGS

//#define OVERALL_TIMINGS

namespace isab {

// -- PointArray 

/* constructor */
PointArray::PointArray(int maxSize)
{
   /* reserve specified number of point cells in vector */
   reserve(maxSize);
}

/* destructor */
PointArray::~PointArray()
{
   /* clear the point vector */
   clear();
}

/* adds a point to the vector */
void PointArray::addPoint( int x, int y )
{
   push_back( MC2Point(x,y) );
   return;
}

/* Returns the number of coordinates in the vector. */
int PointArray::getSize() const
{
   return( size() );
}

/* Resets the add position ( and size ) back to zero. */
void PointArray::reset()
{
   clear();
   return;
}

/* Returns the number of allocated points. */
int PointArray::getAllocatedSize() const
{
   return( capacity() );
}


// -- WinMapPlotter 
/* private destructor */
WinMapPlotter::WinMapPlotter():
   m_afterBlitNotOnBackBuffer(false),
  //m_backBuffer(NULL),
   //  m_oldBitmap(NULL),
  m_backDC(NULL),
   //  m_tempDC(NULL),
  m_width(0),
  m_height(0),
  m_penWidth(1), m_penStyle(PS_SOLID),
  m_penColor( MS_RGB(255,255,255) ),
  m_pen(NULL),
  m_brushColor( MS_RGB(0,0,0) ),
  m_brush(NULL),
  m_fillColor( MS_RGB(128,128,128) ),
  m_fillBrush(NULL),
  m_fontWidth(0), m_fontHeight(0),
  m_curFontName(""), m_curFontSize(0),
  m_dragRect(NULL),
  m_frameRect(NULL),
  m_xform(NULL),
  m_frameWidth(0), 
  m_frameRed(0), m_frameGreen(0), m_frameBlue(0),
  m_pWinPaintCallback(NULL),
  m_nTime(0),
  m_nCountRedraw(0),
  m_nPrimitivesDrawed(0),
  m_nPenCached(0),
  m_backColor(0),
  m_backFillingBrush(NULL),
  m_bStatic(TRUE),
  m_dScale(MAX_UINT32)
//   m_screenSnapshot(NULL),
//   m_screenBitmap(NULL),
//  m_oldScreenBitmap(NULL)
{
}

/* second-phase constructor */
int WinMapPlotter::construct(int width, int height)
{
   m_ddraw = DxDraw::getInstance();
   resize(width, height);
        
	
   /* get the parent window's DC */
        //   HDC parentDC = GetDC(NULL);

   /* create a compatible DC from the passed DC */
   //   m_backDC = CreateCompatibleDC( parentDC );
   //   m_tempDC = CreateCompatibleDC( parentDC );

//    if(m_backDC == NULL || m_tempDC == NULL) {
//       /* the creation failed */
//       return(-1);
//    }

   /* release it, now that we have used it */
   //   ReleaseDC(NULL, parentDC);

   /* create the backbuffer */
//    m_backBuffer = CreateBitmap(width, height, 1,
//                                GetDeviceCaps(m_backDC, BITSPIXEL),
//                                NULL);

//    m_ddraw->createSurface(m_snapshotSurf, width, height);
//    m_ddraw->createSurface(m_cacheSurf, width, height);
   
//    if ()
   
//    m_tempBuffer = CreateBitmap(width, height, 1,
//                                GetDeviceCaps(m_backDC, BITSPIXEL),
//                                NULL);

//    if(m_backBuffer == NULL || m_tempBuffer == NULL) {
//       /* cannot create bitmap */
//       return(-2);
//    }

   /* select the backbuffer into the device context */
//    m_oldBitmap = (HBITMAP)SelectObject(m_backDC, m_backBuffer);
//    m_oldtBitmap = (HBITMAP)SelectObject(m_tempDC, m_tempBuffer);
//    if(m_oldBitmap == NULL || m_oldtBitmap == NULL) {
//       /* selection failed */
//       return(-3);
//    }

//   parentDC = GetDC(NULL);

   /* create a screen snapshot compatible DC from the parent DC */
//    m_screenSnapshot = CreateCompatibleDC( parentDC );
//    if(m_screenSnapshot == NULL) {
//       /* the creation failed */
//       return(-4);
//    }

   /* release parent dc, now that we have used it */
   //   ReleaseDC(NULL, parentDC);

//    /* create the snapshot bitmap */
//    m_screenBitmap = CreateBitmap(width, height, 1,
//                                GetDeviceCaps(m_backDC, BITSPIXEL),
//                                NULL);

//    if(m_screenBitmap == NULL) {
//       /* cannot create bitmap */
//       return(-5);
//    }

//    /* select the backbuffer into the device context */
//    HBITMAP m_oldScreenBitmap = (HBITMAP)SelectObject(m_screenSnapshot, m_screenBitmap);
//    if(m_oldScreenBitmap == NULL) {
//       /* selection failed */
//       return(-6);
//    }

   /* set the width and the height */
//    m_width = width;
//    m_height = height;

   /* set the default position */
//    m_dX = m_dY = 0;

   /* set the default attributes */
   updateCurrentPen();
   updateCurrentBrush();
   updateFillingBrush();

   /* create the null pen */
   m_nullPen = CreatePen(PS_NULL, 1, MS_RGB(0,0,0));

   /* create the null brush */
   m_nullBrush = (HBRUSH)GetStockObject( NULL_BRUSH );

   /* clear the buffer */
   clearScreen();

   /* setup the basic properties of the font */
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
   setFont(STRING("Lucida Console"), 12);

   m_backDC = 0;
   
   /* complete success */
   return(0);
}

/* allocates a new WinMapPlotter */
WinMapPlotter* WinMapPlotter::allocate(int width,int height)
{
   /* allocate a new object */
   WinMapPlotter* newObj = new WinMapPlotter;
   if(newObj == NULL) {
      /* memory not there */
      return(NULL);
   }

   /* do second-phase construction */
   if(newObj->construct(width, height) != 0) {
      /* nope, didnt happen */
      delete newObj;
      return(NULL);
   }

   return(newObj);
}

/* destructor */
WinMapPlotter::~WinMapPlotter()
{
   m_snapshotSurf.Release();
   m_cacheSurf.Release();
   
   /* clean up the resources */
   DeleteObject(m_nullBrush);
   DeleteObject(m_nullPen);
   if(m_pen) DeleteObject(m_pen);
   if(m_brush) DeleteObject(m_brush);
   if(m_fillBrush) DeleteObject(m_fillBrush);
   if(m_backFillingBrush) DeleteObject(m_backFillingBrush);

   for(penMap::iterator itPen = m_penCache.begin(); itPen != m_penCache.end(); itPen++)
   {
      DeleteObject((*itPen).second);
   }

   for(brushMap::iterator itBrush = m_brushCache.begin(); itBrush != m_brushCache.end(); itBrush++)
   {
      DeleteObject((*itBrush).second);
   }

   /* restore the old DC */
//   SelectObject(m_backDC, m_oldBitmap);
////   SelectObject(m_tempDC, m_oldtBitmap);
//   DeleteObject(m_backBuffer);
//   DeleteObject(m_tempBuffer);
   DeleteDC(m_backDC);

   SelectObject(m_screenSnapshot, m_oldScreenBitmap);
   DeleteObject(m_screenBitmap);
   DeleteDC(m_screenSnapshot);
}

/**
 *	 Sets static flag - map is not moving/zooming.
 */
void WinMapPlotter::setStatic(BOOL bStatic)
{
   m_bStatic = bStatic;
}

/**
 *	 Set map scale.
 */
void WinMapPlotter::setScale(double dScale)
{
   m_dScale = dScale;
}

/**
 *   Fills the screen with the background color.
 */
void WinMapPlotter::clearScreen()
{
   // if not locked, this two calls will do nothing
   unDC();
   m_ddraw->backSurf().unlock();

   // clear screen
   m_ddraw->backSurf().clear(m_backColor);
   return;
}

/**
 *   Sets the background color.
 *   <br />
 *   NB! The server will not send impossible values once the
 *   description formats are done.
 *   @param red   Red value   0-255.
 *   @param green Green value 0-255.
 *   @param blue  Blue value  0-255.
 */
void WinMapPlotter::setBackgroundColor( unsigned int red,
                                        unsigned int green,
                                        unsigned int blue )
{
   /* set the brush color */
   m_brushColor = MS_RGB(red, green, blue);
   m_backColor = MS_RGB(red, green, blue);
   /* update the brush */
   updateCurrentBrush();
   return;
}

/**
 *	  Set background filling color.
 */
void WinMapPlotter::setBackFillColor(COLORREF cBackColor)
{
   m_backColor = cBackColor;
   if(m_backFillingBrush)
      DeleteObject(m_backFillingBrush);
   m_backFillingBrush = CreateSolidBrush(m_backColor);
}

/**
 *   Sets the color of the pen.
 *   <br />
 *   NB! The server will not send impossible values once the
 *   description formats are done.
 *   @param red   Red value   0-255.
 *   @param green Green value 0-255.
 *   @param blue  Blue value  0-255.
 */
void WinMapPlotter::setPenColor( unsigned int red,
                                 unsigned int green,
                                 unsigned int blue )
{
   if (m_penColor != MS_RGB(red, green, blue)){
      /* set the text color */
#ifdef USE_GDI_DRAWING
      SetTextColor( DC(), MS_RGB(red,green,blue) );
      /* set the pen color */
      m_penColor = MS_RGB(red, green, blue);

#endif
      /* update the pen */
      updateCurrentPen();
   }
   return;
}

void WinMapPlotter::drawLine(int nSx, int nSy, int nEx, int nEy)
{
#ifdef USE_GDI_DRAWING
   /* move to the first point */
   MoveToEx(DC(), nSx, nSy, NULL);

   /* draw the line */
   LineTo(DC(), nEx, nEy);
#endif
   m_nPrimitivesDrawed++;
}


/**
 *   Sets the fill color for polygons.
 *   Default implementation sets the pen color.
 *   @see setPenColor.
 */
void WinMapPlotter::setFillColor( unsigned int red,
                                  unsigned int green,
                                  unsigned int blue )
{
   if (m_fillColor != MS_RGB(red,green,blue)){
      /* update the fill color */
      m_fillColor = MS_RGB(red,green,blue);
      /* update the filling brush */
      updateFillingBrush();
   }
}

/**
 *   Draws a polyline defined by the array of points.
 */
void WinMapPlotter::drawPolyLine( std::vector<MC2Point>::const_iterator begin,
                                  std::vector<MC2Point>::const_iterator end )
{

   if(m_penWidth > MIN_PEN_WIDTH_TO_DRAW_AS_POLYGON){
      int nPrevPenWidth;

      nPrevPenWidth = m_penWidth;
      setLineWidth(1);
      
      int size = std::distance( begin, end );

      // it seems if size <= 1 - this is not normal polygon and we can't paint it
      //
      // anyway, if size <= 1 - rest of fuction will crash thread
      if (size <= 1) return;

      // find normal for every segment
      std::vector<MC2Point> norms;
      int k;

      const MC2Point* apoints = &(*begin);
      
      for(k = 1; k<size; k++){
         MC2Point tnorm(0,0);
         tnorm.getX() = apoints[k].getY() - apoints[k-1].getY();
         tnorm.getX() = tnorm.getX() < 10000 ? tnorm.getX() : 10000;
         tnorm.getX() = tnorm.getX() > -10000 ? tnorm.getX() : -10000;
         tnorm.getY() = apoints[k-1].getX() - apoints[k].getX();
         tnorm.getY() = tnorm.getY()<10000?tnorm.getY():10000;
         tnorm.getY() = tnorm.getY()>-10000?tnorm.getY():-10000;
         double nhight = tnorm.getX()*tnorm.getX() + tnorm.getY()*tnorm.getY();
         if (nhight) {
            nhight = nPrevPenWidth/(sqrt(nhight)*2);
            tnorm.getX() *= nhight;
            tnorm.getY() *= nhight;
         }
         norms.push_back(tnorm);
      }

      // find vertexes
      int contoursize = size*4 - 4;
      std::vector<MC2Point> contour;// = new POINT[contoursize+1];
      MC2Point tpoint(0,0);
   
      tpoint.getX() = apoints[0].getX() + norms[0].getX();
      tpoint.getY() = apoints[0].getY() + norms[0].getY();
      contour.push_back(tpoint);

      for(k = 1; k<size-1; k++){
         tpoint.getX() = apoints[k].getX() + norms[k-1].getX();//[2*k-1]
         tpoint.getY() = apoints[k].getY() + norms[k-1].getY();
         contour.push_back(tpoint);
         tpoint.getX() = apoints[k].getX() + norms[k].getX();//[2*k]
         tpoint.getY() = apoints[k].getY() + norms[k].getY();
         contour.push_back(tpoint);
      }
   
      tpoint.getX() = apoints[size-1].getX() + norms[size-2].getX();//[2*size - 3]
      tpoint.getY() = apoints[size-1].getY() + norms[size-2].getY();
      contour.push_back(tpoint);

      tpoint.getX() = apoints[size-1].getX() - norms[size-2].getX();//[2*size - 2]
      tpoint.getY() = apoints[size-1].getY() - norms[size-2].getY();//[2*size - 2]
      contour.push_back(tpoint);

      for(k = size-2; k > 0; k--){
         tpoint.getX() = apoints[k].getX() - norms[k].getX();//[contoursize - 1 - 2*k]
         tpoint.getY() = apoints[k].getY() - norms[k].getY();
         contour.push_back(tpoint);
         tpoint.getX() = apoints[k].getX() - norms[k-1].getX();//[contoursize - 2*k]
         tpoint.getY() = apoints[k].getY() - norms[k-1].getY();
         contour.push_back(tpoint);
      }

      tpoint.getX() = apoints[0].getX() - norms[0].getX();//[contoursize - 1]
      tpoint.getY() = apoints[0].getY() - norms[0].getY();
      contour.push_back(tpoint);

      tpoint.getX() = contour[0].getX();//[contoursize]
      tpoint.getY() = contour[0].getY();
      contour.push_back(tpoint);

      const POINT* beginP = reinterpret_cast< const POINT* > (&contour[0]);
      
#ifdef USE_GDI_DRAWING
      Polygon(DC(), beginP, contoursize+1);
#endif
      m_nPrimitivesDrawed ++;

      setLineWidth(nPrevPenWidth);
      
      /* draw rounded caps on polyline ends */
#ifdef USE_GDI_DRAWING
      Ellipse( DC(), begin->getX(), begin->getY() + 1, begin->getX() , begin->getY() + 1);
#endif
      std::vector<MC2Point>::const_iterator pr = end;
      pr--;
#ifdef USE_GDI_DRAWING
      Ellipse( DC(), pr->getX(), pr->getY() + 1, pr->getX() , pr->getY() + 1);
#endif
      m_nPrimitivesDrawed += 2;
   }
   else {
#ifdef USE_GDI_DRAWING
      const POINT* beginP2 = reinterpret_cast< const POINT* > (&*begin);
      Polyline(DC(), beginP2, end - begin );
#endif
      m_nPrimitivesDrawed ++;
   }

   return;
}

int WinMapPlotter::getInflatedEllipseSize(int nBaseSize)
{
  /* switch(nBaseSize) {
      case 10: return 12; break;
      case 11: return 13; break;
      case 12: return 14; break;
      case 13: return 15; break;
      case 14: return 16; break;
      case 15: return 18; break;
      case 16: return 19; break;
      case 17: return 20; break;
      case 18: return 21; break;
      case 19: return 22; break;
      case 20: return 24; break;
      case 21: return 25; break;
      case 22: return 26; break;
      case 23: return 27; break;
      case 24: return 28; break;
      case 25: return 30; break;
      case 26: return 31; break;
      case 27: return 32; break;
      case 28: return 33; break;
      case 29: return 34; break;
      case 30: return 36; break;
      case 31: return 37; break;
      case 32: return 38; break;
      case 33: return 39; break;
      case 34: return 40; break;
      case 35: return 42; break;
      case 36: return 43; break;
      case 37: return 44; break;
      case 38: return 45; break;
      case 39: return 46; break;
      case 40: return 48; break;
      case 41: return 49; break;
      case 42: return 50; break;
      case 43: return 51; break;
      case 44: return 52; break;
      case 45: return 54; break;
      case 46: return 55; break;
      case 47: return 56; break;
      case 48: return 57; break;
      case 49: return 58; break;
      case 50: return 60; break;
      case 51: return 61; break;
      case 52: return 62; break;
      case 53: return 63; break;
      case 54: return 64; break;
      case 55: return 66; break;
      case 56: return 67; break;
      case 57: return 68; break;
      case 58: return 69; break;
      case 59: return 70; break;
      case 60: return 72; break;
      case 61: return 73; break;
      case 62: return 74; break;
      case 63: return 75; break;
      case 64: return 76; break;
      case 65: return 78; break;
      case 66: return 79; break;
      case 67: return 80; break;
      case 68: return 81; break;
      case 69: return 82; break;
      case 70: return 84; break;
      case 71: return 85; break;
      case 72: return 86; break;
      case 73: return 87; break;
      case 74: return 88; break;
      case 75: return 90; break;
      case 76: return 91; break;
      case 77: return 92; break;
      case 78: return 93; break;
      case 79: return 94; break;
      case 80: return 100; break;
      case 81: return 101; break;
      case 82: return 103; break;
      case 83: return 104; break;
      case 84: return 105; break;
      case 85: return 106; break;
      case 86: return 108; break;
      case 87: return 109; break;
      case 88: return 110; break;
      case 89: return 111; break;
      case 90: return 113; break;
      case 91: return 114; break;
      case 92: return 115; break;
      case 93: return 116; break;
      case 94: return 118; break;
      case 95: return 119; break;
      case 96: return 120; break;
      case 97: return 121; break;
      case 98: return 123; break;
      case 99: return 124; break;
      case 100: return 125; break;
   }*/

//   if (nBaseSize > 70) {
  //    return 250;
   //}
   return (int)(nBaseSize * ELLIPSE_SIZE_INFLATE_RATIO);
  // return (int)(nBaseSize * nBaseSize * 0.0006+1*nBaseSize+0.125);
}

void 
WinMapPlotter::drawPolyLineWithColor( std::vector<MC2Point>::const_iterator begin,
                                      std::vector<MC2Point>::const_iterator end,
                                      uint32 color,
                                      int lineWidth )
{
   // Please optimize.
   WinMapPlotter::setLineWidth( lineWidth );

//    
//    color = MS_RGB( uint8( color >> 16 ), uint8( color >>  8 ), uint8( color ));
// 
// 
//    BOOL bUpdatePen = FALSE;
//    if ( m_penWidth != lineWidth) {
//       bUpdatePen = TRUE;
//       m_penWidth = lineWidth;
//    }
//    if (m_penColor != color ) {
//       bUpdatePen = TRUE;
//       /* set the text color */
//       SetTextColor( DC(), color);
//       /* set the pen color */
//       m_penColor =  color; //MS_RGB(red, green, blue);     
//    } 
//    if (bUpdatePen)
//       /* update the pen */
//       updateCurrentPen();


   WinMapPlotter::setPenColor( uint8( color >> 16 ),
                               uint8( color >>  8 ),
                               uint8( color ) ); 

   WinMapPlotter::setFillColor( uint8( color >> 16 ),
                                uint8( color >>  8 ),
                                uint8( color ) ); 

   
   WinMapPlotter::drawPolyLine( begin, end );
   
}

/**
 *   Puts the dimensions of the displayed map in the
 *   variables.
 */
void WinMapPlotter::getMapSizePixels(Rectangle& size) const
{
   size = Rectangle(0, 0, m_width, m_height);
   return;
}

/**
 *   Creates a pointvector of size <code>size</code>.
 */
PointVector* WinMapPlotter::createPointVector(int size)
{
   PointArray* newArray = new PointArray(size);
   return(newArray);
}

/**
 *   Returns the pointvector to the MapPlotter.
 */
void WinMapPlotter::deletePointVector(PointVector* vect)
{
   PointArray* array = static_cast<PointArray*>(vect);
   vect = NULL;
   return;
}

// Functions that should be re-implemented. ---------------------->

// Text functions -------------------------------------------------

/**
 *   Creates a text string for use with the other text methods.
 *   Default implementation returns NULL.
 */
STRING* WinMapPlotter::createString(const char* text)
{
   STRING* newStr = new STRING(text);
   return(newStr);
}


/**
 *   Deletes a text string.
 *   Default implementation does nothing.
 */   
void WinMapPlotter::deleteString(STRING* text)
{
   delete text;
   text = NULL;
   return;
}

/**
 *   @return The number of characters in the string.
 */
int WinMapPlotter::getStringLength( const STRING& text )
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

/**
 *   Sets font. The description format in the server
 *   and the MapPlotter must know what needs to be put
 *   here.
 *   @param fontName Name of the font in client-specific format.
 *   @param size     Size of the font in UNIT.
 */
void WinMapPlotter::setFont( const STRING& fontName, int size )
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

   LogFile::Print("PLOTTER : Font Set : %s [%d]\n", fontName, size);

   return;
}

/**
 *   Returns the rotation step possible by the plotter.
 *   <br />Default implementation returns 0.
 *   @return The minimum number of degrees that characters can
 *           be rotated.
 */
int WinMapPlotter::getMinTextRotation()
{
   return(0);
}

int
WinMapPlotter::utf8ToUnicode( const STRING& text,
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
 *   Draws text on the display using the pen color and transparent
 *   background.
 *   @param text      The start of the text to draw.
 *   @param points    The center point of the text.
 *   @param startIdx  The first character to draw.
 *   @param nbrChars  The number of characters to draw.
 */
void WinMapPlotter::drawText( const STRING& text,
                              const MC2Point& point,
                              int startIdx,
                              int nbrChars,
                              float angle )
{
#ifdef USE_GDI_DRAWING
   // Extract the portion of it that we are interested in.
   wchar_t uniStr[MAX_STR_LEN];

   int uniLen = utf8ToUnicode( text, startIdx, nbrChars, uniStr, MAX_STR_LEN);

   /* set the background drawing mode to transparent */
   int nOldMode = SetBkMode(DC(), TRANSPARENT);

   /* set text alignment */
   // Note that the specified point is the point of the center, 
   // but the text will be drawn at the baseline.
   // Need to compensate for this.
   int nOldTextAlign = SetTextAlign(DC(), TA_CENTER | TA_BASELINE | TA_NOUPDATECP);

   /* axis adjust deltas */
   int xDelta = 0, yDelta = 0;

   /* select our font */
   HFONT newFont = createFont(angle);
   if(newFont == NULL) return;
   /* select the new font into our DC */
   HFONT oldFont = (HFONT)SelectObject(DC(), newFont);
   
   SIZE strSize;
   GetTextExtentPoint32W(DC(), uniStr, uniLen, &strSize);

   // Assume that the difference from baseline to center is roughly
   // the height / 4.
   int delta = strSize.cy / 4;
  
   // Compensate for that the text will be drawn at the baseline and
   // not at the center.
   float radAngle = angle / 180 * M_PI;
   xDelta = int( delta * sin( radAngle ) );
   yDelta = int( delta * cos( radAngle ) );

   
   /* draw the text */
   ExtTextOutW(DC(), 
            point.getX() + xDelta, point.getY() + yDelta,
            ETO_OPAQUE, NULL,
            uniStr, uniLen, NULL);
   
/* delete our font */
   SelectObject(DC(), oldFont);
   DeleteObject(newFont);

   /* restore the background drawing mode to opaque */
   SetBkMode(DC(), nOldMode);
   SetTextAlign(DC(), nOldTextAlign);

#if 0
   // Debug code
   setLineWidth( 4 );
   std::vector<MC2Point> pointVec;
   pointVec.push_back( point );
   setPenColor( 0, 0, 0 );
   drawPolyLine( pointVec.begin(), pointVec.end() );
   MC2Point otherPoint( point.getX() + xDelta, point.getY() + yDelta );
   pointVec.clear();
   pointVec.push_back( otherPoint );
   setPenColor( 0, 255, 0 );
   drawPolyLine( pointVec.begin(), pointVec.end() );
#endif  
   m_nPrimitivesDrawed++;
#endif
   return;
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
isab::Rectangle WinMapPlotter::getStringAsRectangle( const STRING& text,
                                                     const MC2Point& point,
                                                     int startIdx,
                                                     int nbrChars,
                                                     float angle )
{

   // Extract the portion of it that we are interested in.
   wchar_t uniStr[MAX_STR_LEN];

   int uniLen = utf8ToUnicode( text, startIdx, nbrChars, uniStr, MAX_STR_LEN);

   HFONT newFont = createFont((int)angle);
   if(newFont == NULL) return 0;
   /* select the new font into our DC */
   HFONT oldFont = (HFONT)SelectObject(DC(), newFont);

   /* calculate the size of the string */
   SIZE strSize;
#ifdef USE_GDI_DRAWING
   GetTextExtentPoint32W(DC(), uniStr, uniLen, &strSize);
#endif

   SelectObject(DC(), oldFont);
   DeleteObject(newFont);

   /* the rectangle's dimensions are based on the center point and the 
      font height and the total string width */
   int rectX = point.getX() - (strSize.cx >> 1);
   int rectY = point.getY() - (strSize.cy >> 1);

   Rectangle rect;
   
   /* zero angle specified */
   if(angle == 0.0f) {
      /* return the new rectangle */
      rect = Rectangle(rectX, rectY, strSize.cx, strSize.cy);
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
int WinMapPlotter::getStringAsRectangles( std::vector<Rectangle>& boxes,
                                          const STRING& text,
                                          const MC2Point& point,
                                          int startIdx,
                                          int nbrChars,
                                          float angle )
{
#ifdef USE_GDI_DRAWING
   
   // Extract the portion of it that we are interested in.
   wchar_t uniStr[MAX_STR_LEN];

   int uniLen = utf8ToUnicode( text, startIdx, nbrChars, uniStr, MAX_STR_LEN);
   
   /* select our font */
   HFONT newFont = createFont((int)angle);
   if(newFont == NULL) return 0;
   /* select the new font into our DC */
   HFONT oldFont = (HFONT)SelectObject(DC(), newFont);
   
   /* calculate the size of the string */
   SIZE strSize;
   GetTextExtentPoint32W(DC(), uniStr, uniLen, &strSize);

   /* delete our font */
   SelectObject(DC(), oldFont);
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
   std::vector<MC2Point> origArray;

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

      //         Rectangle rotBox = rotatedRect(origArray[curOrig].getX(),
      //                                        origArray[curOrig].getY(),
      //                                        origArray[curOrig].getX()+textW,
      //                                        origArray[curOrig].getY()+textH, 
      //                                        angle);
      Rectangle rotBox = Rectangle(origArray[curOrig].getX() - textW/2,
            origArray[curOrig].getY() - textH/2,
            textW,
            textH );

      boxes.push_back(rotBox);
      curOrig++;
   }
   /* clear origin vector */
   origArray.clear();

   /* returns the number of characters processed */
#endif
   return(nbrChars);
}

// Misc ----------------------------------------------------------

/**
 *   Returns true if the drawing is double buffered.
 *   Default implementation returns false.
 *   @return True if the drawing is double buffered.
 */
bool WinMapPlotter::doubleBuffered() const
{
   return(true);
}

/**
 *   Tells the MapPlotter that it is ok to change buffer
 *   if it is doubleBuffered. Default implementation does
 *   nothing.
 *   @return doubleBuffered().
 */
bool WinMapPlotter::drawBuffer( const Rectangle& rect )
{
   return(true);
}

HDC WinMapPlotter::DC(){

   if (!m_backDC){
      if (m_ddraw){
         return m_backDC = m_ddraw->backSurf().dc();
      }
   }else{
      return m_backDC;
   }
   return NULL;
}

void WinMapPlotter::unDC() {
   if (m_backDC) {
      m_ddraw->backSurf().unlock();
      m_backDC = 0;
   }
}

void WinMapPlotter::endFrame()
{
   unDC();
}

/**
 *   If the MapPlotter needs to prepare the plotting in
 *   any way it should do it in this method which is called
 *   before drawing.
 *   <br />
 *   The default implementation does not do anything.
 */
void WinMapPlotter::prepareDrawing()
{
   unDC(); // ensure will take new dc
#ifdef OVERALL_TIMINGS
   FILE *f = fopen("/overall.txt", "at");
   if(f){
      fprintf(f, "drawing begins %d\n", GetTickCount());
      fclose(f);
   }
#endif

   m_nCountRedraw++;
   m_nTime = GetTickCount();
   m_nPrimitivesDrawed = 0;
   return;
}

/**
 *   Must be called by the drawing engine everytime a drawing
 *   is completed. This is to be able to free up the resources
 *   that may have been used during the draw.
 *   <br />
 *   The default implementation does not do anything.
 */
void WinMapPlotter::triggerApplicationRedraw() {
   if ( NULL == m_pWinPaintCallback) return;

#ifdef DRAWING_COMPLETE_TIMINGS
   DWORD started = GetTickCount();
#endif   
   unDC();

#ifdef DRAWING_COMPLETE_TIMINGS
   DWORD undced = GetTickCount();
#endif   
   m_afterBlitNotOnBackBuffer = true;

   //blitTo(NULL, 0, 0); // dc is now ignored


#ifdef DRAWING_COMPLETE_TIMINGS
   DWORD complete = GetTickCount();

   FILE *f = fopen("/drawingcomlete_timings.txt", "at");
   if (f){

      fprintf(f, "====\nstarting: %d\n undced: %d\ncomplete: \n", started, undced, complete);
      fprintf(f, "  overall: %d\n", complete - started);      
      fprintf(f, "starting-undced: %d\n", undced-started );
      fprintf(f, "undced-complete: %d\n", complete-undced );
   
      fclose(f);
   }

#endif

#ifdef OVERALL_TIMINGS
   FILE *f = fopen("/overall.txt", "at");
   if(f){
      fprintf(f, "drawing ended %d\n", GetTickCount());
      fclose(f);
   }
#endif


   m_bStatic = TRUE;

  


}

/**
 *   Converts the bytes into a bitmap that can be drawn by the
 *   MapPlotter.
 *   Default implementation returns NULL.
 *   Should we add the type of image too, e.g. PNG etc.?
 */
BitMap* WinMapPlotter::createBitMap(bitMapType type,
                                const uint8* bytes,
                                int nbrBytes,
                                int dpiCorrectionFactor)
{
   /* create the bitmap */
   WinBitmap* newObj = WinBitmap::allocate(bytes, nbrBytes, dpiCorrectionFactor);
   if(newObj == NULL) return(NULL);

   /* return the newly created object */
   return(newObj);
}


/**
 *   Draws the bitmap at x and y.
 *   Default implementation draws a small rectangle.
 */
void WinMapPlotter::drawBitMap( const MC2Point& center,
                                const BitMap* bitMap)
{
   /* get our platform specific bitmap */
   const WinBitmap* winBmp = static_cast<const WinBitmap*>(bitMap);
   /* draws the bitmap centred around the specified point */
   int bX = center.getX() - (winBmp->width() >> 1);
   int bY = center.getY() - (winBmp->height() >> 1);
   /* do the blit depending on bitmap type */
   if(winBmp->isMasked()) {   /* masked BMP */
      
      PixelBox imgVisibleBox = winBmp->GetVisibleRect(); 
      MC2Point topLeftCoord = imgVisibleBox.getTopLeft();

#ifdef USE_GDI_DRAWING
      MaskBlt(DC(), center.getX(), center.getY() - imgVisibleBox.getHeight(),
              imgVisibleBox.getWidth(), imgVisibleBox.getHeight(),
              winBmp->getDC(),
              topLeftCoord.getX(),topLeftCoord.getY(),
              winBmp->getMask(),
              topLeftCoord.getX(), topLeftCoord.getY(),
              MAKEROP4(SRCCOPY,0x00AA0029));

#endif
   }
   else {                     /* normal BMP */
#ifdef USE_GDI_DRAWING
      BitBlt(DC(), bX, bY, 
             winBmp->width(), winBmp->height(),
             winBmp->getDC(),
             0, 0,
             MAKEROP4(SRCCOPY,0));

#endif
   }
   m_nPrimitivesDrawed++;
   return;
}

/**
 *   Gets the size of the specified BitMap in pixels
 *   Has to be implemented in derived class.
 *   @param size Rectangle which the bitmap is inside.
 *   @param bmp  Bitmap to get the size from.
 *   @param centerXY The center of the bitmap (position where it is drawn).
 */
void WinMapPlotter::getBitMapAsRectangle(Rectangle& size,
                                         const MC2Point& origCenter,
                                         const BitMap* bmp)
{
   /* get our platform specific bitmap */
   const WinBitmap* winBmp = static_cast<const WinBitmap*>(bmp);
   /* draws the bitmap centred around the specified point */
   int bX = origCenter.getX() - (winBmp->width() >> 1);
   int bY = origCenter.getY() - (winBmp->height() >> 1);

   PixelBox imgBox = Rectangle(0, 0, winBmp->width(), winBmp->height()); 

   if (winBmp->isMasked()) {
      PixelBox imgVisibleBox = winBmp->GetVisibleRect(); 
      //Get the correct alignment of the visible part of the image.
      imgVisibleBox.moveTopLeftTo(MC2Point(origCenter.getX(), origCenter.getY() - imgVisibleBox.getHeight()));
      size = imgVisibleBox; 
   } else {
      /* set the Rectangle */
      size = Rectangle(bX, bY, winBmp->width(), winBmp->height());
   }

   return;
}

/**
 *   Deletes the supplied bitmap.
 */
void WinMapPlotter::deleteBitMap( BitMap* bitMap )
{
   WinBitmap* winBmp = static_cast<WinBitmap*>(bitMap);
   delete winBmp;
   winBmp = NULL;
   return;
}

/**
 *   Returns the wanted bitmap extension.
 *   Default implementation returns png. Dot will be appended
 *   by TileMapHandler.
 */
const char* WinMapPlotter::getBitMapExtension() const
{
   return( "tga" );
}

/**
 *   Draws a polygon filled with the fill color and with a border
 *   with set with setPenColor and linewith as set by setLineWidth
 *   and dashstyle set by setDashStyle.
 *   If borderColor == penColor or lineWidth == 0 no border should
 *   be drawn.
 *   <br />
 *   Default implementation draws a polyline using the fill color.
 */
void WinMapPlotter::drawPolygon( std::vector<MC2Point>::const_iterator begin,
                                 std::vector<MC2Point>::const_iterator end )
{
   if (begin == end){
      // No need to draw anything!
      return;
   }

#ifdef USE_GDI_DRAWING
   SelectObject(DC(), m_nullPen);
   const POINT* beginP = reinterpret_cast< const POINT* > (&begin[0]);
   Polygon(DC(), beginP, end - begin);
   SelectObject(DC(), m_pen);
#endif
   m_nPrimitivesDrawed++;

   return;
}

void WinMapPlotter::
drawPolygonWithColor( std::vector<MC2Point>::const_iterator begin,
                      std::vector<MC2Point>::const_iterator end,
                      uint32 fillColor )
{
   // Please optimize.
   WinMapPlotter::setFillColor( uint8( fillColor >> 16 ),
                                uint8( fillColor >>  8 ),
                                uint8( fillColor ) );

   WinMapPlotter::drawPolygon( begin, end );
}


/**
 *   Draws a spline of some sort.
 *   <br />
 *   Default implementation draws a polyline if not filled else polygon.
 */
void WinMapPlotter::drawSpline( bool filled,
                                std::vector<MC2Point>::const_iterator begin,
                                std::vector<MC2Point>::const_iterator end )
{
   return;
}

/**
 *   Same as drawPolygons, but the polygons must be convex. Can be
 *   implemented to speed things up.
 *   <br />
 *   Default implementation calls drawPolygons.
 */
void WinMapPlotter::drawConvexPolygon( std::vector<MC2Point>::const_iterator begin,
                                       std::vector<MC2Point>::const_iterator end )
{
   return;
}

/**
 *   Draws an arc.
 *   Default implementation draws a line from one corner to the other
 *   in the rectangle.
 */
void WinMapPlotter::drawArc( bool filled,
                             const Rectangle& rect,
                             int startAngle,
                             int stopAngle)
{

   return;
}

/**
 *   Draws a rectangle.
 *   @param filled     If true the rectangle is filled.
 *   @param rect       Dimensions of the rectangle.
 *   @param cornerSize Radius of corners.
 *   Default implementation cannot draw rounded corners and uses
 *   drawPolyLine or drawConvexPolygon.
 */
void WinMapPlotter::drawRect( bool filled,
                              const Rectangle& rect,
                              int cornerSize)
{
   /* create the Rectangle */
   RECT winRect;
   winRect.left = rect.getX();
   winRect.top = rect.getY();
   winRect.right = rect.getX() + rect.getWidth();
   winRect.bottom = rect.getY() + rect.getHeight();

   /* check if rectangle is filled */
#ifdef USE_GDI_DRAWING
   if(filled) {
      SelectObject(DC(), m_nullPen);
      FillRect(DC(), &winRect, m_fillBrush);
      SelectObject(DC(), m_pen);
   }
   else {
      
      HBRUSH oldBrush = (HBRUSH)SelectObject(DC(), m_nullBrush);
      /* draw the outline */
      ::Rectangle(DC(), 
                  winRect.left, winRect.top,
                  winRect.right, winRect.bottom);
      SelectObject(DC(), oldBrush );
   }
#endif
   m_nPrimitivesDrawed++;

   return;
}

/**
 *   Sets the width of the lines.
 *   Default implementation does nothing.
 */
void WinMapPlotter::setLineWidth( int width )
{
   if (m_penWidth != width){
      /* set the new width */
      m_penWidth = width;
      /* update our pen */
      updateCurrentPen();
   }
}

/**
 *   Sets the cap style of the pen.
 *   Default implementation does nothing.
 */
void WinMapPlotter::setCapStyle( enum capStyle )
{
   return;
}

/**
 *   Sets the dash style of the pen.
 *   Default implementation does nothing.
 */
void WinMapPlotter::setDashStyle( enum dashStyle style )
{
   switch(style)
   {
      case(nullDash): m_penStyle = PS_NULL; break;
      case(solidDash): m_penStyle = PS_SOLID; break;
      case(dottedDash): m_penStyle = PS_DASH; break;
      case(dashedDash): m_penStyle = PS_DASH; break;
      default: m_penStyle = PS_NULL; break;
   }
   return;
}

/**
 *   Puts the dimensions of the displayed map in the
 *   variables. Must be overridden if the drawing units
 *   of the map is not pixels.
 *   <br />
 *   Default implementation returns getMapSizePixels.
 */
void WinMapPlotter::getMapSizeDrawingUnits(Rectangle& size) const
{
   getMapSizePixels(size);
   return;
}

// -- Windows Specific Methods

/* creates a pen using the current pen parameters, deletes the
   old pen if required */
void WinMapPlotter::updateCurrentPen()
{
   PenAttribs tPenAttr(m_penStyle, m_penWidth, m_penColor);

   penMap::iterator fMap;
   fMap = m_penCache.find(tPenAttr);

   if(fMap == m_penCache.end()){
      /* allocate a new HPEN using the current parameters */
      m_pen = CreatePen(m_penStyle, m_penWidth, m_penColor);

	  penMapVal newVal;
	  newVal.first = tPenAttr;
	  newVal.second = m_pen;
	  m_penCache.insert(newVal);
   }
   else{
      m_pen = (*fMap).second;
   }

   /* set to the DC if valid */
   if(m_pen) {
#ifdef USE_GDI_DRAWING
      SelectObject(DC(), m_pen);
#endif
   }

   /* return */
   return;
}

/* creates a brush using the current brush parameters, deletes the
   old brush if required */
void WinMapPlotter::updateCurrentBrush()
{
   brushMap::iterator fMap;
   fMap = m_brushCache.find(m_brushColor);

   if(fMap == m_brushCache.end()){
      /* allocate a new HBRUSH using the current parameters */
      m_brush = CreateSolidBrush(m_brushColor);

	  brushMapVal newVal;
	  newVal.first = m_brushColor;
	  newVal.second = m_brush;
	  m_brushCache.insert(newVal);

	  m_nPenCached++;
   }
   else{
      m_brush = (*fMap).second;
   }
   
   if(m_brush) {
#ifdef USE_GDI_DRAWING
      SelectObject(DC(), m_brush);
#endif
   }

   /* return */
   return;
}

/* creates a fill brush using the current brush parameters, 
   deletes the old brush if required */
void WinMapPlotter::updateFillingBrush()
{
   brushMap::iterator fMap;
   fMap = m_brushCache.find(m_fillColor);

   if(fMap == m_brushCache.end()){
      /* allocate a new HBRUSH using the current parameters */
      m_fillBrush = CreateSolidBrush(m_fillColor);

	  brushMapVal newVal;
	  newVal.first = m_fillColor;
	  newVal.second = m_fillBrush;
	  m_brushCache.insert(newVal);

	  m_nPenCached++;
   }
   else{
      m_fillBrush = (*fMap).second;
   }

   /* set to the DC if valid */
   if(m_fillBrush) {
#ifdef USE_GDI_DRAWING
      SelectObject(DC(), m_fillBrush);
#endif
   }

   return;
}

/* blits the internal back buffer to the specified DC */
void WinMapPlotter::blitTo(HDC destDC, int32 dX, int32 dY)
{
#ifdef BLITTO_TIMINGS
   FILE *f = fopen("/blitto.txt", "at");
   fprintf(f, "====\n");
#endif

   HRESULT hres = DD_OK;
		
   // destDC seems to be the screen
   // BackDC is the last map made, in its own DC in order to not be tainted by non map spesific shiz
   // TempDC is what ends up on the screen, backbuffer + cursor and buttons.
#ifdef BLITTO_TIMINGS
   fprintf(f, "started: %d\n", GetTickCount());
#endif   

      /* draw a frame if required */
      if((m_xform) && (m_frameRect)) {
         /* get the points from coords */
         MC2Point topLeft(0,0), bottomRight(0,0);

         /* get top-left corner */
         m_xform->transformPointUsingCosLat(topLeft.getX(), topLeft.getY(),
                                            MC2Coordinate(m_frameRect->getMaxLat(), 
                                                          m_frameRect->getMinLon()));
         /* get bottom-right corner */
         m_xform->transformPointUsingCosLat(bottomRight.getX(), bottomRight.getY(),
                                            MC2Coordinate(m_frameRect->getMinLat(), 
                                                          m_frameRect->getMaxLon()));

         /* draw the frame */
         RECT fRect;
         fRect.left   = topLeft.getX();
         fRect.top    = topLeft.getY();
         fRect.right  = bottomRight.getX();
         fRect.bottom = bottomRight.getY();

#ifdef USE_GDI_DRAWING
      drawFrame(DC(), &fRect,
                   m_frameWidth,
                   m_frameRed, m_frameGreen, m_frameBlue);

#endif
      }
#ifdef BLITTO_TIMINGS
   fprintf(f, "drawed frame: %d\n", GetTickCount());
#endif

      /* call listener */
      if ( NULL != m_pWinPaintCallback  ) {
      unDC();
#ifdef BLITTO_TIMINGS
      fprintf(f, "undc done: %d\n", GetTickCount());
#endif         
      m_pWinPaintCallback->beforeBlit( NULL ); 
#ifdef BLITTO_TIMINGS
      fprintf(f, "before blit complete: %d\n", GetTickCount());
#endif      
      if (m_pWinPaintCallback->needToDrawCursor()) { 

         m_cacheSurf.doBlit(m_ddraw->backSurf());
#ifdef BLITTO_TIMINGS
         fprintf(f, "cached: %d\n", GetTickCount());
#endif
         m_ddraw->flip();
#ifdef BLITTO_TIMINGS
         fprintf(f, "flipped: %d\n", GetTickCount());
#endif
         m_pWinPaintCallback->afterBlit(NULL);
#ifdef BLITTO_TIMINGS         
         fprintf(f, "afterblit done: %d\n", GetTickCount());
#endif
         unDC();
#ifdef BLITTO_TIMINGS
         fprintf(f, "undc done: %d\n", GetTickCount());
#endif
         m_ddraw->backSurf().doBlit(m_cacheSurf);
#ifdef BLITTO_TIMINGS
         fprintf(f, "uncached: %d\n", GetTickCount());
#endif
         } else {
            // q-alo: fixing 2029
            // here can be situation, when afterBlit of m_pWinPaintCallback
            // paint different things on one back buffer
            // so we need to call afterBlit only when not called after last painting

         m_ddraw->flip();
#ifdef BLITTO_TIMINGS
         fprintf(f, "flipped: %d\n", GetTickCount());
#endif
            if(m_afterBlitNotOnBackBuffer){
					
            m_pWinPaintCallback->afterBlit(NULL);
#ifdef BLITTO_TIMINGS            
            fprintf(f, "afterblit done: %d\n", GetTickCount());
#endif               
               m_afterBlitNotOnBackBuffer = false;
            }
         }
      }

      /* draw the drag rectangle if required */
      if(m_dragRect) {
         /* draw the non-filled rectangle */
      //drawFrame(destDC, m_dragRect, 1, 0, 0, 0);
      }
#ifdef BLITTO_TIMINGS
   fclose(f);
#endif
   return;

   }

/* resizes the plotter to the specified size */
/* Returns true on success, false on error */
bool WinMapPlotter::resize(int32 newWidth, int32 newHeight)
{
   unDC();

   /* set the width and the height */
   m_width = newWidth;
   m_height = newHeight;

   if (m_snapshotSurf) m_snapshotSurf.Release();
   if (m_cacheSurf) m_cacheSurf.Release();

   m_ddraw->recreateBackBuffer(m_width, m_height);
   //   m_ddraw->backSurf().clear(m_backColor); // clear backbuffer to default back color

   m_ddraw->createSurface(m_snapshotSurf, newWidth, newHeight);
   m_ddraw->createSurface(m_cacheSurf, newWidth, newHeight);

   /* clear screen */
   clearScreen();

   /* success */
   return(true);
}

/* creates a Win32 specific font - supports rotated fonts */
HFONT WinMapPlotter::createFont(float angle)
{
   HFONT newFont = NULL;

   /* add the font angle to the font data */
   m_fontData.lfEscapement = m_fontData.lfOrientation = int(angle*10.0f);

   /* try to create the font */
   newFont = ::CreateFontIndirect(&m_fontData);

   /* success, return the created font */
   return(newFont);
}

/* gets the rotated bounding rectangle for the specified angle and points */
isab::Rectangle WinMapPlotter::rotatedRect(int x1, int y1, int x2, int y2, 
                                           float angle)
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
   rotate2D(-halfW, -halfH, angle, (int*)&rotVerts[0].x, (int*)&rotVerts[0].y, cx, cx);
   rotate2D( halfW, -halfH, angle, (int*)&rotVerts[1].x, (int*)&rotVerts[1].y, cx, cx);
   rotate2D( halfW,  halfH, angle, (int*)&rotVerts[2].x, (int*)&rotVerts[2].y, cx, cx);
   rotate2D(-halfW,  halfH, angle, (int*)&rotVerts[3].x, (int*)&rotVerts[3].y, cx, cx);

   /* get the minimum and maximum bound points */
   minBound.x = min(rotVerts[0].x, min(min(rotVerts[1].x,rotVerts[2].x), rotVerts[3].x));
   minBound.y = min(rotVerts[0].y, min(min(rotVerts[1].y,rotVerts[2].y), rotVerts[3].y));
   maxBound.x = max(rotVerts[0].x, max(max(rotVerts[1].x,rotVerts[2].x), rotVerts[3].x));
   maxBound.y = max(rotVerts[0].y, max(max(rotVerts[1].y,rotVerts[2].y), rotVerts[3].y));

   /* get the dimensions of the rectangle */
   int w = maxBound.x - minBound.x;
   int h = maxBound.y - minBound.y;

   /* return the rotated rectangle */
   return( Rectangle(minBound.x, minBound.y, w, h) );
}

/* Draws a frame on the map. It uses a Win32 RECT and the specified 
   frame width and color */
void WinMapPlotter::drawFrame(HDC dc,
                              LPRECT frame, 
                              int fWidth,
                              int r, int g, int b)
{
   /* set the color and width */
   HPEN newPen = CreatePen(PS_SOLID,
                           fWidth,
                           MS_RGB(r,g,b));
   HPEN oldPen = (HPEN)SelectObject(dc, newPen);
   /* set a null-brush, and save the old brush in use */
   HBRUSH oldBrush = (HBRUSH)SelectObject(dc, m_nullBrush);
   /* draw the rectangle */
   ::Rectangle(dc, 
               frame->left, frame->top,
               frame->right, frame->bottom);
   /* restore the old brush */
   SelectObject(dc, oldBrush);
   /* restore old pen */
   SelectObject(dc, oldPen);

   ::DeleteObject(newPen);

   m_nPrimitivesDrawed++;
   return;
}

/* Sets a drag rectangle to be drawn on the target DC after 
   blitting the backbuffer to it.
   Pass NULL to disable the drag rectangle */
void WinMapPlotter::setDragRect(LPRECT dragPtr)
{
   m_dragRect = dragPtr;
   return;
}


/* Sets a frame to be drawn on the map. 
   Uses given TransformMatrix to do the MC2Coord to 
   screen-point translation. Uses given width and color.
   Pass NULL to disable the frame rectangle */
void WinMapPlotter::setFrameRect(TransformMatrix* tMatrix,
                                 MC2BoundingBox* frame,
                                 int fWidth,
                                 int r, int g, int b)
{
   m_xform = tMatrix;
   m_frameRect = frame;
   m_frameWidth = fWidth;
   m_frameRed = r;
   m_frameGreen = g;
   m_frameBlue = b;
   return;
}

void WinMapPlotter::setWinPaintCallback( WinPaintCallback* pWinPaintCallback )
{
   ASSERT(m_ddraw);
   m_ddraw->SetOwner(pWinPaintCallback->getHWND());
   m_pWinPaintCallback = pWinPaintCallback;
}

void WinMapPlotter::makeScreenSnapshot()
{
#ifdef SNAPSHOT_MOVING_TIMINGS
   FILE *f = fopen("/movesnapshot.txt", "at");
   
   if(f){
      fprintf(f, " ----- creating shapshot \n");
      fclose(f);
   }
#endif
   
   unDC(); // ensure DD not locked
   //m_ddraw->CacheBackBuffer(m_snapshotSurf);
   m_snapshotSurf.doBlit(m_ddraw->backSurf());
}

void WinMapPlotter::moveScreenSnapshot(const MC2Point& deltaPoint)
{
   // no shift if no delta
   if ((deltaPoint.getX() == 0)
       && (deltaPoint.getY() == 0)) return;
   
#ifdef SNAPSHOT_MOVING_TIMINGS
   DWORD times = GetTickCount();
#endif   
   clearScreen();
   // clear screen leaves DD in DD mode, so no unlocking needed

   m_ddraw->backSurf().doBlit(m_snapshotSurf, deltaPoint.getX(), deltaPoint.getY());

#ifdef SNAPSHOT_MOVING_TIMINGS
   FILE *f = fopen("/movesnapshot.txt", "at");
   
   if(f){
      fprintf(f, "moving to: \t %dx%d\t blitting time \t%d\n", deltaPoint.getX(), deltaPoint.getY(), GetTickCount() - times);
      fclose(f);
   }
#endif
}

void WinMapPlotter::zoomScreenSnapshot(double factor, const MC2Point& zoomPoint)
{
   // Calculate zoomed size of screen snapshot.
   int nNewWidth  = m_width / factor;
   int nNewHeight = m_height / factor;

   // Calculate helper information.
   int nDeltaX = zoomPoint.getX() - m_width/2;
   int nDeltaY = zoomPoint.getY() - m_height/2;
   int nDeltaXZoomed = nDeltaX / factor;
   int nDeltaYZoomed = nDeltaY / factor;

   // Calculate shift for zooming of screen snapshot.
   int nShiftX = nDeltaXZoomed - nDeltaX;
   int nShiftY = nDeltaYZoomed - nDeltaY;

   clearScreen();
   unDC();

   m_ddraw->backSurf().doStretchBlit(m_snapshotSurf,
                                     m_width/2 - nNewWidth / 2 - nShiftX,
                                     m_height/2 - nNewHeight / 2 - nShiftY, 
                                     nNewWidth, nNewHeight);

}

bool WinMapPlotter::snapshotHandlingImplemented() const
{
   return true;
}

PixelBox WinMapPlotter:: getMapRect() const
{
   return PixelBox(MC2Point(0,0),MC2Point(m_width,m_height));
}

PenAttribs& PenAttribs::operator=(const PenAttribs& a){
	Style = a.Style;
	Width = a.Width;
	Color = a.Color;
	return (*this);
}

int operator<(const PenAttribs& a,const PenAttribs& b){
   if(a.Style != b.Style) return a.Style < b.Style;
   if(a.Width != b.Width) return a.Width < b.Width;
   return a.Color < b.Color;
}

int operator==(const PenAttribs& a,const PenAttribs& b){
   return ((a.Style == b.Style) && (a.Width == b.Width) && (a.Color == b.Color));
}


void WinMapPlotter::cropString(STRING& text,
                                     unsigned int maxWidth)
{
   //TODO: Implement
}

void WinMapPlotter::drawOverlayText(
   const WFString& text,
   TextCachingBlock& cacheBlock,
   const OverlayItemVisualSpec::TextAlignment alignment,
   FontSpec* font,
   const Rectangle& stringRect,
   const OverlayItemVisualSpec::RGB& textColor)
{
   //TODO: Implement
}

Font* WinMapPlotter::createFont(FontSpec* fontSpec)
{
   //TODO: Implement
   return NULL;
}

void WinMapPlotter::deleteFont(Font* font)
{
   //TODO: Implement
}

void WinMapPlotter::setFontStyle(Font* font)
{
   //TODO: Implement
}

void WinMapPlotter::resetFontStyle()
{
   //TODO: Implement
}

void WinMapPlotter::drawImageSpec(const WFAPI::ImageSpec* imgSpec,
                                  const MC2Point& topLeftPos)
{
   //TODO: Implement
}


}; /* namespace isab */



