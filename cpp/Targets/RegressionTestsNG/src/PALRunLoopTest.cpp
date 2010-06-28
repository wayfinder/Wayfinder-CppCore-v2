/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "PALRunLoopTest.h"
#include "PALRunLoop.h"
#include "PALThreading.h"
#include "TileMapUtil.h"

static int TEST_TIME = 500;
static int TEST_TIME_EPSILON = 100;

enum state_t { TIMER_EXPIRATION };

struct PALRunLoopTest::Impl {
   // The context we use to create timers
   pal::RunLoopContext* runLoopContext;
   state_t state;
   unsigned int curTimerId;
   int testStartTime;
   bool callbackFired;
};

PALRunLoopTest::PALRunLoopTest(Nav2Session& session, const char* name) : 
   RegressionTest(name)
{
   m_impl = new Impl;

   pal::pstatus status = pal::createRunLoopContext(
      &m_impl->runLoopContext);

   assert(pal::PAL_OK == status);
   
   m_impl->state = TIMER_EXPIRATION;
}

void PALRunLoopTest::timerCallback(int id)
{   
   switch(m_impl->state) {
   case TIMER_EXPIRATION:
      if(id == m_impl->curTimerId) {
         m_impl->callbackFired = true;
      }
      break;
   }
}

void PALRunLoopTest::startTest() {
   
   m_impl->testStartTime = TileMapUtil::currentTimeMillis();
   m_impl->callbackFired = false;   

   int elapsedTime = 0;

   pal::performIteration();

   /**
    *  Test firing an event which will take place after 500 ms
    */
   pal::requestTimer(m_impl->runLoopContext,
                     500,
                     PALRunLoopTest::timerCallbackProxy,
                     this,
                     &m_impl->curTimerId);
   
   while(!m_impl->callbackFired && elapsedTime < TEST_TIME + TEST_TIME_EPSILON)
   {
      pal::threadSleep(5);
      elapsedTime = TileMapUtil::currentTimeMillis() - m_impl->testStartTime;
      pal::performIteration();      
   }

   if(!m_impl->callbackFired) {
      setTestFailed("The timer did not fire within the expected interval");
   }
   
   /**
    *  Test firing an event which will take place after 500 ms, then cancel
    *  it after 250 ms, make sure it does not fire
    */ 
   m_impl->callbackFired = false;
   elapsedTime = 0;
   
   pal::requestTimer(m_impl->runLoopContext,
                     500,
                     PALRunLoopTest::timerCallbackProxy,
                     this,
                     &m_impl->curTimerId);

   bool cancelled = false;
   
   while(elapsedTime < TEST_TIME + TEST_TIME_EPSILON)
   {      
      pal::threadSleep(5);
      elapsedTime = TileMapUtil::currentTimeMillis() - m_impl->testStartTime;
      pal::performIteration();

      if(elapsedTime > TEST_TIME / 2) {
         pal::pstatus status =
            pal::cancelTimer(m_impl->runLoopContext, m_impl->curTimerId);

         if(status == pal::PAL_ERR_INVALID_TIMER) {
            setTestFailed("Timer id mismatch, returned invalid timer");
         } else if(status == pal::PAL_ERR_EXPIRED_TIMER) {
            setTestFailed("Timer id mismatch, returned timer already expired");
         } else {
            cancelled = true;
         }
      }
   }

   if(m_impl->callbackFired) {
      setTestFailed("Callback called even though we cancelled timer");
   }
      
   setTestPassed();
}

PALRunLoopTest::~PALRunLoopTest() {
   delete m_impl;
}

void PALRunLoopTest::timerCallbackProxy(unsigned int id, void* data)
{
   PALRunLoopTest* t = static_cast<PALRunLoopTest*>(data);
   t->timerCallback(id);
}


