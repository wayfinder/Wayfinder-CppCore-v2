/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "S60Nav2APIImpl.h"
#include "HardwareIDArray.h"
#include "ProgramVersion.h"
#include "WFString.h"
#include "Nav2APIImpl.h"
#include "WFTextUtil.h"
#include "StartupData.h"
#include "S60ReadyToRunListener.h"

namespace WFAPI {

S60Nav2APIImpl::S60Nav2APIImpl(Nav2APIImpl* nav2APIImpl)
   : m_nav2APIImpl(nav2APIImpl),
     m_statusListener(NULL),
     m_startupData(NULL),
     imeiFetcher(NULL),
     imsiFetcher(NULL),
     m_httpProto(true),
     m_httpRequestString(),
     m_programVersion(0, 9, 3)
{
}

S60Nav2APIImpl::~S60Nav2APIImpl()
{
   delete imeiFetcher;
   delete imsiFetcher;
   delete m_startupData;
}

void
S60Nav2APIImpl::fetchImei() 
{
   TRAPD(err, imeiFetcher = CImeiFetcher<S60Nav2APIImpl>::NewL(*this, ETrue));
   if (err != KErrNone) {
      // NewL leaved we should try to continue with the startup. But maybe
      // we should add a notification to the UI that something is wrong.
      fetchImsi();
   }
}

void
S60Nav2APIImpl::fetchImsi()
{
   TRAPD(err, imsiFetcher = CImsiFetcher<S60Nav2APIImpl>::NewL(*this, ETrue));
   if (err != KErrNone) {
      // NewL leaved we should try to continue with the startup. But maybe
      // we should add a notification to the UI that something is wrong.
      secondStageStartup();
   }
}

// Callback function
void
S60Nav2APIImpl::ImeiNumberSet(const TDesC& imeiNumber)
{
   char* ret = WFTextUtil::newTDesDupL(imeiNumber);
   imeiStr = ret;
   delete ret;
   ret = NULL;
   fetchImsi();
}

void
S60Nav2APIImpl::ErrorReceivingImei()
{
   imeiStr = "";
   fetchImsi();
}

// Callback function
void
S60Nav2APIImpl::ImsiNumberSet(const TDesC& imsiNumber)
{
   char* ret = WFTextUtil::newTDesDupL(imsiNumber);
   imsiStr = ret;
   delete ret;
   ret = NULL;
   secondStageStartup();
}

void
S60Nav2APIImpl::ErrorReceivingImsi()
{
   imsiStr = "";
   secondStageStartup();
}
// Second phase start.
void
S60Nav2APIImpl::secondStageStartup()
{
   // One could make this more dynamic instead of hardcoding to Vorbis
   WFString audioTypeDirName("Vorbis");
   WFString httpUserAgent; // Empty not used
   WFString httpFixedHost; // Empty not used
   // Allocate tracking object, not turn it on
   bool useTracking = true; 
   // No service window at startup, and no WFID
   bool wayfinderIDStartUp = false; 
   
   HardwareIDArray hwids;

   hwids.push_back(HardwareID(imeiStr, "imei"));
   hwids.push_back(HardwareID(imsiStr, "imsi"));

   S60ReadyToRunListener * readyToRunListener = new S60ReadyToRunListener();
   readyToRunListener->ConstructL();
   readyToRunListener->setReceiver(m_nav2APIImpl);

   S60ReadyToRunListener* statusRunListener = new S60ReadyToRunListener();
   statusRunListener->ConstructL();

   Nav2StatusNotifier* notifier = new Nav2StatusNotifier(m_statusListener,
                                                         statusRunListener);
   
   statusRunListener->setReceiver(notifier);
   
   m_nav2APIImpl->start(m_startupData,
                        audioTypeDirName,
                        m_programVersion,
                        m_clientType, 
                        m_clientTypeOptions,
                        m_httpProto,
                        httpUserAgent,
                        m_httpRequestString,
                        httpFixedHost,
                        useTracking,
                        wayfinderIDStartUp,
                        hwids,
                        readyToRunListener,
                        notifier);
}

AsynchronousStatus
S60Nav2APIImpl::start(Nav2StatusListener* statusListener, 
                      StartupData* startupData,
                      const WFString& clientType,
                      const WFString& clientTypeOptions,
                      bool httpProto,
                      const WFString& httpRequestString,
                      const ProgramVersion& programVersion)
{
   m_statusListener = statusListener;
   ///Tidieus copy of the startupData so it won't go out of scope.
   m_startupData = new StartupData(*startupData);
   m_clientType = clientType;
   m_clientTypeOptions = clientTypeOptions;
   m_httpProto = httpProto;
   m_httpRequestString = httpRequestString;
   m_programVersion = programVersion;

   fetchImei();
   return AsynchronousStatus(RequestID(RequestID::INVALID_REQUEST_ID),
                             OK,
                             "",
                             "");
}

} // End namespace WFAPI
