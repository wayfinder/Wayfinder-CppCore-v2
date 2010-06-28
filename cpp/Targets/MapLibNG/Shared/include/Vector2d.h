/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef VECTOR2D_H
#define VECTOR2D_H

#include "FixedPointType.h"
#include <vector>
#include "Vector3d.h"
#include "VertexEnvironment.h"

class Vector2d {
public:
   Vector2d( fixedp x, fixedp y ) : x(x), y(y) { }
   Vector2d() : x(0), y(0) {}
   Vector2d( const vec3x& rhs ) : x( rhs.x ), y( rhs.y ) { }
   Vector2d( const Vector2d& rhs ) : x( rhs.x ), y( rhs.y ) { }
   Vector2d& operator=( const Vector2d& rhs ) {
         x = rhs.x;
         y = rhs.y;
         return *this;
      }
   fixedp x, y;

   enum VectorTraits { NUM_ELEMENTS = 2 };
};

// typedef std::vector<Vector2d> VertexBuffer;


#endif /* VECTOR2D_H */
