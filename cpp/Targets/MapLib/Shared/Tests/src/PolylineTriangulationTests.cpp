/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "config.h"
#include "PolylineTriangulationTests.h"
#include "Vector3d.h"
#include "PolylineTriangulation.h"
#include "TestHelpers.h"
#include "TestFramework.h"
#include <math.h>

namespace PolylineTriangulationTests {
using namespace PolylineTriangulation;

// template<typename StorageType>
// void testCase1Impl( TestReporter& reporter )
// {
//    NEW_TEST_FUNCTION( reporter );

//    typedef std::vector<Vector3dBase<StorageType> > VectorBuffer;
         
//    Vector3dBase<StorageType> p0 = getVector<StorageType>( 1.0f, 1.0f );
//    Vector3dBase<StorageType> p1 = getVector<StorageType>( 2.0f, 2.0f );
//    Vector3dBase<StorageType> p2 = getVector<StorageType>( 2.0f, 4.0f );

//    VectorBuffer contour;
   
//    contour.push_back(p0);
//    contour.push_back(p1);
//    contour.push_back(p2);
      
//    VectorBuffer output;
      
//    typename StorageType::Type width = StorageType::fromFloat( 1.0f );

//    std::vector<unsigned char> leftOrRight;
   
//    triangulate( contour.begin(), contour.end(),
//                 std::back_inserter( output ),
//                 std::back_inserter( leftOrRight ),
//                 width,
//                 StorageType::fromFloat(1.0f) );

//    CHECK( output[0].x == output[1].y, reporter );
//    CHECK( output[0].y == output[1].x, reporter );

//    using Vector3dMath::equalsVec;
      
//    CHECK( output[2].x == output[3].y, reporter );
//    CHECK( output[2].y == output[3].x, reporter );
      
//    CHECK( equalsVec( output[4], getVector<StorageType>( 1.0, 2.0 ) ), reporter );
//    CHECK( equalsVec( output[5], getVector<StorageType>( 3.0, 2.0 ) ), reporter );
//    CHECK( equalsVec( output[6], getVector<StorageType>( 1.0, 4.0 ) ), reporter );
//    CHECK( equalsVec( output[7], getVector<StorageType>( 3.0, 4.0 ) ), reporter );
// }

// void testCase1( TestReporter& reporter ) {
//    testCase1Impl<DoubleStorageType>( reporter );
//    testCase1Impl<FloatStorageType>( reporter );
//    testCase1Impl<FixedStorageType>( reporter );
// }

// template<typename StorageType>
// void testCase2Impl( TestReporter& reporter )
// {
//    // NEW_TEST_FUNCTION( reporter );
//    // typedef std::vector<Vector3dBase<StorageType> > VectorBuffer;
//    // Vector3dBase<StorageType> p0 = getVector<StorageType>( 0.0f, 0.0f );
//    // Vector3dBase<StorageType> p1 = getVector<StorageType>( 10.0f, 0.0f );
//    // Vector3dBase<StorageType> p2 = getVector<StorageType>( 0.0f, 10.0f );

//    // VectorBuffer contour;

//    // contour.push_back(p0);
//    // contour.push_back(p1);
//    // contour.push_back(p2);
   
//    // VectorBuffer output;
   
//    // typename StorageType::Type width = StorageType::fromFloat( 1.0f );

//    // triangulate( contour.begin(), contour.end(), std::back_inserter( output ),
//    //              width,
//    //              StorageType::fromFloat(1.0f) );
//    // for( unsigned int i = 0; i < output.size(); i++ ) {
//    //    std::cout << "output["<< i <<"] = " << output[i] << std::endl;
//    // }

// }

// void testCase2( TestReporter& reporter )
// {
//    testCase2Impl<DoubleStorageType>( reporter );
//    testCase2Impl<FloatStorageType>( reporter );
//    testCase2Impl<FixedStorageType>( reporter );
   
// }

// void testCase3( TestReporter& reporter )
// {
   
   
// }

void testCase4(TestReporter& reporter)
{
   NEW_TEST_FUNCTION(reporter);
   vec3f p0(0.0f, 0.0f);
   vec3f p1(0.0f, 1.0f);
   vec3f p2(2.0f, 2.0f);

   vbuf3f buf;
   vbuf3f result;
   buf.push_back(p0);
   buf.push_back(p1);
   buf.push_back(p2);
   PolylineTriangulation::triangulateQuick(buf, result, 1.0, 2.0);
      
}
} // End of namespace

