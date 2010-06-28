/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "config.h"

#include "GtkTileMapToolkit.h"
#include "MapLibCallback.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <assert.h>



/**
 * Sync class to be used in GDK callbacks, such as
 * timer and idle callbacks.
 */
class GdkCBSync {
public:
   GdkCBSync() {
      gdk_threads_enter();
   }
   ~GdkCBSync() {
      gdk_threads_leave();
   }
};

static GStaticMutex s_timerMutex = G_STATIC_MUTEX_INIT;

/**
 * Sync class in order to protect our own data structures
 * from access from different threads.
 */
class GSync {
public:
   GSync() {
      g_static_mutex_lock(&s_timerMutex);
   }
   ~GSync() {
      g_static_mutex_unlock(&s_timerMutex);
   }
};


GtkTileMapToolkit::GtkTileMapToolkit():
   m_lastTimer(0),
   m_lastIdle(0)
{

}

GtkTileMapToolkit::~GtkTileMapToolkit()
{
   for(TimerMap_t::iterator it = m_timers.begin(); it != m_timers.end(); it++)
   {
      g_source_destroy(it->second.source);
      delete it->second.info;
   }
   
   for(IdleTimerMap_t::iterator it = m_idles.begin(); it != m_idles.end(); it++)
   {
      g_source_destroy(it->second.source);
      delete it->second.info;
   }
}

gboolean GtkTileMapToolkit::idleCallback(CallbackInfo* info) 
{
   GdkCBSync sync;
   uint32 idle_number = info->context_id;
   
   info->area->m_idles[idle_number].listener->runIdleTask(idle_number);
   info->area->m_idles.erase(idle_number);
   
   delete info;

   return false;
}

gboolean GtkTileMapToolkit::timeoutCallback(CallbackInfo* info)
{
   
   GdkCBSync sync;
   uint32 timer_id = info->context_id;
   
   TimerMap_t& timers(info->area->m_timers);

   TimerMap_t::iterator itr = timers.find(timer_id);
   
   if(itr != timers.end()) {
      TileMapTimerListener* listener = itr->second.listener;
      timers.erase(timer_id);
      listener->timerExpired(timer_id);
   }

   delete info;
   
   return false;
}

uint32
GtkTileMapToolkit::requestTimer(TileMapTimerListener* client,
                             uint32 timeoutMS,
                             TileMapToolkit::priority_t )
{

   GSync sync;
   uint32 timerID = ++m_lastTimer;
   
   CallbackInfo *info = new CallbackInfo;
   info->area = this;
   info->context_id = timerID;
   
   GSource* source = g_timeout_source_new(timeoutMS);
   g_source_set_callback(source,
                         (GSourceFunc)GtkTileMapToolkit::timeoutCallback,
                         info,
                         NULL);
   g_source_attach(source, NULL);
                         
   TimerData data = {client, source, info};
   m_timers[timerID] = data;

   return timerID;
}

bool
GtkTileMapToolkit::cancelTimer(TileMapTimerListener* client,
                               uint32 timerID)
{   
   GSync sync;
   TimerMap_t::iterator itr = m_timers.find(timerID);
   if(itr != m_timers.end()) {
      g_source_destroy(itr->second.source);

      delete itr->second.info;
      m_timers.erase(itr);
      return true;
   } else {
      return false;      
   }
}

TileMapPeriodicTimer*
GtkTileMapToolkit::createPeriodicTimer( TileMapTimerListener* listener,
                                        TileMapToolkit::priority_t prio )
{
   assert(listener);
   GSync sync;
   int lastTimer = ++m_lastTimer;
   return new GenericTileMapPeriodicTimer( listener,
                                           this,
                                           lastTimer,
                                           prio );
}

uint32
GtkTileMapToolkit::requestIdle(TileMapIdleListener* client)
{   
   GSync sync;
   uint32 idleID = ++m_lastIdle;

   CallbackInfo *info = new CallbackInfo;

   info->area = this;
   info->context_id = idleID;

   GSource* source = g_idle_source_new();
   g_source_set_priority(source,  G_PRIORITY_HIGH_IDLE);
   g_source_set_callback(source,
                         (GSourceFunc)GtkTileMapToolkit::idleCallback,
                         info,
                         NULL);
   g_source_attach(source, NULL);

   IdleData data = {client, source, info};
   m_idles[idleID] = data;

   return idleID;
}

bool
GtkTileMapToolkit::cancelIdle(TileMapIdleListener* client,
                              uint32 idleID)
{
   //TODO: REMOVE HERE
   return false;
}


void 
GtkTileMapToolkit::initCallbackTrigger()
{
   // No need to do anything for Gtk.
}

/**
 * Temporary wrapper class for callback listener 
 * to be used as TileMapTimerListener.
 */
class MapLibCallbackTimerListener : public TileMapTimerListener
{
public:
   MapLibCallbackTimerListener(WFAPI::MapLibCallback* callback) :
         m_callback(callback) {}


   virtual ~MapLibCallbackTimerListener() {}

   /**
    *   To be called by TileMapToolkit when a timer expires.
    *   @param id The id of the timer as given when reqTimeout was
    *             called.
    */
   virtual void timerExpired(uint32 id) {
      m_callback->callback();
      delete this;
   }

private:
   WFAPI::MapLibCallback* m_callback;
};

/**
 * Gtk calls this static function when it can after the g_idle_add_full.
 */
gboolean gtkCallback(void* callback) 
{
   // Do the real callback.
   static_cast<WFAPI::MapLibCallback*>(callback)->callback();
   // Return false to stop this from being called again, must be triggered
   // by triggerCallbacks.
   return false;
}

void 
GtkTileMapToolkit::triggerCallbacks(WFAPI::MapLibCallback* callback)
{
   //requestTimer(new MapLibCallbackTimerListener(callback), 1, PRIO_HIGH);
   g_idle_add_full(G_PRIORITY_DEFAULT, gtkCallback, callback, NULL);
}


