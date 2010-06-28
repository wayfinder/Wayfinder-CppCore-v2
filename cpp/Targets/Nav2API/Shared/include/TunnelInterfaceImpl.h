/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_TUNNEL_INTERFACE_IMPL_H
#define WFAPI_TUNNEL_INTERFACE_IMPL_H

#include "WFAPIConfig.h"
#include "InterfaceImpl.h"
#include "SynchronousStatus.h"
#include "AsynchronousStatus.h"
#include "HttpTunnelGuiMess.h"
#include "TunnelInterface.h"
#include "ReplyData.h"

#include <map>
#include <utility>

namespace WFAPI {

// Forward declaration
class TunnelListener;

/**
 * The behind the scenes class for the Tunnel Interface.
 */   
class TunnelInterfaceImpl : public InterfaceImpl,
                            public isab::HttpTunnelGuiMessObserver {
public:

   /**
    * Constructor.
    */
   TunnelInterfaceImpl(Nav2APIImpl* api);
   
   /**
    * Destructor.
    */
   virtual ~TunnelInterfaceImpl();

   /**
    * Add a listener for favourite events.
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
    * Implementation of abstract function from HttpTunnelGuiMessObserver.
    */
   //@{
   /**
    * DataReceived
    * Called when the data in a HttpTunnelGuiMess has been parsed.
    * @param seqId      Sequence id from message
    * @param startByte  Index of first byte in data
    * @param endByte    Index of last byte in data
    * @param total      Total number of bytes in the file
    * @param url        URL
    * @param data       Data pointer
    * @param dataLength Length of the data
    */
   virtual void DataReceived(uint32 seqId,
                             uint32 startByte,
                             uint32 endByte,
                             uint32 total,
                             const char* url,
                             const char* data,
                             uint32 dataLength);
   //@}

   
   

protected:
   
private:

   /**
    * To request data from server
    *
    * @param URL The url to the data to request.
    * @param seqID The ID of the request.
    * @param startByte The first byte to request.
    *
    * @return The status of the request.
    */
   AsynchronousStatus requestTunnelData(WFString URL,
                                        wf_uint32 seqID,
                                        wf_uint32 startByte);
   /**
    * To get the user UIN
    *
    * @return The user UIN as a string.
    */
   WFString getUIN();

   /**
    * To get the server prefix that is used, by that means "nav" when server
    * address is nav.services.vodafone.com.
    *
    * @return The server prefix.
    */
   WFString getServerInstance();
   
   /// Typedef for a set of listeners.
   typedef std::set<TunnelListener*> ListenerCont;

   /// Set of listeners.
   ListenerCont m_listeners;

   /**
    * Map that holds the sequenceID and RequestID for the original request
    * made by the user of the API.
    */
   typedef std::map<const wf_uint32, const RequestID> RequestIDHolder;

   /**
    * Map that holds the internal requestID and sequence number. Used to find
    * which is the original requestID for every request.
    */
   typedef std::map<const RequestID, const wf_uint32> InternalRequestIDHolder;

   /// Holds the sequence ID and requestID for every request.
   RequestIDHolder m_requestIDHolder;

   /// Holds which RequestID the correpond to which sequenceID
   InternalRequestIDHolder m_internalRequestIDHolder;

   /// sequenceID of the requested data from server.
   wf_uint32 m_seqID;

   /// Holds the current requestID.
   RequestID m_currentRequestID;
   
};
   
} // End namespace WFAPI

#endif // End WFAPI_TUNNEL_INTERFACE_IMPL_H
