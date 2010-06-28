/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "arch.h"
#include "MifImageLoader.h"
#include "Image.h"
#include <fbs.h>

MifImageLoader::MifImageLoader()
   : PlatformImageLoader()
{
   m_imageConverter = ImageConverter::allocate();
}

MifImageLoader::~MifImageLoader()
{
   delete m_imageConverter;
}


int MifImageLoader::load( RGBA32BitMap& bitMap )
{
   Image* tmpImage = Image::allocate();

   if ( tmpImage == NULL ) {
      return -1;
   }

   if ( m_imageConverter->loadImage( m_source, m_size, tmpImage, 1 ) != KErrNone )
   {
      delete tmpImage;
      return -2;
   }

   CFbsBitmap* tmpBitMap = tmpImage->GetBitmap();
   CFbsBitmap* tmpMask = tmpImage->GetMask();
   
   TSize size = tmpBitMap->SizeInPixels();   

   for ( int row = 0; row < size.iHeight; row++ ) {
      for ( int col = 0; col < size.iWidth; col++ ) {         
         TRgb color;
         TRgb maskColor;
         tmpMask->GetPixel( maskColor, TPoint( col, row ) );
         unsigned int val = 0;
         if ( maskColor.Value() ) {
            tmpBitMap->GetPixel( color, TPoint( col, row ) );
            val = color.Value() | 0xFF000000;
         }
         bitMap.data.push_back( val );
      }
   }
   
   delete tmpImage;

   int width = size.iWidth;
   int height = size.iHeight;
   
   bitMap.imgWidth = bitMap.memWidth = width;
   bitMap.imgHeight = bitMap.memHeight = height;
   // bitMap.data.resize( bitMap.memWidth * bitMap.memHeight , 0 );
   
   return 0;
}

