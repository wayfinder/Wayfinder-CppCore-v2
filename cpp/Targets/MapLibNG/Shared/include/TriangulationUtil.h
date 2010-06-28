/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _TRIANGULATIONUTIL_H_
#define _TRIANGULATIONUTIL_H_

#include "TileFeature.h"
#include "TileMapFormatDesc.h"

namespace TriangulationUtil {

inline uint32
getPolylineInnerPixelWidth( const TilePrimitiveFeature& prim,
                            uint32 scaleIndex,
                            float scale,
                            float correctedScale,
                            uint32 scaleFactor)
{
   MC2_ASSERT( prim.getType() == TileFeature::line );
   uint32 widthPixels = 1;

   // Get the width argument given in pixels
   uint32 tmpWidthPixels = MAX_UINT32;
   const SimpleArg* widthArg = 
      static_cast<const SimpleArg*>((prim.getArg(TileArgNames::width)));
   if ( widthArg != NULL ) {
      tmpWidthPixels = widthArg->getValue( scaleIndex );
   }

   // Check for meters arg first, since we must send the
   // old arg for the old clients.
   const SimpleArg* widthMetersArg =
      static_cast<const SimpleArg*>(
         (prim.getArg(TileArgNames::width_meters)));
   
   bool useMeters = false;
   if ( widthMetersArg != NULL ) {
      uint32 tmpWidthMeters = widthMetersArg->getValue( scaleIndex );
      if ( ( tmpWidthMeters & MAX_UINT8 ) != MAX_UINT8 ) {
         useMeters = true;
         widthPixels = uint32(tmpWidthMeters / correctedScale);
         // Make sure the width is at least the normal width argument
         if ( ( tmpWidthPixels & MAX_UINT8 ) != MAX_UINT8 ) {
            widthPixels = MAX(widthPixels, tmpWidthPixels);
         }
         if ( widthPixels == 0 ) {
            // Must have odd width or more than zero. But zero is even
            // so it will also get here.
            ++widthPixels;
         }
      }
   }
   if ( ! useMeters ) {
      if ( ( tmpWidthPixels & MAX_UINT8 ) != MAX_UINT8 ) {
         widthPixels = tmpWidthPixels;
      }
   }
  
   widthPixels = widthPixels * scaleFactor;
   
   return widthPixels;
}
   
} // End of namespace TriangulationUtil




#endif /* _TRIANGULATIONUTIL_H_ */
