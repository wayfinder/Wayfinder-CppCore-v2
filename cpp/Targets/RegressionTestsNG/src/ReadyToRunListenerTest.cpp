/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "ReadyToRunListenerTest.h"
#include "Thread.h"

#if defined __linux__
#  include <GtkReadyToRunListener.h>
#elif defined(_WIN32_WCE)
#  include <WMReadyToRunListener.h>
#elif defined NAV2_CLIENT_SERIES60_V3 || defined NAV2_CLIENT_SERIES60_V5
#  include <S60ReadyToRunListener.h>
#elif defined (IPHONE)
#include <IPReadyToRunListener.h>
#endif

#define LIMIT 25
// #define RTRTEST_DEBUG_INFO

using namespace WFAPI;

class TestThread : public isab::Thread {
public:
   TestThread(ReadyToRunListener* listener) :
      isab::Thread("RTRTestThread")
   {
      m_listener = listener;
   }
   
   virtual void run() {
      for(int i = 0; i < LIMIT; i++) {
#ifdef RTRTEST_DEBUG_INFO
         printf("TestThread::run() - m_listener->ready() : %d\n", i);
#endif
         m_listener->ready();         
      }
   }
private:
   ReadyToRunListener* m_listener;
};

ReadyToRunListenerTest::ReadyToRunListenerTest(Nav2Session& sess,
                                               const char* name) :
   RegressionTest(name, 10000)
{
   m_thread = NULL;
   m_listener = NULL;
   m_instCount = 0;
}

ReadyToRunListenerTest::~ReadyToRunListenerTest()
{
   if(m_thread) {
      m_thread->join(5000);
      delete m_thread;
   }

   delete m_listener;
}

void ReadyToRunListenerTest::incCount()
{
   m_instCount++;

#ifdef RTRTEST_DEBUG_INFO
   printf("ReadyToRunListener::incCount() : %d\n", m_instCount);
#endif
   
   if(m_instCount == LIMIT) {
      setTestPassed();
   }
}

void ReadyToRunListenerTest::doCallbacks()
{
   if(isab::Thread::currentThread() == m_thread) {
      setTestFailed("The ReadyToRunListener called doCallbacks() in the"
                    "same context as the thread that called ready().");
   } else {
      incCount();      
   }
}

void ReadyToRunListenerTest::startTest()
{
#if defined __linux__
   m_listener = new GtkReadyToRunListener;
#elif defined IPHONE
   m_listener = new IPReadyToRunListener;
#elif defined(_WIN32_WCE)
   WMReadyToRunListener* wmListener = new WMReadyToRunListener;

   if(!wmListener->init(0)) {
      delete wmListener;
      setTestFailed("Could not initialize Windows Mobile ReadyToRunListener.");
      return;
   }
   
   m_listener = wmListener;

#elif defined NAV2_CLIENT_SERIES60_V3 || defined NAV2_CLIENT_SERIES60_V5
   S60ReadyToRunListener* s60Listener= new S60ReadyToRunListener;
   // m_listener = new S60ReadyToRunListener;
   s60Listener->ConstructL();
   m_listener = s60Listener;
   
#endif

   

   m_listener->setReceiver(this);
   
   m_thread = new TestThread(m_listener);
   m_thread->start();
}

