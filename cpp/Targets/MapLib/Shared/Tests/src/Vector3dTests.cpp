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
#include "Vector3dTests.h"
#include "Vector3d.h"
#include "Vector3dMath.h"

void Vector3dTests::testInitialization( TestReporter& reporter )
{
   NEW_TEST_FUNCTION( reporter );

   // Vector3d vFixed( FixedStorageType::one(),
   //                  FixedStorageType::zero(),
   //                  FixedStorageType::one() );

   // Vector3d::XYHelper xyHelper;

   
   // CHECK( vFixed.x == FixedStorageType::one(), reporter );
   // CHECK( vFixed.y == FixedStorageType::zero(), reporter );
   // CHECK( vFixed.z == FixedStorageType::one(), reporter );

   // CHECK( xyHelper.getX( vFixed ) == FixedStorageType::one(), reporter );
   // CHECK( xyHelper.getY( vFixed ) == FixedStorageType::zero(), reporter );
}

void Vector3dTests::testArithmetic( TestReporter& reporter )
{
   NEW_TEST_FUNCTION( reporter );
   
   // Vector3d start( fixedp::fromInt( 100 ),
   //                 fixedp::fromInt( 100 ),
   //                 fixedp::fromInt( 100 ) );

   // fixedp factor = fixedp::fromDouble( 0.5 );

   // start = (start * factor);

   // double dX = start.x.toDouble();
   // double dY = start.y.toDouble();
   // double dZ = start.z.toDouble();
   
   // CHECK( Vector3dMath::equals<DoubleStorageType>( dX, 50.0 ), reporter );
   // CHECK( Vector3dMath::equals<DoubleStorageType>( dY, 50.0 ), reporter );
   // CHECK( Vector3dMath::equals<DoubleStorageType>( dZ, 50.0 ), reporter );

   // start = start / factor;

   // dX = start.x.toDouble();
   // dY = start.y.toDouble();
   // dZ = start.z.toDouble();

   // CHECK( Vector3dMath::equals<DoubleStorageType>( dX, 100.0 ), reporter );
   // CHECK( Vector3dMath::equals<DoubleStorageType>( dY, 100.0 ), reporter );
   // CHECK( Vector3dMath::equals<DoubleStorageType>( dZ, 100.0 ), reporter );
}
