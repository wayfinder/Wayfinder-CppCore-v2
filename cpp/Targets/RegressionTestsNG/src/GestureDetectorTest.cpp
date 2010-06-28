/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "GestureDetectorTest.h"

#include "Control/Input/GestureDetector.h"
#include "PointerEvent.h"

/// Impl class.
struct GestureDetectorTest::Impl {

   /// Constructor.
   Impl(GestureListener* listener) :
      detector(listener),
      expectingOnFlingCall(false) {}

   /// The gesture detector.
   GestureDetector detector;

   /// If an onFling call should be expected.
   bool expectingOnFlingCall;
};

GestureDetectorTest::GestureDetectorTest(Nav2Session& session, const char* name) : 
   RegressionTest(name), GestureListener()
{
   m_impl = new Impl(this);
}

void GestureDetectorTest::startTest() {

   PointerEvent downEvent;
   PointerEvent::State state;
   state.timeStampMillis = 0;
   state.location = WFAPI::ScreenPoint(0,0);
   state.code = PointerEvent::FINGER;
   state.type = PointerEvent::POINTER_DOWN;
   state.tapCount = 1;
   downEvent.modStates.push_back(state);
   downEvent.allStates.push_back(state);
   
   PointerEvent moveEvent;
   state.timeStampMillis = 10;
   state.location = WFAPI::ScreenPoint(10,10);
   state.type = PointerEvent::POINTER_MOVE;
   moveEvent.modStates.push_back(state);
   moveEvent.allStates.push_back(state);

   PointerEvent upEvent;
   state.timeStampMillis = 20;
   state.location = WFAPI::ScreenPoint(20,20);
   state.type = PointerEvent::POINTER_UP;
   upEvent.modStates.push_back(state);
   upEvent.allStates.push_back(state);

   PointerEvent slowMoveEvent;
   state.timeStampMillis = 200;
   state.location = WFAPI::ScreenPoint(1,1);
   state.type = PointerEvent::POINTER_MOVE;
   slowMoveEvent.modStates.push_back(state);
   slowMoveEvent.allStates.push_back(state);

   PointerEvent slowUpEvent;
   state.timeStampMillis = 400;
   state.location = WFAPI::ScreenPoint(2,2);
   state.type = PointerEvent::POINTER_UP;
   slowUpEvent.modStates.push_back(state);
   slowUpEvent.allStates.push_back(state);

   // First test the slow events.
   // These should not generate an onFling call.
   m_impl->detector.onPointerDown(downEvent);
   m_impl->detector.onPointerMove(slowMoveEvent);
   if (m_impl->detector.onPointerUp(slowUpEvent)) {
      // Got handled for too slow flicking. Wrong.
      setTestFailed("detector.onPointerUp returned true for very slow updates.");
      return;
   }

   // Now test the quick events.
   m_impl->detector.onPointerDown(downEvent);
   m_impl->detector.onPointerMove(moveEvent);
   m_impl->expectingOnFlingCall = true;
   if (! m_impl->detector.onPointerUp(upEvent)) {
      // Didn't get handled for quick flicking. Wrong.
      setTestFailed("detector.onPointerUp returned false for very quick updates.");
      return;
   }
}

GestureDetectorTest::~GestureDetectorTest() {
   delete m_impl;
}

bool
GestureDetectorTest::onFling(const PointerEvent& event, 
                             float velocityX,
                             float velocityY)
{
   if (m_impl->expectingOnFlingCall) {
      setTestPassed();
   } else {
      setTestFailed("Got call to onFling when not expecting it.");
   }

   return true;
}

