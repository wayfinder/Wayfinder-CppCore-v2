/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "IPhoneNav2API.h"
#include "JTC/JTCInit.h"
#include "IPReadyToRunListener.h"
#include "HardwareIDArray.h"
#include "ProgramVersion.h"
#include "WFString.h"
#include "Nav2APIImpl.h"
#include "Nav2APISharedImpl.h"
#include "JTC/JTCInit.h"
#include "IPhoneConnectionManager.h"

#include <iostream>

#import <UIKit/UIKit.h>

namespace WFAPI {

IPhoneNav2API::IPhoneNav2API()
   : Nav2API()
{
   JTCInit::initJTC();

   m_sharedImpl->m_programVersion = ProgramVersion(0, 9, 54);

   // Use below to enble billing with App store.
   //m_sharedImpl->m_clientType = "wf9-iphone-apps";

   m_sharedImpl->m_clientType = "wf-linux-demo";
}

AsynchronousStatus IPhoneNav2API::start(Nav2StatusListener* statusListener, 
                                        StartupData* startupData) 
{
   ReadyToRunListener* listener = new IPReadyToRunListener;
   listener->setReceiver(getImpl());

   ReadyToRunListener* notifyListener = new IPReadyToRunListener;
   Nav2StatusNotifier* notifier = new Nav2StatusNotifier(statusListener,
                                                         notifyListener);
   notifyListener->setReceiver(notifier);
   
   
   WFString httpUserAgent; // Empty not used
   // WFString httpRequestString; // Empty not used
   WFString httpFixedHost; // Empty not used
   // Allocate tracking object, not turn it on
   bool useTracking = true; 
   // No service window at startup, and no WFID
   bool wayfinderIDStartUp = false; 

   HardwareIDArray hwids;

   WFString imeiStr("DummyIMEIDoNotUse");

   // Use this imei instead to avoid trouble with no user account.
   // WFString imeiStr("LinuxNav2APITestIMEI001")
      
   hwids.push_back(HardwareID(imeiStr, "imei"));

   // Read the unique identifier for the device, also known as UDID.
   NSString* udid = [[UIDevice currentDevice] uniqueIdentifier];
   WFString cstrUdid = [udid UTF8String];

   // TODO: Enable when the server is updated with this new licence key.
   hwids.push_back(HardwareID(cstrUdid, "iphone_dev_id"));

   if (startupData->getConnectionManager() == NULL){
      startupData->setConnectionManager(new IPhoneConnectionManager);      
   }

   
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

   return AsynchronousStatus(
      RequestID(RequestID::INVALID_REQUEST_ID),
      OK,
      "",
      "");
}

SynchronousStatus IPhoneNav2API::makeHardwareIDs(HardwareIDArray& hwids)
{
   return SynchronousStatus(0, "", "");
}


IPhoneNav2API::~IPhoneNav2API()
{

}

} // End of namespace WFAPI

