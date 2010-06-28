/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _NAV2STATUSNOTIFIER_H_
#define _NAV2STATUSNOTIFIER_H_

#include <vector>
#include "Nav2StatusListener.h"
#include "ReadyToRunListener.h"
#include "Mutex.h"

namespace WFAPI {
class AsynchronousStatus;
}

class Nav2StatusNotifier : public WFAPI::ReadyToRunReceiver {
public:
   enum Nav2StatusType { STARTUP_COMPLETE,
                         MAPLIB_STARTUP_COMPLETE,
                         STOP_COMPLETE };

   Nav2StatusNotifier(WFAPI::Nav2StatusListener* listener,
                      WFAPI::ReadyToRunListener* trigger);

   virtual ~Nav2StatusNotifier();
   
   void addMessage(Nav2StatusType type, WFAPI::StatusCode code);
   
   void addError(const WFAPI::AsynchronousStatus& status);
   
   virtual void doCallbacks();

   /**
    * Appends a listener to the m_listeners vector.
    *
    * @param listener The listener to append.
    */
   void addListener(WFAPI::Nav2StatusListener* listener);

   /**
    * Removes a listener from the m_listeners vector.
    * 
    * @param listener The listener to append.
    */ 
   void removeListener(WFAPI::Nav2StatusListener* listener);
   
   /**
    * Inserts a listener first in the m_listeners vector.
    *
    * @param listener The listener to insert.
    */
   void prependListener(WFAPI::Nav2StatusListener* listener);
   
private:

   struct Nav2Notice {
      Nav2StatusType type;
      WFAPI::StatusCode code;
   };
   
   isab::Mutex* m_mutex;
   std::vector<Nav2Notice> m_notices;
   std::vector<WFAPI::AsynchronousStatus> m_errors;
   
   //WFAPI::Nav2StatusListener* m_listener;
   WFAPI::ReadyToRunListener* m_trigger;

   /// Typedef for a set of Nav2StatusListeners
   typedef std::vector<WFAPI::Nav2StatusListener*> ListenerCont;

   /**
    * Set of listeners.
    */
   ListenerCont m_listeners;

};

#endif /* _NAV2STATUSNOTIFIER_H_ */
