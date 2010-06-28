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
#include <string.h>
#include <e32std.h>

#include "Thread.h"
#include "Runnable.h"
#include "ThreadID.h"
#include "Synchronized.h"
#include "Log.h"
#include "WFTextUtil.h"
#include "GlobalData.h"
#include "nav2util.h"
#include <stdio.h>
#include "InnerThread.h"

namespace isab {

#ifdef NAV2_CLIENT_UIQ3
# define USE_THREAD_EXCEPTION_HANDLER
#endif

#if defined USE_THREAD_EXCEPTION_HANDLER
   void logException(enum TExcType arg)
   {
      class Log log("EXCEPTION");
      
      const char* tName = "UnknownThread";
      const char* eName = NULL;
      if( Thread::currentThread() ){
         tName = Thread::currentThread()->getName();
      }
      switch(arg){
      case EExcGeneral:              eName = "EExcGeneral";              break;
      case EExcIntegerDivideByZero:  eName = "EExcIntegerDivideByZero";  break;
      case EExcSingleStep:           eName = "EExcSingleStep";           break;
      case EExcBreakPoint:           eName = "EExcBreakPoint";           break;
      case EExcIntegerOverflow:      eName = "EExcIntegerOverflow";      break;
      case EExcBoundsCheck:          eName = "EExcBoundsCheck";          break;
      case EExcInvalidOpCode:        eName = "EExcInvalidOpCode";        break;
      case EExcDoubleFault:          eName = "EExcDoubleFault";          break;
      case EExcStackFault:           eName = "EExcStackFault";           break;
      case EExcAccessViolation:      eName = "EExcAccessViolation";      break;
      case EExcPrivInstruction:      eName = "EExcPrivInstruction";      break;
      case EExcAlignment:            eName = "EExcAlignment";            break;
      case EExcPageFault:            eName = "EExcPageFault";            break;
      case EExcFloatDenormal:        eName = "EExcFloatDenormal";        break;
      case EExcFloatDivideByZero:    eName = "EExcFloatDivideByZero";    break;
      case EExcFloatInexactResult:   eName = "EExcFloatInexactResult";   break;
      case EExcFloatInvalidOperation:eName = "EExcFloatInvalidOperation";break;
      case EExcFloatOverflow:        eName = "EExcFloatOverflow";        break;
      case EExcFloatStackCheck:      eName = "EExcFloatStackCheck";      break;
      case EExcFloatUnderflow:       eName = "EExcFloatUnderflow";       break;
      case EExcAbort:                eName = "EExcAbort";                break;
      case EExcKill:                 eName = "EExcKill";                 break;
      case EExcUserInterrupt:        eName = "EExcUserInterrupt";        break;
      case EExcDataAbort:            eName = "EExcDataAbort";            break;
      case EExcCodeAbort:            eName = "EExcCodeAbort";            break;
      case EExcMaxNumber:            eName = "EExcMaxNumber";            break;
      case EExcInvalidVector:        eName = "EExcInvalidVector";        break;
      }
      if(0 == strncmp(tName, "Wayfinder", strlen("Wayfinder"))){
         tName += strlen("Wayfinder");
      }
      log.error("Caught %s in thread %s", eName, tName);
   }
#else
   void logException(enum TExcType)
   {
      //do nothing
   }
#endif

   void exceptionHandler(enum TExcType arg)
   {
      logException(arg);
      class Thread* panicThread = Thread::currentThread();
      if( panicThread ){
         //starts a terminateandjoin process in the main thread.
         ::Panic(panicThread); 
         panicThread->m_alive= false;
      }
      User::Exit(1);
   }
}

class isab::Thread*
isab::Thread::currentThread()
{
   // Get from TLS, stored in start_hook.
   isab::TlsGlobalData* globalData = getTlsGlobalData();    
   if(globalData != NULL) {
      class InnerThread* innerThread = globalData->m_innerThread;
      return innerThread ? innerThread->getThread() : NULL;
   } else {
      return NULL;
   }
}

isab::Thread::Thread(const char* name) : m_alive(false)
{
   init(this, name);
}

isab::Thread::Thread(class Runnable* runnable,
                     const char* name) : m_alive(false)
{
   init(runnable, name);
}

