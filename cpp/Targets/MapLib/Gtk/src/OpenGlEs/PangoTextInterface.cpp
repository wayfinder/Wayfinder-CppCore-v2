/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "PangoTextInterface.h"
#include <iostream>
#include <cmath>

#include "BitMapUtil.h"

int
PangoTextInterface::getStringLength( const STRING& text )
{
   return g_utf8_strlen( text, -1 ); 
}

/**
 * Get UTF-8 substring from an UTF-8 string
 * @param str utf-8 string
 * @param start start index 
 * @param length of sub string
 * @return allocated utf-8 sub string
 */

static char *subStringUTF8(const char *str, int startIdx, int length) {

   gchar *start = g_utf8_offset_to_pointer( str, startIdx );
   gchar *end = g_utf8_offset_to_pointer( str, startIdx + length );

   char *subText = new char[end - start + 2]; // +2 for some extra safety :P
   memset(subText, 0, end - start + 2); 
   return g_utf8_strncpy( subText, start, length);   
}

const char* getFont()
{
   // override default font
   static const char font[] = "Times Slant 12";

   return font;
   //return m_fontName.c_str();
}

/**
 *   Returns the pixel size for a given layout and a string.
 */

static void getTextSize( PangoLayout *layout, 
                  const char *str,
                  int &width, int &height )
{
   pango_layout_set_text( layout, str, -1 );
   pango_layout_get_pixel_size( layout, &width, &height );
}

void
PangoTextInterface::allocColor(GdkColor &theColor,
                               unsigned int red,
                               unsigned int green,
                               unsigned int blue)
{
   // transform range from 0-255 to 0-65535
   theColor.red = static_cast<int>(red/ 256.0 * 0xFFFF);
   theColor.green = static_cast<int>(green / 256.0 * 0xFFFF);
   theColor.blue = static_cast<int>(blue / 256.0 * 0xFFFF);

   // allocate color
   MC2_ASSERT( gdk_colormap_alloc_color( m_colorMap, &theColor, true, true ) );
}

PangoTextInterface::PangoTextInterface() :
   BitMapTextLoader(),
   m_context( gdk_pango_context_get() ), // default context
   m_layout( pango_layout_new( m_context ) ),
   m_colorMap( NULL ),
   m_visual( NULL)
{
   
   PangoFontDescription *font_desc = 
      pango_font_description_from_string( getFont() );

   pango_layout_set_font_description( m_layout, font_desc );
   pango_font_description_free(font_desc);

   m_visual = gdk_visual_get_best_with_depth( 32 );
   MC2_ASSERT( m_visual );
   m_colorMap = gdk_colormap_new( m_visual, true );
   MC2_ASSERT( m_colorMap );
}

