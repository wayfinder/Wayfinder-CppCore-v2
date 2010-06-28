/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include <arch.h>
#include <windows.h>

namespace isab {

   /**
    *   Encapsulation of a Windows event.
    *   Used in Conditional which is used in Monitor.
    */
   class WinEvent {
   public:

      /**
       *   Creates a new WinEvent.
       */
      WinEvent();

      /**
       *   Deletes the associated event.
       */
      ~WinEvent();

      /**
       *   Posts the event.
       */
      void post();

      /**
       *   Waits for the event to be posted.
       */
      void wait();

      /**
       *   Resets the event to unposted state.
       */
      void reset();

      /**
       *   Returns true if the event has been posted.
       */
      bool posted();

      /// Handle to windows event object.
      HANDLE m_event; 
      
   };
}

isab::WinEvent::WinEvent()
{
   m_event = CreateEvent(0, 1, 0, 0);
}

isab::WinEvent::~WinEvent()
{
   CloseHandle(m_event);
}

void
isab::WinEvent::post()
{
   SetEvent(m_event);
}

void
isab::WinEvent::wait()
{
   WaitForSingleObject(m_event, INFINITE);
}

void
isab::WinEvent::reset()
{
   ResetEvent(m_event);
}

bool
isab::WinEvent::posted()
{
   long rc = WaitForSingleObject(m_event, 0);
   return rc != WAIT_TIMEOUT;
}


