/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_LINUX_LOCATION_INTERFACE_IMPL_H
#define WFAPI_LINUX_LOCATION_INTERFACE_IMPL_H

#include "LocationInterfaceImpl.h"
#include "LocationInterface.h"

namespace WFAPI {

class LinuxLocationInterfaceImpl : public LocationInterfaceImpl {
public:
   
   /**
    * Constructor.
    *
    * @param api A pointer to the Nav2APIImpl.
    */
   LinuxLocationInterfaceImpl(Nav2APIImpl* api);

   /**
    * Destructor
    */
   virtual ~LinuxLocationInterfaceImpl();

   /**
    * Checks if the client is registered and the tracking is on.
    *
    * @return The status LBS_STATUS_STARTED if the lbs has been started,
    *         LBS_STATUS_NOT_STARTED if the lbs has not been started.
    */    
   virtual SynchronousStatus lbsStarted();
   
   /**
    * This asynchronous function starts up the LBS. When operation is done
    * the added LocationListeners will get a callback to startedLbs if
    * successfully started, otherwise error will get called with a error
    * code from StatusCode or LocationStatusCode
    *
    * @return The status and if status is OK.
    */
   virtual AsynchronousStatus startLbs();
   
   /**
    * Stop LBS updates.
    *
    * @return The status of the operation, OK if operation succeeded and
    *         either a status from StatusCode or LocationStatusCode.
    */
   virtual SynchronousStatus stopLbs();

protected:

private:

};
} // End namespace WFAPI

#endif // End WFAPI_LINUX_LOCATION_INTERFACE_IMPL_H
