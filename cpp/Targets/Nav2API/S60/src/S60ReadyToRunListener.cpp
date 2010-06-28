/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "S60ReadyToRunListener.h"
#include "Mutex.h"
#include "WFAPISync.h"

namespace WFAPI {

S60ReadyToRunListener::S60ReadyToRunListener()
   : CActive(CActive::EPriorityStandard),
     ReadyToRunListener()
{
   m_mutex = new isab::Mutex();
   RThread thisThread;
   m_guiSideThread = thisThread.Id();
   thisThread.Close();
   m_eventGen = RTREventGenerator::NewL(*this);
}

void S60ReadyToRunListener::ConstructL()
{
   CActiveScheduler::Add(this);
}

WFAPI::S60ReadyToRunListener::~S60ReadyToRunListener()
{
   delete m_mutex;
   delete m_eventGen;
}

void S60ReadyToRunListener::ready()
{
   WFAPISync sync(m_mutex);
   
   if(IsActive()) {
      return;
   }
   
   iStatus = KRequestPending;
   SetActive();
   
   //QT
   //Ta mutex
   RThread otherThread;
   otherThread.Open(m_guiSideThread);
   TRequestStatus* test = &iStatus;
   otherThread.RequestComplete(test, KErrNone);
   otherThread.Close(); 
}

void WFAPI::S60ReadyToRunListener::Terminate()
{
   WFAPISync sync(m_mutex);
   m_terminated = true;
}

void S60ReadyToRunListener::RunL()
{
   WFAPISync sync(m_mutex);
   
   if(m_terminated) {
      return;
   }
   
   // Will be run whenever the ReadyToRunListener is ready and has
   // issued a User::RequestComplete
   // Generate doCallbackEvent, this will prevent a deadlock when doing
   // a sync request to Nav2 from the callback functions due to the
   // lock at the top of this function.
   generateEvent(doCallbackEvent);
}

void S60ReadyToRunListener::DoCancel()
{
   //Should not happen
}

void WFAPI::S60ReadyToRunListener::HandleGeneratedEventL(RTREvent event)
{
   switch (event) {
      case doCallbackEvent:
         doCallbacks();
         break;
   }
}

void WFAPI::S60ReadyToRunListener::generateEvent(RTREvent event)
{
   m_eventGen->SendEventL(event);
}

} // End of namespace WFAPI
