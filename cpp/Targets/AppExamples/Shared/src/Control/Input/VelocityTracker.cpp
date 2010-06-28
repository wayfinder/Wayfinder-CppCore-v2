/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#define CORE_LOGGING_MODULE_NAME "VelocityTracker"
#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE
#include "CoreDebugPrint.h"

#include "Control/Input/VelocityTracker.h"
#include "PointerEvent.h"

#include <cstdlib>
#include <list>
#include <vector>

static const unsigned int MAX_NBR_PAST_ELEMENTS = 10;
static const unsigned int LONGEST_PAST_TIME = 200;
static const float MAX_VELOCITY = 1000.0; // I have no idea..

struct VelocityTracker::Impl {
   Impl() {}

   typedef std::list<PointerEvent> EventsType;
   EventsType events;
   
};

VelocityTracker::VelocityTracker()
{   
   m_impl = new Impl();
}

VelocityTracker::~VelocityTracker()
{
   delete m_impl;
}  


void VelocityTracker::addMovement(const PointerEvent& event)
{
   // Do not consider multi touch.
   if (event.modStates.size() == 1 ) {
      m_impl->events.push_back(event);
      if (m_impl->events.size() > MAX_NBR_PAST_ELEMENTS) {
         m_impl->events.pop_front();
      }
   }
}

void VelocityTracker::reset()
{
   m_impl->events.clear();
}

void VelocityTracker::getCurrentVelocity(float& velocityX, float& velocityY)
{
   velocityX = velocityY = 0.0;
   
   if (m_impl->events.empty()) {
      return;
   }

   const PointerEvent& latestEvent = m_impl->events.back();

   unsigned int latestTime = latestEvent.modStates.front().timeStampMillis;
   unsigned int acceptableTime = 0;
   if (latestTime > LONGEST_PAST_TIME) {
      acceptableTime = latestTime - LONGEST_PAST_TIME;
   }
   
   std::vector<PointerEvent> eventsToConsider;
   
   for (Impl::EventsType::const_iterator it = m_impl->events.begin();
        it != m_impl->events.end(); ++it) {
      if ((*it).modStates.front().timeStampMillis < acceptableTime) {
         // Too old, skip.
         continue;
      }

      eventsToConsider.push_back(*it);
   }

   if (eventsToConsider.size() < 2) {
      return;
   }

   if (eventsToConsider.size() > 3) {
      // Skip the last received event, since it is probably pretty noisy.
      // (This is how Android does it)
      eventsToConsider.pop_back();
   }
   
   unsigned int oldestTime =
      eventsToConsider.front().modStates.front().timeStampMillis;
   WFAPI::ScreenPoint oldestPoint =
      eventsToConsider.front().modStates.front().location;

   float accumX = 0;
   float accumY = 0;
   using namespace std;   
   for (unsigned int i = 1; i < eventsToConsider.size(); ++i) {

      const PointerEvent::State& curState = eventsToConsider[i].modStates.front();
      
      unsigned int dur = curState.timeStampMillis - oldestTime;
      if (dur == 0) {
         continue;
      }

      int dist = curState.location.getX() - oldestPoint.getX();
      float vel = float(dist)/dur;   // pixels/frame.
      accumX = (accumX == 0) ? vel : (accumX + vel) * .5f;

      // cout << "dist = " << dist << endl;
      // cout << "vel = " << vel;
      // cout << "accumX = " << accumX << endl;
      
      dist = curState.location.getY() - oldestPoint.getY();
      vel = float(dist)/dur;   // pixels/frame.
      accumY = (accumY == 0) ? vel : (accumY + vel) * .5f;
   }

   
   // Cap to maximum / minimum allowed values.
   velocityX = accumX < 0.0f ? std::max(accumX, -MAX_VELOCITY)
      : std::min(accumX, MAX_VELOCITY);

   velocityY = accumY < 0.0f ? std::max(accumY, -MAX_VELOCITY)
      : std::min(accumY, MAX_VELOCITY);
      
}


/*
  public void computeCurrentVelocity(int units, float maxVelocity) {
  for (int pos = 0; pos < MotionEvent.BASE_AVAIL_POINTERS; pos++) {
  final float[] pastX = mPastX[pos];
  final float[] pastY = mPastY[pos];
  final long[] pastTime = mPastTime[pos];
  final int lastTouch = mLastTouch;

  // find oldest acceptable time
  int oldestTouch = lastTouch;
  if (pastTime[lastTouch] > 0) { // cleared ?
  oldestTouch = (lastTouch + 1) % NUM_PAST;
  final float acceptableTime = pastTime[lastTouch] - LONGEST_PAST_TIME;
  while (pastTime[oldestTouch] < acceptableTime) {
  oldestTouch = (oldestTouch + 1) % NUM_PAST;
  }
  }

  // Kind-of stupid.
  final float oldestX = pastX[oldestTouch];
  final float oldestY = pastY[oldestTouch];
  final long oldestTime = pastTime[oldestTouch];
  float accumX = 0;
  float accumY = 0;
  float N = (lastTouch - oldestTouch + NUM_PAST) % NUM_PAST + 1;
  // Skip the last received event, since it is probably pretty noisy.
  if (N > 3) N--;

  for (int i=1; i < N; i++) {
  final int j = (oldestTouch + i) % NUM_PAST;
  final int dur = (int)(pastTime[j] - oldestTime);
  if (dur == 0) continue;
  float dist = pastX[j] - oldestX;
  float vel = (dist/dur) * units;   // pixels/frame.
  accumX = (accumX == 0) ? vel : (accumX + vel) * .5f;

  dist = pastY[j] - oldestY;
  vel = (dist/dur) * units;   // pixels/frame.
  accumY = (accumY == 0) ? vel : (accumY + vel) * .5f;
  }

  mXVelocity[pos] = accumX < 0.0f ? Math.max(accumX, -maxVelocity)
  : Math.min(accumX, maxVelocity);
  mYVelocity[pos] = accumY < 0.0f ? Math.max(accumY, -maxVelocity)
  : Math.min(accumY, maxVelocity);

  if (localLOGV) Log.v(TAG, "Y velocity=" + mYVelocity +" X velocity="
  + mXVelocity + " N=" + N);
  }
  }
*/ 
