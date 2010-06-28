/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _S60READYTORUNLISTENER_H_
#define _S60READYTORUNLISTENER_H_

#include <e32base.h>
#include "ReadyToRunListener.h"
#include "EventGenerator.h"

namespace isab {
class Mutex;
}

namespace WFAPI {

class S60ReadyToRunListener : public CActive,
                              public ReadyToRunListener {
public:
   S60ReadyToRunListener();

   void ConstructL();

   virtual ~S60ReadyToRunListener();
   
   virtual void ready();
   
   virtual void RunL();

   void DoCancel();

   void Terminate();

   enum RTREvent {
      doCallbackEvent
   };

   /**
    * Callback function from Eventgenerator when we have called
    * EventGenerator::SendEventL. This function is called with
    * the same event as parameter as we did send to SendEventL.
    *
    * @param event The event that was generated.
    */
   void HandleGeneratedEventL(RTREvent event);
   
private:

   /**
    * Internal function that generates an event, uses the
    * EventGenerator, calls EventGenerator::SendEventL.
    *
    * @param event The event to generate.
    */
   void generateEvent(RTREvent event);
   
   S60ReadyToRunListener& operator=(const S60ReadyToRunListener& rhs);
   S60ReadyToRunListener(const S60ReadyToRunListener& rhs);
   
   TThreadId m_guiSideThread;

   /// The protecting mutex
   isab::Mutex* m_mutex;

   bool m_terminated;

   /**
    * Eventgenerator that helps us to prevent deadlocks. When calling
    * doCallbacks directly from RunL we have called sync and if one callback
    * that gets called by doCallbacks makes a sync request to Nav2 there
    * is a big risk for deadlock.
    */
   typedef CEventGenerator
   <S60ReadyToRunListener, enum RTREvent> RTREventGenerator;

   /// The eventgenerator that generates events.
   RTREventGenerator* m_eventGen;

};

} //End of namespace WFAPI

#endif /* _S60READYTORUNLISTENER_H_ */
