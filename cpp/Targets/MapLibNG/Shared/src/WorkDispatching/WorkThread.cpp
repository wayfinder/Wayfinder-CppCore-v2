/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "WorkDispatching/WorkThread.h"
#include "WorkDispatching/WorkPerformer.h"
#include "WorkDispatching/WorkListener.h"
#include "FileReaderThread.h"
#include "MainThreadScheduler.h"
#include <sched.h>
#include <deque>
#include <vector>
#include "PlatformFactory.h"
#include "TileMapUtil.h"
#include <pthread.h>
#include <algorithm>
#include "Environment.h"

#define CORE_LOGGING_MODULE_NAME "WorkThread"
// #define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE
#include "CoreDebugPrint.h"

struct WorkThread::Impl {
   Impl(WorkPerformer* performer,
        unsigned int sleepTimeMs,
        unsigned int workPerIteration) :
      terminated(false),
      callbackDealtWith(true),
      workToDo(false),
      scheduler(PlatformFactory::createMainThreadSheduler()),
      performer(performer),
      sleepTimeMs(sleepTimeMs),
      workPerIteration(workPerIteration),
      running(false)
   {
      pthread_mutex_t tmpMutex = PTHREAD_MUTEX_INITIALIZER;
      pthread_cond_t tmpCond = PTHREAD_COND_INITIALIZER;
      mutex = tmpMutex;
      cond = tmpCond;
   }

   ~Impl() {
      delete scheduler;
   }

   // Whether or not the thread context has been terminated (via kill)
   bool terminated;

   // The single mutex that protects all the resources of the thread
   pthread_mutex_t mutex;

   // A condition variable which is used to notify the thread that work
   // is available
   pthread_cond_t cond;

   // The pthread id of the thread
   pthread_t threadId;

   // Flag used to alert the thread that there is available work to do,
   // associated with the condition variable
   bool workToDo;

   // Whether or not the previously performed work has been dealt with
   // by the callback. Until it has, no further work is performed. Associated
   // with the condition variable
   bool callbackDealtWith;

   // The work queue of the thread (where new jobs are added)
   deque<WorkThread::WorkLoad> work;

   // The work loads that have previously been in work but are now completed
   vector<WorkLoad> finished;

   // The scheduling class we use to perform callbacks on the main thread
   MainThreadScheduler* scheduler;

   // The class containing the algorithm that constitutes the actual work part
   WorkPerformer* performer;

   // The amount of time that should be spent sleeping between work actions
   unsigned int sleepTimeMs;

   // The amount of work operations that will be performed during a single iteration
   unsigned int workPerIteration;
   
   // Whether or not the thread is actively running
   bool running;
};

void WorkThread::mainThreadCallback(void* data) {
   WorkThread* w = (WorkThread*)data;
   
   w->notifyCallbacks();
}

void* WorkThread::threadStartFunc(void *inData)
{
   // Called from newly created pthread, starts the run loop of our thread
   
   WorkThread* thread = static_cast<WorkThread*>(inData);
   thread->run();
}

WorkThread::WorkThread(WorkPerformer* performer,
                       unsigned int sleepTimeMs,
                       unsigned int workPerIteration)
{

   m_impl = new Impl(performer, sleepTimeMs, workPerIteration);

   pthread_create(&m_impl->threadId, NULL, threadStartFunc, this);
}

void WorkThread::scheduleWork(WorkListener* listener, void* data)
{
   WorkLoad w;
   w.workListener = listener;
   w.data = data;

   // Acquire the mutex, add work to work queue, set flag and
   // then broadcast the condition variable so that the thread will
   // wake up
   int rc;
   coreprintln("MT: scheduling work, locking mutex");
   rc = pthread_mutex_lock(&m_impl->mutex);
   m_impl->work.push_back(w);
   m_impl->workToDo = true;
   rc = pthread_cond_broadcast(&m_impl->cond);
   rc = pthread_mutex_unlock(&m_impl->mutex);
   coreprintln("MT: done scheduling work, releasingmutex");
}

bool WorkThread::cancelWork(WorkListener* listener, void* data)
{
   bool erased = false;

   // Acquire the mutex so that we are free to modify the work
   // vector, then try to find the matching work request, if
   // found, remove it from the vector.

   coreprintln("MT: cancelling work, acquiring mutex");
   int rc;
   rc = pthread_mutex_lock(&m_impl->mutex);
   
   for(unsigned int i = 0; i < m_impl->work.size(); i++) {
      WorkLoad& cur = m_impl->work[i];
      if(cur.data == data && cur.workListener == listener) {
         m_impl->work.erase(m_impl->work.begin() + i);
         erased = true;
         break;
      }
   }

   coreprintln("MT: done cancelling work, releasing mutex");
   rc = pthread_mutex_unlock(&m_impl->mutex);

   return erased;
}

