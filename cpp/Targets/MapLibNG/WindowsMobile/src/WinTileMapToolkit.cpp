/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "TileMapHandler.h"
#include "WinTileMapToolkit.h"
#include "LogFile.h"
#include <algorithm>

#define CORE_LOGGING_MODULE_NAME "WinTileMapToolkit"
// #define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE

#include "CoreDebugPrint.h"

/* the timer list */
static WinTileMapToolkit::timerList_t gTimerList;

/* the idle list */
static WinTileMapToolkit::idleList_t gIdleList;

/* our idle handler's handle */
HINSTANCE gAppInstance = NULL;

/* the handle to the parent window */
HWND g_parentWin;

/* the current ID of Idle timers */
#define INITIAL_IDLE_TIMER_ID 0x100
#define INVALID_IDLE_TIMER_ID 0x0
static uint32 gCurrentIdleTimerId = INITIAL_IDLE_TIMER_ID;

static bool gIdleTimerScheduled = false;

#define NUM_IDLE_TO_RUN    64

/* the timer event handling procedure */
static VOID CALLBACK TimerHandler(HWND hwnd,
                                  UINT uMsg,
                                  UINT idEvent,
                                  DWORD dwTime)
{
   /* redundant check to make sure that the event is correct */
   if(uMsg == WM_TIMER) {
      /* handle the timer */
      WinTileMapToolkit::timerList_t::iterator it = gTimerList.find(idEvent);
      /* check if timer is in list */
      if(it == gTimerList.end()) return;

      /* kill the timer so it does not repeat again! */
      KillTimer(g_parentWin, idEvent);
      /* remove the timer from the list */
      TileMapTimerListener* listener = it->second;
      uint32 nIdEvent = it->first;
      gTimerList.erase(it);      
      listener->timerExpired(nIdEvent);
   }
   return;
}

static VOID CALLBACK IdleTimerHandler(HWND hwnd,
                                      UINT uMsg,
                                      UINT idEvent,
                                      DWORD dwTime)
{
   if(uMsg == WM_TIMER) {
      MSG msg;

      bool messagesInQueue = HIWORD(GetQueueStatus(QS_ALLINPUT)) != 0;
       
      // bool messagesInQueue = PeekMessage(&msg,
      //                                    g_parentWin,
      //                                    0, 0,
      //                                    PM_NOREMOVE);
      
      if(!messagesInQueue) {
         coreprintln("No other pending messages in queue, "
                     "executing at most %u out of %d idle tasks.",
                     NUM_IDLE_TO_RUN, gIdleList.size());

         unsigned int numProcessed = 0;
         
         // Run all idle tasks
         while(!gIdleList.empty() && numProcessed++ < NUM_IDLE_TO_RUN) {            
            WinTileMapToolkit::idleList_t::value_type entry = gIdleList.front();

            gIdleList.pop_front();
            entry.second->runIdleTask(entry.first);
         }
         
         if(gIdleList.empty()) {
            // Then remove periodic timer 
            KillTimer(g_parentWin, idEvent);
            
            gIdleTimerScheduled = false;
         }
      } else {
         coreprintln("Pending messages (%u) in queue == not idle state",
                     msg.message);
      }
   }
}
   
/* constructor */
WinTileMapToolkit::WinTileMapToolkit(HWND parentWin, HINSTANCE appInst)
:  m_nCurrTimerId(0), m_pWinPaintCallback(NULL),
   m_bCanRepaint(TRUE)
{
   coreprintln(__FUNCTION__);
   gAppInstance = appInst;
   g_parentWin = parentWin;   
}

/* destructor */
WinTileMapToolkit::~WinTileMapToolkit()
{
   /**         This only makes sense if there is just one instance of 
     *         WinTileMapToolkit. Since, deleting one object will clean
     *         up the timers of the other toolkits too. Maybe we should
     *         not clean up the timers at all since Windows will do that
     *         for us at shutdown (most probably).
     */
   /* clear up the timer list */
   for(timerList_t::iterator it = gTimerList.begin();
       it != gTimerList.end();
       ++it) {
          /* cancel the timer */
          KillTimer(g_parentWin, it->first);
   }
   /* empty the list */
   gTimerList.clear();

   /* clear up the idle list */
   gIdleList.clear();
}

/**
 *   Requests a timer from the system. When the timer
 *   expires client->timerExpired(id) should be called.
 *   @param client  Requester to be called when it is done.
 *   @param timeoutMS Timeout in milliseconds.
 *   @return The id of the timer.
 */
