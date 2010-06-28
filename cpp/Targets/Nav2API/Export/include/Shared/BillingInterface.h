/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_BILLING_INTERFACE_H
#define WFAPI_BILLING_INTERFACE_H

#include "WFAPIConfig.h"
#include "Interface.h"
#include "SynchronousStatus.h"
#include "AsynchronousStatus.h"

namespace WFAPI {

// Forward declarations
class Nav2APIImpl;
class BillingInterfaceImpl;
class BillingListener;

/**
 * Interface class for billing operations.
 *
 * The following billing flows are currently supported.
 *
 * 1.   Some request is issued to the server
 *      (for instance RouteInterface::routeRequest).
 *
 * 2.   The current user account does not have sufficient
 *      rights in order to process the request and the
 *      registered listener gets a reply in its Listener::error
 *      method including an AsynchronousStatus object.
 * 
 * 3.1. [Alternative 1] In case the current phone platform and 
 *      client supports web based billing, the status object
 *      contains an URL in AsynchronousStatus::getStatusURL()
 *      which points to a web page where the user can purchase
 *      products in order to be able to perform the previous
 *      action succesfully. In case of completing the purchase,
 *      the web page will automatically update the user account
 *      on the server with the new updated rights.
 *      For this alternative there is no need to use the
 *      BillingInterface.
 *
 * 3.2. [Alternative 2] In case the current phone platform and
 *      client supports client side billing (i.e. non web based)
 *      through direct client side third part API calls, the status object
 *      will instead return a non empty string when calling the
 *      AsynchronousStatus::getStatusData() method.
 *      This status data will contain a product id which corresponds
 *      to the product that can be purchased in order to be able
 *      to perform the previous action succesfully.
 *      The status data string is formatted as 
 *      "<status data type>;<status data value>".
 *      Currently the iPhone, with App Store billing, is the only 
 *      supported platform for this type of billing integration.
 *      For the iPhone platform the status data type is "iPhoneAppStore"
 *      and the status data value is the corresponding App Store product
 *      id.
 *
 * 4.2  [Alternative 2] The client application calls the third party
 *      billing methods supplying the product id originating from the 
 *      status data. Succesful billing, results in that a transaction
 *      id or receipt is received.
 *      This means that the user has been billed, but the server has not yet
 *      been updated with the corresponding user rights.
 * 
 * 5.2  [Alternative 2] In order to update the rights on the server due 
 *      to the purchased product, the application calls 
 *      BillingInterface::verifyThirdPartyTransaction() supplying the 
 *      transaction id or receipt from the previous billing as parameter. 
 *      The server will then verify the purchase and upon success add the
 *      corresponding rights to the user account and indicate the successful
 *      operation by calling BillingListener::thirdPartyTransactionVerified.
 *      If the verification fails, then no rights will be added and the
 *      Listener::error will be called.
 *
 * @see BillingListener
 * @see SynchronousStatus
 */
class BillingInterface : public Interface
{

public:

   /**
    * Destructor.
    */
   virtual ~BillingInterface();
   
   /**
    * Add a listener for billing events.
    * @see BillingListener
    *
    * @param listener The BillingListener to add. 
    *                 It may not be deleted until it is removed by
    *                 calling removeBillingListener.
    */
   void addBillingListener(BillingListener* listener);

   /**
    * Remove a listener.
    *
    * @param listener The BillingListener to remove.
    *                 It should now be deleted by caller, unless caller
    *                 keeps it around for later use.
    */
   void removeBillingListener(BillingListener* listener);

   /**
    * Verify a third party transaction with the server.
    * In case of succesful verification of the transaction,
    * the associcated rights will be added to the current account on 
    * the server.
    *
    * This method should be called after completed client side billing
    * (non web based) from purchasing a product originating
    * from SynchronousStatus::getStatusData.
    *
    * @see SynchronousStatus
    *
    * @param transactionID The transaction ID, or receipt, 
    *                      of the third party transaction.
    * @param selectionString The selection that the user made from the
    *                        StatusData from SynchronousStatus.
    *                        
    * @return The status and if status is OK the RequestID that will be
    *         returned in the BillingListener::thirdPartyTransactionVerified() 
    *         function.
    */
   AsynchronousStatus verifyThirdPartyTransaction(const WFString& transactionID,
                                                  const WFString& selectionString);

   /**
    * Internal function.
    */
   BillingInterfaceImpl* getImpl();
    
private:
   /**
    * Do not create BillingInterface get it from the Nav2API class.
    */
   BillingInterface(Nav2APIImpl* nav2APIimpl);

   /**
    * We do not allow copies of this interface.
    */
   BillingInterface(const BillingInterface&);
   const BillingInterface& operator=(const BillingInterface&);
   
   friend class Nav2APIImpl;

   struct impl;

   impl* m_impl;
};

} // End namespace WFAPI

#endif // End WFAPI_BILLING_INTERFACE_H
