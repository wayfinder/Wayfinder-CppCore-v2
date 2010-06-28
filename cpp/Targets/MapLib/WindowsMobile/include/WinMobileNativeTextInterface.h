/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _WINMOBILENATIVETEXTINTERFACE_H_
#define _WINMOBILENATIVETEXTINTERFACE_H_

#include "NativeTextInterface.h"
#include "BitMapLoader.h"

class WinMobileNativeTextInterface : public NativeTextInterface,
                                     public BitMapTextLoader {
public:

   WinMobileNativeTextInterface();
   
   /**
    *   @see NativeTextInterface::getStringAsRectangles
    */

   virtual int getStringAsRectangles( 
      vector<isab::Rectangle>& boxes,
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
    *   Get if the text rendering direction is left to right
    *   or not (arabic, hebrew is right to left) for the
    *   specified text.
    *   Default implementation returns true.
    */
   virtual bool textDirectionLeftToRight( const STRING& text ) {
      return true;
   }

   /**
    *   Creates a platform specific font using the specification fontSpec.
    */ 
   virtual isab::Font* createFont(WFAPI::FontSpec* fontSpec);

   /**
    *   Deallocates a platform specific font.
    */ 
   virtual void deleteFont(isab::Font* font);
   
private:
   /**
    *    Convert the desired part of the utf8 string to unicode.
    *
    *    @param   text     The utf8 string.
    *    @param   startIdx The index of the first char of the string.
    *    @param   nbrChars The number of chars in the string.
    *                      If -1, use max length of the string.
    *    @param   uniStr   Prealloced unicode string to place result in.
    *    @param   uniStrLen   The length of the prealloced unicode 
    *                         string.
    *    @return  Number of characters in the returned string.
    */

   int utf8ToUnicode( const STRING& text,
                      int startIdx,
                      int nbrChars,
                      wchar_t* uniStr,
                      int uniStrLen );

   /**
    *   Creates a new GDI font with the specified angle.
    *
    *   @param   angle   The wanted angle of the font.
    *
    *   @return  The created font.
    */
   
   HFONT createFont( float angle );

   /**
    *   Creates a rectangle which encloses the rectangle obtained
    *   by rotating the supplied rectangle defined by the two points.
    */

   isab::Rectangle rotatedRect(int x1, int y1,
                               int x2, int y2, 
                               float angle);
   
   /* the current font */
   HFONT m_curFont;
   int32 m_fontWidth, m_fontHeight;
   /* for caching fonts */
   STRING m_curFontName;
   int32 m_curFontSize;
   
   LOGFONT m_fontData;
   TEXTMETRIC m_textMetrics;

   HDC m_dc;
};

#endif /* _WINMOBILENATIVETEXTINTERFACE_H_ */





















