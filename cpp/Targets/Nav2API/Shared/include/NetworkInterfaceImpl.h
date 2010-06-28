/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_NETWORK_INTERFACE_IMPL_H
#define WFAPI_NETWORK_INTERFACE_IMPL_H

#include "WFAPIConfig.h"
#include "InterfaceImpl.h"
#include "SynchronousStatus.h"
#include "RequestID.h"
#include "WFStringArray.h"
#include "NetworkLinkID.h"
#include <set>

// Forward declarations
class ConnectionManager;

namespace isab {
// Forward declarations
class GuiProtMess;
}

namespace WFAPI {

// Forward declarations
class NetworkListener;


/**
 * The behind the scenes class for Network Interface.
 */
class NetworkInterfaceImpl : public InterfaceImpl {
public:

   /**
    * Constructor.
    */ 
   NetworkInterfaceImpl(Nav2APIImpl* api);

   /**
    * Destructor.
    */
   virtual ~NetworkInterfaceImpl();

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
    * Functions wrapped from NetworkInterface.
    */
   //@{

   /**
    * Add a listener for network events.
    * @see NetworkListener
    *
    * @param listener The NetworkListener to add. 
    *                 It may not be deleted until it is removed by
    *                 calling removeNetworkListener.
    */
   void addNetworkListener(NetworkListener* listener);

   /**
    * Remove any set NetworkLinkID and returns StatusCode::OK if the
    * operation was successful.
    *
    * @return The status of the operation.
    */
   SynchronousStatus clearNetworkLinkID();

   /**
    * Get if it is set that the network link layer is to be keept up 
    * or not.
    *
    * @return If to keep network link up.
    */
   SynchronousStatus getKeepLinkUp(bool& val);

   /**
    * Get the currently set NetworkLinkID.
    *
    * @param networkLinkID Set to the current NetworkLinkID.
    * @return The status of the operation, statusCode::OK if the
    * operation was successful.
    */
   SynchronousStatus getNetworkLinkID(NetworkLinkID& networkLinkID);

   /**
    * Remove a listener.
    *
    * @param listener The NetworkListener to remove.
    *                 It should now be deleted by caller, unless caller
    *                 keeps it around for later use.
    */
   void removeNetworkListener(NetworkListener* listener);

   /**
    * Set if to keep the network link layer up or dissconnect it if
    * it is not used.
    *
    * @param newVal If to keep network link up.
    */  
   SynchronousStatus setKeepLinkUp(bool newVal);

   /**
    * Set the current NetworkLinkID.
    *
    * @param networkLinkID The new current NetworkLinkID.
    * @return The status of the operation, statusCode::OK if the
    * operation was successful.
    */
   SynchronousStatus setNetworkLinkID(const NetworkLinkID& networkLinkID);

   /**
    * This asynchronous function starts a test of the connection to the server
    * using the current NetworkLinkID, when operation is done the added
    * NetworkListeners will get a callback with the RequestID returned by
    * this call.
    *
    * @return The status and if status is OK the RequestID that will be
    *         returned in the NetworkListeners testServerConnectionReply 
    *         function.
    */
   AsynchronousStatus testServerConnection();

   /**
    * This function forwards the error message to the listeners
    *
    * @param status The AsynchronousStatus containing the error codes.
    */
   void reportNetworkError(AsynchronousStatus status);

  /**
   * This function sets the connectionManager to use.
   * The connection manager will be deallocated at the destruction of the
   * network interface impl.
   *
   * @param connMgr The connectionManager to use.
   */
   void setConnectionManager(ConnectionManager* connMgr);

   /**
    * This function returns the connectionManager that is used.
    * 
    * @return The current connectionManager.
    */
   ConnectionManager* getConnectionManager();
   
   //@}   
protected:

private:

   typedef std::set<NetworkListener*> ListenerCont;
   /**
    * Set of listeners.
    */
   ListenerCont m_listeners;

   ConnectionManager* m_connMgr;
};

} // End namespace WFAPI

#endif // End WFAPI_NETWORK_INTERFACE_IMPL_H

