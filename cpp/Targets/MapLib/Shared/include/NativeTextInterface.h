/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef NATIVETEXTINTERFACE_H
#define NATIVETEXTINTERFACE_H

#include "MapPlotterTypes.h"
#include "MC2Point.h"
#include "MapPlotterConfig.h"
#include <vector>
#include "BitMapTextLoader.h"
#include "FontType.h"

namespace WFAPI {
class FontSpec;
}

class NativeTextInterface 
{
public:
   /**
    *   Puts the bounding boxes of each character in str
    *   into the vector boxes.
    *   <br />
    *   @param boxes  Vector to add the boxes to.
    *   @see drawText
    *   @return Number of characters in str.
    */
   
   virtual int getStringAsRectangles( std::vector<isab::Rectangle>& boxes,
                                      const STRING& text,
                                      const MC2Point& center,
                                      int startIdx = 0,
                                      int nbrChars = -1,
                                      float angle = 0.0 ) = 0;
   /**
    *   puts the bounding boxes of each character in str
    *   into the vector boxes.
    *   <br />
    *   @param boxes  Vector to add the boxes to.
    *   @see drawText
    *   @return Number of characters in str.
    */

   virtual isab::Rectangle getStringAsRectangle( const STRING& text,
                                                 const MC2Point& center,
                                                 int startIdx = 0,
                                                 int nbrChars = -1,
                                                 float angle = 0.0 ) = 0;

   /**
    * Returns the dimensions of the text in pixels.
    *
    * @param text The text to get the dimensions for.
    * @return A rectangle containing the width and height in pixels
    *         for the text.
    */   
   virtual isab::Rectangle getTextDimensions(const STRING& text)
   {
      isab::Rectangle tmp = getStringAsRectangle(text, MC2Point(0, 0));

      isab::Rectangle ret(0, 0, tmp.getWidth(), tmp.getHeight());

      return ret;
   }
    
   /**
    *   Sets font. The description format in the server
    *   and the MapPlotter must know what needs to be put
    *   here.
    *   @param fontName Name of the font in client-specific format.
    *   @param size     Size of the font in UNIT.
    */

   virtual void setFont( const STRING& fontName, int size ) = 0;

   /**
    *   Creates a text string for use with the other text methods.
    *   Default implementation returns NULL.
    */
   virtual STRING* createString(const char* text) = 0;

   /**
    *   Deletes a text string.
    *   Default implementation does nothing.
    */   
   virtual void deleteString(STRING* text) = 0;

   /**
    *   Creates a platform specific font using the specification fontSpec.
    */ 
   virtual isab::Font* createFont(WFAPI::FontSpec* fontSpec) = 0;

   /**
    *   Deallocates a platform specific font.
    */ 
   virtual void deleteFont(isab::Font* font) = 0;
   
   /**
    *   @return The number of characters in the string.
    */
   virtual int getStringLength( const STRING& text ) = 0;

   /**
    *   Get if the text rendering direction is left to right
    *   or not (arabic, hebrew is right to left) for the
    *   specified text.
    *   Default implementation returns true.
    */

   virtual bool textDirectionLeftToRight( const STRING& text ) {
      return true;
   }

};

#endif /* NATIVETEXTINTERFACE_H */
