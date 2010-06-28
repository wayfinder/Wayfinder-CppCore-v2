/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "TestFramework.h"
#include "PolygonTriangulationTests.h"
#include "TestHelpers.h"
#include "PolygonTriangulation.h"
#include <algorithm>

namespace PolygonTriangulationTests {

   template<typename StorageType>
   void testBaseTriangulationImpl(TestReporter& reporter) {
      NEW_TEST_FUNCTION(reporter);
      Vector3dBase<StorageType> p0 = getVector<StorageType>(0.0f, 0.0f);
      Vector3dBase<StorageType> p1 = getVector<StorageType>(0.0f, 1.0f);
      Vector3dBase<StorageType> p2 = getVector<StorageType>(1.0f, 1.0f);
      Vector3dBase<StorageType> offs = getVector<StorageType>(0.0f, 0.0f);
      
      std::vector<Vector3dBase<StorageType> > contour;

      contour.push_back(p0);
      contour.push_back(p1);
      contour.push_back(p2);

      std::reverse(contour.begin(), contour.end());
   
      std::vector<Vector3dBase<StorageType> > polygon;

      typename Vector3dBase<StorageType>::XYHelper xyHelper;

      CHECK(PolygonTriangulation::triangulate( contour.begin(), contour.end(),
                                               offs,
                                               xyHelper,
                                               polygon ),
            reporter);
   
      CHECK( polygon.size() == 3, reporter);
   
      CHECK( polygon[0] == contour[0], reporter );
      CHECK( polygon[1] == contour[1], reporter );
      CHECK( polygon[2] == contour[2], reporter );
   }

   void testBaseTriangulation(TestReporter& reporter) {
      testBaseTriangulationImpl<DoubleStorageType>(reporter);
      // testBaseTriangulationImpl<FixedStorageType>(reporter);
      testBaseTriangulationImpl<FloatStorageType>(reporter);
   }

   template<typename StorageType>
   void testRectTriangulationImpl(TestReporter& reporter) {
      NEW_TEST_FUNCTION( reporter );
      
      Vector3dBase<StorageType> p0 = getVector<StorageType>(1.0f, 0.0f);
      Vector3dBase<StorageType> p1 = getVector<StorageType>(1.0f, 1.0f);
      Vector3dBase<StorageType> p2 = getVector<StorageType>(-1.0f, 0.0f);
      Vector3dBase<StorageType> p3 = getVector<StorageType>(-1.0f, -1.0f);
      Vector3dBase<StorageType> offs = getVector<StorageType>(0.0f, 0.0f);
      
      std::vector<Vector3dBase<StorageType> > contour;

      contour.push_back(p0);
      contour.push_back(p1);
      contour.push_back(p2);
      contour.push_back(p3);

      std::reverse(contour.begin(), contour.end());
   
      std::vector<Vector3dBase<StorageType> > polygon;

      typename Vector3dBase<StorageType>::XYHelper xyHelper;
      
      CHECK(PolygonTriangulation::triangulate( contour.begin(), contour.end(),
                                               offs,
                                               xyHelper, polygon ), reporter);

      std::reverse(polygon.begin(), polygon.end());
   
      CHECK( polygon.size() == 2*3, reporter);

      CHECK( polygon[0] == contour[0], reporter );
      CHECK( polygon[1] == contour[3], reporter );
      CHECK( polygon[2] == contour[2], reporter );      
      CHECK( polygon[3] == contour[2], reporter );
      CHECK( polygon[4] == contour[1], reporter );
      CHECK( polygon[5] == contour[0], reporter );
   }


   void testRectTriangulation(TestReporter& reporter) {
      testRectTriangulationImpl<DoubleStorageType>( reporter );
      // testRectTriangulationImpl<FixedStorageType>( reporter );
      testRectTriangulationImpl<FloatStorageType>( reporter );
   }

