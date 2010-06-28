/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef TEXTRESULT_H
#define TEXTRESULT_H

#include "TextPlacement/TextPlacementNotice.h"


class TextResult {
public:
   
   /**
    *    The Name of some feature that we wants to draw.
    */
   void setString( STRING* string );

   /**
    *    The name and format of the font.
    */
   void setFontName( const STRING* fontName );
     
   /**
    *    The type of the text.
    */
   void setTypeOfString( uint16 type );

   /**
    *    This is not used anymore we thinks.
    *    We use FontName as a carrier of all font information
    *    except color now.
    */
   void setFontSize( int fontSize );

   /**
    *    Sets the color of the pen.
    */
   void setFontColor( 
      unsigned int r, 
      unsigned int g, 
      unsigned int b );

   /**
    *    The rectangle .
    */
   void setStringAsRect( const isab::Rectangle& stringAsRect );
      
   /**
    *    The rectangle .
    */
   const isab::Rectangle& getStringAsRect( ) const;

   
   /**
    *    Returns the name of the feature.
    */
   const STRING& getString() const;
   
   /**
    *    Returns the fonts name.
    */
   const STRING& getFontName() const;

   /**
    *    Returns the type for this textstring..
    */
   uint16 getTypeOfString() const;

   /**
    *    Not used anymore.
    */
   int getFontSize() const;

   /**
    *    Returns the color that is used to draw the text.
    */
   void getFontColor( 
      unsigned int& r, 
      unsigned int& g, 
      unsigned int& b ) const;

   /**
    *   Returns the tpn vector.
    */
   const std::vector<TextPlacementNotice>& getTPN() const;
   
   /**
    *   Returns the tpn vector.
    */
   std::vector<TextPlacementNotice>& getTPN();
   
   /**
    *    Adds a tpn to the tpn vector.
    */
   void addTPN( const TextPlacementNotice& tpn );

   /**
    *    The text that we wants to draw.
    */
   STRING* m_string;

   /**
    *    A string that tells us what font and what settings to use 
    *    for that font when drawing the corresponding text.
    */
   const STRING* m_fontName;
 

private:

   // Type of text to draw
   uint16 m_type;

   // Rectangle describing the area that the text occupies.
   isab::Rectangle m_stringAsRect;
      
   /**
    *
    */
   int m_fontSize;

   /**
    *  Red, Green and Blue value for the pen used to draw this text.
    */
   int m_r;
   int m_g;
   int m_b;

   /**
    *    A vector that holds information about a text string that
    *    can be divided into several smaller parts. 
    */
   std::vector<TextPlacementNotice> m_stringNotices;
};



#endif /* TEXTRESULT_H */
