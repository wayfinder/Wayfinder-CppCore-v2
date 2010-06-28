/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#define CORE_LOGGING_MODULE_NAME "GestureDetector"
#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE
#include "CoreDebugPrint.h"
#include <math.h>                               \

#include "Control/Input/GestureDetector.h"
#include "Control/Input/GestureListener.h"
#include "Control/Input/VelocityTracker.h"
#include "PointerEvent.h"
#include "TileMapUtil.h"

// Unit is pixels per millisecond.
static const float MIN_FLING_DETECTION_VELOCITY = 0.05;

static const float MIN_SQ_FLING_VELOCITY = 0.25;
static const float MIN_FLING_VELOCITY = sqrt(MIN_SQ_FLING_VELOCITY);

// Flicking can not be done before this amount of time since multi touch events.
static const unsigned int MULTI_TOUCH_DELAY_BEFORE_FLICKING_MS = 100;

struct GestureDetector::Impl {
   /**
    *  Constructor.
    *
    *  @param listener The GestureListener to set.
    */
   Impl(GestureListener* listener) :
      listener(listener),
      lastMultiTouchTimeStamp(0) {}

   /// The gesture listener.
   GestureListener* listener;

   /// The velocity tracker.
   VelocityTracker velocityTracker;

   /// The timestamp when last receiving multi touch events.
   unsigned int lastMultiTouchTimeStamp;
};

GestureDetector::GestureDetector(GestureListener* listener)
{   
   m_impl = new Impl(listener);
}

GestureDetector::~GestureDetector()
{
   delete m_impl;
}  

bool GestureDetector::onPointerDown(const PointerEvent& event)
{
   // Store event.

   if (event.allStates.size() == 1) {
      // Don't consider multi touch events when calculating velocity.
      m_impl->velocityTracker.addMovement(event);
   } 

   return false;
}

bool GestureDetector::onPointerMove(const PointerEvent& event)
{
   // Store event.

   if (event.allStates.size() == 1) {
      // Don't consider multi touch events when calculating velocity.
      m_impl->velocityTracker.addMovement(event);
   } else {
      // Store the last multi touch time stamp.
      m_impl->lastMultiTouchTimeStamp = TileMapUtil::currentTimeMillis();
   }

   return false;
}

bool GestureDetector::onPointerUp(const PointerEvent& event)
{

   if (event.allStates.size() > 1) {
      // Don't consider multi touch events when calculating velocity.
      // Store the last multi touch time stamp.
      m_impl->lastMultiTouchTimeStamp = TileMapUtil::currentTimeMillis();
      return false;
   }   

   // Check that multi touch events were not received too recent.
   if (TileMapUtil::currentTimeMillis() <
       m_impl->lastMultiTouchTimeStamp + MULTI_TOUCH_DELAY_BEFORE_FLICKING_MS) {
      m_impl->velocityTracker.reset();
      return false;
   }

   // OK to calculate fling velocity.
   
   // Store event.
   m_impl->velocityTracker.addMovement(event);

   // Calculate velocity.
   float velocityX;
   float velocityY;
   
   m_impl->velocityTracker.getCurrentVelocity(velocityX, velocityY);

//    coreprintln("velX = %f, velY = %f", velocityX, velocityY);
   
    m_impl->velocityTracker.reset();
    bool retVal = false;
    if (fabs(velocityX) > MIN_FLING_DETECTION_VELOCITY ||
        fabs(velocityY) > MIN_FLING_DETECTION_VELOCITY) {

       // Check if needed to give an extra boost to the fling velocity.
       float sqAbsVelocity = velocityX*velocityX + velocityY*velocityY;

       if (sqAbsVelocity < MIN_SQ_FLING_VELOCITY) {
          // Adding extra boost!
          float increaseFactor = (MIN_FLING_VELOCITY / sqrt(sqAbsVelocity));
          coreprintln("Flicking: increasing velocity with factor %f", increaseFactor);
          velocityX *= increaseFactor;
          velocityY *= increaseFactor;
       }
       
       // Call listener.
//      coreprintln("About to call onFling");
       retVal = m_impl->listener->
          onFling(event, velocityX, velocityY);
    }
    
//   coreprintln("Retval = %d \n", retVal);
    return retVal;
}