   template<typename StorageType>   
   void testConvexTriangulationImpl(TestReporter& reporter)
   {
      NEW_TEST_FUNCTION( reporter );
      Vector3dBase<StorageType> p0 = getVector<StorageType>( 1.0f, 0.0f );
      Vector3dBase<StorageType> p1 = getVector<StorageType>( 0.707f, 0.707f );
      Vector3dBase<StorageType> p2 = getVector<StorageType>( 0.0f, 1.0f );
      Vector3dBase<StorageType> p3 = getVector<StorageType>( -0.707f, 0.707f );
      Vector3dBase<StorageType> p4 = getVector<StorageType>( -1.0f, 0.0f );
      Vector3dBase<StorageType> p5 = getVector<StorageType>( -0.707f, -0.707f );
      Vector3dBase<StorageType> p6 = getVector<StorageType>( 0.0f, -1.0f );
      Vector3dBase<StorageType> p7 = getVector<StorageType>( 0.707f, -0.707f );
      Vector3dBase<StorageType> offs = getVector<StorageType>(0.0f, 0.0f);
      
      std::vector<Vector3dBase<StorageType> > contour;
   
      contour.push_back(p0);
      contour.push_back(p1);
      contour.push_back(p2);
      contour.push_back(p3);
      contour.push_back(p4);
      contour.push_back(p5);
      contour.push_back(p6);
      contour.push_back(p7);

      std::reverse(contour.begin(), contour.end());
   
      std::vector<Vector3dBase<StorageType> > polygon;

      typename Vector3dBase<StorageType>::XYHelper xyHelper;
      
      CHECK(PolygonTriangulation::triangulate( contour.begin(), contour.end(),
                                               offs,
                                               xyHelper, polygon ), reporter );

      std::reverse(polygon.begin(), polygon.end());
   
      CHECK( polygon.size() == 6*3, reporter );

      CHECK( polygon[0] == p7, reporter );
      CHECK( polygon[1] == p1, reporter );
      CHECK( polygon[2] == p3, reporter  );
   
      CHECK( polygon[3] == p3, reporter  );
      CHECK( polygon[4] == p5, reporter );
      CHECK( polygon[5] == p7, reporter  );    

      CHECK( polygon[6] == p7, reporter  );
      CHECK( polygon[7] == p0 , reporter );
      CHECK( polygon[8] == p1, reporter  );    

      CHECK( polygon[9] == p1, reporter  );
      CHECK( polygon[10] == p2, reporter );
      CHECK( polygon[11] == p3, reporter  );    

      CHECK( polygon[12] == p3, reporter  );
      CHECK( polygon[13] == p4, reporter );
      CHECK( polygon[14] == p5, reporter  );
   
      CHECK( polygon[15] == p5, reporter  );
      CHECK( polygon[16] == p6 , reporter );
      CHECK( polygon[17] == p7, reporter  );
   }

   void testConvexTriangulation(TestReporter& reporter) {
      testConvexTriangulationImpl<DoubleStorageType>( reporter );
      // testConvexTriangulationImpl<FixedStorageType>( reporter );
      testConvexTriangulationImpl<FloatStorageType>( reporter );
   }

   template<typename StorageType>   
   void testHodgmanPolygonImpl(TestReporter& reporter)
   {
      NEW_TEST_FUNCTION( reporter );
      Vector3dBase<StorageType> p0 = getVector<StorageType>( 0.0f, 0.0f );
      Vector3dBase<StorageType> p1 = getVector<StorageType>( 0.0f, 4.0f );
      Vector3dBase<StorageType> p2 = getVector<StorageType>( 1.0f, 3.0f );
      Vector3dBase<StorageType> p3 = getVector<StorageType>( 0.0f, 2.0f );
      Vector3dBase<StorageType> p4 = getVector<StorageType>( 0.0f, 1.0f );
      Vector3dBase<StorageType> p5 = getVector<StorageType>( 1.0f, 0.0f );
      Vector3dBase<StorageType> offs = getVector<StorageType>(0.0f, 0.0f);
      
      std::vector<Vector3dBase<StorageType> > contour;
   
      contour.push_back(p0);
      contour.push_back(p1);
      contour.push_back(p2);
      contour.push_back(p3);
      contour.push_back(p4);
      contour.push_back(p5);

      std::vector<Vector3dBase<StorageType> > polygon;

      typename Vector3dBase<StorageType>::XYHelper xyHelper;
      
      CHECK(PolygonTriangulation::triangulate( contour.begin(), contour.end(),
                                               offs,
                                               xyHelper,
                                               polygon ), reporter);
   }

   void testHodgmanPolygon(TestReporter& reporter) {
      testHodgmanPolygonImpl<DoubleStorageType>( reporter );
      testHodgmanPolygonImpl<FloatStorageType>( reporter );
      // testHodgmanPolygonImpl<FixedStorageType>( reporter );
   }

