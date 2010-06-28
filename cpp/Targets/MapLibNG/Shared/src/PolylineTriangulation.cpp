/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "PolylineTriangulation.h"
#include <math.h>
#include "config.h"


/*
 * The following functions work on 3d vectors but treats them as
 * 2d.
 */ 

/**
 * Normalizes vector vec to length 1.
 */ 
inline void norm2dVec( vec3f& vec ) {
   const float vecLength = sqrt(vec.x * vec.x + vec.y * vec.y );
   
   vec.x /= vecLength;
   vec.y /= vecLength;
}

/**
 * Scales the vector by factor fac
 */ 
inline void multVec( vec3f& vec, const float& fac )
{
   vec.x *= fac;
   vec.y *= fac;
}

/**
 * Creates a perpendicular vector to vec and assigns it to vec
 */ 
inline void perpVec( vec3f& vec )
{
   vec.x = -vec.x;
   std::swap( vec.x, vec.y );   
}

void
PolylineTriangulation::triangulate(const vbuf3f& buf,
                                   vbuf3f& result,
                                   float cosLat,
                                   float width)
{   
   if (buf.size() < 2) {
      result.clear();
      return;
   }

   //
   // Precalculated values for speedup
   //
   
   float cosLatSquaredInv = 1.0f / (cosLat * cosLat);
   float halfWidth = width / 2.0;

   //
   // Add begin cap
   //
   {
      vec3f vec = buf[1] - buf[0];

      norm2dVec( vec );
      multVec( vec, halfWidth );
         
      vec3f beginCapPoint(buf[0].x - vec.x, buf[0].y - vec.y);
         
      result.push_back(beginCapPoint);
      result.push_back(beginCapPoint);
   }
   
   for (unsigned int i = 0; i < buf.size() - 1; i++) {
      
      // We start by creating a vector that is aligned with the line.
      vec3f offsetVec = buf[i + 1] - buf[i];

      // Counter the effect of cosLat adjusted widths by scaling the y
      // component of the vector down.
      offsetVec.y *= cosLatSquaredInv;
            
      // Find the length to normalize.
      norm2dVec( offsetVec );
      
      // We rotate it to be perpendicular to the line.
      perpVec( offsetVec );

      // We need to adjust the width differently for cosLat depending upon
      // the angle of the vector
      float adjWidth =
         (width * ( 1.0 + fabs( offsetVec.y ) * ( cosLat - 1.0 ) ));
      
      // Expand to length width / 2
      multVec( offsetVec, adjWidth );

      vec3f p0(buf[i].x + offsetVec.x, buf[i].y + offsetVec.y);
      vec3f p1(buf[i].x - offsetVec.x, buf[i].y - offsetVec.y);
      vec3f p2(buf[i + 1].x + offsetVec.x, buf[i + 1].y + offsetVec.y);
      vec3f p3(buf[i + 1].x - offsetVec.x, buf[i + 1].y - offsetVec.y);
      
      result.push_back(p0);
      result.push_back(p1);
      result.push_back(p2);
      result.push_back(p3);
   }

   //
   //  Add endcap point
   //
   {
      int lastElem = buf.size() - 1;
      vec3f testVec = buf[lastElem] - buf[lastElem-1];

      norm2dVec( testVec );
      multVec( testVec, halfWidth );
         
      vec3f endCapPoint(buf[lastElem].x + testVec.x,
                        buf[lastElem].y + testVec.y);
         
      result.push_back(endCapPoint);
      result.push_back(endCapPoint);       
   }
}

