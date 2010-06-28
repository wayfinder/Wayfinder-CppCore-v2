/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_S60_NAV2_API_IMPL_H
#define WFAPI_S60_NAV2_API_IMPL_H

#include "WFAPIConfig.h"
#include "Nav2API.h"
#include "Nav2APIImpl.h"
#include "HardwareIDArray.h"
#include "ImeiFetcher.h"
#include "ImsiFetcher.h"
#include <e32base.h>
#include "ProgramVersion.h"

namespace WFAPI {

class S60ReadyToRunListener;
class Nav2StatusListener;

/**
 * Implementation class for Symbian S60 platform.
 * Used for hiding implementation from S60Nav2API.
 */
class S60Nav2APIImpl
{
   
public:
   
   /**
    * Construct a new S60Nav2API.
    *
    * @param nav2APIImpl Pointer to the navAPIImpl object.
    */
   S60Nav2APIImpl(Nav2APIImpl* nav2APIImpl);
   
   /**
    * Destructor.
    */
   virtual ~S60Nav2APIImpl();
   
   /**
    * @see Nav2API.
    *
    * @param statusListener The listener for status events.
    * @param startupData The startup data containing paths etc.
    * @param clientType Should be set to the type of client using the API's.'
    * @param clientType Should be set to the client type options. Normally empty.
    * @param httpProto Should be set to true if Nav2 should use http,
    *                  false if using sockets.
    * @param httpRequestString The http server used when sending requests
    *                          to the server.
    * @param programVersion The version of the application.
    */
   AsynchronousStatus start(Nav2StatusListener* statusListener, 
                                    StartupData* startupData,
                                    const WFString& clientType,
                                    const WFString& clientTypeOptions,
                                    bool httpProto,
                                    const WFString& httpRequestString,
                                    const ProgramVersion& programVersion);
   
   /**
    * Callback function used by the ImeiFetcher when the IMEI is fetched.
    *
    * @param imeiNumber The fetched IMEI number. 
    */
   void ImeiNumberSet(const TDesC& imeiNumber);

   /**
    * Called by ImeiFetcher when failed to get the imei.
    */
   void ErrorReceivingImei();

   /**
    * Callback function used by the ImsiFethcer when the IMSI is fetched.
    *
    * @param imsiNumber The fetched IMSI number. 
    */
   void ImsiNumberSet(const TDesC& imsiNumber);

   /**
    * Called by ImsiFetcher when failed to get the imsi.
    */
   void ErrorReceivingImsi();

protected:

private:

   /**
    * Handles the second phase of the startup, after the IMEI and IMSI are fetched.
    */
   void secondStageStartup();
   
   /**
    * To fetch the IMEI.
    */
   void fetchImei();
   
   /**
    * To fetch the IMSI.
    */
   void fetchImsi();

   /// Holds the pointer to the nav2APIImpl object.
   Nav2APIImpl* m_nav2APIImpl;

   /// Holds the statuslistener sent in as parameter in start function.
   Nav2StatusListener* m_statusListener;
   
   /// Holds startup data sent in as parameter in start function.
   StartupData* m_startupData;

   /// Holds the client type.
   WFString m_clientType;   
   
   /// Holds the client type options.
   WFString m_clientTypeOptions;   
   
   /// Holds the IMEI string
   WFString imeiStr;
   
   /// Holds the IMSI string
   WFString imsiStr;

   /// Holds object that fethes the IMEI.
   CImeiFetcher<S60Nav2APIImpl>* imeiFetcher;

   /// Holds object that fethes the IMSI.
   CImsiFetcher<S60Nav2APIImpl>* imsiFetcher;

   /// True if nav2 should use http for connection instead of sockets.
   bool m_httpProto;

   /**
    * Contains the http server, by default this is set to ordinary
    * navigator server but for some applications this might be different.
    */
   WFString m_httpRequestString;

   /// The version of the application.
   ProgramVersion m_programVersion;
};
   
} // End namespace WFAPI

#endif // End WFAPI_S60_NAV2_API_IMPL_H
