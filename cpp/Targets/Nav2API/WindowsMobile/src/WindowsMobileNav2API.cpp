/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "WindowsMobileNav2API.h"
#include "WindowsMobileNav2APIImpl.h"
#include "HardwareIDArray.h"
#include "ProgramVersion.h"
#include "WFString.h"
#include "Nav2APIImpl.h"
#include "WMReadyToRunListener.h"
#include "Nav2APISharedImpl.h"

#define CORE_LOGGING_MODULE_NAME "WindowsMobileNav2API"
#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE

#include "CoreDebugPrint.h"

namespace WFAPI {

struct WindowsMobileNav2API::Impl {
   WindowsMobileNav2APIImpl* m_impl;
};

WindowsMobileNav2API::WindowsMobileNav2API()
   : Nav2API()
{
   m_impl = new Impl;
   m_impl->m_impl = NULL;
   // Client type
   m_sharedImpl->m_clientType = "wf-ppc-5-demo";
   m_sharedImpl->m_programVersion = ProgramVersion(0, 9, 55);
}

WindowsMobileNav2API::~WindowsMobileNav2API()
{
   delete m_impl->m_impl;
   delete m_impl;
}

AsynchronousStatus WindowsMobileNav2API::start(Nav2StatusListener* statusListener, 
                                               StartupData* startupData)
{
   if(!m_impl->m_impl) {
      m_impl->m_impl = new WindowsMobileNav2APIImpl(getImpl());
   }

   WMReadyToRunListener* listener = new WMReadyToRunListener;
   listener->init(0);
   listener->setReceiver(getImpl());

   WMReadyToRunListener* notifyListener = new WMReadyToRunListener;
   notifyListener->init(0);
   Nav2StatusNotifier* notifier = new Nav2StatusNotifier(statusListener,
                                                         notifyListener);
   notifyListener->setReceiver(notifier);
   
   WFString httpUserAgent; // Empty not used
   WFString httpFixedHost; // Empty not used
   // Allocate tracking object, not turn it on
   bool useTracking = true; 
   // No service window at startup, and no WFID
   bool wayfinderIDStartUp = false; 

   HardwareIDArray hwids;

   WFString imei, imsi;
   SynchronousStatus status = m_impl->m_impl->getUserIdentificationData(imei, imsi);
   if(status.getStatusCode() == OK) {

      coreprintln("IMEI: %s", imei);
      coreprintln("IMSI: %s", imsi);

      hwids.push_back(HardwareID(imei, "imei"));

      hwids.push_back(HardwareID(imsi, "imsi"));

      getImpl()->start(startupData,
                       m_sharedImpl->m_audioTypeDirName,
                       m_sharedImpl->m_programVersion, 
                       m_sharedImpl->m_clientType, 
                       m_sharedImpl->m_clientTypeOptions,
                       m_sharedImpl->m_httpProto,
                       httpUserAgent,
                       m_sharedImpl->m_httpRequestString,
                       httpFixedHost,
                       useTracking,
                       wayfinderIDStartUp,
                       hwids,
                       listener,
                       notifier);

      
   }
   
   return AsynchronousStatus(
      RequestID(RequestID::INVALID_REQUEST_ID),
      status.getStatusCode(),
      status.getStatusMessage(),
      status.getStatusURL());    
}

} // End of namespace WFAPI
