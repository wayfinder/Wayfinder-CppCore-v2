/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "PALRunLoop.h"
#include "PALRunLoopLinux.h"

namespace pal {

GStaticMutex GSync::s_timerMutex = G_STATIC_MUTEX_INIT;

/**
 *  The function that will be called via the gdk main
 *  loop.
 */ 
gboolean timer_callback(void* callback) {   
   CallbackData* cdata = (CallbackData*)callback;

   TimerMap& timers(cdata->context->timers);

   TimerMap::iterator itr = timers.find(cdata->id);
   
   if(itr != timers.end()) {
      timers.erase(cdata->id);
   }

   cdata->func(cdata->id, cdata->data);
   delete cdata;
   
   return false;
}

pstatus performIteration()
{
   // Do a non blocking-iteration
   gtk_main_iteration_do(false);
   return PAL_OK;
}

pstatus createRunLoopContext(RunLoopContext** context) {
   *context = new RunLoopContext;
   (*context)->timer_count = 0;
   
   return PAL_OK;
}

pstatus requestTimer(RunLoopContext* context,
                     unsigned int timeOutMs,
                     pal_run_loop_func func,
                     void* data,
                     unsigned int* id)
{
   GSync sync;
   unsigned int timerId = ++context->timer_count;
   
   CallbackData *info = new CallbackData;
   info->context = context;
   info->id = timerId;
   info->data = data;
   info->func = func;
   
   GSource* source = g_timeout_source_new(timeOutMs);
   g_source_set_callback(source,
                         (GSourceFunc)timer_callback,
                         info,
                         NULL);
   g_source_attach(source, NULL);
                         
   TimerData timerData = {source, info};
   context->timers[timerId] = timerData;

   *id = timerId;
   
   return PAL_OK;
}

pstatus cancelTimer(TimerMap& timers, TimerMap::iterator itr) {
   if (itr != timers.end()) {
      g_source_destroy(itr->second.source);

      delete itr->second.info;
      return PAL_OK;
   } else {
      return PAL_ERR_EXPIRED_TIMER;      
   }   
}

pstatus cancelTimer( RunLoopContext* context,
                     unsigned int timerId )
{
   if(timerId > context->timer_count) {
      return PAL_ERR_INVALID_TIMER;
   }
   
   GSync sync;
   TimerMap& timers(context->timers);

   TimerMap::iterator itr = timers.find(timerId);

   pstatus ret = cancelTimer(timers, itr);
   
   if(ret == PAL_OK) {
      timers.erase(itr);
   }
}

pstatus destroyRunLoopContext(RunLoopContext* context)
{
   for(TimerMap::iterator itr = context->timers.begin();
       itr != context->timers.end();
       itr++)
   {
      cancelTimer(context->timers, itr);
   }
   
   delete context;
   return PAL_OK;
}

} // End of namespace pal


