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
#include "Monitor.h"
#include "Thread.h"
#include "TimeUtils.h"

namespace isab{

Monitor::waitStatus
Monitor::wait(long timeout)
{
  Monitor::waitStatus status = Monitor::DONT_OWN;
  m_mutex.lock();
  if(ownsLock()){
    ThreadID owner = m_owner;
    SyncStackFrame* syncStack = m_syncStack;
    m_owner = ThreadID();
    m_syncStack = NULL;
    if(timeout >= 0){
      m_mutex.unlock();
      try{
        long time = TimeUtils::millis();
        m_monitor.wait(timeout);
        time = TimeUtils::millis() - time;
        if(time > timeout){
          status = Monitor::TIMED_OUT;
        } else {
          status = Monitor::WOKE_ON_NOTIFY;
        }
      }
      catch(JTCSystemCallException){
        status = Monitor::WAIT_FAIL;
      }
      catch(JTCIllegalMonitorStateException){
        /* This should never happen. All necessary checks that the
           calling Thread owns this monitor has already been made.*/
        status = Monitor::DONT_OWN;
      }
      catch(JTCInterruptedException){
        status = Monitor::INTERRUPTED;
      }
      m_mutex.lock();
    } else {
      m_mutex.unlock();
      try{
        m_monitor.wait();
        status = Monitor::WOKE_ON_NOTIFY;
      }
      catch(JTCSystemCallException){
        status = Monitor::WAIT_FAIL;
      }
      catch(JTCIllegalMonitorStateException){
        /* This should never happen. All necessary checks that the
           calling Thread owns this monitor has already been made.*/
        status = Monitor::DONT_OWN;
      }
      catch(JTCInterruptedException){
        status = Monitor::INTERRUPTED;
      }
      m_mutex.lock();
    }
    m_owner = owner;
    m_syncStack = syncStack;
  }
  m_mutex.unlock();
  return status;
}

bool
Monitor::notify()
{
  bool ret = false;
  m_mutex.lock();
  if(ownsLock()){
    ret = true;
    m_monitor.notify();
  }
  m_mutex.unlock();
  return ret;
}

bool
Monitor::notifyAll()
{
  bool ret = false;
  m_mutex.lock();
  if(ownsLock()){
    ret = true;
    m_monitor.notifyAll();
  }
  m_mutex.unlock();
  return ret;
}

Monitor::lockStatus
Monitor::lock()
{
  Monitor::lockStatus status = Monitor::LOCK_LOCKED;
  JTCSynchronized* tmpSync = NULL;
  SyncStackFrame* prevFrame = NULL;
  try{
    tmpSync = new JTCSynchronized(m_monitor);
  }
  catch(JTCSystemCallException){
    status = Monitor::LOCK_FAIL;
  }
  if(status == Monitor::LOCK_LOCKED && tmpSync != NULL){
    m_mutex.lock();
    prevFrame = m_syncStack;
    m_syncStack = new SyncStackFrame(prevFrame, tmpSync);
    m_owner = ThreadID::currentThreadID();
    m_mutex.unlock();
  }
  return status;
}

Monitor::tryStatus
Monitor::tryLock()
{
  Monitor::tryStatus status = Monitor::ALREADY_LOCKED;
  m_mutex.lock();
  ThreadID currentID = ThreadID::currentThreadID();
  if(m_owner == ThreadID() || ownsLock()){
    status = Monitor::TRY_LOCKED;
    JTCSynchronized* tmpSync = NULL;
    SyncStackFrame* prevFrame = NULL;
    m_mutex.unlock();
    try{
      tmpSync = new JTCSynchronized(m_monitor);
    }
    catch(JTCSystemCallException){
      status = Monitor::TRY_FAIL;
    }
    m_mutex.lock();
    if(status != Monitor::TRY_FAIL && tmpSync != NULL && prevFrame != NULL){
      prevFrame = m_syncStack;
      m_syncStack = new SyncStackFrame(prevFrame, tmpSync);
      m_owner = currentID;
    }
  }
  m_mutex.unlock();
  return status;
}

Monitor::unlockStatus
Monitor::unlock()
{
  Monitor::unlockStatus status = Monitor::NOT_OWNER;
  m_mutex.lock();
  if(ownsLock()){
    SyncStackFrame* top = m_syncStack;
    m_syncStack = top->prev;
    delete top;
    if(m_syncStack == NULL) {
       m_owner = ThreadID();
       status = Monitor::RELEASED;
    } else {
       status = Monitor::STILL_OWNER;
    }
  }
  m_mutex.unlock();
  return status;
}

}
