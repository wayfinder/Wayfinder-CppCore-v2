/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#define CORE_LOGGING_MODULE_NAME "BitMapExpanderPow2"
#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE

#include "CoreDebugPrint.h"
#include "config.h"
#include "BitMapExpanderPow2.h"
#include "Utility.h"



bool isPow2( int n ) {
   return ( ( n & ( n - 1 ) ) == 0 );
}

unsigned int nearestLargerPow2( unsigned int n ) {

   unsigned int pow = 1;
   if ( n & 0x80000000 ) {
      MC2_ASSERT( !"Overflowing input for nearestLargerPow2." );
   }
   while ( ( n >>= 1 ) != 0 ) {
      pow++;
   }
   
   return 1 << pow;
}

int BitMapExpanderPow2::expand( const pal::wf_uint8* srcData,
                                pal::wf_uint32 srcWidth,
                                pal::wf_uint32 srcHeight,
                                RGBA32BitMap& bitMap )
{
   bool widthIsPow2 = isPow2( srcWidth );
   bool heightIsPow2 = isPow2(srcHeight );
   
   if( widthIsPow2 && heightIsPow2 ) {
      const WFAPI::wf_uint32* asInt =
         reinterpret_cast<const WFAPI::wf_uint32*>(srcData);
      
      bitMap.data.insert(bitMap.data.begin(), asInt, asInt + srcWidth * srcHeight);
      bitMap.imgWidth = srcWidth;
      bitMap.imgHeight = srcWidth;
      bitMap.memWidth = srcWidth;
      bitMap.memHeight = srcHeight;
      return 0;
   }

   bitMap.imgWidth = srcWidth;
   bitMap.imgHeight = srcHeight;

   if ( !widthIsPow2 ) {
      bitMap.memWidth = nearestLargerPow2( bitMap.imgWidth );
   } else {
      bitMap.memWidth = bitMap.imgWidth;
   }

   if ( !heightIsPow2 ) {
      bitMap.memHeight = nearestLargerPow2( bitMap.imgHeight );
   } else {
      bitMap.memHeight = bitMap.imgHeight;
   }
   
   bitMap.data.resize( bitMap.memWidth * bitMap.memHeight * 4, 0 );

   
   //    for ( unsigned int col = 0; col < bitMap.memWidth*4; col++ )  {
   //       bitMap.data.at( ( bitMap.memHeight / 2 ) * bitMap.memWidth*4 + col ) = 0xFF;
   //    }

   //    for ( unsigned int row = 0; row < bitMap.memHeight; row++ )  {
   //       bitMap.data.at( ( bitMap.memWidth / 2 ) * 4 + bitMap.memWidth * 4 * row  ) =

   const WFAPI::wf_uint32* asInt =
      reinterpret_cast<const WFAPI::wf_uint32*>(srcData);
   
   for ( unsigned int row = 0; row < bitMap.imgHeight; ++row ) {
      
      std::copy( asInt + row * srcWidth,
                 asInt + ( row + 1 ) * srcWidth,
                 bitMap.data.begin() + row * bitMap.memWidth );
   }


   return 0;
}