uint32 WinTileMapToolkit::requestTimer(TileMapTimerListener* client,
                                       uint32 timeoutMS,
                                       priority_t prio )
{
   if (!m_bCanRepaint)
      return 0;


   // Use prio.
   /* try to create a timer for the specified period */
   m_nCurrTimerId += 1000000;
   uint32 id = SetTimer(g_parentWin, m_nCurrTimerId, timeoutMS, TimerHandler);
   int nTickCount = GetTickCount();

   if(id == 0) {
      /* could not allocate a timer */
      return(INVALID_TIMER);
   }
   
   /* add the timer to the global timer list */
   gTimerList.insert( std::make_pair(id,client) );

   /* return the id of the timer */
   return(id);
}

/**
 *   Requests that the client should not be called.
 */
bool WinTileMapToolkit::cancelTimer(TileMapTimerListener* client,
                                    uint32 timerID)
{
   /* remove it from the queue */
   timerList_t::iterator it = gTimerList.find(timerID);
   /* check if timer is in list */
   if(it == gTimerList.end())
      return(false);

   /* kill the specified timer */
   gTimerList.erase(it);
   
   if(KillTimer(g_parentWin, timerID)) return(true);
   return(false);
}

TileMapPeriodicTimer*
WinTileMapToolkit::createPeriodicTimer( TileMapTimerListener* listener,
                                        TileMapToolkit::priority_t prio )
{
   return new WinTileMapPeriodicTimer(g_parentWin, listener);
}

/**
 *   Requests to be notified once when the system is idle.
 *   0 should not be returned.
 *   @param client The client to be called.
 *   @return An id which will be used when calling the client
 */
uint32 WinTileMapToolkit::requestIdle(TileMapIdleListener* client)
{   
   uint32 id = gCurrentIdleTimerId++;

   /* check if the timer ID has wrapped around */
   if(gCurrentIdleTimerId < INITIAL_IDLE_TIMER_ID) {
      gCurrentIdleTimerId = INITIAL_IDLE_TIMER_ID;
   }

   gIdleList.push_front( std::make_pair(id,client) );

   if(!gIdleTimerScheduled) {
      uint32 res = SetTimer(g_parentWin, id, 100, IdleTimerHandler);
      
      gIdleTimerScheduled = true;

      if(res == 0) {
         coreprintln_error("Failed to call SetTimer.");
      }
   }
   
   /* return the id of the timer */
   return(id);
}

/**
 *   Request not to be called when the system is idle for the 
 *   supplied client and id. 
 */
bool WinTileMapToolkit::cancelIdle(TileMapIdleListener* client, uint32 id )
{
   /* find the timers id */
   idleList_t::iterator it = std::find(gIdleList.begin(),
                                  gIdleList.end(),
                                  std::make_pair(id, client));
   /* check if timer is in list */
   if(it == gIdleList.end()) return(false);
   /* remove the timer from the list */
   gIdleList.erase(it);
   /* success */
   return(true);
}

/**
 *   Signals the platform that the map needs to be redrawn.
 *   The platform should call client->repaint as soon as possible.
 *   @param client The client to call when repaint is possible.
 */
uint32 WinTileMapToolkit::requestRepaint(TileMapHandler* client)
{
   /* call listener */
   if ( NULL != m_pWinPaintCallback  )
      m_pWinPaintCallback->afterBlit(NULL);

   /* initiate an immediate repaint */
   if (m_bCanRepaint)
      client->repaint(0);

   return(0);
}


/**
 *   Returns the available memory and the biggest block.
 */
uint32 WinTileMapToolkit::availableMemory(uint32& biggestBlock) const
{
   /* get the memory status of the machine */
   MEMORYSTATUS memStatus;
   /* we need to set the size of the structure ourselves...Windows is 
      finicky about this. */
   memStatus.dwLength = sizeof(MEMORYSTATUS);
   GlobalMemoryStatus(&memStatus);
   /* get the biggest block */
   biggestBlock = memStatus.dwAvailPhys;
   /* return the amount of physical memory available */
   return(memStatus.dwAvailPhys + memStatus.dwAvailVirtual);
}

/**
 *   Returns an id string containing info about the client
 *   which will be used when requesting the TileMapFormatDesc
 *   from the server. Max 20 chars.
 */
const char* WinTileMapToolkit::getIDString() const
{
   return( "WinClientConn/0.01" );
}

void WinTileMapToolkit::setWinPaintCallback( WinPaintCallback* pWinPaintCallback )
{
   m_pWinPaintCallback = pWinPaintCallback;
}

void WinTileMapToolkit::setCanRepaint(BOOL bCanRepaint){
   m_bCanRepaint = bCanRepaint;
}


