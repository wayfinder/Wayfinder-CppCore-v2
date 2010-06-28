/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "Environment.h"
#define CORE_LOGGING_MODULE_NAME "Env"
// #define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE

#if defined (__linux__)
#include <gtk/gtk.h>
#endif

#ifdef IPHONE
#include <IPhoneThreadUtil.h>
#include <CoreFoundation/CFRunLoop.h>
#include <pthread.h>
#include <sched.h>
#endif

#include "CoreDebugPrint.h"

#if defined (__unix__) || defined (__MACH__) 
#include <sys/time.h>
#elif defined(_WIN32_WCE)
#include <windows.h>
/* needed for compiling on Win32 as there is no sys\time.h */
#elif defined (__SYMBIAN32__)
# include <sys/time.h>
# include <e32base.h>
# include <e32std.h>
# include <e32def.h>
#else
#error "Unknown system"
#endif

namespace Environment {

#ifdef __SYMBIAN32__

typedef TUint32          uint32;
typedef TInt64           int64;

inline uint32 LOW(int64 x) 
{
   return x & 0xffffffff;   
}
#endif

unsigned int currentTimeMillis()
{
#if defined (__unix__) || defined (__MACH__) || defined(IPHONE)   
   struct timeval tv;
   gettimeofday(&tv, 0);
   return (tv.tv_usec/1000 + tv.tv_sec * 1000);
#elif defined(__SYMBIAN32__)
   TTime now;
   now.UniversalTime(); // or UniversalTime
   return LOW(now.Int64() / 1000);
#elif defined(_WIN32_WCE)
   return( unsigned int (GetTickCount()) );
#endif 
}

void doPlatformIteration() {
#if defined(__linux__)
   gtk_main_iteration_do(false);
#elif defined(IPHONE)   
   SInt32 res = CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0.1, false);

   switch(res) {
   case kCFRunLoopRunFinished:
      coreprintln("The run loop mode mode has no sources or timers.");
      break;
   case kCFRunLoopRunStopped:
      coreprintln("The run loop was stopped with CFRunLoopStop.");
      break;
   case kCFRunLoopRunTimedOut:
      // coreprintln("The time interval seconds passed.");
      break;
   case kCFRunLoopRunHandledSource:
      break;
   }

   IPhoneThreadUtil::sleep(0.016f);
#elif defined(_WIN32_WCE)
   MSG msg;
   memset( &msg, 0, sizeof(msg) );

   int numMessages = 0;
      
   while( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
   {
      TranslateMessage( &msg );
      DispatchMessage( &msg );
      memset( &msg, 0, sizeof(msg) );
      numMessages++;
   }

   coreprintln("Dispatched %d messages @ %u",
               numMessages, currentTimeMillis());

   Sleep(100);
#elif defined(__SYMBIAN32__)
   TInt error = 0;
   CActiveScheduler::RunIfReady(error, CActive::EPriorityStandard);
#endif
}


bool
conditionalWait(const bool& conditionVariable, unsigned int timeOutMs)
{   
   unsigned int startTime = currentTimeMillis();
      

   while(!conditionVariable) {
      doPlatformIteration();
      
      unsigned int elapsedTime = currentTimeMillis() - startTime;
      
      if(elapsedTime >= timeOutMs) {
         return false;
      }
   }

   return true;
}

void sleep(unsigned int timeOutMs)
{
   unsigned int startTimeMs = currentTimeMillis();

   doPlatformIteration();
   
   while( currentTimeMillis() - startTimeMs < timeOutMs) {
      doPlatformIteration();
   }
}

} // End of namespace Environment


