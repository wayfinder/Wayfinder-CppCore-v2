/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef BILLING_TESTER_H
#define BILLING_TESTER_H

#include "BillingListener.h"
#include "BillingInterface.h"
#include "Nav2API.h"
#include "RequestID.h"
#include "WFStringArray.h"
#include "GenericTester.h"

using namespace WFAPI;

/**
 * Test class that implements billing testing.
 */
class BillingTester : public BillingListener, 
                      public GenericTester
{

public:
   
   /**
    * Constructor, adds itself as listener to BillingInterface
    *
    * @param billingInterface The billing interface which to add
    *                           ourself as listener.
    */
   BillingTester(BillingInterface& billingInterface);
                   
   /**
    * Destructor.
    */
   ~BillingTester();
   
   /**
    * Asks the user to verify a third party transaction with the server.
    */
   void verifyThirdPartyTransaction();

public: // From base class

   /**
    * Called when an error has occured.
    */
   virtual void error(const AsynchronousStatus& status);

   /**
    * From BillingListener
    *
    * Called upon succesful verification of a third party transaction 
    * by the server due to a previous call to 
    * BillingInterface::verifyThirdPartyTransaction. This means that
    * the server has added the associated rights from the purchase
    * to the current account.
    *
    * @param requestID The request id for the callback is matches the 
    *                  request id received in the AsynchronousStatus
    *                  in the call to 
    *                  BillingInterface::verifyThirdPartyTransaction.
    */
   virtual void thirdPartyTransactionVerified(RequestID requestID);

   /**
    * Handle the input and return true if it was consumed.
    *
    * @param input The input to handle.
    * @param goBack Out parameter. Will be set to true if the
    *               input indicated that the tester should go
    *               out of scope.
    * @return True if the input was handled, false otherwise.
    */
   bool handleInput(const char* input, bool& goBack);

private:

   /**
    * Adds the command options valid for this class. The storage is a map
    * where the key is a String and the value is a CommandHolder.
    * The CommandHolder holds the actual command to perform so by finding
    * in the map after user input we can map this against a real command.
    */
   void addCommandOptions();

   /// the BillingInterface
   BillingInterface& m_billingInterface;

};

#endif
