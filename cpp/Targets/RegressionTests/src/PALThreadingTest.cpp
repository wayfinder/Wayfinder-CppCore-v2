/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#define CORE_LOGGING_MODULE_NAME "PALTHREADINGTEST"
//#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE

#include "CoreDebugPrint.h"
#include "PALThreadingTest.h"
#include "RegressionTestErrorPrinting.h"
#include "Nav2Session.h"
#include "PAL.h"

typedef struct data {
   int i;
   PALThreadingTest* threadTest;
   pal::Thread thread;
} data_t;

PALThreadingTest::PALThreadingTest(Nav2Session& session, const char* name)
   : RegressionTest(name),
     m_res(0),
     m_data_ready(false)
{
   m_condmut = pal::createRecursiveMutex();
   m_cond = pal::createConditional();
   //not needed if done in the RegressionTestEnvironment:
   //pal::initThread();
}

PALThreadingTest::~PALThreadingTest()
{
   pal::destroyConditional(m_cond);

   //not needed if done in the RegressionTestEnvironment:
   //pal::deinitThread();
}


void PALThreadingTest::startTest()
{
   pal::pstatus pStatus;
   data_t* data1 = new data_t;

   //first try to create a thread that we never use:
   pal::Thread thread0 = pal::createThread(&threadFunc, data1);
   //if this gives a memory leak, something is wrong.
   pal::destroyThread(thread0);
   thread0 = NULL;
   
   //now create a mutex that the two threads will use to protect shared data:
   m_mutex = pal::createMutex();
   if(m_mutex == NULL) {
      setTestFailed("Create mutex failed");
      return;
   }
   //create a recursive mutex too:
   m_recursiveMutex = pal::createRecursiveMutex();
   if(m_recursiveMutex == NULL) {
      setTestFailed("Create recursive mutex failed");
      return;
  }

   //set some data:
   data1->i = 3;
   data1->threadTest = this;

   //try to print a pointer to the main thread:
   coreprintln("main thread: currentThread()=%p", pal::currentThread());
   coreprintln("main thread: getCurrentThreadID()=%d", pal::getCurrentThreadID());
   
   //create the first thread to use for processing:
   //also, give this thread a name.
   pal::Thread thread1 = pal::createThread(&threadFunc, data1, "thread1");
   coreprintln("thread1=%p", thread1);
   if(thread1 == NULL) {
      setTestFailed("Create thread failed");
      return;
   }  else {
      int res = strcmp(pal::getThreadName(thread1), "thread1");
      if(res != 0) {
         setTestFailed("getThreadName() broken");
         return;
      }
      data1->thread = thread1;
      pStatus = pal::startThread(thread1);
      if(pStatus != pal::PAL_OK) {
         setTestFailed("Start thread failed");
         return;
      }
   }

   //create another set of data
   data_t* data2 = new data_t;
   data2->i = 3;
   data2->threadTest = this;
   pal::Thread thread2 = pal::createThread(&threadFunc, data2);
   coreprintln("thread2=%p", thread2);
   if(thread2 == NULL) {
      setTestFailed("Create thread failed");
      return;
   }  else {
      data2->thread = thread2;

      pStatus = pal::startThread(thread2);
      if(pStatus != pal::PAL_OK) {
         setTestFailed("Start thread failed");
         return;
      }
   }

   int i = 0;
   int timeout = 10000; //10 seconds
   //now wait for the threads to do their work:
   while(m_res != 4 && i < timeout) {
      //do nothing
      pal::threadSleep(1);
      i++;
   }
   if(i == timeout) {
      setTestFailed("Doing thread work failed");
      return;
   }
   //getting this far means mutexes and recursive mutexes work.
   

   
   //now lets try if the conditionals work
   //create another set of data
   //used for the conditional waiting thread
   data_t* data3 = new data_t;
   data3->i = 4;
   data3->threadTest = this;
   pal::Thread thread3 = pal::createThread(&threadFunc2, data3);
   coreprintln("thread3=%p", thread3);
   if(thread3 == NULL) {
      setTestFailed("Create thread failed");
      return;
   }  else {
      data3->thread = thread3;

      pStatus = pal::startThread(thread3);
      if(pStatus != pal::PAL_OK) {
         setTestFailed("Start conditional testing thread failed");
         return;
      }
   }

   //sleep to let thread3 enter wait
   pal::threadSleep(200);
   
   //create another set of data
   //used for the conditional signal thread
   data_t* data4 = new data_t;
   data4->i = 5;
   data4->threadTest = this;
   pal::Thread thread4 = pal::createThread(&threadFunc3, data4);
   coreprintln("thread4=%p", thread2);
   if(thread4 == NULL) {
      setTestFailed("Create thread failed");
      return;
   }  else {
      data4->thread = thread4;

      pStatus = pal::startThread(thread4);
      if(pStatus != pal::PAL_OK) {
         setTestFailed("Start conditional testing thread 2 failed");
         return;
      }
   }

   pal::threadSleep(1000); // let the other threads finish


   //now clean up the resources we used:
   delete data1;
   data1 = NULL;
   delete data2;
   data2 = NULL;
   pal::destroyMutex(m_mutex);
   m_mutex = NULL;
   pal::destroyRecursiveMutex(m_recursiveMutex);
   m_recursiveMutex = NULL;
   pal::destroyThread(thread1);
   thread1 = NULL;
   pal::destroyThread(thread2);
   thread2 = NULL;

   delete data3;
   data1 = NULL;
   delete data4;
   data2 = NULL;
   pal::destroyConditional(m_cond);
   m_cond = NULL;
   pal::destroyMutex(m_condmut);
   m_condmut = NULL;
   pal::destroyThread(thread3);
   thread3 = NULL;
   pal::destroyThread(thread4);
   thread4 = NULL;
}


