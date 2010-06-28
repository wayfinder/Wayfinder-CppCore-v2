/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef VECTOR3DMATH_H
#define VECTOR3DMATH_H

#include <math.h>
#include "Vector3d.h"
#include "FixedPointType.h"
#include "MathUtility.h"

namespace vmath {

/**
 *  Normalizes the three dimensional vector. Uses fast inverse square
 *  root to calculate length of vector.
 *
 */

// #define USE_FAST_NORMALIZE

static inline vec3f& normalize(vec3f& vec) {
   float sqSum =
      vec.x * vec.x +
      vec.y * vec.y +
      vec.z * vec.z;

#ifndef USE_FAST_NORMALIZE
   float length = sqrt(sqSum);
   
   vec.x /= length;
   vec.y /= length;
   vec.z /= length;
#else
   float invLength = MathUtility::fastInvSqrt(sqSum);
   
   vec.x *= invLength;
   vec.y *= invLength;
   vec.z *= invLength;
#endif
   
   return vec;
}

static inline float length(vec3f& vec) {
   return MathUtility::SQRT( vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

}

// /**
//  *  Returns the two dimensional cross product, which is a scalar and not a vector.
//  */
      
// template<class Vector>
// inline static typename Vector::Storage::Type
// crossProduct2d(const Vector& lhs, const Vector& rhs) {
//    return lhs.x * rhs.y - rhs.x * lhs.y;
// }
   
// /**
//  *  Algorithms for determining how consecutive lines turn. Input data can be
//  *  either points or lines. 
//  *  
//  *
//  *           v1 = p2 - p1
//  *           v2 = p3 - p2
//  *
//  *           p2 ----------- p3
//  *           |
//  *           |
//  *           |
//  *           |
//  *        p1 |
//  *
//  *    
//  *    @return >0 for P2 left of the line through P0 and P1
//  *            =0 for P2 on the line
//  *            <0 for P2 right of the line.
//  *
//  */

      
// enum TurnResult { TURN_RIGHT = -1 , TURN_NONE = 0, TURN_LEFT = 1 };
      
// template<class Vector>
// inline static TurnResult getTurningDirectionInternal(const Vector& v1, const Vector& v2) {
//    typedef typename Vector::Storage::Type StorageType;
//    StorageType res = crossProduct2d(v1, v2);
         
//    const static StorageType ZERO =     Vector::Storage::zero();
      
//    if( equals<typename Vector::Storage> ( res, ZERO ) ) {
//       return TURN_NONE;
//    } else if( res > ZERO ) {
//       return TURN_RIGHT;
//    } else {
//       return TURN_LEFT;
//    } 
// }
      
// template<class Vertex>
// inline static TurnResult getTurningDirection(const Vertex& p0,
//                                              const Vertex& p1,
//                                              const Vertex& p2)
// {
//    Vertex v1 = p1 - p0;
//    Vertex v2 = p2 - p0;

//    return getTurningDirectionInternal(v2, v1);
// }
      
// template<class Vector>
// inline static TurnResult getTurningDirection(Vector& v1, Vector& v2) {
//    Vector v3 = v1 + v2;
//    return getTurningDirectionInternal(v3, v1);
// }


// /**
//  *   Square-root helper functions.
//  *
//  */

// /**
//  *
//  *    Returns the length of a vector. 
//  *
//  *    @param  vec  The vector to operate on.    
//  */ 
   
// template<typename Vector>
// inline static typename Vector::Storage::Type length(const Vector& vec) {
//    typename Vector::Storage::UpperType x(vec.x);
//    typename Vector::Storage::UpperType y(vec.y);
//    typename Vector::Storage::UpperType z(vec.z);

//    typename Vector::Storage::UpperType sq = sqrt( x * x + y * y + z * z );
      
//    return typename Vector::Storage::Type( sq );
// }

// /**
//  *    A perpendicular 3d-vector on the 2d x-y plane is
//  *    defined as (-y, x,  z).
//  *
//  *    @param vec The vector to be made perpendicular.
//  *    
//  */

   
// template<class Vector>
// inline static void perpVector(Vector& vec)
// {
//    vec.x = -vec.x;
//    std::swap(vec.x, vec.y);         
// }


// /**
//  *    Normalizes a vector ( sets it's length to one ).
//  *
//  *    @param vec The vector to be normalized.
//  *
//  */    
   
// template<typename Vector>
// inline static void normalize(Vector& vec) {
//    typename Vector::Storage::Type l =
//       Vector::Storage::one() / Vector3dMath::length(vec);
      
//    vec.x *= l;
//    vec.y *= l; 
//    vec.z *= l;
// }

// /**
//  *    Normalizes a vector ( sets it's length to one ).
//  *
//  *    @param vec The vector to be normalized.
//  *
//  */    
   
// template<>
// inline static void normalize(Vector3dBase<FixedStorageType>& vec) {      

//    // fixedp64 x(vec.x);
//    // fixedp64 y(vec.y);
//    // fixedp64 z(vec.z);

//    // fixedp64 l = sqrt( x * x + y * y + z * z );

//    // fixedp64 tempX = fixedp64(vec.x) / l;
//    // fixedp64 tempY = fixedp64(vec.y) / l;
//    // fixedp64 tempZ = fixedp64(vec.z) / l;
      
//    // vec.x = fixedp(tempX);
//    // vec.y = fixedp(tempY); 
//    // vec.z = fixedp(tempZ);
// }

   
// /**
//  *   Returns the perpendicular normalized vector of vec.
//  *
//  *   @param vec The vector to be normalized and made perpendicular.-
//  *   
//  */ 
   
// template<class Vector>
// inline static void perpNormVector(Vector& vec)
// {
//    normalize(vec);
//    perpVector(vec);
// }



// /**
//  *    Calculates the intersection between the lines formed by
//  *    p1p2 and p3p4, if such an intersection exists.
//  *    
//  *
//  *    @param p1               First point of first line
//  *    @param p2               Second point of first line
//  *    @param p3               First point of second line
//  *    @param p4               Second point of second line
//  *    @param intersection     Intersection point
//  *
//  *    @ret                    If the lines are parallel, LINE_PARALLEL will
//  *                            be returned. IF they are not intersecting,
//  *                            LINE_NON_INTERSECTING will be returned. In
//  *                            both cases, intersection will not be set. If
//  *                            on the other hand LINE_INTERSECTING is
//  *                            returned, an intersection was found and the
//  *                            intersection point will be set accordingly.
//  *
//  */
   
// enum IntersectionResult { LINE_PARALLEL, LINE_NON_INTERSECTING, LINE_INTERSECTING };
      
// template<class Point>
// inline static IntersectionResult get2dIntersection(const Point& p1, const Point& p2,
//                                                    const Point& p3, const Point& p4,
//                                                    Point& intersection)
// {
//    /**
//     *  The variables are defined as such:
//     *
//     *  l = line between p1 and p2
//     *  s = line between p3 and p4
//     * 
//     *  l.x = p0.x + t*alpha
//     *  l.y = p0.y + t*beta
//     *
//     *  s.x = p2.x + s*gamma
//     *  s.y = p2.y + s*theta
//     */ 

//    /**
//     * alpha, beta, gamma and theta are the gradients of the lines.
//     * */

//    typedef typename Point::Storage::Type StorageType;
//    typedef typename Point::Storage::UpperType UpperStorageType;

      
//    static const StorageType ZERO = Point::Storage::zero();
//    static const StorageType ONE  = Point::Storage::one();
      
//    StorageType alpha   = p2.x - p1.x;
//    StorageType beta    = p2.y - p1.y;
//    StorageType gamma   = p4.x - p3.x;
//    StorageType theta   = p4.y - p3.y;
         
//    StorageType denominator = theta*alpha - gamma*beta;
         
//    if( equals<typename Point::Storage>( denominator, ZERO ) ) {
//       return LINE_PARALLEL;
//    }

//    StorageType t = ( ( p3.x - p1.x * theta + p1.y - p3.y) * gamma ) / denominator;   
//    StorageType s = ( ( p1.y - p3.y * alpha + p3.x - p1.x) * beta ) / denominator;

//    if( s < ZERO || s > ONE || t < ZERO || t > ONE )
//    {            
//       return LINE_NON_INTERSECTING;  
//    }
      
//    intersection.x = p3.x + StorageType(s * gamma);
//    intersection.y = p3.y + StorageType(s * theta);
   
//    return LINE_INTERSECTING;
// }

// }
#endif /* VECTOR3DMATH_H */

   
