/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "config.h"
#include "BitMapPixelBoxAlgorithms.h"
#include <algorithm>
#include <iterator>
#include "Utility.h"

bool nonZero( unsigned int pixel )
{
   return pixel != 0;
}

PixelBox
BitMapPixelBoxAlgorithms::getNonZeroQuadrant( const RGBA32BitMap& bitMap )
{
   typedef std::vector<unsigned int>::const_iterator Iterator;
   MC2Point upperLeft;
   MC2Point lowerRight;
   Iterator it =
      std::find_if( bitMap.data.begin(), bitMap.data.end(), nonZero );
   
   if ( it != bitMap.data.end() ) {
      std::iterator_traits<Iterator>::difference_type dist =
         std::distance( bitMap.data.begin(), it );

      unsigned int xPos = dist % bitMap.memWidth;
      unsigned int yPos = dist / bitMap.memWidth;

      MC2_ASSERT( xPos < bitMap.imgWidth );
      MC2_ASSERT( yPos < bitMap.imgHeight );

      bool isLeft = xPos < ( bitMap.imgWidth / 2 );
      bool isUpper =  yPos < ( bitMap.imgHeight / 2);
      
      upperLeft.getX() = isLeft ? 0 : bitMap.imgWidth / 2;
      upperLeft.getY() = isUpper ? 0 : bitMap.imgHeight / 2;

      lowerRight.getX() = upperLeft.getX() + bitMap.imgWidth / 2;
      lowerRight.getY() = upperLeft.getY() + bitMap.imgHeight / 2;

   } 

   return PixelBox( upperLeft, lowerRight );
}

unsigned int getAlphaValue() {
    int i = 1 ;
    bool littleEndian = *((char *) &i ) ;

    if( littleEndian ) {
       return 0xFF000000;
    } else {
       return 0x000000FF;
    }
}

PixelBox BitMapPixelBoxAlgorithms::getColoredArea( const RGBA32BitMap& bitMap )
{
   unsigned int minX = bitMap.imgWidth;
   unsigned int maxX = 0;

   unsigned int minY = bitMap.imgHeight;
   unsigned int maxY = 0;

   unsigned int alphaVal = getAlphaValue();
   
   for ( unsigned rowOffs = 0;
         rowOffs < bitMap.data.size();
         rowOffs += bitMap.memWidth ) {

      unsigned int curY = rowOffs / bitMap.memWidth;

      for ( unsigned int col = 0; col < bitMap.memWidth; col++ ) {

         unsigned int color = bitMap.data[ rowOffs + col ];
         
         if ( ( color & alphaVal ) != 0 ) {
            unsigned int curX = col;

            if( curX < minX ) {
               minX = curX;
            }

            if( curX > maxX ) {
               maxX = curX;
            }

            if( curY < minY ) {
               minY = curY;
            }

            if( curY > maxY ) {
               maxY = curY;
            }
         }
      }
   }
   
   PixelBox ret =  PixelBox( MC2Point( minX, minY), MC2Point( maxX, maxY) );

   return ret;
}
