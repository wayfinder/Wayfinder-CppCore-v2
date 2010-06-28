/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "arch.h"

#include <windows.h>
#include "Thread.h"
#include "Runnable.h"
#include "ThreadID.h"
#include "Synchronized.h"

// FIXME: Move to init-function. It could fail.
DWORD isab::Thread::c_storageIndex = TlsAlloc();

namespace isab {
   
   /**
    *   InnerThread holds windows specific thread info.
    *   A bit messy.
    */
   class InnerThread {
   public:
      /**
       *   Creates a new inner thread with the name name.
       */
      InnerThread(Thread* thread,
                  Runnable* runnable,
                  const char* name);
      
      ~InnerThread();
                 
      /**
       *   Returns the Thread of the InnerThread.
       */
      Thread* getThread() const;
      
      /**
       *  Returns the handle of the WindowsThread.
       */
      HANDLE getHandle() const;
      
      /**
       *    Starts a new thread.
       */
      int start();

      /**
       *    Returns the thread id of the thread.
       */
      ThreadID getID() const;

      /**
       *    Returns the runnable of the inner thread.
       */
      Runnable* getRunnable() const;

      /**
       *    Returns the name of the innner thread.
       */
      const char* getName() const;

      int getThreadInfo(char* data, int dataLength);
   private:

      /** Runnable - the object to start the run thread in */
      Runnable* m_runnable;
      /** The Thread object that belong to the thread */
      Thread* m_thread;

      /** The ThreadID thread id of the inner thread */
      ThreadID m_id;

      /** The raw windows thread id */
      DWORD m_rawId;

      /** The thread handle of the inner thread */
      HANDLE m_threadHandle;

      /** The name of the thread */
      char* m_name;
      
      /**
       *  Start function that will get the runnable by casting
       *  a void pointer.
       */
      static DWORD WINAPI start_hook(LPVOID void_innerThread);
   };
}

isab::InnerThread::
InnerThread(Thread* thread,
            Runnable* runnable,
            const char* name)
{
   m_runnable  = runnable;
   m_thread    = thread;
   m_threadHandle = NULL;
   if ( name == NULL ) {
      name = "NULL";
   }
   m_name = new char[strlen(name)+1];
   strcpy(m_name, name);
   // Don't use the name yet
}

isab::InnerThread::~InnerThread()
{
   delete[] m_name;
   m_name = NULL;
}

isab::ThreadID
isab::InnerThread::getID() const
{
   return m_id;
}

isab::Thread*
isab::InnerThread::getThread() const
{
	return m_thread;
}

isab::Runnable*
isab::InnerThread::getRunnable() const
{
	return m_runnable;
}

isab::Thread*
isab::Thread::currentThread()
{
   void* ptr = TlsGetValue(c_storageIndex);
   InnerThread* innerThread = (InnerThread*)ptr;
   if ( ! innerThread ) {
      return NULL;
   }
   return innerThread->getThread();
}

HANDLE
isab::InnerThread::getHandle() const
{
	return m_threadHandle;
}

const char* 
isab::InnerThread::getName() const
{
   return m_name;
}


