/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_NETWORK_INTERFACE_H
#define WFAPI_NETWORK_INTERFACE_H

#include "WFAPIConfig.h"
#include "Interface.h"
#include "NetworkLinkID.h"
#include "NetworkListener.h"
#include "SynchronousStatus.h"
#include "AsynchronousStatus.h"

namespace WFAPI {

// Forward declarations
class NetworkListener;
class Nav2APIImpl;
class NetworkInterfaceImpl;


/**
 * Interface class for network.
 */
class NetworkInterface : public Interface
{

public:

   /**
    * Constructor.
    */
   NetworkInterface();

   /**
    * Destructor.
    */
   virtual ~NetworkInterface();

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
    * @param val Set if to keep network link up or not.
    * @return The status of the operation, statusCode::OK if the
    *         operation was successful.
    */
   SynchronousStatus getKeepLinkUp(bool& val);

   /**
    * Get the currently set NetworkLinkID.
    *
    * @param networkLinkID Set to the current NetworkLinkID.
    * @return The status of the operation, statusCode::OK if the
    *         operation was successful.
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
    * @return The status of the operation, statusCode::OK if the
    *         operation was successful.
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
    * Internal function.
    */
   NetworkInterfaceImpl* getImpl();

private:
   /**
    * Do not create NetworkInterface get it from the Nav2API class.
    */
   NetworkInterface(Nav2APIImpl* nav2APIimpl);

   /**
    * We do not allow copies of this interface.
    */
   NetworkInterface(const NetworkInterface&);
   const NetworkInterface& operator=(const NetworkInterface&);

   friend class Nav2APIImpl;

   struct impl;

   impl* m_impl;
};

} // End namespace WFAPI

#endif // End WFAPI_NETWORK_INTERFACE_H
