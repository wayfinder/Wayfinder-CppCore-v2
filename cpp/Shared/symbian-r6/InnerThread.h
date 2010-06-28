/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _INNERTHREAD_H_
#define _INNERTHREAD_H_

#include "Thread.h"
#include "Runnable.h"
#include "ThreadID.h"
#include "Synchronized.h"
#include "Log.h"
#include "GlobalData.h"
#include "nav2util.h"

namespace isab {

class Thread;
class Runnable;

/**
 *   InnerThread holds symbian specific thread info.
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
   /** Destructor. */
   ~InnerThread();
      
   /**
    *   Returns the Thread of the InnerThread.
    */
   Thread* getThread() const;
      
   /**
    *  Returns the handle of the SymbianThread.
    */
   class RThread& getHandle();
      
   /**
    *    Starts a new thread.
    */
   int start();

   /**
    *    Returns the thread id of the thread.
    */
   class ThreadID getID() const;

   /**
    *    Returns the runnable of the inner thread.
    */
   class Runnable* getRunnable() const;

   /**
    *    Returns the name of the innner thread.
    */
   const char* getName() const;

   int getThreadInfo(char* data, int datalength);

private:

   /** Runnable - the object to start the run thread in */
   class Runnable* m_runnable;
   /** The Thread object that belong to the thread */
   class Thread* m_thread;

   /** The ThreadID thread id of the inner thread */
   class ThreadID m_id;

   /** The thread handle of the inner thread */
   class RThread m_threadHandle;

   /** The name of the thread */
   char* m_name;

   /** Keeps track of wether the call to m_threadHandle.Create()
    * was successfull or not*/
   bool m_created;

   /**
    * Sets alive in Thread, used by start_hook.
    */
   void setAlive( bool alive );

   /**
    * Start function when creating thread.
    */
   static TInt start_hook( TAny *aPtr );
};

}

#endif /* _INNERTHREAD_H_ */
