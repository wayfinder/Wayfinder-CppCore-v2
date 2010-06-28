/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#define CORE_LOGGING_MODULE_NAME "Nav2StatusNotifier"
//#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE
#include "CoreDebugPrint.h"

#include "CoreDebugPrint.h"
#include "Nav2StatusNotifier.h"
#include "WFAPISync.h"
#include "AsynchronousStatus.h"
#include "RequestID.h"
#include <algorithm>

#ifdef __linux__
#include <GtkReadyToRunListener.h>
#endif

using namespace WFAPI;

Nav2StatusNotifier::Nav2StatusNotifier(Nav2StatusListener* listener,
                                       WFAPI::ReadyToRunListener* trigger)
   : ReadyToRunReceiver()
{
   m_listeners.push_back(listener);
   m_trigger = trigger;
   m_mutex = new isab::Mutex;
}

Nav2StatusNotifier::~Nav2StatusNotifier()
{
   delete m_mutex;
   delete m_trigger;
}

void Nav2StatusNotifier::addMessage(Nav2StatusType type, StatusCode code)
{
   switch(type) {
   case STARTUP_COMPLETE:
      coreprintln("addMessage: STARTUP_COMPLETE");
      break;
   case MAPLIB_STARTUP_COMPLETE:
      coreprintln("addMessage: MAPLIB_STARTUP_COMPLETE");
      break;
   case STOP_COMPLETE:
      coreprintln("addMessage: STOP_COMPLETE");
      break;
   }
   
   WFAPISync sync(m_mutex);
      
   Nav2Notice n = { type, code };
   m_notices.push_back(n);
   m_trigger->ready();
}

void Nav2StatusNotifier::addError(const WFAPI::AsynchronousStatus& status)
{
   WFAPISync sync(m_mutex);

   m_errors.push_back(status);
   m_trigger->ready();
}

void Nav2StatusNotifier::doCallbacks()
{   
   std::vector<Nav2Notice> notices;
   std::vector<AsynchronousStatus> errors;
   
   {
      WFAPISync sync(m_mutex);
      notices = m_notices;
      m_notices.clear();
   }
   
   for(std::vector<Nav2Notice>::iterator itr = notices.begin();
       itr != notices.end(); itr++)
   {
      Nav2Notice& cur = *itr;

      if (m_listeners.empty()) {
         break;
      }
      
      switch(cur.type) {
         case STARTUP_COMPLETE:
            if(m_listeners.empty()) {
               coreprintln("Got STARTUP_COMPLETE, but no listeners!");
            }
            
            for (ListenerCont::iterator it = m_listeners.begin();
                 it != m_listeners.end(); ++it ) {
               coreprintln("Issuing STARTUP_COMPLETE");
               (*it)->startupComplete();
            }
            break;
         case MAPLIB_STARTUP_COMPLETE:
            if(m_listeners.empty()) {
               coreprintln("Got MAPLIB_STARTUP_COMPLETE, but no listeners!");
            }
            
            for (ListenerCont::iterator it = m_listeners.begin();
                 it != m_listeners.end(); ++it ) {
               coreprintln("Issuing MAPLIB_STARTUP_COMPLETE");
               (*it)->mapLibStartupComplete();
            }
            break;
         case STOP_COMPLETE:
            if(m_listeners.empty()) {
               coreprintln("Got STOP_COMPLETE, but no listeners!");
            }
            
            for (ListenerCont::iterator it = m_listeners.begin();
                 it != m_listeners.end(); ++it ) {
               coreprintln("Issuing STOP_COMPLETE");
               (*it)->stopComplete();
            }

            //Perhaps move this to a better place..
#ifdef __linux__
            GtkReadyToRunListener::disableRuntime();
#endif
            break;
      }
   }
   
   {
      WFAPISync sync(m_mutex);
      errors = m_errors;
      m_errors.clear();
   }
   
   for(std::vector<AsynchronousStatus>::iterator itr = errors.begin();
       itr != errors.end(); itr++)
   {
      if (m_listeners.empty()) {
         coreprintln("Error could be not be issued as there is no listener");
         break;
      }
      
      const AsynchronousStatus status = *itr;
      for (ListenerCont::iterator it = m_listeners.begin();
           it != m_listeners.end(); ++it ) {
         coreprintln("Issuing error: %s", status.getStatusMessage().c_str());
         (*it)->error(status);
      }
   }
}

void
Nav2StatusNotifier::addListener(WFAPI::Nav2StatusListener* listener)
{
   removeListener(listener);
   
   if (listener != NULL) {
      m_listeners.push_back(listener);
   }
}

void
Nav2StatusNotifier::prependListener(WFAPI::Nav2StatusListener* listener)
{
   removeListener(listener);
   
   if (listener != NULL) {
      m_listeners.insert(m_listeners.begin(), listener);
   }
}

void Nav2StatusNotifier::removeListener(WFAPI::Nav2StatusListener* listener)
{
   std::vector<WFAPI::Nav2StatusListener*>::iterator itr =
      std::find(m_listeners.begin(), m_listeners.end(), listener);

   if(itr != m_listeners.end()) {
      m_listeners.erase(itr);
   }
}
