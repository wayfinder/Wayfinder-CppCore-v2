/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "FixedPointTypeTests.h"
#include "FixedPointType.h"
#include "TestFramework.h"
#include "Vector3d.h"
#include "Vector3dMath.h"

void FixedPointTypeTests::testSqrt( TestReporter& reporter )
{
   NEW_TEST_FUNCTION( reporter );
   
   // fixedp num1 = fixedp::fromInt( 128 );   
   // num1 = num1 * num1;

   // CHECK( num1.sqrt() == fixedp::fromInt( 128 ), reporter );

   // fixedp64 num2 = fixedp64::fromInt( 32768 );
   // num2 = num2 * num2;

   // CHECK( num2.sqrt() == fixedp64::fromInt( 32768 ), reporter );
}

void FixedPointTypeTests::testArithmetic( TestReporter& reporter )
{
   // NEW_TEST_FUNCTION( reporter );

   // fixedp num1 = fixedp::fromInt(8);
   // fixedp num2 = fixedp::fromInt(16);
   // fixedp fac1 = fixedp::fromDouble(0.5);
   // fixedp fac2 = fixedp::fromDouble(0.75);
   
   // CHECK( num1 + num2 == fixedp::fromInt(24), reporter );
   // CHECK( num1 / num2 == fixedp::fromDouble(0.5), reporter );
   // CHECK( num1 * num2 == fixedp::fromInt(128), reporter );
   // CHECK( num1 - num2 == fixedp::fromInt(-8), reporter );

   // CHECK( num1 * fac1 == fixedp::fromInt(4), reporter );
   // CHECK( num1 * fac2 == fixedp::fromInt(6), reporter );
   // CHECK( num2 * fac1 == fixedp::fromInt(8), reporter );
   // CHECK( num2 * fac2 == fixedp::fromInt(12), reporter );
   
   // fixedp64 num3 = fixedp64::fromInt(32768);
   // fixedp64 num4 = fixedp64::fromInt(16384);

   // fixedp64 fac3 = fixedp64::fromDouble(0.25);
   // fixedp64 fac4 = fixedp64::fromDouble(0.50);
   
   // CHECK( num3 + num4 == fixedp64::fromInt(49152), reporter );
   // CHECK( num3 / num4 == fixedp64::fromDouble(2.0), reporter );
   // CHECK( num3 * num4 == fixedp64::fromInt(536870912), reporter );
   // CHECK( num3 - num4 == fixedp64::fromInt(16384), reporter );

   // CHECK( num3 * fac3 == fixedp64::fromInt( 8192 ), reporter );
   // CHECK( num3 * fac4 == fixedp64::fromInt( 16384 ), reporter );
   // CHECK( num4 * fac3 == fixedp64::fromInt( 4096 ), reporter );
   // CHECK( num4 * fac4 == fixedp64::fromInt( 8192 ), reporter );   
}
