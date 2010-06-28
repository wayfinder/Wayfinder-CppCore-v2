/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef MONITOR_I_H
#define MONITOR_I_H

#include "Thread.h"
#include "ThreadID.h"

namespace isab {

/** Implements the monitor synchronizing primitive. Used by bracketing 
    the critical code region with calls to the <code>lock</code> and
    <code>unlock</code> methods like this:
    <verbatim>
    Monitor mon;
    mon.lock();
    // Critical code goes here. 
    mon.unlock();
    </verbatim>
    The <code>Monitor</code> also supplies the <code>wait</code>,
    <code>notify</code>, and <code>waitMany</code> methods. Threads
    calling <code>wait</code> blocks until timeout or another thread
    calls one of the <code>notify</code> methods. Threads calling
    either the <code>wait</code> or one of the <code>notify</code>
    methods must already hold the monitors lock or the calls will have
    no effect.
*/
   class Monitor {
#include "../factored/MonitorInterface.incl"
      public:
         /** Claim this monitor. This is a nonblocking version of lock.
         * @return true if the monitor was successfully claimed.
         *         false otherwise.
          */
         Monitor::tryStatus tryLock();
  
      private:
         /** Struct used to keep track of recursive locking of the monitor. */
         struct SyncStackFrame{
            /** The previous lock on this Monitor. */
            SyncStackFrame* prev;
            /** The lock associated with this lock. */
            JTCSynchronized* sync;
            /** Constuctor of SyncstackFrame. 
            * @param frm a pointer to the previous SyncStackFrame. Usually
            *            the syncstack so far.
            * @param snc a pointer to a JTCSynchronized object. *snc
            *            represents the lock on the monitor. 
             */
            SyncStackFrame(SyncStackFrame* frm, JTCSynchronized* snc);
            /** The destructor. Calls delete on the sync member. */
            ~SyncStackFrame();
         };
         /** A stack of locks on this monitor. Used to implement recursive
           locking, i.e. to let a Thread lock several times on the same
           Monitor. */
         SyncStackFrame* m_syncStack;
         /** The <code>JTCMonitor</code> that implements the synchronizing
           parts. */
         JTCMonitor m_monitor;
         /** A mutex that protects all calls to the monitor. Please note that
           while <code>m_mutex</code> is a <code>JTCMutex</code> it cant
           handle recursive calls, i.e. no mutex protected method may call
           another mutex protected method. */
         JTCMutex m_mutex;
         /** Remembers which thread it was that locked the monitor. When a
           thread tries to unlock the monitor, the calling thread is
           compared to this value to ensure that it's the same thread that
           unlocks as locks.
          */
         ThreadID m_owner;
         /** Tests if the currently executing thread owns the lock.
         * @return true if the currently executing thread is the same thread
         *         that locked the monitor.
          */
         bool ownsLock() const;
   };

//===================================================================
//==            Inlines of MonitorImpl::SyncStackFrame             == 

inline Monitor::SyncStackFrame::SyncStackFrame(SyncStackFrame* frm, 
                                               JTCSynchronized* snc) 
  : prev(frm), sync(snc)
{
} 

inline Monitor::SyncStackFrame::~SyncStackFrame()
{
  prev = NULL;
  delete sync;
}

//===================================================================
//==            Inlines of MonitorImpl                             ==

inline bool Monitor::ownsLock() const
{
  return m_owner == ThreadID::currentThreadID();
}


inline Monitor::Monitor() : m_syncStack(NULL)
{
}

inline Monitor::~Monitor()
{
}

}
#endif // MONITOR_I_H
