/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_BILLING_INTERFACE_IMPL_H
#define WFAPI_BILLINGE_INTERFACE_IMPL_H

#include "WFAPIConfig.h"
#include "InterfaceImpl.h"
#include "SynchronousStatus.h"
#include "AsynchronousStatus.h"
#include "HelperEnums.h"

namespace WFAPI {

// Forward declaration
class BillingListener;

/**
 * The behind the scenes class for the Billing Interface.
 */   
class BillingInterfaceImpl : public InterfaceImpl {
public:

   /**
    * Constructor.
    */
   BillingInterfaceImpl(Nav2APIImpl* api);
   
   /**
    * Destructor.
    */
   virtual ~BillingInterfaceImpl();


   /**
    * Implementation of abstract function from InterfaceImpl.
    */
   //@{
   /**
    * Function called when sending reply from Nav2. This is called when
    * receiving replies from asynchronous requests.
    *
    * @param guiProtMess The returning data of the reply.
    * @param requestID The RequestID for the guiProtMess.
    */
   void receiveAsynchronousReply(const isab::GuiProtMess& guiProtMess,
                                 RequestID requestID);
   //@}
   
   /**
    * Add a listener for favourite events.
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

protected:
   
private:

   /// Typedef for a set of listeners.
   typedef std::set<BillingListener*> ListenerCont;

   /// Set of listeners.
   ListenerCont m_listeners;

};
   
} // End namespace WFAPI

#endif // End WFAPI_BILLING_INTERFACE_IMPL_H