void WorkThread::run()
{
   m_impl->running = true;
   
   while(!m_impl->terminated) {
      coreprintln("WT: Start of loop, locking mutex");
      int rc = pthread_mutex_lock(&m_impl->mutex);
                
      // Wait for work to arrive, until so happens, do a conditional
      // wait on the mutex
      while(!m_impl->workToDo && !m_impl->callbackDealtWith) {
         coreprintln("WT: Going to conditional wait, releasing mutex");
         rc = pthread_cond_wait(&m_impl->cond, &m_impl->mutex);
      }
      
      size_t numWorkToDo =
         std::min(static_cast<unsigned int>(m_impl->work.size()),
                  m_impl->workPerIteration);

      vector<WorkLoad> jobsToDo;
         
      for(unsigned int i = 0; i < numWorkToDo; i++) {
         jobsToDo.push_back(m_impl->work[0]);
         m_impl->work.pop_front();
      }
         
      if(m_impl->work.empty()) {
         m_impl->workToDo = false;            
      }

      vector<WorkLoad> finishedTmp;
         
      /*
       *  Since we no longer are dependant upon the work
       *  vector (we have removed the items that we will
       *  work on) we release the mutex. Otherwise, callers
       *  would block until we are completely done when
       *  issuing new scheduling requests.
       */ 
      unsigned int start = TileMapUtil::currentTimeMillis();

      coreprintln("WT: Going to work, releasing mutex");
      rc = pthread_mutex_unlock(&m_impl->mutex);
      
      for(unsigned int i = 0; i < jobsToDo.size(); i++) {
         WorkLoad w = jobsToDo[i];
         m_impl->performer->performWork(w.data);
         finishedTmp.push_back(w);
      }
      
      if(m_impl->sleepTimeMs > 0) {
         usleep(m_impl->sleepTimeMs * 1000);         
      }

      coreprintln("WT: Work done, locking mutex");
      /**
       *  Work now completed, reacquire the mutex and
       *  update the vector containing finished items.
       */ 
      rc = pthread_mutex_lock(&m_impl->mutex);

      
      m_impl->finished.insert(m_impl->finished.end(),
                              finishedTmp.begin(),
                              finishedTmp.end());

      /**
       *  Issue notification to main thread.
       */ 
      if(m_impl->callbackDealtWith) {
         m_impl->callbackDealtWith = false;
         m_impl->scheduler->scheduleOnMainThread(mainThreadCallback,
                                                 this);
      }
      
      coreprintln("WT: Notified, releasing mutex");
      rc = pthread_mutex_unlock(&m_impl->mutex);
   }
   
   m_impl->running = false;

   pthread_exit(NULL);         
}

WorkThread::~WorkThread()
{
   // Make sure that the thread is completely non-operational
   // before deleting stuff
   kill();
   
   while(m_impl->running || !m_impl->callbackDealtWith) {
      // To invoke platform iterations
      Environment::sleep(10);
      usleep(5*1000);
   }
   
   delete m_impl; 
}

void WorkThread::notifyCallbacks()
{
   int rc;

   // First, acquire and clear the items that
   // have been finished

   coreprintln("MT: notifyCallbacks, locking mutex");
   rc = pthread_mutex_lock(&m_impl->mutex);
   vector<WorkLoad> tmp = m_impl->finished;
   m_impl->finished.clear();
   m_impl->callbackDealtWith = true;
   rc = pthread_mutex_unlock(&m_impl->mutex);
   coreprintln("MT: Fetched data, releasing mutex");
   
   // Then, notify all the callbacks

   if(m_impl->running) {
      for(unsigned int i = 0; i < tmp.size(); i++) {
         WorkLoad w = tmp[i];
         w.workListener->onWorkDone(w.data);
      }      
   }
}

void WorkThread::kill()
{
   m_impl->terminated = true;

   int rc;
   coreprintln("WT: kill, locking mutex");
   rc = pthread_mutex_lock(&m_impl->mutex);
   rc = pthread_cond_broadcast(&m_impl->cond);
   rc = pthread_mutex_unlock(&m_impl->mutex);
   coreprintln("WT: kill done, releasing mutex");
}
