/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_NETWORK_LINK_ID_H
#define WFAPI_NETWORK_LINK_ID_H

#include "WFAPIConfig.h"
#include "WFString.h"

namespace WFAPI {

/**
 * Holder of a network link id.
 */
class NetworkLinkID
{

public:

   /**
    * The special NetworkLinkID that tries using the system default
    * NetworkLinkID.
    */
   static const wf_int32 TRY_DEFAULT;

   /**
    * The special NetworkLinkID that always needs to ask the user
    * for NetworkLinkID.
    */
   static const wf_int32 ALWAYS_ASK;

   /**
    * When the network access point is not using a proxy, the
    * m_proxyPort should be set to this value.
    */
   static const wf_uint32 INVALID_PROXY_PORT;

   /**
    * Default constructor.
    * Sets the id to TRY_DEFAULT, the proxy port to INVALID_PROXY_PORT
    * and the proxy host to empty string.
    */
   NetworkLinkID();
   
   /**
    * Constructor.
    *
    * @param id The network link id.
    */
   NetworkLinkID(wf_int32 id);

   /**
    * Constructor.
    * Use this constructor when not having a proxy (host and port)
    * 
    * @param id The network link id.
    * @param proxyPort The proxyPort, if using proxy.
    * @param proxyHost The proxyHost, if using proxy.
    */
   NetworkLinkID(wf_int32 id, wf_uint32 proxyPort, WFString proxyHost);

   /**
    * Destructor.
    */
   virtual ~NetworkLinkID();

   /**
    * Get the network link id.
    *
    * @return The raw id.
    */
   wf_int32 getID() const;

   /**
    * Get the port, only valid if using proxy
    *
    * @return The port for this accesspoint. 
    */
   wf_uint32 getProxyPort() const;

   /**
    * Get the host, only valid if using proxy
    *
    * @return The host for this accesspoint. 
    */
   const WFString& getProxyHost() const;

   /**
    * Sets the id.
    *
    * @param id The new id.
    */
   void setID(wf_int32 id);

   /**
    * Sets the proxy port.
    *
    * @param proxyPort The new proxy port.
    */
   void setProxyPort(wf_uint32 proxyPort);

   /**
    * Sets the proxy host.
    *
    * @param proxyHost The new proxy host.
    */
   void setProxyHost(const WFString& proxyHost);

private:

   /// The id.
   wf_int32 m_id;

   /// The port, if using proxy
   wf_uint32 m_proxyPort;

   /// The host, if using proxy
   WFString m_proxyHost;

};

} // End namespace WFAPI

#endif // End WFAPI_NETWORK_LINK_ID_H
