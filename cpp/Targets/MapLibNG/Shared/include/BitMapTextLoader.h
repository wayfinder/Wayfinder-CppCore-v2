/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef BITMAPTEXTLOADER_H
#define BITMAPTEXTLOADER_H

#include "MapPlotterConfig.h"
#include "BitMapLoader.h"

class BitMapTextLoader : public BitMapLoader {
public:
   BitMapTextLoader()
      : BitMapLoader(),
        m_string( NULL ),
        m_startIndex( 0 ),
        m_endIndex( 0 ),
        m_r(0), m_g( 0 ), m_b( 0 )
      {
         
      }
   
   void setStringToDraw( const STRING* stringToDraw,
                         int startIndex,
                         int endIndex,
                         unsigned int r,
                         unsigned int g,
                         unsigned int b )
      {
         //this could be seen as a helper function to the next, ie
         //set string to draw without background box
         setStringToDraw(stringToDraw, startIndex, endIndex, r, g, b, false);
      }

   void setStringToDraw( const STRING* stringToDraw,
                         int startIndex,
                         int endIndex,
                         unsigned int r,
                         unsigned int g,
                         unsigned int b,
                         bool hasBackgroundBox)
      {
         m_string = stringToDraw;
         m_startIndex = startIndex;
         m_endIndex = endIndex;
         m_hasBackgroundBox = hasBackgroundBox;
         m_r = r;
         m_g = g;
         m_b = b;
      }   
   
protected:
   const STRING* m_string;
   int m_startIndex;
   int m_endIndex;
   bool m_hasBackgroundBox;
   unsigned int m_r, m_g, m_b;
};

#endif /* BITMAPTEXTLOADER_H */