int
isab::InnerThread::start()
{
   // The documentation says that the parameter is
   // ignored in the low level functions, but does
   // not mention it in the AfxBeginThread documentation.
   unsigned stackSize = 0; 
   
   // There is at least three ways to make a thread.
   if ( m_threadHandle != NULL) {
      return Thread::ALREADY_STARTED;
   }
   m_threadHandle = CreateThread(NULL,              // lpThreadAttributes
                                 stackSize,         // dwStackSize
                                 start_hook,        // lpStartAddress
                                 (void*)this,       // lpParameter
                                 CREATE_SUSPENDED,  // dwCreationFlags
                                 &m_rawId);         // lpThreadID
   if (m_threadHandle == NULL) {
      return Thread::UNKNOWN_FAILURE;
   }
   
   m_id = ThreadID(m_rawId, 0 /* m_threadHandle*/ );
   m_thread->setAlive(true);
   //
   // Thread is started, but suspended. When we get here the needed
   // data (m_rawId, m_id, m_threadHandle) is already valid.
   ResumeThread(m_threadHandle);   // FIXME - handle return value
   return Thread::GOOD_START;

// Another way
//    if ( m_winThread == NULL ) {
//       m_winThread = AfxBeginThread((AFX_THREADPROC)start_hook,
//                                    (void*)this,
//                                    THREAD_PRIORITY_NORMAL,
//                                    stackSize);
//       if ( m_winThread == NULL ) {
//          return Thread::UNKNOWN_FAILURE;
//       } else {
//          // Now we should be running start_hook in a new thread.
//          return Thread::GOOD_START;
//       }
//    } else {
//       return Thread::ALREADY_STARTED;
//    }
}

DWORD WINAPI
isab::InnerThread::start_hook(LPVOID void_innerThread)
{
   // Extract the inner thread sent to the function as a void.
   InnerThread* innerThread = (InnerThread*)void_innerThread;
   
   // Register the innerthread
   // FIXME: Ensure that no races can occure here.
   TlsSetValue(Thread::c_storageIndex, innerThread);
   innerThread->m_runnable->run();
   innerThread->m_thread->setAlive(false);
   // The thread has stopped.
   // Unregister the inner thread
   TlsSetValue(Thread::c_storageIndex, NULL);
   return 0;
}

int isab::InnerThread::getThreadInfo(char* data, int dataLength)
{
   snprintf(data, dataLength, "%s : rawId: 0x%"PRIx32" : handle: %p",
            getName(), m_rawId, m_threadHandle);
   data[dataLength - 1] = '\0';
   return strlen(data);
}


isab::Thread::Thread(const char* name)
{
   // Create new inner thread.
   m_innerThread = new InnerThread(this, this, name);
}

isab::Thread::Thread(Runnable* runnable,
                     const char* name)
{
   m_innerThread = new InnerThread(this, runnable, name);
}

isab::Thread::~Thread()
{
   delete m_innerThread;
}

int
isab::Thread::start()
{
   return m_innerThread->start();
}

void
isab::Thread::terminate()
{
   // Send the terminate on to the runnable if
   // it isn't this thread.
   if ( this != m_innerThread->getRunnable() ) {
      m_innerThread->getRunnable()->terminate();
   }
}

bool
isab::Thread::isAlive()
{
   Synchronized sync(m_mutex);
   return m_alive;
}

int
isab::Thread::join(long millis, int nanos)
{   
   // Hmm. If we join ourselves we should return, I think.
   if ( ThreadID::currentThreadID() == m_innerThread->getID() ) {
      return Thread::PROPER_JOIN;
   }

   // FIXME: Races?
   if ( ! isAlive() ) {
      return Thread::PROPER_JOIN;
   }
   DWORD timeout;
   if ( millis < 0 && nanos < 0 ) {
      timeout = INFINITE;
   } else {
      timeout = millis + (nanos / 1000);
   }
   DWORD waitRes = WaitForSingleObject(m_innerThread->getHandle(),
         millis + (nanos / 1000) );
   if ( waitRes == WAIT_TIMEOUT ) {
      return Thread::TIME_OUT;         
   } else if ( waitRes == WAIT_ABANDONED ) {
      return Thread::JOIN_FAIL;
   }
   return Thread::PROPER_JOIN;
}


int
isab::Thread::sleep(long millis, int nanos)
{
   Sleep(millis);
   return AWOKE;
}

void
isab::Thread::yield()
{
   // Sleep 0 means yield.
   Sleep(0);
}

const char* 
isab::Thread::getName() const
{
   return m_innerThread->getName();
}

int isab::Thread::getThreadInfo(char* data, int dataLength)
{
   return m_innerThread->getThreadInfo(data, dataLength);
}
