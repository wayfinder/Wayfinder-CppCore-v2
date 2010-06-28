/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _SYMBIANNATIVETEXTINTERFACE_H_
#define _SYMBIANNATIVETEXTINTERFACE_H_

#include "NativeTextInterface.h"
#include "BitMapTextLoader.h"
#include "FontNotice.h"
#include <vector>
#include <map>

/* forward declarations */
class CFbsFont;
// class CCoeControl;
// class CMdaServer;
// class ImageConverter;
class CCoeControl;
class CSurface;
// class CWindowGc;
// class CFbsBitGc;


class SymbianNativeTextInterface : public NativeTextInterface,
                                   public BitMapTextLoader {

public:

   SymbianNativeTextInterface( CCoeControl& parent,
                               TInt aColorDepth );
   
   /**
    *   @see NativeTextInterface::getStringAsRectangles
    */

   virtual int getStringAsRectangles( std::vector<isab::Rectangle>& boxes,
                                      const STRING& text,
                                      const MC2Point& center,
                                      int startIdx,
                                      int nbrChars,
                                      float angle );
   
   /**
    *   @see NativeTextInterface::getStringAsRectangle
    */
   
   virtual isab::Rectangle getStringAsRectangle( const STRING& text,
                                                 const MC2Point& point,
                                                 int startIdx =  0,
                                                 int nbrChars = -1,
                                                 float angle  = 0.0 );

   /**
    *   @see NativeTextInterface::setFont
    */
   
   virtual void setFont( const STRING& fontName, int size );

   /**
    *   @see NativeTextInterface::getStringLength
    */
   
   virtual int getStringLength( const STRING& text );

   /**
    *   @see NativeTextInterface::createString
    */

   virtual STRING* createString( const char* text );

   /**
    *   @see NativeTextInterface::deleteString
    */

   virtual void deleteString( STRING* text );
   
   /**
    *   @see BitMapLoader::load
    */

   
   virtual int load( RGBA32BitMap& bitMap );

   /**
    *   Creates a platform specific font using the specification fontSpec.
    */ 
   virtual isab::Font* createFont(WFAPI::FontSpec* fontSpec);

   /**
    *   Deallocates a platform specific font.
    */ 
   virtual void deleteFont(isab::Font* font) ;
private:

   // CSurface* m_buffers[c_nbrDrawBuffers];
     
   // /// The snapshot buffer.
   // CSurface* m_screenSnapshot;
      
   // /* stores color information */
   // TRgb m_backColor;   /* Wanted background color */
   // TRgb m_penColor;   /* Wanted foreground (Drawing) color */
   // TRgb m_fillColor;   /* Wanted set fill color */
   // /// Foreground color for the mask
   // TRgb m_maskForeColor;
   // /// Background color for the mask
   // TRgb m_maskBackColor;
   // /// Last set foreground color
   // TRgb m_lastPenColor;
   // /// Last set fill color
   // TRgb m_lastFillColor;
   CFbsFont* iCurFont;   /* current font */
   CSurface* iFontQueryBuffer;
   
   TInt iColorDepth;
   CCoeControl& iParent;
   
   typedef std::map<FontNotice, CFbsFont*> fontCache_t;
   fontCache_t m_fontCache;
   
   // TInt iCurSize;      /* current line drawing size */
   // TInt iCapStyle;      /* stores the type of line ending to use */
   // TInt iDashStyle;   /* stores the type of line drawing to use */
   // TPtrC8 iDataBuffer;   /* used for memory image loading */
   // CCoeControl& iParent;   /* the parent control */
   // /// The color depth.
   // int m_colorDepth;

   // /* the image converter for bitmaps */
   // ImageConverter* iImageConverter;

   // /* for testing drawing speeds */
   // uint32 logStart, logEnd;
   // uint32 m_adjustTimeDelta;

   // /// Max rotated cache size constants.
   // enum {
   //    // At most 50k can be used for the cached bitmaps.
   //    c_maxCacheBmpSize = 50*1024,
   //    // And at least 500k memory must be available before using
   //    // this cache.
   //    c_minMemFree = 500*1024
   // };

   // /**
   //  *    String with angle notice.
   //  *    first is heap alloced string. Don't forget to delete afterwards.
   //  *    second is angle.
   //  */
   // typedef std::pair< HBufC*, int > strAngle_t;
 
   // /**
   //  *    Rotated text alphamaps.
   //  */
   // typedef std::pair<strAngle_t, alphaMap*> rotBmp_t;
      
   // /**
   //  *    List of cached rotated text alphamaps.
   //  */
   // std::list< rotBmp_t > m_rotatedBmps;

   void rotatePoint( float inX, float inY, 
                     float angle, 
                     float* outX, float* outY);

   /* returns the bounds of a rotated rectangle */
   isab::Rectangle getRotatedBBox( const MC2Point& center, 
                                   int width, int height,
                                   float angle);
   bool textDirectionLeftToRight( const STRING& text );

};

#endif /* _SYMBIANNATIVETEXTINTERFACE_H_ */
