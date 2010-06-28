/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef WINTILEMAPTOOLKIT_H
#define WINTILEMAPTOOLKIT_H

#include "TileMapToolkit.h"
#include "WinPaintCallback.h"
#include <list>
#include <map>

/* invalid timer */
#define INVALID_TIMER   0xFFFFFFFF

typedef std::map< uint32,TileMapTimerListener* > periodicTimerList_t;

/* the periodic timer list */
static periodicTimerList_t gPeriodicTimerList;

/* the periodic timer event handling procedure */
static VOID CALLBACK PeriodicTimerHandler(HWND hwnd,
                                          UINT uMsg,
                                          UINT idEvent,
                                          DWORD dwTime)
{
   /* redundant check to make sure that the event is correct */
   if(uMsg == WM_TIMER) {
      /* handle the timer */
      periodicTimerList_t::iterator it = gPeriodicTimerList.find(idEvent);
      /* check if timer is in list */
      if(it == gPeriodicTimerList.end()) return;

      /* call timer listener */
      TileMapTimerListener* listener = it->second;
      uint32 nIdEvent = it->first;
      listener->timerExpired(nIdEvent);
   }
   return;


}

class WinTileMapPeriodicTimer
   :  public TileMapPeriodicTimer,
      public TileMapTimerListener {
private:
   HWND  m_hwndTimerWindowHandler;

public:
   /**
    *   Creates a TileMapPeriodicTimer with the supplied listener.
    *   The listener must implement periodicCallback(uint32)
    */
   WinTileMapPeriodicTimer( HWND hwndTimerWindowHandler,
                            TileMapTimerListener* listener) :
         TileMapPeriodicTimer( listener, 0 ) {
      m_hwndTimerWindowHandler = hwndTimerWindowHandler;
      m_timerID = 0;
      m_running  = false;
      m_periodMS = INVALID_TIMER;
   }

   ~WinTileMapPeriodicTimer() {
      periodicTimerList_t::iterator it = gPeriodicTimerList.find(m_timerID);
      /* check if timer is in list */
      if(it != gPeriodicTimerList.end()) 
         gPeriodicTimerList.erase(it);
      KillTimer(m_hwndTimerWindowHandler, m_timerID);
   }

   void start( uint32 period_millis ) {
      if(!m_running) {
         m_running = TRUE;

         // Timer already works - no need to recreate it.
         if(m_timerID  && period_millis == m_periodMS)
            return;
         
         m_periodMS = period_millis;

         // Recreate the timer with specific period if differs from previous value.
         if(m_timerID) {
            periodicTimerList_t::iterator it = gPeriodicTimerList.find(m_timerID);
            /* check if timer is in list */
            if(it != gPeriodicTimerList.end()) 
               gPeriodicTimerList.erase(it);
            KillTimer(m_hwndTimerWindowHandler, m_timerID);
         }
         // Set periodic timer.
         m_timerID = SetTimer(m_hwndTimerWindowHandler, GetTickCount(), m_periodMS, PeriodicTimerHandler);
		 gPeriodicTimerList.insert( std::make_pair(m_timerID, m_listener) );
      }
   }

   bool active() {
      return m_running != 0;
   }
   
   void stop() {
      if ( m_running ) {
         m_running = FALSE;
      }
   }

   void timerExpired( uint32 id ) {
      if ( m_running ) {
         m_listener->timerExpired( m_timerID );
      }
   }

   uint32 m_periodMS;
   int m_running;
};

/* Win32 specific TileMapToolkit */
class WinTileMapToolkit : public TileMapToolkit
{
public:
   static LRESULT CALLBACK
   CallbackWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
   
   /* Types of queues */
   /* the timer list */
   typedef std::map< uint32,TileMapTimerListener* > timerList_t;

   /* the idle list */
   typedef std::pair< uint32,TileMapIdleListener* > idleEntry_t;
   typedef std::list< idleEntry_t > idleList_t;

   /* current timer identifier */
   uint32 m_nCurrTimerId;

   /* constructor */
   WinTileMapToolkit(HWND parentWin,HINSTANCE appInst);

   /* destructor */
   ~WinTileMapToolkit();

   
   /// Creates a new periodic timer. 
   TileMapPeriodicTimer*
   createPeriodicTimer(TileMapTimerListener* listener,
                       priority_t prio );

   /**
    *   Requests a timer from the system. When the timer
    *   expires client->timerExpired(id) should be called.
    *   @param client  Requester to be called when it is done.
    *   @param timeoutMS Timeout in milliseconds.
    *   @return The id of the timer.
    */
   uint32 requestTimer(TileMapTimerListener* client,
                       uint32 timeoutMS,
                       priority_t prio );

   /**
    *   Requests that the client should not be called.
    */
   bool cancelTimer(TileMapTimerListener* client,
                    uint32 timerID);

   /**
    *   Requests to be notified once when the system is idle.
    *   0 should not be returned.
    *   @param client The client to be called.
    *   @return An id which will be used when calling the client
    */
   uint32 requestIdle(TileMapIdleListener* client);

   /**
    *   Request not to be called when the system is idle for the 
    *   supplied client and id. 
    */
   bool cancelIdle(TileMapIdleListener* client, uint32 id );

   /**
    *   Signals the platform that the map needs to be redrawn.
    *   The platform should call client->repaint as soon as possible.
    *   @param client The client to call when repaint is possible.
    */
   uint32 requestRepaint(TileMapHandler* client);

   /**
    *   Returns the available memory and the biggest block.
    */
   uint32 availableMemory(uint32& biggestBlock) const;

   /**
    *   Returns an id string containing info about the client
    *   which will be used when requesting the TileMapFormatDesc
    *   from the server. Max 20 chars.
    */
   const char* getIDString() const;

   /**
    *    Setting listener for painting events
    *    @param pWinPaintCallback listener
    */
   void setWinPaintCallback( WinPaintCallback* pWinPaintCallback = NULL );

   void setCanRepaint(BOOL bCanRepaint);

   /**
    * Initialize callback triggers, i.e. calls to triggerCallbacks.
    */
   virtual void initCallbackTrigger() {}

   /**
    * Trigger a callback to the specified callback function.
    *
    * @param callback The callback function to call.
    *
    */
   virtual void triggerCallbacks(WFAPI::MapLibCallback* callback) { }
private:
   /* WinPaintCallback */
   WinPaintCallback* m_pWinPaintCallback;      

   BOOL m_bCanRepaint;
};

#endif
