/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_TUNNEL_INTERFACE_H
#define WFAPI_TUNNEL_INTERFACE_H

#include "WFAPIConfig.h"
#include "Interface.h"
#include "SynchronousStatus.h"
#include "AsynchronousStatus.h"

namespace WFAPI {

// Forward declarations
class Nav2APIImpl;
class TunnelInterfaceImpl;
class TunnelListener;

/**
 * Interface class for requesting tunnel data from the server.
 *
 * This interface can also be used for requesting data used for the
 * implementation of the service window. When implementing the service window
 * the first call when requesting data needs to be
 * AsynchronousStatus requestData(destURL, parameter, clientVersion),
 * this is because it is adding neccessary parameters to the url that is needed
 * to start the flow of the service window correctly.
 *
 * When requesting further data, it is ok to request it through the  
 * AsynchronousStatus requestData(destURL) function.
 */

class TunnelInterface : public Interface
{

public:

   /**
    * Destructor.
    */
   virtual ~TunnelInterface();
   
   /**
    * Add a listener for billing events.
    * @see TunnelListener
    *
    * @param listener The TunnelListener to add. 
    *                 It may not be deleted until it is removed by
    *                 calling removeTunnelListener.
    */
   void addTunnelListener(TunnelListener* listener);

   /**
    * Remove a listener.
    *
    * @param listener The TunnelListener to remove.
    *                 It should now be deleted by caller, unless caller
    *                 keeps it around for later use.
    */
   void removeTunnelListener(TunnelListener* listener);

   /**
    * To request data from the server through the core.
    *
    * @param destURL The URL for the data to request.
    * @param parameter Additional parameters to add to the URL.
    * @param clientVersion The application's version.
    *
    * @return The status and if status is OK the RequestID that will be
    *         returned in the TunnelListener::dataReceived() function.
    */
   AsynchronousStatus requestData(WFString destURL,
                                  WFString parameter,
                                  WFString clientVersion);
   
   /**
    * To request data from the server through the core.
    *
    * @param destURL The URL for the data to request.
    *
    * @return The status and if status is OK the RequestID that will be
    *         returned in the TunnelListener::dataReceived() function.
    */
   AsynchronousStatus requestData(WFString destURL);
      
   /**
    * Internal function.
    */
   TunnelInterfaceImpl* getImpl();
    
private:
   /**
    * Do not create TunnelInterface get it from the Nav2API class.
    */
   TunnelInterface(Nav2APIImpl* nav2APIimpl);

   /**
    * We do not allow copies of this interface.
    */
   TunnelInterface(const TunnelInterface&);
   const TunnelInterface& operator=(const TunnelInterface&);
   
   friend class Nav2APIImpl;

   struct impl;

   impl* m_impl;
};

} // End namespace WFAPI

#endif // End WFAPI_BILLING_INTERFACE_H
