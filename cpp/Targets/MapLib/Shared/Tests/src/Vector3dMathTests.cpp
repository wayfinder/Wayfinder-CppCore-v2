/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "Vector3dMathTests.h"
#include "TestFramework.h"
#include "Vector3d.h"
#include "Vector3dMath.h"
#include "TestHelpers.h"


namespace Vector3dMathTests {
   
//    void testEquality( TestReporter& reporter )
//    {
//       NEW_TEST_FUNCTION( reporter );
   
//       double d1 = 50.0;
//       double d2 = 100 * 0.5;

//       CHECK( Vector3dMath::equals<DoubleStorageType>( d1, d2 ), reporter );

//       d1 = 0.0;
//       d2 = DoubleStorageType::precision();

//       CHECK( !Vector3dMath::equals<DoubleStorageType>( d1, d2 ), reporter );
   
//       // fixedp fx1 = fixedp::fromInt( 50 );
//       // fixedp fx2 = fixedp::fromInt( 100 ) * fixedp::fromDouble( 0.5 );

//       // CHECK( Vector3dMath::equals<FixedStorageType>( fx1, fx2 ), reporter );
   
//       // fx2 += FixedStorageType::precision();
   
//       // CHECK( !Vector3dMath::equals<FixedStorageType>( fx1, fx2 ), reporter );
   
//       float fp1 = 50.0f;
//       float fp2 = 100.0f * 0.5f;
   
//       CHECK( Vector3dMath::equals<FloatStorageType>( fp1, fp2 ), reporter );

//       fp1 = 0.0f;
//       fp2 = FloatStorageType::precision();

//       CHECK( !Vector3dMath::equals<FloatStorageType>( fp1, fp2 ), reporter );
   
//    }
   
//    template<typename StorageType>
//    void testIntersectionInternal(TestReporter& reporter) {
//       NEW_TEST_FUNCTION( reporter );
      
//       Vector3dBase<StorageType> p1 = getVector<StorageType>(0.0f, 0.0f);
//       Vector3dBase<StorageType> p2 = getVector<StorageType>(1.0f, 1.0f);
//       Vector3dBase<StorageType> p3 = getVector<StorageType>(0.0f, 1.0f);
//       Vector3dBase<StorageType> p4 = getVector<StorageType>(1.0f, 0.0f);

//       Vector3dBase<StorageType> intersectionPoint;

//       /*   Two basic intersecting lines. They intersect at (0.5f, 0.5f)
//        *
//        *    p3 ...            _/ p2
//        *         ...       --/
//        *            ..   --/
//        *              --/.
//        *           --/   ..
//        *         --/       ..
//        *       __/           ....
//        *    p1                   p4
//        *
//        */

//       Vector3dMath::IntersectionResult vr =
//          Vector3dMath::get2dIntersection( p1, p2, p3, p4,
//                                                 intersectionPoint);

//       CHECK( vr == Vector3dMath::LINE_INTERSECTING, reporter );

//       CHECK( Vector3dMath::equals<StorageType>(intersectionPoint.x,
//                                                StorageType::fromFloat( 0.5f ) ),
//              reporter);
             
//       CHECK( Vector3dMath::equals<StorageType>(intersectionPoint.y,
//                                                StorageType::fromFloat( 0.5f )),
//              reporter);

//       /*   Two very non-intersecting lines.
//        *
//        *   p4             p3       p2
//        *   ----------------      / 
//        *                        / 
//        *                       / 
//        *                      / 
//        *                     / 
//        *                    / 
//        *                   /
//        *               p1 /
//        *
//        */

//       p4 = getVector<StorageType>(-1.0f, 1.0f);

//       vr = Vector3dMath::get2dIntersection( p1, p2, p3, p4, intersectionPoint);

//       CHECK( vr == Vector3dMath::LINE_NON_INTERSECTING, reporter );

//       p3 = p1;
//       p4 = p2;


//       /*    Two parallel lines (equal to each-other in this case)
//        *
//        *                         
//        *                   p2, p4
//        *                 /
//        *                /
//        *               /
//        *              /
//        *             /
//        *            /
//        *    p1, p3 /
//        */


//       vr = Vector3dMath::get2dIntersection( p1, p2, p3, p4, intersectionPoint);
//       CHECK( vr == Vector3dMath::LINE_PARALLEL, reporter );
//    }

//    template<typename StorageType>
//    void testIntersectionComplicatedInternal(TestReporter& reporter) {
//       NEW_TEST_FUNCTION( reporter );
      
//       Vector3dBase<StorageType> p1 = getVector<StorageType>( -1.3f, -2.7f );
//       Vector3dBase<StorageType> p2 = getVector<StorageType>( 2.7f, 1.3f );
//       Vector3dBase<StorageType> p3 = getVector<StorageType>( -32.0f, -1.0f);
//       Vector3dBase<StorageType> p4 = getVector<StorageType>( -2.0f, -1.0f );

//       Vector3dBase<StorageType> intersectionPoint;

//       Vector3dMath::IntersectionResult vr =
//          Vector3dMath::get2dIntersection( p1, p2, p3, p4, intersectionPoint);
   
//       CHECK( vr == Vector3dMath::LINE_NON_INTERSECTING, reporter );
//    }



//    template<typename StorageType>
//    void testTurningInternal( TestReporter& reporter ) {
//       NEW_TEST_FUNCTION( reporter );
      
//       /*      Sharp turn right at p2.
//        *
//        *
//        *           p2----------------- p3
//        *           |
//        *           |
//        *           |
//        *           |
//        *           |
//        *           |
//        *           |
//        *       p1  |
//        *
//        */

//       Vector3dBase<StorageType> p1 = getVector<StorageType>(0.0f, 0.0f);
//       Vector3dBase<StorageType> p2 = getVector<StorageType>(0.0f, 1.0f);
//       Vector3dBase<StorageType> p3 = getVector<StorageType>(1.0f, 1.0f);

//       Vector3dBase<StorageType> v1 = p2 - p1;
//       Vector3dBase<StorageType> v2 = p3 - p2;

//       Vector3dMath::TurnResult tr = Vector3dMath::getTurningDirection(v1, v2);
      
//       CHECK( tr == Vector3dMath::TURN_RIGHT, reporter );

//       /*      Sharp turn left at p2.
//        *
//        *
//        *      p3   -------------------- p2
//        *                                |
//        *                                |
//        *                                |
//        *                                |
//        *                                |
//        *                                |
//        *                                |
//        *                                | 
//        *                             p1 |     
//        */
   
//       p3 = getVector<StorageType>(-1.0f, 1.0f);
//       v2 = p3 - p2;
   
//       tr = Vector3dMath::getTurningDirection(v1, v2);
//       CHECK( tr == Vector3dMath::TURN_LEFT, reporter );
//    }

//    void testIntersectionComplicated( TestReporter& reporter )
//    {
//       testIntersectionComplicatedInternal<FloatStorageType>( reporter );
//       // testIntersectionComplicatedInternal<FixedStorageType>( reporter );
//       testIntersectionComplicatedInternal<DoubleStorageType>( reporter );
//    }
   
//    void testIntersection( TestReporter& reporter )
//    {
//       testIntersectionInternal<FloatStorageType>( reporter );
//       // testIntersectionInternal<FixedStorageType>( reporter );
//       testIntersectionInternal<DoubleStorageType>( reporter );
//    }

//    void testTurning( TestReporter& reporter )
//    {
//       testTurningInternal<FloatStorageType>( reporter );
//       // testTurningInternal<FixedStorageType>( reporter );
//       testTurningInternal<DoubleStorageType>( reporter );
//    }

}




