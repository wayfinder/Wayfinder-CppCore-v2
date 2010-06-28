/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "BillingInterfaceImpl.h"
#include "BillingListener.h"
#include "Nav2APIImpl.h"
#include "SynchronousStatus.h"
#include "AsynchronousStatus.h"


// Include GuiProt
#include "GuiProtMess.h"
#include "GuiProtFavoriteMess.h"
#include "GuiProtEnums.h"
#include "NavRequestType.h"
#include "NParamBlock.h"

#include "Nav2Logging.h"

using namespace isab;
using namespace std;

namespace WFAPI {

BillingInterfaceImpl::BillingInterfaceImpl(Nav2APIImpl* api)
   : InterfaceImpl(api)
{
   
}
   
BillingInterfaceImpl::~BillingInterfaceImpl()
{

}

void
BillingInterfaceImpl::receiveAsynchronousReply(const isab::GuiProtMess&
                                                 guiProtMess,
                                                 RequestID requestID)
{
   AsynchronousStatus status = createAsynchStatus(&guiProtMess, requestID);

   // Check status if OK.
   if (status.getStatusCode() != OK) {
      // Bad status. Report error function for all listeners.
      reportError(status, m_listeners.begin(), m_listeners.end());
      return;
   }

   isab::GuiProtEnums::MessageType type = guiProtMess.getMessageType();
   switch (type) {

   case isab::GuiProtEnums::GUI_TO_NGP_DATA_REPLY: {
      const isab::DataGuiMess& mess =
         static_cast<const isab::DataGuiMess&>(guiProtMess);
      
      int32 type = -1;
      NonOwningBuffer dataMessageType(
         const_cast<uint8*>(mess.getData()), mess.getSize());
         
      if (dataMessageType.remaining() >= 2) {
         type = dataMessageType.readNextUnaligned16bit();
      }
      switch (type) {
      case isab::navRequestType::NAV_VERIFY_THIRD_PARTY_TRANSACTION_REPLY:
         // Third party transaction verified succesfully.
         for(ListenerCont::iterator it = m_listeners.begin();
             it != m_listeners.end(); ++it) {
            (*it)->thirdPartyTransactionVerified(requestID);
         }
         break;
      default:
         break;
      }
         
      break;
   }
   default:
      nav2log << "BillingInterfaceImpl::receiveAsynchronousReply of "
              << "unknown type: " << int(type) << endl;
      break;
   } 
}
   
void
BillingInterfaceImpl::addBillingListener(BillingListener* listener)
{
   m_listeners.insert(listener);
}

void
BillingInterfaceImpl::removeBillingListener(BillingListener* listener)
{
   m_listeners.erase(listener);
}
   
AsynchronousStatus 
BillingInterfaceImpl::verifyThirdPartyTransaction(const WFString& transactionID,
                                                  const WFString& selectionString)
{
   using namespace std;
   isab::NParamBlock requestParams;
   
   requestParams.addParam(isab::NParam(6400, transactionID.c_str()));
   requestParams.addParam(isab::NParam(6401, selectionString.c_str()));

   return sendAsynchronousNGPRequestToNav2(
      requestParams, isab::navRequestType::NAV_VERIFY_THIRD_PARTY_TRANSACTION_REQ);

}

} // End namespace WFAPI
