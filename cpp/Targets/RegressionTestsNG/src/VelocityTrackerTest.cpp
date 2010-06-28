/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "VelocityTrackerTest.h"

#define CORE_LOGGING_MODULE_NAME "VelocityTrackerTest"
#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE
#include "CoreDebugPrint.h"

#include "Control/Input/VelocityTracker.h"
#include "PointerEvent.h"
#include <math.h>

struct VelocityTrackerTest::Impl {

   Impl() {}

   VelocityTracker velocityTracker;
   
};

VelocityTrackerTest::VelocityTrackerTest(Nav2Session& session, const char* name) : 
   RegressionTest(name)
{
   m_impl = new Impl;
}

void VelocityTrackerTest::startTest() {
   
   PointerEvent::State state;
   state.code = PointerEvent::FINGER;
   state.type = PointerEvent::POINTER_DOWN;
   state.tapCount = 1;
   state.location = WFAPI::ScreenPoint(0,0);
   state.timeStampMillis = 0;
   
   PointerEvent p1;
   p1.modStates.push_back(state);
   p1.allStates.push_back(state);

   state.location = WFAPI::ScreenPoint(10,10);
   state.timeStampMillis = 10;
   PointerEvent p2;
   p2.modStates.push_back(state);
   p2.allStates.push_back(state);
   
   m_impl->velocityTracker.addMovement(p1);
   m_impl->velocityTracker.addMovement(p2);
   
   float velX;
   float velY;

   m_impl->velocityTracker.getCurrentVelocity(velX, velY);

   if (velX != 1.0 || velY != 1.0) {
      coreprintln("velX = %f, velY = %f\n", velX, velY);
      setTestFailed("Wrong velocity");
      return;
   }

   // An identical event more. Should be same as before.
   PointerEvent p3;
   p3.modStates.push_back(state);
   p3.allStates.push_back(state);
   m_impl->velocityTracker.addMovement(p3);
   
   m_impl->velocityTracker.getCurrentVelocity(velX, velY);
   if (velX != 1.0 || velY != 1.0) {
      coreprintln("velX = %f, velY = %f\n", velX, velY);
      setTestFailed("Wrong velocity");
      return;
   }

   // Test reset.
   m_impl->velocityTracker.reset();
   m_impl->velocityTracker.getCurrentVelocity(velX, velY);
   if (velX != 0.0 || velY != 0.0) {
      coreprintln("velX = %f, velY = %f\n", velX, velY);
      setTestFailed("Should be 0 velocity after clearing");
      return;
   }

   // Add only one movement. Should also return in 0 velocity.
   m_impl->velocityTracker.addMovement(p1);
   m_impl->velocityTracker.getCurrentVelocity(velX, velY);
   if (velX != 0.0 || velY != 0.0) {
      coreprintln("velX = %f, velY = %f\n", velX, velY);
      setTestFailed("Should be 0 velocity after only one movement");
      return;
   }

   // p1 and p2 just as before.
   m_impl->velocityTracker.addMovement(p2);
   
   // And add an additional point with same distance but doubled speed.
   // Velocity should then be:
   // 1: 10/10 = 1
   // 2: 20/15 = 1.33
   // Weight should be done as follows:
   // 1/2 + 1.333/2 = 1.1
   
   PointerEvent p4;
   state.timeStampMillis = 15;
   state.location = WFAPI::ScreenPoint(20,15);
   p4.modStates.push_back(state);
   p4.allStates.push_back(state);
   m_impl->velocityTracker.addMovement(p4);   

   m_impl->velocityTracker.getCurrentVelocity(velX, velY);
   if (fabs(velX - 1.166667) > 0.0001 ||
       fabs(velY - 1.0) > 0.0001) {
      coreprintln("velX = %f, velY = %f\n", velX, velY);
      setTestFailed("Should be 1.166667, 1.0 velocity");
      return;
   }

   m_impl->velocityTracker.reset();

   // Test negative going in negative direction.
   PointerEvent np1;
   state.location = WFAPI::ScreenPoint(100,100);
   state.timeStampMillis = 0;
   np1.allStates.push_back(state);
   np1.modStates.push_back(state);
   PointerEvent np2;
   state.location = WFAPI::ScreenPoint(90,90);
   state.timeStampMillis = 10;
   np2.allStates.push_back(state);
   np2.modStates.push_back(state);

   m_impl->velocityTracker.addMovement(np1);   
   m_impl->velocityTracker.addMovement(np2);   

   m_impl->velocityTracker.getCurrentVelocity(velX, velY);
   if (velX != -1.0 || velY != -1.0) {
      coreprintln("velX = %f, velY = %f\n", velX, velY);
      setTestFailed("Should be -1.0 velocity");
      return;
   }

   m_impl->velocityTracker.reset();
   // Only 10 latest positions should be considered.
   // Out of the 20, put unreasonable values in the first 10.
   // Then, if more than 3 events, the last one should be discarded.
   // Put obviously bad value in that one also.
   int locations[20] =
       {10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,
        0,10,20,30,40,100,150,300,600,1000000};
   // The following result should be expected.
   // Velocities:
   // 1: 10/10
   // 2: 20/20
   // 3: 30/30
   // 4: 40/40
   // 5: 100/50
   // 6: 150/60
   // 7: 300/70
   // 8: 600/80
   //
   // Result should then be (run in bc -l):
   // (((1/2 + 100/50/2)/2 + 150/60/2)/2 + 300/70/2)/2 + 600/80/2
   
   int dist = 1;
   WFAPI::ScreenPoint loc(0,0);
   for (unsigned int i = 0; i < 20; ++i) {
      PointerEvent p;
      // Sampled every 10th milli second.
      dist = dist * 2;
      loc.getX() = locations[i];
      loc.getY() = locations[i];
      state.location = loc;
      state.timeStampMillis = i * 10;
      p.modStates.push_back(state);
      p.allStates.push_back(state);
      m_impl->velocityTracker.addMovement(p);
      // coreprintln("Adding movement: time %d, loc x = %d, loc y = %d",
      //             state.timeStampMillis,
      //             loc.getX(),
      //             loc.getY());
   }

   m_impl->velocityTracker.getCurrentVelocity(velX, velY);
   if ( fabs(velX - 5.32142857142) > 0.0001 ||
        fabs(velY - 5.32142857142) > 0.0001) {
      coreprintln("velX = %f, velY = %f\n", velX, velY);
      setTestFailed("Error in velocity");
      return;
   }   
   
   setTestPassed();
}

VelocityTrackerTest::~VelocityTrackerTest() {
   delete m_impl;
}
