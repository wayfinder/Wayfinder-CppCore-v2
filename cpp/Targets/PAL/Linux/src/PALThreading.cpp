/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "PALThreading.h"
#include "JTC.h"
#include <cassert>

namespace pal {

class LinuxJTCThread : public JTCThread {
public:
   LinuxJTCThread(ThreadStartFunc function, void* data, const char* name) :
      JTCThread(name),
      m_function(function),
      m_data(data),
      m_hasExited(false),
      m_hasBeenStarted(false),
      m_name(name)
      {
      }

   virtual ~LinuxJTCThread() {
   }


   virtual void run() {
      m_hasBeenStarted = true;
      m_function(m_data);
      m_hasExited = true;
   }

   bool hasExited() {
      return m_hasExited;
   }

   bool hasBeenStarted() {
      return m_hasBeenStarted;
   }

   void setOwnerStruct(Thread thread) {
      m_ownerStruct = thread;
   }

   Thread getOwnerStruct() {
      return m_ownerStruct;
   }
   
   const char* getName() {
      return m_name;
   }

private:
   ThreadStartFunc m_function;
   void* m_data;
   bool m_hasExited;
   bool m_hasBeenStarted;
   Thread m_ownerStruct;
   const char* m_name;
};

struct LinuxThread {
   JTCHandleT<LinuxJTCThread> thread;
};

struct LinuxMutex {
   JTCMutex mutex;
};

struct LinuxRecursiveMutex {
   JTCRecursiveMutex recursiveMutex;
};

struct LinuxConditional {
   JTCCond conditional;
};

pstatus initThread() {
   JTCInit::initJTC();
   return PAL_OK;
}

pstatus deinitThread() {
   delete JTCInit::g_jtcInitialize;
   JTCInit::g_jtcInitialize = NULL;
   return PAL_OK;
}

Thread createThread(ThreadStartFunc function, void* data, const char* name) {
   LinuxThread* thread;
   if(thread = new LinuxThread()) {
      thread->thread = new LinuxJTCThread(function, data, name);
      thread->thread->setOwnerStruct(thread);
      return thread;
   } else {
      return NULL;
   }
}

pstatus startThread(Thread thread)
{
   if(thread) {
      LinuxThread* linuxThread = (LinuxThread*)thread;
      if(linuxThread->thread) {
         linuxThread->thread->start();
         return PAL_OK;
      } else {
         return PAL_ERR_CANNOT_START_BROKEN_THREAD;
      }
   } else {
      return PAL_ERR_CANNOT_START_NULL_THREAD;
   }
}

pstatus destroyThread(Thread thread)
{
   if(thread) {
      LinuxThread* linuxThread = (LinuxThread*)thread;
      //actually the below code could be replaced with a simple 
      //delete linuxThread;
      //but all these checks could be nice to have when replacing JTC with raw
      //pthreads, if we ever do that.

      
      //fist check if the thread has returned from it's run function:
      if (linuxThread->thread && linuxThread->thread->hasExited()) {
         //in this case the thread has cleaned itself up, so just delete the struct
         delete linuxThread;
         return PAL_OK;
      }
      else {
         //it has not yet exited, so it might be still running, or it has not
         ///been started.
         if (linuxThread->thread && linuxThread->thread->hasBeenStarted()) { 
            return PAL_ERR_CANNOT_DESTROY_RUNNING_THREAD;           
         }
         else {
            //do not delete the JTC implementation, since it's cleaned up
            //automatically thanks to the autopointer style declaration in the
            //LinuxThread struct
            //delete linuxThread->thread;
            delete linuxThread;
            return PAL_OK;
         }
      }
   }
   else {
      return PAL_ERR_CANNOT_DESTROY_NULL_THREAD;
   }
}

Thread currentThread() {
   JTCThread* jtc = JTCThread::currentThread();
   LinuxJTCThread* linuxJTCThread = static_cast<LinuxJTCThread*>(jtc);
   return linuxJTCThread->getOwnerStruct();
}

const char* getThreadName(Thread thread) {
   if(thread) {
      LinuxThread* linuxThread = (LinuxThread*)thread;
      return linuxThread->thread->getName();
   } else {
      return NULL;
   }
}

wf_int32 getCurrentThreadID() {
#ifdef IPHONE
	//this needs to be fixed instead of doing this:
   return 0;
#else
   return (wf_int32)JTCThreadId::self();
#endif
	
}

Mutex createMutex() {
   LinuxMutex* linuxMutex;
   if(linuxMutex = new LinuxMutex) {
      return linuxMutex;
   } else {
      //could not even create a LinuxMutex
      return NULL;
   }
}

pstatus destroyMutex(Mutex mutex) {
   LinuxMutex* linuxMutex = (LinuxMutex*)mutex;
   delete linuxMutex;
   return PAL_OK;
}

bool lockMutex(Mutex mutex) {
   LinuxMutex* linuxMutex = (LinuxMutex*)mutex;
   linuxMutex->mutex.lock();
   //currently JTC always return true on lock operations
   return true;
}

bool unlockMutex(Mutex mutex) {
   LinuxMutex* linuxMutex = (LinuxMutex*)mutex;
   linuxMutex->mutex.unlock();
   //currently JTC always return true on unlock operations
   return true;
}

bool tryLockMutex(Mutex mutex) {
   LinuxMutex* linuxMutex = (LinuxMutex*)mutex;
   return linuxMutex->mutex.trylock();
}

RecursiveMutex createRecursiveMutex() {
   LinuxRecursiveMutex* linuxRecursiveMutex;
   if(linuxRecursiveMutex = new LinuxRecursiveMutex) {
      return linuxRecursiveMutex;

   } else {
      //could not even create a LinuxRecursiveMutex
      return NULL;
   }
}

pstatus destroyRecursiveMutex(RecursiveMutex recursiveMutex) {
   LinuxRecursiveMutex* linuxRecursiveMutex = (LinuxRecursiveMutex*)recursiveMutex;
   delete linuxRecursiveMutex;
   return PAL_OK;
}

bool lockRecursiveMutex(RecursiveMutex recursiveMutex) {
   LinuxRecursiveMutex* linuxRecursiveMutex = (LinuxRecursiveMutex*)recursiveMutex;
   return linuxRecursiveMutex->recursiveMutex.lock();
}

bool unlockRecursiveMutex(RecursiveMutex recursiveMutex) {
   LinuxRecursiveMutex* linuxRecursiveMutex = (LinuxRecursiveMutex*)recursiveMutex;
   return linuxRecursiveMutex->recursiveMutex.unlock();
}

bool tryLockRecursiveMutex(RecursiveMutex recursiveMutex) {
   LinuxRecursiveMutex* linuxRecursiveMutex = (LinuxRecursiveMutex*)recursiveMutex;
   return linuxRecursiveMutex->recursiveMutex.trylock();
}

Conditional createConditional() {
   LinuxConditional* linuxConditional;
   if(linuxConditional = new LinuxConditional) {
         return linuxConditional;
   } else {
      return NULL;
   }
}

pstatus destroyConditional(Conditional conditional) {
   LinuxConditional* linuxConditional = (LinuxConditional*)conditional;
   delete linuxConditional;
   return PAL_OK;   
}

pstatus conditionalWait(Conditional conditional, Mutex mutex) {
   LinuxConditional* linuxConditional = (LinuxConditional*)conditional; 
   LinuxMutex* linuxMutex = (LinuxMutex*)mutex;  
   linuxConditional->conditional.wait(linuxMutex->mutex);
   return PAL_OK;
}

pstatus conditionalWait(Conditional conditional, Mutex mutex, wf_uint32 timeout) {
   LinuxConditional* linuxConditional = (LinuxConditional*)conditional; 
   LinuxMutex* linuxMutex = (LinuxMutex*)mutex;  
   bool status;
   status = linuxConditional->conditional.wait(linuxMutex->mutex, timeout);
   if(status == true) {
      return PAL_OK;      
   }
   if(status == false) {
      return PAL_CONDITIONAL_WOKE_UP_FROM_TIMEOUT;
   }
   return PAL_OK; 
}

pstatus conditionalWaitRecursive(Conditional conditional, RecursiveMutex recursiveMutex) {
   LinuxConditional* linuxConditional = (LinuxConditional*)conditional; 
   LinuxRecursiveMutex* linuxRecursiveMutex = (LinuxRecursiveMutex*)recursiveMutex;
   linuxConditional->conditional.wait(linuxRecursiveMutex->recursiveMutex);
   return PAL_OK;
}

pstatus conditionalWaitRecursive(Conditional conditional, RecursiveMutex recursiveMutex, wf_uint32 timeout) {
   LinuxConditional* linuxConditional = (LinuxConditional*)conditional; 
   LinuxRecursiveMutex* linuxRecursiveMutex = (LinuxRecursiveMutex*)recursiveMutex;
   bool status;
   status = linuxConditional->conditional.wait(linuxRecursiveMutex->recursiveMutex,
   timeout);
   if(status == true) {
      return PAL_OK;      
   }
   if(status == false) {
      return PAL_CONDITIONAL_WOKE_UP_FROM_TIMEOUT;
   }
   return PAL_OK;
}

pstatus conditionalSignal(Conditional conditional) {
   LinuxConditional* linuxConditional = (LinuxConditional*)conditional;
   linuxConditional->conditional.signal();
   return PAL_OK;
}

pstatus conditionalBroadcast(Conditional conditional) {
   LinuxConditional* linuxConditional = (LinuxConditional*)conditional;
   linuxConditional->conditional.broadcast();
   return PAL_OK;
}

pstatus threadSleep(int mseconds) {
   JTCThread::sleep(mseconds);
   return PAL_OK;
}

} // End of namespace pal
