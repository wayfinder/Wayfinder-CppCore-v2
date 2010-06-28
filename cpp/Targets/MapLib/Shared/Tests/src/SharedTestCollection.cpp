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
#include "SharedTestCollection.h"
#include "Vector3dTests.h"
#include "Vector3dMathTests.h"
#include "FixedPointTypeTests.h"
#include "PolygonTriangulationTests.h"
#include "PolylineTriangulationTests.h"
#include "PositionInterpolatorTests.h"
#include "PlotterAdapter3dTests.h"
#include "TimeHelpersTests.h"
#include "InterpolationMathTests.h"
#include "StackedOverlapTests.h"
#include "IntervalInterpolatorTests.h"

void SharedTestCollection::runAllTests( TestReporter& reporter )
{
   Vector3dTests::testInitialization( reporter );
   Vector3dTests::testArithmetic( reporter );
   // Vector3dMathTests::testEquality( reporter );
   // Vector3dMathTests::testIntersection( reporter );
   // Vector3dMathTests::testIntersectionComplicated( reporter );
   // Vector3dMathTests::testTurning( reporter );

   PositionInterpolatorTests::testConstantSpeed( reporter );
   PositionInterpolatorTests::testIncreasingSpeed( reporter );
   PositionInterpolatorTests::testDecreasingSpeed( reporter );
   PositionInterpolatorTests::testExtremes( reporter );
   PositionInterpolatorTests::testCatchUp( reporter );
   PositionInterpolatorTests::testCatchUpDirection( reporter );
   PositionInterpolatorTests::testTraversedDistance( reporter );

   InterpolationMathTests::testAngleCalculations( reporter );

   IntervalInterpolatorTests::testSimple( reporter );
   IntervalInterpolatorTests::testHeights( reporter );
   IntervalInterpolatorTests::testOneSided( reporter );
   
   FixedPointTypeTests::testSqrt( reporter );
   FixedPointTypeTests::testArithmetic( reporter );

   PolygonTriangulationTests::testBaseTriangulation( reporter );
   PolygonTriangulationTests::testRectTriangulation( reporter );
   PolygonTriangulationTests::testConvexTriangulation( reporter );
   PolygonTriangulationTests::testHodgmanPolygon ( reporter );
   PolygonTriangulationTests::testComplicatedPolygon( reporter );
   PolygonTriangulationTests::testRealWorldSplitPolygon( reporter );
   PolygonTriangulationTests::testRealWorldPolygon( reporter );
   PlotterAdapter3dTests::testCompareFloatAndInteger( reporter );

   // PolylineTriangulationTests::testCase1( reporter );
   // PolylineTriangulationTests::testCase2( reporter );
   // PolylineTriangulationTests::testCase3( reporter );
   PolylineTriangulationTests::testCase4( reporter );
   TimeHelpersTests::testRunningPeriod( reporter );

   StackedOverlapTests::test1(reporter);
}