int PangoTextInterface::load( RGBA32BitMap& bitMap )
{
   if( !m_string ) {
      return NULL_STRING;
   }
   
   int char_length;
   const STRING& text = *m_string;
   
   if( m_startIndex == m_endIndex || m_endIndex == -1) {   
      char_length = g_utf8_strlen( text, -1 );
      pango_layout_set_text( m_layout, text, - 1 );
   } else {
      // text iterator, get start
      char *text_i = g_utf8_offset_to_pointer( text, m_startIndex );
      
      const char *endOfString = g_utf8_offset_to_pointer( text_i,
                                                          m_endIndex - m_startIndex );
      // length of first utf-8 character
      char_length = g_utf8_strlen( text_i, endOfString - text_i );
   
      pango_layout_set_text( m_layout,
                             text_i,
                             endOfString - text_i );
   }

   // get sub string from utf-8
   char *subText = subStringUTF8(text, m_startIndex, char_length );
   int width, height;
   getTextSize( m_layout, subText, width, height );
   delete[] subText;
   
   if( width == 0 ) {
      return ZERO_WIDTH;
   } else if( height == 0 ) {
      return ZERO_HEIGHT;
   }

   GdkPixmap* pixMap = gdk_pixmap_new( NULL, width, height, 32 );
   
   GdkGC* gc = gdk_gc_new( pixMap );
   
   gdk_drawable_set_colormap( pixMap, m_colorMap );
       
   gdk_draw_rectangle( pixMap, gc, 
                       true,  // filled
                       0, 0, -1, -1 );
   
   GdkColor fontColor;

   // transform range from 0-255 to 0-65535
   fontColor.red = static_cast<int>(m_r/ 256.0 * 0xFFFF);
   fontColor.green = static_cast<int>(m_g / 256.0 * 0xFFFF);
   fontColor.blue = static_cast<int>(m_b / 256.0 * 0xFFFF);
   
   gdk_draw_layout_with_colors( pixMap, gc, 0, 0, m_layout,
                                &fontColor, NULL);
   
   GdkImage* img = gdk_image_get( pixMap, 0, 0, width, height );

   g_object_unref( pixMap );
   g_object_unref( gc );   
   
   int widthAdjust = 0;
   int heightAdjust = 0;
   int extraWidthAdjust = 0;
   
   if(m_hasBackgroundBox) {
      // this is an offset to center the text into the roadsign box:
      widthAdjust = 8;
      heightAdjust = 2;
      extraWidthAdjust = 1;
   }

   bitMap.imgWidth = bitMap.memWidth = img->width+widthAdjust+extraWidthAdjust;
   bitMap.imgHeight = bitMap.memHeight = img->height+heightAdjust;
      
   // Make room for the entire buffer
   bitMap.data.resize(bitMap.imgWidth * bitMap.imgHeight);
   
   int i=0;
   if(m_hasBackgroundBox) {
      BitMapUtil::applyRoadsignBackground(bitMap);
      i=bitMap.imgWidth*heightAdjust/2+widthAdjust/2;
   }
   
   uint32 pixel = 0;
   //loop over the pixels in the image containing the text
   for ( int y = 0; y < img->height; y++ ) {
      for ( int x = 0; x < img->width; x++ ) {
         pixel = gdk_image_get_pixel( img, x, y );
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
   }

   return 0;
}

isab::Rectangle 
PangoTextInterface::getStringAsRectangle( const STRING& text,
                                     const MC2Point& center,
                                     int startIdx,
                                     int nbrChars,
                                     float angle )
{
 
   if( nbrChars == -1 ) {
      nbrChars = getStringLength( text ); 
   }

   MC2_ASSERT( nbrChars!=0 );
   
   // get sub string from utf-8
   char *subText = subStringUTF8(text, startIdx, nbrChars);

   int width, height;
   getTextSize( m_layout, subText, width, height );

   int safeSize =
      static_cast<int>( sqrt( width * width + height * height ) + 0.5 );
   
   delete[] subText;

   return isab::Rectangle( center.getX() - safeSize / 2,
                           center.getY() - safeSize / 2,
                           safeSize, safeSize );  
}

int 
PangoTextInterface::getStringAsRectangles( vector<isab::Rectangle>& boxes,
                                           const STRING& text,
                                           const MC2Point& center,
                                           int startIdx,
                                           int nbrChars,
                                           float angle )
{

   if( nbrChars == -1 ){
      nbrChars = getStringLength( text );
   }
   
   MC2_ASSERT( nbrChars!=0 );

   // Get pixel size of substring 
   char *subText = subStringUTF8( text, startIdx, nbrChars );
   int widthOfString, heightOfString;
   getTextSize( m_layout, subText, widthOfString, heightOfString );

   delete[] subText;

   int halfWidthOfString = widthOfString >> 1;
   // waruns angle goes ccw and we wants it cw so thats why we do -.
   angle = -angle;

   double radAngle = ( angle / 180.0 ) * M_PI;
   double xDiff = cos( radAngle ) * halfWidthOfString; 
   double yDiff = sin( radAngle ) * halfWidthOfString;
   int p1x = (int)(center.getX() - xDiff );
   int p1y = (int)(center.getY() - yDiff );
   int p2x = (int)(center.getX() + xDiff );
   int p2y = (int)(center.getY() + yDiff );
   for ( int i = startIdx; i < startIdx + nbrChars; i++ ) {
      double diffFactor = 
         ( ( i - startIdx + 0.5 ) / (double)nbrChars );
      mc2dbg4 << "diffFactor " << diffFactor << endl;
      MC2Point posOfSubString( 
            (int)(p1x * (1 - diffFactor) + p2x * diffFactor),
            (int)(p1y * (1 - diffFactor) + p2y * diffFactor) );
      
      boxes.push_back( getStringAsRectangle( text, posOfSubString, i, 1, angle ) );
   }
   return nbrChars;
}

void PangoTextInterface::setFont( const STRING& fontName, int size )
{
   return;
}

STRING*
PangoTextInterface::createString( const char* text )
{
   char** ptr = new char*[1];
   ptr[ 0 ] = new char[ strlen( text ) + 1 ];
   strcpy( ptr[ 0 ], text );
   return ptr;
}

void
PangoTextInterface::deleteString( STRING* text )
{
   delete [] *text;
   delete [] text;
}

isab::Font* PangoTextInterface::createFont(WFAPI::FontSpec* fontSpec)
{
   return NULL; 
}

void PangoTextInterface::deleteFont(isab::Font* font)
{
    
}
