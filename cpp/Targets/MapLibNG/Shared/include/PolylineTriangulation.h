/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef POLYLINETRIANGULATION_H
#define POLYLINETRIANGULATION_H

#include "Vector3d.h"

namespace PolylineTriangulation {
   
/**
 * Triangulates a polyline using triangle strips.
 */
void triangulate(const vbuf3f& buf, vbuf3f& result,
                 float cosLat,
                 float width);

/**
 * @return The number of needed vertices to represent a
 *         triangulated polyline that consists numLinePoints
 */
inline unsigned int numNeededVertices(unsigned int numLinePoints) {
   // We cannot triangulate a line from only one point.
   if (numLinePoints < 2){
      return 0;
   } else {
      // 4 * (n - 1) + (2 + 2 for the endcaps)
      return 4 * (numLinePoints);
   }
}

} //End of namespace PolylineTriangulation

#endif /* POLYLINETRIANGULATION_H */
