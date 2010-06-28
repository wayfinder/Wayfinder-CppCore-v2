/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_BILLING_LISTENER_H
#define WFAPI_BILLING_LISTENER_H

#include "WFAPIConfig.h"
#include "Listener.h"

namespace WFAPI {

/**
 * Listener for billing events. The abstract functions are the
 * callback functions that is called for the respective asynchronous
 * function call in BillingInterface.
 *
 * Implement a sub class of BillingListener and add as a listener
 * to the BillingInterface to start getting billing callbacks.
 *
 * The error function, declared in the super class Listener, will be called
 * for all requests that returns with an error.
 *
 * @see BillingInterface
 *
 */
class BillingListener : public Listener
{
   
public:
   
   /**
    * Constructor with no arguments.
    */
   BillingListener();

   /**
    * Destructor.
    */
   virtual ~BillingListener();

   /**
    * Called upon succesful verification of a third party transaction 
    * by the server due to a previous call to 
    * BillingInterface::verifyThirdPartyTransaction. This means that
    * the server has added the associated rights from the purchase
    * to the current account. Failure to verify the transaction
    * will lead to a call to Listener::error() instead.
    *
    * @param requestID The request id for the callback matches the 
    *                  request id received in the AsynchronousStatus
    *                  call to
    *                  BillingInterface::verifyThirdPartyTransaction.
    */
   virtual void thirdPartyTransactionVerified(RequestID requestID) = 0;

};

} // End namespace WFAPI

#endif // End WFAPI_BILLING_LISTENER_H
