/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_ASYNCHRONOUS_STATUS_H
#define WFAPI_ASYNCHRONOUS_STATUS_H

#include "WFAPIConfig.h"
#include "StatusCode.h"
#include "RequestID.h"
#include "WFString.h"
#include "SynchronousStatus.h"

namespace WFAPI {

/**
 * An error reply for a prevoiusly sent asynchronous request.
 */
class AsynchronousStatus : public SynchronousStatus
{

public:

   /**
    * Constructor with values of members.
    *
    * @see BillingInterface
    *
    * @param requestID The RequestID for the request function call.
    * @param statusCode The StatusCode for request.
    * @param statusMessage The string message for the status.
    * @param statusURL An URL to open for the status.
    * @param statusData Status data for the status. May be empty.
    *                   Used to indicate an appropriate client side 
    *                   action in order to handle an error.
    *                   For instance it can be used to indicate which
    *                   product id that could be purchased for expired
    *                   rights. See BillingInterface for further 
    *                   documentation.
    */
   AsynchronousStatus(RequestID requestID,
                      wf_uint32 statusCode,
                      const WFString& statusMessage,
                      const WFString& statusURL,
                      const WFString& statusData = WFString(""));

   /**
    * Destructor.
    */
   virtual ~AsynchronousStatus();

   /**
    * Get the RequestID that this is a status for.
    * 
    * @return The RequestID from the request function call.
    */
   RequestID getRequestID() const;

   /**
    * Get the status as a SynchronousStatus.
    */
   SynchronousStatus operator()() const;

private:

   /**
    * The RequestID that this is a status for.
    */
   RequestID m_requestID;
};

} // End namespace WFAPI

#endif // End WFAPI_ASYNCHRONOUS_STATUS_H



