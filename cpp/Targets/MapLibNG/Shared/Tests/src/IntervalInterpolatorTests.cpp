/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "IntervalInterpolatorTests.h"
#include "IntervalInterpolator.h"
#include "TestFramework.h"
#include "math.h"

#define PR(val) std::cout << #val << " = " << val << std::endl
#define CMP(lhs, rhs) std::cout << #lhs << " == " << #rhs << " -> " << lhs << \
" == " << rhs << std::endl

void IntervalInterpolatorTests::testSimple(TestReporter& reporter)
{
   NEW_TEST_FUNCTION( reporter );

   IntervalInterpolator interpolator(0.0f, 0.0f);

   interpolator.setIntervals(0.0f, 0.0f);
   
   CHECK(interpolator.getValue(1) == 1, reporter);
   
   for(float i = 0.0f; ; i+=0.001f) {
      if(i>1.0f) {
         i = 1.0f;
      }
      
      CHECK( i == interpolator.getValue(i), reporter );
      
      if(i == 1.0f) {
         break;
      }
   }
   
   interpolator.setIntervals(0.25f, 0.25f);
   
   CHECK(interpolator.getValue(1) == 1, reporter);
   
   const float height = interpolator.getMaxVelocity();

   CHECK(height == 1.33333333333333333f, reporter);
   
   const float k = height / 0.25f;
   
   for(float i = 0.0f; ; i+=0.001f) {

      if(i>0.25f) {
         i = 0.25f;
      }

      float diff = fabs(i*k*i*0.5f - interpolator.getValue(i));

      CHECK(diff < 0.0000001f, reporter);
      
      if(i == 0.25f) {
         break;
      }
   }

   const float t1 = interpolator.getValue(0.25f);
   
   for(float i = 0.25f; ; i+=0.001f) {

      if(i>0.75f) {
         i = 0.75f;
      }

      float calcVal = t1 + (i-0.25f)*height;
      float diff = fabs(interpolator.getValue(i) - calcVal);
      
      CHECK( diff < 0.0000001, reporter);
      
      if(i == 0.75f) {
         break;
      }
   }

   const float t1r1 = interpolator.getValue(0.75f);

   for(float i = 0.75f; ; i+=0.001f) {

      if(i>1.0f) {
         i = 1.0f;
      }

      float calcVal = t1r1 + t1 - (1.0f - i) * (k) * (1.0f - i) * 0.5f;
      
      float diff = fabs(interpolator.getValue(i) - calcVal);
      
      CHECK( diff < 0.0000001, reporter);
      
      if(i == 1.0f) {
         break;
      }
   }
}

void IntervalInterpolatorTests::testHeights(TestReporter& reporter)
{
   NEW_TEST_FUNCTION(reporter);
   
   IntervalInterpolator interpolator(0.5f, 0.5f);

   CHECK(interpolator.getMaxVelocity() == 2, reporter);
}

void IntervalInterpolatorTests::testOneSided(TestReporter& reporter)
{
   NEW_TEST_FUNCTION(reporter);
   
   IntervalInterpolator interpolator(0.0f, 0.5f);

   CHECK(interpolator.getValue(1) == 1, reporter);
   CHECK(interpolator.getValue(0.5) == 2.0f / 3.0f, reporter);
   
   interpolator.setIntervals(0.5f, 0.0f);

   CHECK(interpolator.getValue(0.5f) == 1.0f/3.0f, reporter);
}
