/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef RGBA32BITMAP_H
#define RGBA32BITMAP_H

#include <vector>

/**
 *   A RGBA32BitMap consists of a vector of 32 bit value pixels. The first
 *   value of the vector is defined as the top left pixel of the image.
 *   The memWidth and memHeight members indicate the size of the image data in
 *   memory. The imgWidth and imgHeight indicates the size of the actual image
 *   part of the image data. If the memory and image dimensions differ, the
 *   image is located in the rectangle defined as between (0,0) and
 *   (imgWidth, imgHeight). Any additional data is rendering specific padding.
 *   
 */
struct RGBA32BitMap {
   RGBA32BitMap() {
      imgWidth = imgHeight = memWidth = memHeight = 0;
   }
   
   RGBA32BitMap& operator=(const RGBA32BitMap& rhs) {
      this->data = rhs.data;
      this->imgWidth = rhs.imgWidth;
      this->imgHeight = rhs.imgHeight;
      this->memWidth = rhs.memWidth;
      this->memHeight = rhs.memHeight;
      return *this;
   }

   RGBA32BitMap(const RGBA32BitMap& rhs) {
      this->data = rhs.data;
      this->imgWidth = rhs.imgWidth;
      this->imgHeight = rhs.imgHeight;
      this->memWidth = rhs.memWidth;
      this->memHeight = rhs.memHeight;
   }
   
   std::vector<unsigned int> data;
   unsigned int imgWidth, imgHeight;
   unsigned int memWidth, memHeight;
};

#endif /* RGBA32BITMAP_H */
