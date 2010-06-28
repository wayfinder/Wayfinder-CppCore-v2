/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef IMAGETILE_H
#define IMAGETILE_H

#include "BitMapTypes.h"
#include "Geometry/BoundingBox.h"
#include "ImageMap/MapParam.h"


class ImageTile {
public:
   ImageTile(isab::BitMap* bmp, BoundingBox bbox, const MapParam& param);

   ~ImageTile() {
      delete m_bmp;
   }
   
   /**
    *    @return The bounding box for this image tile.
    */
   const BoundingBox& getBBox() const {
      return m_bbox;
   }
   
   /**
    *    @return The associated bitmap for this image tile.
    */
   isab::BitMap* getBitMap() {
      return m_bmp;
   }
   
   /**
    *    @return The parameter string, used as identifier for this image tile.
    */
   const MapParam& getParam() const {
      return m_param;
   }

   uint32 getTimeStamp() const {
      return m_timeStamp;
   }

   void updateTimeStamp(int newTimeStamp) {
      m_timeStamp = newTimeStamp;
   }

   struct Comparator {
      bool operator()(const ImageTile* lhs,
                      const ImageTile* rhs) const
         {
            return lhs->getTimeStamp() < rhs->getTimeStamp();
         }
   };
private:

   /**
    *    The bounding box for this image tile.
    */
   BoundingBox m_bbox;

   /**
    *    The associated bitmap for this image tile.
    */
   isab::BitMap* m_bmp;

   /**
    *    The parameter string, used as identifier for this image tile.
    */
   MapParam m_param;

   uint32 m_timeStamp;
};

#endif /* IMAGETILE_H */