void isab::Thread::init(class Runnable* runnable, const char* name)
{
   const char threadPrefix[] = "Wayfinder";
   char* wfname = new char[strlen(name) + sizeof(threadPrefix) + 4];
   strcpy(wfname, threadPrefix);
   strcat(wfname, name);
   m_innerThread = new InnerThread(this, runnable, wfname);
   delete[] wfname;
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

void isab::Thread::Kill(int reason, class Thread* originator)
{
   if(this != originator){
      m_innerThread->getHandle().Kill(reason);
      m_alive = false;
   }
}


bool
isab::Thread::isAlive()
{
   class Synchronized sync(m_mutex);
   return m_alive;
}

class TTimeIntervalMicroSeconds32 CreateInterval(long millis, int nanos)
{
  if ( millis > MAX_INT32 ) {
     millis = MAX_INT32;
     nanos = 0; // Avoid overflow
  }
  if ( millis*1000 < millis ) { // Overflow
     millis = MAX_INT32 / 1000;
     nanos = 0; // Avoid overflow
  }

  class TTimeIntervalMicroSeconds32 interval = millis*1000 + nanos/1000;
  return interval;
}

int
isab::Thread::join(long millis, int nanos)
{ 
   // Hmm. If we join ourselves we should return, I think.
   if ( ThreadID::currentThreadID() == m_innerThread->getID() ) {
      return Thread::PROPER_JOIN;
   }
   if ( ! isAlive() ) {
      return Thread::PROPER_JOIN;
   }

   // Logon request
   // Logon: When this thread dies, the request completes and 
   // status is set.
   class TRequestStatus status;
   m_innerThread->getHandle().Logon( status ); 
   if ( ! isAlive() ) {
      m_innerThread->getHandle().LogonCancel( status );
      User::WaitForRequest( status );
      return Thread::PROPER_JOIN;
   }

   // Timer request
   class RTimer timer;
   if ( timer.CreateLocal() != KErrNone ) {
      // Error
      timer.Close();
      m_innerThread->getHandle().LogonCancel( status );
      User::WaitForRequest( status );
      return Thread::FAIL;
   }
   class TRequestStatus statusTimer;
   class TTimeIntervalMicroSeconds32 interval = CreateInterval(millis, nanos);
   timer.After( statusTimer, interval );

   // Wait for logon or timer to return
   User::WaitForRequest( status, statusTimer );

   if ( statusTimer == KRequestPending ) { // Not timer that returned
       timer.Cancel(); // Cancels any outstanding request for a timer event
       User::WaitForRequest(statusTimer);
       timer.Close(); // Release timer

      if ( status == KErrCancel ) {
         return Thread::TIME_OUT;
      } else if ( status == KErrNoMemory ) {
         return Thread::JOIN_FAIL;
      } else {
         // One of TExitType
         if ( status == EExitKill || status == EExitTerminate || 
              status == EExitPanic ){
            // Dead somehow
            return Thread::PROPER_JOIN;            
         } else if ( status == EExitPending ) { // Still Alive!
            return Thread::TIME_OUT;
         } else {
            // Should never come here, but...
            return Thread::PROPER_JOIN;
         }
      }
   } else { // Timer is set
      // Unset Logon request
      m_innerThread->getHandle().LogonCancel( status );
      User::WaitForRequest(status); //not really necessary ???
      timer.Cancel(); // Cancels any outstanding request for a timer event
      timer.Close(); // Release timer

      if ( statusTimer == KErrNone ) {
         return Thread::TIME_OUT;
      } else {
         return Thread::JOIN_FAIL;
      }
   }
}

int
isab::Thread::sleep(long millis, int nanos)
{
    class RTimer timer;
    if ( timer.CreateLocal() != KErrNone ) {
        // Error
        timer.Close();
        return Thread::FAIL;
    }

    class TRequestStatus status;
    class TTimeIntervalMicroSeconds32 interval = CreateInterval(millis, nanos);
    timer.After( status, interval );
    User::WaitForRequest( status );
    timer.Close();

    if ( status == KErrNone ) {
        return AWOKE;
    } else {
        return FAIL;
    }
}

void
isab::Thread::yield()
{
   // Sleep 0 might work as yield. 
   sleep(0, 0);
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

