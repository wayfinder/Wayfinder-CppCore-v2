/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "Graphics/Text/PALTextInterface.h"
#include <math.h>
#include <cassert>

using namespace pal;

PALTextInterface::PALTextInterface()
   : NativeTextInterface()
{
   pal::pstatus status = createGraphicsContext(&m_context);
   assert(status == PAL_OK);

   const char* fontFile = "/usr/share/fonts/dejavu/DejaVuSans.ttf";
   FILE* f = fopen(fontFile, "r");
   
   if(f) {
      printf("Font file %s loaded\n", fontFile);
      fclose(f);
   } else {
      printf("Font file %s failed to load\n", fontFile);
   }

   status = pal::setFontForContext(m_context, "DejaVu Sans", "Bold", 16);
   assert(status == pal::PAL_OK);

   pal::setFontOutlineColorForContext(m_context, 255, 255, 255 );
   assert(status == pal::PAL_OK);

   pal::setFontColorForContext(m_context, 0, 0,  0);
   assert(status == pal::PAL_OK);
   
   status = pal::setFontStyleForContext(m_context, 8 | 1);
   assert(status == pal::PAL_OK);
   
   status = pal::setFontFaceForContext(m_context, fontFile);
   assert(status == pal::PAL_OK);
}

int PALTextInterface::getStringAsRectangles( std::vector<isab::Rectangle>& boxes,
                                             const STRING& text,
                                             const MC2Point& center,
                                             int startIdx,
                                             int nbrChars,
                                             float angle )
{
   pal::BitmapMetrics metrics;

   pal::pstatus status = pal::measureTextLine(m_context,
                                              text,
                                              startIdx,
                                              nbrChars,
                                              RGBA8,
                                              &metrics);

   int widthOfString = metrics.m_Width;
   int heightOfString = metrics.m_Height;
   
   MC2_ASSERT( metrics.m_CharCount !=0 );
   
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

isab::Rectangle PALTextInterface::getStringAsRectangle( const STRING& text,
                                                        const MC2Point& point,
                                                        int startIdx,
                                                        int nbrChars,
                                                        float angle )
{
   pal::BitmapMetrics metrics;

   pal::pstatus status= pal::measureTextLine(m_context,
                                             text,
                                             startIdx,
                                             nbrChars,
                                             RGBA8,
                                             &metrics);
   
   int safeSize =
      static_cast<int>( sqrt( metrics.m_Width * metrics.m_Width +
                              metrics.m_Height * metrics.m_Height ) + 0.5 );
   
   return isab::Rectangle( point.getX() - safeSize / 2,
                           point.getY() - safeSize / 2,
                           safeSize, safeSize );  
}

void PALTextInterface::setFont( const STRING& fontName, int size )
{
   //TODO: implement
   // printf("setFont(%s, %d)\n ", fontName, size);
   // pal::setFontForContext(m_context, fontName, "Normal", size);
}

int PALTextInterface::getStringLength( const STRING& text )
{
   // Draw the actual font
   pal::BitmapMetrics metrics;
   pal::pstatus status= pal::measureTextLine(m_context,
                                             text,
                                             0,
                                             -1,
                                             RGBA8,
                                             &metrics);


   return metrics.m_CharCount;
}

STRING* PALTextInterface::createString( const char* text )
{
   char** ptr = new char*[1];
   ptr[ 0 ] = new char[ strlen( text ) + 1 ];
   strcpy( ptr[ 0 ], text );
   return ptr;
}

void PALTextInterface::deleteString( STRING* text )
{
   delete [] *text;
   delete [] text;
}

int PALTextInterface::load( RGBA32BitMap& bitMap )
{
   RGBA32BitMap bmp;

   pal::ResultBuffer resBuffer;

   // These will contain the dimensions of the generated textures
   pal::wf_uint32 widthPixels, heightPixels;
   
   // Draw the actual font
   pal::BitmapMetrics metrics;
   pal::pstatus status = pal::drawTextLineToBuffer(m_context,
                                                   *m_string,
                                                   m_startIndex, m_endIndex - m_startIndex,
                                                   pal::RGBA8,
                                                   &metrics,
                                                   &resBuffer);
   if(status != pal::PAL_OK) {
      return GEN_ERROR;
   }

   const pal::wf_uint32* test = reinterpret_cast<const
      pal::wf_uint32*>(resBuffer.getData());

   bitMap.data.insert(bitMap.data.begin(), test, test + (resBuffer.getSize() / 4));
   bitMap.imgWidth = bitMap.memWidth = metrics.m_Width;
   bitMap.imgHeight = bitMap.memHeight = metrics.m_Height;
   
   return LOAD_OK;
}

isab::Font* PALTextInterface::createFont(WFAPI::FontSpec* fontSpec)
{
   return NULL;
}

void PALTextInterface::deleteFont(isab::Font* font)
{
    
}

