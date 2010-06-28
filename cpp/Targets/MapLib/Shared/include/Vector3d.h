/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _VECTOR3D_H_
#define _VECTOR3D_H_

#include <vector>
#include "Vector3dStorage.h"
#include "FixedPointType.h"


template<typename StorageType>
class Vector3dBase {
public:
   typedef StorageType Storage;

   typename Storage::Type x, y, z;

   Vector3dBase( const Vector3dBase& rhs) : x(rhs.x), y(rhs.y), z(rhs.z) {}

   Vector3dBase(typename Storage::Type x,
                typename Storage::Type y,
                typename Storage::Type z) : x(x), y(y), z(z) {}

   Vector3dBase(typename Storage::Type x,
                typename Storage::Type y) : x(x),
                                            y(y),
                                            z(0) {}

   enum VectorTraits { NUM_ELEMENTS = 3 };
   
   Vector3dBase() {
      x = y = z = Storage::zero();
   }

   Vector3dBase&
   operator=(const Vector3dBase& rhs) {
      x = rhs.x;
      y = rhs.y;
      z = rhs.z;
      
      return *this;
   }
   
   Vector3dBase& operator+=(const Vector3dBase &rhs) {
      x += rhs.x;
      y += rhs.y;
      z += rhs.z;
   
      return *this;
   }
   
   Vector3dBase operator+(const Vector3dBase &rhs) const {
      Vector3dBase ret = Vector3dBase(x, y, z);
   
      ret.x += rhs.x;
      ret.y += rhs.y;
      ret.z += rhs.z;
      
      return ret;
   }
   
   Vector3dBase operator-(const Vector3dBase &rhs) const {
      Vector3dBase ret = Vector3dBase(x, y, z);
      
      ret.x -= rhs.x;
      ret.y -= rhs.y;
      ret.z -= rhs.z;
      
      return ret;
   }
   
   Vector3dBase operator*(const Vector3dBase& rhs) const {
      return Vector3dBase(x * rhs.x, y * rhs.y, z * rhs.z);
   }
   
   Vector3dBase operator*(const typename Storage::Type rhs) const {
      return Vector3dBase(x * rhs, y * rhs, z * rhs);
   }
   
   Vector3dBase operator/(const typename Storage::Type rhs) const {
      return Vector3dBase(x / rhs, y / rhs, z / rhs);
   }
   
   class XYHelper {
   public:
      template<typename Storage>
      inline typename Storage::Type
      getX( const Vector3dBase<Storage>& coord ) const {
         return coord.x;
      }

      template<typename Storage>
      inline typename Storage::Type
      getY( const Vector3dBase<Storage>& coord ) const {
         return coord.y;
      }
   };
};

template<typename Storage>
bool operator==(const Vector3dBase<Storage>& lhs,
                const Vector3dBase<Storage>& rhs)
{
   return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
}

template<typename Storage>
bool operator!=(const Vector3dBase<Storage>& lhs,
                const Vector3dBase<Storage>& rhs)
{
   return lhs.x != rhs.x || lhs.y != rhs.y || lhs.z != rhs.z;
}


/* Imposes strict weak ordering. */
template<typename Storage>
bool operator<(const Vector3dBase<Storage>& lhs,
               const Vector3dBase<Storage>& rhs)
{
   if( lhs.x < rhs.x ) {
      return true;
   } else if( lhs.x > rhs.x ) {
      return false;
   }

   if( lhs.y < rhs.y ) {
      return true;
   } else if( lhs.y > rhs.y ) {
      return false;
   }

   return lhs.z < rhs.z;
}


template<typename Storage>
ostream& operator << (ostream& oStream, const Vector3dBase<Storage>& vector) {
   oStream << "( " << vector.x << ", " << vector.y << ", " << vector.z << " )";
   
   return oStream;
}


typedef Vector3dBase<FixedStorageType> vec3x;
typedef Vector3dBase<FloatStorageType> vec3f;
typedef Vector3dBase<DoubleStorageType> vec3d;

#define VEC_3D_USE_COLOR_BUFFER

struct vec4x {
   vec3x coord;
#ifdef VEC_3D_USE_COLOR_BUFFER
   unsigned int color;
#endif
};

typedef std::vector<vec3x> vbuf3x;
typedef std::vector<vec3f> vbuf3f;
typedef std::vector<vec3d> vbuf3d;
typedef std::vector<vec4x> vbuf4x;


#endif /* _VECTOR3D_H_ */
