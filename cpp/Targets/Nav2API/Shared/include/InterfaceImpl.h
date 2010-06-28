/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_INTERFACE_IMPL_H
#define WFAPI_INTERFACE_IMPL_H

#include "WFAPIConfig.h"
#include "RequestID.h"
#include "AsynchronousStatus.h"
#include <set>

namespace isab {
// Forward declarations
class GuiProtMess;
class Monitor;
class NParamBlock;
}


namespace WFAPI {

// Forward declaration
class Nav2APIImpl;


/**
 * Class for interface implementation.
 */
class InterfaceImpl {

public:
   
   /**
    * Constructor.
    */
   InterfaceImpl(Nav2APIImpl* api);
   
   /**
    * Destructor.
    */
   ~InterfaceImpl();
   
   /**
    * To send a synchronous request to Nav2.
    * This function blocks until a reply is received.
    * 
    * @param guiProtMess The message to be sent.
    * @param requestID (opt.) A RequestID if already obtained for this request.
    * @return The reply GuiProtMess, may be an error message.
    */
    isab::GuiProtMess* sendSynchronousRequestToNav2(
       isab::GuiProtMess* guiProtMess,
       RequestID* requestID = NULL);

   /**
    * To send an asynchronous request to Nav2.
    * This method returns after sending the request. When there is a
    * reply the receiveAsynchronousReply function is called with the
    * RequestID returned from this function call.
    * 
    * @param guiProtMess The message to be sent.
    * @param requestID (opt.) A RequestID if already obtained for this request.
    * @return The status and if status is OK the RequestID for the request.
    */
   AsynchronousStatus sendAsynchronousRequestToNav2(
         isab::GuiProtMess* guiProtMess,
         RequestID* requestID = NULL);

   /**
    * To send an asynchronous request to Nav2 using NGP.
    * 
    * @param params The NGP parameters to send.
    * @param navRequestType The type of request to send, from 
    *                       isab::navRequestType.
    * @param requestID (opt.) A RequestID if already obtained for this request.
    * @return The status and if status is OK the RequestID for the request.
    */
   AsynchronousStatus sendAsynchronousNGPRequestToNav2(
         const isab::NParamBlock& params,
         wf_uint16 navRequestType,
         RequestID* requestID = NULL);
   
   /**
    * Function called when sending reply from Nav2. This is called when
    * receiving replies from synchronous requests.
    *
    * @param guiProtMess The returning data of the reply, is now owned by
    *                    this class.
    * @param requestID The RequestID for the guiProtMess.
    */
   void receiveSynchronousReply(isab::GuiProtMess* guiProtMess,
                                RequestID requestID);
   
   /**
    * Function called when sending reply from Nav2. This is called when
    * receiving replies from asynchronous requests.
    *
    * @param guiProtMess The returning data of the reply.
    * @param requestID The RequestID for the guiProtMess.
    */
   virtual void receiveAsynchronousReply(const isab::GuiProtMess& guiProtMess,
                                         RequestID requestID) = 0;

protected:

  /**
    * Report error to all listeners.
    *
    * @param status The AsynchronousStatus containing the error.
    */
   template<typename ListenerIterator>
   void reportError(const AsynchronousStatus& status,
                    ListenerIterator begin,
                    ListenerIterator end);
   
   /**
    * Create an AsynchronousStatus with OK status.
    *
    * @return An AsynchronousStatus with statusCode OK.
    *         Will contain INVALID_REQUEST_ID as RequestID.
    */
   static AsynchronousStatus createOKAsynchStatus(
      RequestID requestID = RequestID(RequestID::INVALID_REQUEST_ID));

   /**
    * Create an AsynchronousStatus based on the supplied GuiProtMess.
    *
    * @param message The GuiProtMess.
    * @param requestID The RequestID of the reply.
    * @return An AsynchronousStatus with statusCode OK if OK, otherwise
    *         with appropriate failure codes set.
    */
   AsynchronousStatus createAsynchStatus(
      const isab::GuiProtMess* message,
      RequestID requestID = RequestID(RequestID::INVALID_REQUEST_ID));

   /**
    * Get the Nav2APIImpl.
    *
    * @return The Nav2APIImpl.
    */
   Nav2APIImpl* getApi();

private:
   /// Monitor for controlling synchronous requests to Nav2.
   isab::Monitor* m_monitor;

   /// The message obtained from the reply from Nav2.
   isab::GuiProtMess* m_replyData;
   
   /// The Nav2APIImpl.
   Nav2APIImpl* m_api;
};
   
template<typename ListenerIterator> 
void
InterfaceImpl::reportError(const AsynchronousStatus& status,
                           ListenerIterator begin, 
                           ListenerIterator end)
{
   for (ListenerIterator it = begin; it != end; ++it ) {
      if((*it) != NULL) {
         (*it)->error(status);
      }
   }
}

} // End namespace WFAPI


#endif // End WFAPI_INTERFACE_IMPL_H