void PALThreadingTest::threadJobfunction(void* arg) {
   int ii;
   data_t* data = (data_t*)arg;
   pal::lockMutex(m_mutex);
   coreprintln("locked mutex, m_res = %d", m_res);
   pal::lockRecursiveMutex(m_recursiveMutex);
   coreprintln("locked recursive mutex, m_res = %d", m_res);
   data->i = 2;
   for(int i=0; i<=10; i++) {
      //coreprintln("Doing work in a thread...");
      ii=i/(i+1);
      pal::threadSleep(1);
   }
   coreprintln("current thread=%p", data->thread);
   coreprintln("currentThread()=%p", pal::currentThread());
   coreprintln("getThreadName(currentThread())=%s", pal::getThreadName(pal::currentThread()));
   coreprintln("work thread: getCurrentThreadID()=%d", pal::getCurrentThreadID());
   
   m_res += 1;
   threadJobfunctionRecursive(data);
   pal::unlockRecursiveMutex(m_recursiveMutex);
   coreprintln("unlocked recursive mutex");
   pal::unlockMutex(m_mutex);
   coreprintln("unlocked mutex");

}


void PALThreadingTest::threadJobfunctionRecursive(void* arg) {
   int ii;
   data_t* data = (data_t*)arg;
   pal::lockRecursiveMutex(m_recursiveMutex);
   coreprintln("locked recursive mutex, m_res = %d", m_res);
   data->i = 2;
   for(int i=0; i<=10; i++) {
      //coreprintln("Doing work in a thread...");
      ii=i/(i+1);
      pal::threadSleep(1);
   }
   coreprintln("current thread=%p", data->thread);
   m_res += 1;
   pal::unlockRecursiveMutex(m_recursiveMutex);
   coreprintln("unlocked recursive mutex");

}

void threadFunc(void* arg)
{
   data_t* data = static_cast<data_t*> (arg);
   data->threadTest->threadJobfunction(arg);
}

void threadFunc2(void* arg)
{
   data_t* data = static_cast<data_t*> (arg);
   data->threadTest->wait_for_data_to_process();
}

void threadFunc3(void* arg)
{
   data_t* data = static_cast<data_t*> (arg);
   data->threadTest->prepare_data_for_processing();
}


void PALThreadingTest::process_data() {
   //do data processing here...
   //getting here means we passed the test
   setTestPassed();
}

void PALThreadingTest::wait_for_data_to_process()
{
   pal::lockRecursiveMutex(m_condmut);
   while(!m_data_ready)
   {
      coreprintln("Entering conditional wait...");
      pal::conditionalWaitRecursive(m_cond, m_condmut);
      coreprintln("Leaving conditional wait!");
   }
   process_data();
   pal::unlockRecursiveMutex(m_condmut);
}

void PALThreadingTest::prepare_data_for_processing()
{
   //retrieve_data();
   //prepare_data();
   pal::lockRecursiveMutex(m_condmut);
   m_data_ready=true;
   pal::unlockRecursiveMutex(m_condmut);

   coreprintln("About to signal the conditional");
   pal::threadSleep(300);
   pal::conditionalBroadcast(m_cond);
}
