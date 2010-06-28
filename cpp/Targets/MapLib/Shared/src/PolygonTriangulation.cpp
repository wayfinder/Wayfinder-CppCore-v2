/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "PolygonTriangulation.h"

/**
 *    Determines if a point is inside a triangle (more efficient
 *    than calling the polygon functions). 
 *
 *    @param   begin    Iterator of points containing 
 *                      the beginning of the triangle.
 *    @param   end      End iterator of the triangle, 
 *                      i.e. one position after the last point.
 *    @param   p        The point to check.
 *    @return  True if the point was inside the triangle, 
 *             false otherwise.
 */
   
inline bool pointInsideTriangle(const vec3f& p0,
                                const vec3f& p1,
                                const vec3f& p2,
                                const vec3f& i)
{
   vec3f e0 = i  - p0;
   vec3f e1 = p1 - p0;
   vec3f e2 = p2 - p0;

   const static float ZERO    = 0.0f;
   const static float ONE     = 1.0f;

   float u, v, d;
      
   if( e1.x == ZERO  )
   {      
      if( e2.x == ZERO ) {
         return false; //Malformed triangle, would give division by zero
      }

      u = e0.x / e2.x;
      
      if ( u < ZERO || u > ONE) {
         return false;
      }

      v = (e0.y - e2.y * u) / e1.y;
      
      if( v < ZERO ) {
         return false;
      }
   } else {
      d = e2.y * e1.x - e2.x * e1.y;

      if( d == ZERO ) {
         return false;
      }

      u = ( e0.y * e1.x - e0.x * e1.y ) / d;

      if ( u < ZERO || u > ONE) {
         return false;
      }

      v = (e0.x - e2.x * u) / e1.x;

      if( v < ZERO ) {
         return false;
      }
   }
   
   if( u + v > ONE) {
      return false;
   } else {
      return true;
   }
}

inline int isLeft( const vec3f& p0, const vec3f& p1, const vec3f& p2 )
{
   double res =
      double( p1.x - p0.x ) * ( p2.y - p0.y ) -
      double( p2.x - p0.x )*  ( p1.y - p0.y );
       
   if ( res > 0.5 ) {
      return 1;
   } else if ( res < -0.5 ) {
      return -1;
   } else {
      return 0; 
   }
}

inline bool triangleCanBeRemoved(vec3f& left,
                                 vec3f& center,
                                 vec3f& right,
                                 vbuf3f::iterator begin,
                                 vbuf3f::iterator end)
{
   
   /* If we make a left turn when going from center to right, the
    * triangle is concave and thus not suitable for removal. */
      
   if( isLeft( left, center, right ) > 0 )  { 
      return false;
   }
      
   for(; begin != end; begin++) {
      vec3f& p = *begin;

      if( p == left || p == center || p == right) {
         continue;
      }

      if( pointInsideTriangle( left, center, right, p ) ) {
         return false;
      }
   }
         
   return true;
}

inline vbuf3f::iterator getNextVertex( vbuf3f::iterator begin,
                                       vbuf3f::iterator end,
                                       vbuf3f::iterator front)
{
   if(++front == end) {
      front = begin;
   }
         
   return front;
}

bool PolygonTriangulation::triangulate(vbuf3f& contour, vbuf3f& output)
{
   // Start processing
   vbuf3f::iterator begin = contour.begin();
   vbuf3f::iterator front = contour.begin();
   vbuf3f::iterator end = contour.end();
   
   unsigned int n = contour.size();
      
   if( n < 2) {
      return true;
   } else if(n == 2) {
      output.push_back( *front++ );
      output.push_back( *front++ );
      output.push_back( *begin );
      return true;
   } else if( n == 3) {
      output.push_back( *front++ );
      output.push_back( *front++ );
      output.push_back( *front++ );
      return true;
   } 

   vbuf3f::iterator left   = begin;
   vbuf3f::iterator center = left + 1;
   vbuf3f::iterator right  = center + 1;
   
   unsigned int count = 0;
   
   /* We will end up with n - 2 triangles, and 2 remaining vertices (n) */
   while( n > 2 ) {
      if( triangleCanBeRemoved( *left, *center, *right, begin, end) ) {         
         //Then add triangle.
         output.push_back( *left );
         output.push_back( *center );
         output.push_back( *right );
         
         --n;

         if(n > 2) {
            left = contour.erase(center);
            begin = contour.begin();
            end = contour.end();

            count = 0;
            
            if(left == end) {
               left = begin;
            }
            
            center = getNextVertex(begin, end, left);
            right  = getNextVertex(begin, end, center);            
         }
      } else {
         left = center;
         center = right;
         right = getNextVertex(begin, end, right);
         count++;
         
         if( count > n ) {
            // We've now wrapped around without removing a polygon.
            // If we continue iterating, we will get stuck in an infinite loop.
            return false;
         }
         
      }
   }
       
   return true;
}

