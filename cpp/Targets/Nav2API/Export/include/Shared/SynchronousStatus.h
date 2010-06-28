/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_SYNCHRONOUS_STATUS_H
#define WFAPI_SYNCHRONOUS_STATUS_H

#include "WFAPIConfig.h"
#include "WFString.h"

namespace WFAPI {

/**
 * A return status for a synchronous function.
 *
 * @see BillingInterface for documentation on how to perform 
 *      in client side billing in case the status is an error 
 *      due to lacking rights on the server.
 */
class SynchronousStatus
{

public:

   /**
    * Constructor with values of members.
    *
    * @see BillingInterface
    *
    * @param statusCode The StatusCode for the request function call.
    * @param statusMessage The string message for the status.
    * @param statusURL An URL to open for the status.
    * @param statusData Status data for the status. May be empty.
    *                   Used to indicate an appropriate client side 
    *                   action in order to handle an error.
    *                   For instance it can be used to indicate which
    *                   product id that could be purchased for expired
    *                   rights. See BillingInterface for further 
    *                   documentation about this.
    */
   SynchronousStatus(wf_uint32 statusCode,
                     const WFString& statusMessage,
                     const WFString& statusURL, 
                     const WFString& statusData = WFString(""));

   /**
    * Destructor.
    */
   virtual ~SynchronousStatus();
   
   /**
    * Get the StatusCode. It can either be one of the error codes
    * in StatusCode or one of the error codes that the specific
    * type of request sent has defined.
    *
    * @return The StatusCode of the request.
    */
   wf_uint32 getStatusCode() const;

   /**
    * Get the status message of this error.
    *
    * @return The string message for this status.
    */
   const WFString& getStatusMessage() const;

   /**
    * Get the status URL of this error, is empty if there is no URL the
    * user can go to to resolve this error, like purchase access to more
    * regions. If this URL is not empty the status message can be used
    * to query the end user if the URL should be opened or not. 
    * This URL can be opened in an browser.
    *
    * @return The string message for this status.
    */
   const WFString& getStatusURL() const;

   /**
    * Get the status data for the status. May be empty.
    * Used to indicate an appropriate client side action in order to handle 
    * an error. 
    *
    * The status data string is formatted as 
    * "<status data type>;<status data value>".
    *
    * The status data can for instance be used to indicate which product 
    * id that could be purchased for expired rights. 
    *
    * Currently the only supported action is client side billing
    * through third party APIs, and the only supported platform for this
    * is the iPhone, with App Store (in app) billing.
    *
    * For the iPhone platform the status data type is "iPhoneAppStore"
    * and the status data value is the corresponding App Store product
    * id. This product id corresponds to the product that can be purchased 
    * in order to be able to perform the previous action succesfully (causing
    * the error). After completed client side billing requests, 
    * BillingInterface::verifyThirdPartyTransaction must be called in order
    * to register and verify the purchase with the server.
    *
    * @see BillingInterface for further documentation on the billing
    *      flows.
    *
    * @return The status data.
    */
   const WFString& getStatusData() const;


   /**
    * Return if status is OK.
    *
    * @return True if status is OK.
    */
   bool operator()() const;

private:


   /**
    * The StatusCode.
    */
   wf_uint32 m_statusCode;

   /**
    * The string message for the status.
    */
   WFString m_statusMessage;

   /**
    * An URL to open for the status.
    */
   WFString m_statusURL;

   /**
    * The status data for the status. May be empty.
    * Used to indicate an appropriate client side action in order to handle 
    * an error. 
    *
    * The status data string is formatted as 
    * "<status data type>;<status data value>".
    *
    * The status data can for instance be used to indicate which product 
    * id that could be purchased for expired rights. 
    *
    * Currently the only supported action is client side billing
    * through third party APIs, and the only supported platform for this
    * is the iPhone, with App Store (in app) billing.
    *
    * For the iPhone platform the status data type is "iPhoneAppStore"
    * and the status data value is the corresponding App Store product
    * id. This product id corresponds to the product that can be purchased 
    * in order to be able to perform the previous action succesfully (causing
    * the error). After completed billing requests, 
    * BillingInterface::verifyThirdPartyTransaction must be called in order
    * to register and verify the purchase with the server.
    *
    * @see BillingInterface for further documentation on the billing
    *      flows.
    */
   WFString m_statusData;
};

} // End namespace WFAPI

#endif // End WFAPI_SYNCHRONOUS_STATUS_H
