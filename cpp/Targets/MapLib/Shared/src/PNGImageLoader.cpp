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
#include "PNGImageLoader.h"
#include <vector>
#include "PNGUtil.h"


/*
isab::BitMap* PNGImageLoader::loadToTexture( const unsigned char* pngSource,
                                             unsigned int size )
{
   std::vector<unsigned char> decodedImg;
   unsigned long width, height;
   
   int error = PNGUtil::loadPNG( decodedImg,
                                 width, height,
                                 pngSource, size );

   assert( error != 0 );

   isab::BitMap* ret =
      m_texManager->storeRGBA32Texture( &decodedImg[0], width, height );

   return ret;
}
*/

int PNGImageLoader::load( RGBA32BitMap& bitMap )
{
   unsigned long width, height;
   std::vector< unsigned char > decodedImg;
   int error = PNGUtil::loadPNG( decodedImg,
                                 width, height,
                                 m_source, m_size );
   MC2_ASSERT( error == 0 );

   bitMap.imgWidth = bitMap.memWidth = width;
   bitMap.imgHeight = bitMap.memHeight = height;
   bitMap.data.resize( bitMap.memWidth * bitMap.memHeight , 0 );

   memcpy( &bitMap.data[0], &decodedImg[0],
           bitMap.memWidth * bitMap.memHeight *
           sizeof( unsigned int ) / sizeof( unsigned char ) );

   return 0;
}
