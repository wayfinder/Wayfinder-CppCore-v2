/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "IPhoneImageSpec.h"
#include <CoreGraphics/CGContext.h>
#include <CoreGraphics/CGBitmapContext.h>

IPhoneImageSpec::IPhoneImageSpec(CGImageRef src)
: ImageSpecInternal()
{
   m_src = CGImageRetain(src);      
}

IPhoneImageSpec::~IPhoneImageSpec()
{
   CGImageRelease(m_src); 
}

WFAPI::wf_uint32 IPhoneImageSpec::getWidth() const
{
   return CGImageGetWidth(m_src);
}

WFAPI::wf_uint32 IPhoneImageSpec::getHeight() const
{
   return CGImageGetHeight(m_src);
}

int IPhoneImageSpec::load( RGBA32BitMap& bitMap ) const
{
   WFAPI::wf_uint32 width = getWidth();
   WFAPI::wf_uint32 height = getHeight();
   
   CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
   // We resize the vector to hold the whole image
   bitMap.data.resize(height*width,0);
   // And use the consecutive memory area of the vector as a drawing area
   unsigned char* rawData = reinterpret_cast<unsigned char*>(&bitMap.data[0]);
   
   unsigned int bytesPerPixel = 4;
   unsigned int bytesPerRow = bytesPerPixel * width;
   unsigned int bitsPerComponent = 8;
   // Create the context from the rawData pointer
   CGContextRef context =
      CGBitmapContextCreate(rawData, width, height,
                            bitsPerComponent, bytesPerRow, colorSpace,
                            kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big);
   CGColorSpaceRelease(colorSpace);
   // Draw the image on the context
   CGContextDrawImage(context, CGRectMake(0, 0, width, height), m_src);
   CGContextRelease(context);

   // Finally, set the size of the image.
   bitMap.imgWidth = width;
   bitMap.imgHeight = height;
   bitMap.memWidth = width;
   bitMap.memHeight = height;
}