   template<typename StorageType>
   void testComplicatedPolygonImpl(TestReporter& reporter)
   {
      NEW_TEST_FUNCTION( reporter );
      Vector3dBase<StorageType> p0 = getVector<StorageType>( 0.0f, 0.0f );
      Vector3dBase<StorageType> p1 = getVector<StorageType>( 1.0f, 0.0f );
      Vector3dBase<StorageType> p2 = getVector<StorageType>( 1.0f, 1.0f );
      Vector3dBase<StorageType> p3 = getVector<StorageType>( 2.0f, 1.0f );
      Vector3dBase<StorageType> p4 = getVector<StorageType>( 2.0f, 2.0f );
      Vector3dBase<StorageType> p5 = getVector<StorageType>( 0.0f, 2.0f );
      Vector3dBase<StorageType> offs = getVector<StorageType>(0.0f, 0.0f);
      
      std::vector<Vector3dBase<StorageType> > contour;
   
      contour.push_back(p0);
      contour.push_back(p1);
      contour.push_back(p2);
      contour.push_back(p3);
      contour.push_back(p4);
      contour.push_back(p5);

      std::reverse(contour.begin(), contour.end());
   
      std::vector<Vector3dBase<StorageType> > polygon;


      typename Vector3dBase<StorageType>::XYHelper xyHelper;
      
      CHECK(PolygonTriangulation::triangulate( contour.begin(), contour.end(),
                                               offs,
                                               xyHelper, polygon ), reporter );
      
      std::reverse(polygon.begin(), polygon.end());
   
      CHECK( polygon.size() == 4*3, reporter);
   
      CHECK( polygon[0] == p5, reporter );
      CHECK( polygon[1] == p0, reporter );
      CHECK( polygon[2] == p2, reporter );
   
      CHECK( polygon[3] == p2, reporter );
      CHECK( polygon[4] == p3, reporter );
      CHECK( polygon[5] == p5, reporter );    

      CHECK( polygon[6] == p0, reporter );
      CHECK( polygon[7] == p1, reporter );
      CHECK( polygon[8] == p2, reporter );    

      CHECK( polygon[9] == p3, reporter );
      CHECK( polygon[10] == p4, reporter );
      CHECK( polygon[11] == p5, reporter );
   }

   void testComplicatedPolygon(TestReporter& reporter) {
      testComplicatedPolygonImpl<DoubleStorageType>( reporter );
      testComplicatedPolygonImpl<FloatStorageType>( reporter );
      // testComplicatedPolygonImpl<FixedStorageType>( reporter );
   }

   template<typename StorageType>
   void testRealWorldPolygonImpl(TestReporter& reporter) {
      NEW_TEST_FUNCTION( reporter );
      std::vector<Vector3dBase<StorageType> > contour;
      Vector3dBase<StorageType> offs = getVector<StorageType>(0.0f, 0.0f);
      
      contour.push_back( getVector<StorageType> ( 4.0f, 8.0f )  );
      contour.push_back( getVector<StorageType> ( 9.0f, 4.0f )  );
      contour.push_back( getVector<StorageType> ( 13.0f, 3.0f ) );
      contour.push_back( getVector<StorageType> ( 13.0f, 2.0f ) );
      contour.push_back( getVector<StorageType> ( 3.0f, 4.0f )  );
      contour.push_back( getVector<StorageType> ( 1.0f, 4.0f )  );
      contour.push_back( getVector<StorageType> ( 1.0f, 8.0f )  );

      std::vector<Vector3dBase<StorageType> > polygon;

      typename Vector3dBase<StorageType>::XYHelper xyHelper;
      
      CHECK(PolygonTriangulation::triangulate( contour.begin(), contour.end(),
                                               offs,
                                               xyHelper, polygon ), reporter );
   }

   void testRealWorldPolygon(TestReporter& reporter) {
      testRealWorldPolygonImpl<FloatStorageType>( reporter );
      testRealWorldPolygonImpl<DoubleStorageType>( reporter );
      // testRealWorldPolygonImpl<FixedStorageType>( reporter );
   }

   template<typename StorageType>
   void testRealWorldSplitPolygonImpl( TestReporter& reporter ) {
      NEW_TEST_FUNCTION( reporter );
      
      std::vector<Vector3dBase<StorageType> > contour;

      contour.push_back( getVector<StorageType>( 20.00f,  20.0f  ) );
      contour.push_back( getVector<StorageType>( 20.00f,  1.0f  ) );
      contour.push_back( getVector<StorageType>( 15.0f,   3.0f  ) );
      contour.push_back( getVector<StorageType>( 5.0f,   0.0f ) );
      contour.push_back( getVector<StorageType>( 1.0f,   4.0f  ) );
      contour.push_back( getVector<StorageType>( 0.0f,   5.0f  ) );
      contour.push_back( getVector<StorageType>( 3.0f,   6.0f  ) );
      contour.push_back( getVector<StorageType>( 10.0f,   10.0f  ) );
      contour.push_back( getVector<StorageType>( 12.0f,   14.0f  ) );
      contour.push_back( getVector<StorageType>( 16.0f,   16.0f  ) );
      Vector3dBase<StorageType> offs = getVector<StorageType>(0.0f, 0.0f);
      
      std::vector<Vector3dBase<StorageType> > polygon;

      typename Vector3dBase<StorageType>::XYHelper xyHelper;
      
      CHECK(PolygonTriangulation::triangulate( contour.begin(), contour.end(),
                                               offs,
                                               xyHelper,
                                               polygon ), reporter );
   }


   void testRealWorldSplitPolygon( TestReporter& reporter ) {
      testRealWorldSplitPolygonImpl<DoubleStorageType>( reporter );
      testRealWorldSplitPolygonImpl<FloatStorageType>( reporter );
      // testRealWorldSplitPolygonImpl<FixedStorageType>( reporter );
   }
}
