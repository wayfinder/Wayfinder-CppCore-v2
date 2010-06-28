/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef POLYGONTRIANGULATIONIMPL_H
#define POLYGONTRIANGULATIONIMPL_H

#include "InsideUtil.h"

namespace PolygonTriangulation {
   
   template<typename Vertex, typename VertexIterator>
   static bool triangleCanBeRemoved(Vertex& left,
                                    Vertex& center,
                                    Vertex& right,
                                    VertexIterator begin,
                                    VertexIterator end)
   {
      typename Vertex::XYHelper hlp;
   
      /* If we make a left turn when going from center to right, the
       * triangle is concave and thus not suitable for removal. */
      
      if( InsideUtil::isLeft( left, center, right, hlp ) > 0 )  { 
         return false;
      }

      
      for(; begin != end; begin++) {
         Vertex& p = *begin;

         if( p == left || p == center || p == right) {
            continue;
         }

         if( InsideUtil::pointInsideTriangle( left, center, right, p, hlp ) ) {
            return false;
         }
      }
         
      return true;
   }

   template<typename VertexIterator>
   static VertexIterator getNextVertex( VertexIterator begin,
                                        VertexIterator end,
                                        VertexIterator front) {
      if(++front == end) {
         front = begin;
      }
         
      return front;
   }

   template<typename VertexBufferType>
   bool triangulatePolygon( VertexBufferType& contour,
                            VertexBufferType& output ) {

      typename VertexBufferType::iterator begin = contour.begin();
      typename VertexBufferType::iterator end = contour.end();
   
      unsigned int n = static_cast<unsigned int>( std::distance(begin, end) );

      typename VertexBufferType::iterator front = begin;
      
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
      
      typename VertexBufferType::iterator left   = begin;
      typename VertexBufferType::iterator center = left + 1;
      typename VertexBufferType::iterator right  = center + 1;

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
               /* Alternative way: 
                  left = begin;
                  center = left + 1;
                  right = center + 1;

                  determine if this is faster. could screw up locality of reference,
                  but on the other hand, less logic needs to be performed.
               */
            
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
               //We've now wrapped around without removing a polygon. Must be
               //badness..
               return false;
            }
         
         }
      }
         
      return true;
   }   

}

#endif /* POLYGONTRIANGULATIONIMPL_H */
