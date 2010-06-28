/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _WINDOWSMOBILENAV2APIIMPL_H_
#define _WINDOWSMOBILENAV2APIIMPL_H_

#include "WFAPIConfig.h"
#include "Nav2API.h"
#include "Nav2APIImpl.h"
#include "HardwareIDArray.h"
#include "ProgramVersion.h"

#define TAPI_API_HIGH_VERSION 0x00020002
#define TAPI_API_LOW_VERSION 0x00020000
#define EXT_API_HIGH_VERSION 0x00010000
#define EXT_API_LOW_VERSION 0x00010000


namespace WFAPI {

/**
 * Implementation class for Windows Mobile platform.
 * Used for hiding implementation from S60Nav2API.
 */
class WindowsMobileNav2APIImpl {
public:
   /**
    * Construct a new S60Nav2API.
    *
    * @param nav2APIImpl Pointer to the navAPIImpl object.
    */
   WindowsMobileNav2APIImpl(Nav2APIImpl* nav2APIImpl);

   /**
    * Destructor.
    */
   virtual ~WindowsMobileNav2APIImpl();

   /**
    * @see Nav2API.
    *
    * @param statusListener The listener for status events.
    * @param startupData The startup data containing paths etc.
    * @param clientType Should be set to the type of client using the API's.'
    * @param httpProto Should be set to true if Nav2 should use http,
    *                  false if using sockets.
    * @param httpRequestString The http server used when sending requests
    *                          to the server.
    * @param programVersion The version of the application.
    */ 
   AsynchronousStatus start(Nav2StatusListener* statusListener, 
                            StartupData* startupData,
                            const WFString& clientType,
                            bool httpProto,
                            const WFString& httpRequestString,
                            const ProgramVersion& programVersion);

   /**
    * Get the user identification data for this phone.
    * By that means IMEI and IMSI.
    *
    * @param imeiStr Where the IMEI will be stored.
    * @param imsiStr Where the IMSI will be stored.
    *
    * @return The status, if it suceeded or not.
    */
   SynchronousStatus getUserIdentificationData(WFString& imeiStr, WFString& imsiStr);

};

} // End of namespace WFAPI

#endif /* _WINDOWSMOBILENAV2APIIMPL_H_ */
