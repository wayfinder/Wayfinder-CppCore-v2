/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#define CORE_LOGGING_MODULE_NAME "TunnelInterfaceImpl"
// #define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE

#include "CoreDebugPrint.h"

#include "TunnelInterfaceImpl.h"
#include "TunnelListener.h"
#include "Nav2APIImpl.h"
#include "Nav2API.h"
#include "SynchronousStatus.h"
#include "AsynchronousStatus.h"
#include "HttpTunnelGuiMess.h"
#include "GuiProtMess.h"
#include "GuiParameterEnums.h"
#include "LanguageHelper.h"
#include "TunnelInterfaceStatusCode.h"

#include <utility>
#include <string>


#define HEADER_DELIMITER "\r\n\r\n"

using namespace isab;
using namespace std;

namespace WFAPI {

TunnelInterfaceImpl::TunnelInterfaceImpl(Nav2APIImpl* api)
   : InterfaceImpl(api),
     m_seqID(0),
     m_currentRequestID(RequestID::INVALID_REQUEST_ID)
{
   
}
   
TunnelInterfaceImpl::~TunnelInterfaceImpl()
{

}

void
TunnelInterfaceImpl::addTunnelListener(TunnelListener* listener)
{
   m_listeners.insert(listener);
}

void
TunnelInterfaceImpl::removeTunnelListener(TunnelListener* listener)
{
   m_listeners.erase(listener);
}

void
TunnelInterfaceImpl::receiveAsynchronousReply(const isab::GuiProtMess&
                                               guiProtMess,
                                               RequestID requestID)
{
   AsynchronousStatus status = createAsynchStatus(&guiProtMess, requestID);
   m_currentRequestID = requestID;
   // Check status if OK.
   if (status.getStatusCode() != OK) {
      // Bad status. Report error function for all listeners.
      reportError(status, m_listeners.begin(), m_listeners.end());
      return;
   }

   GuiProtEnums::MessageType type = guiProtMess.getMessageType();
   switch (type) {

   case GuiProtEnums::GUI_TO_NGP_DATA_REPLY:
   case GuiProtEnums::GUI_TUNNEL_DATA_REPLY:
   {
      HttpTunnelGuiMess::ParseTunnelMess(
         static_cast<const DataGuiMess*>(&guiProtMess), this);
      return;
   }
   break;
   case GuiProtEnums::REQUEST_FAILED:
   {
      InternalRequestIDHolder::iterator internalRequest =
         m_internalRequestIDHolder.find(requestID);
      if(internalRequest == m_internalRequestIDHolder.end()) {
         // This should not happen
         return;
      } else {

         RequestIDHolder::iterator requestInfo =
            m_requestIDHolder.find(internalRequest->second);
         if(requestInfo == m_requestIDHolder.end()) {
            // This should not happen
            return;
         } else {
            AsynchronousStatus errorStatus(requestInfo->second,
                                           REQUEST_FAILED, "", "");
            reportError(errorStatus, m_listeners.begin(), m_listeners.end());
            m_requestIDHolder.erase(requestInfo);
            m_internalRequestIDHolder.erase(internalRequest);
            return;
         }
      
      }
   }
   break;
   default:
      coreprintln(
         "TunnelInterfaceImpl::receiveAsynchronousReply of unknown type: %d",
         type);
      break;
   }
}



AsynchronousStatus
TunnelInterfaceImpl::requestData(WFString destURL,
                                 WFString parameter,
                                 WFString clientVersion)
{
   string additionalURL;
   
   WFString clientType = getApi()->getNav2API()->getClientType();
   
   WFString uin = getUIN();
  
   WFString serverInstance = getServerInstance();
      
   TextLanguage lang = getApi()->getSessionData()->getTextLanguage();
   WFString langCode = LanguageHelper::getLanguageCodeForText(lang);
      
   // Build up the additional URL.
   additionalURL += "l=";
   additionalURL += langCode.c_str();
   
   additionalURL += "&c=";
   additionalURL += clientType.c_str();

   additionalURL += "&s=";
   if(serverInstance != ""){
      additionalURL += serverInstance.c_str();
   }
   additionalURL += "&u=";
   if(uin != ""){
      additionalURL += uin.c_str();
   }
   additionalURL += "&v=";
   additionalURL += clientVersion.c_str();

   //Build up the complete URL.
   string completeURL = destURL.c_str();
   size_t foundQMark;
   foundQMark = completeURL.find("?");
   if(foundQMark == string::npos) {
      completeURL += "?";
   } else {
      completeURL += "&";
   }
 
   completeURL += additionalURL.c_str();
   if(additionalURL != "" && parameter != "") {
      completeURL += "&";
   }
   completeURL += parameter.c_str();

   m_seqID++;
   AsynchronousStatus status = requestTunnelData(completeURL.c_str(),
                                                 m_seqID,
                                                 0);

   m_requestIDHolder.insert(std::make_pair(m_seqID, status.getRequestID()));
      
   return status;
}

AsynchronousStatus
TunnelInterfaceImpl::requestData(WFString destURL)
{
   m_seqID++;
   AsynchronousStatus status = requestTunnelData(destURL.c_str(), m_seqID,  0);
   m_requestIDHolder.insert(std::make_pair(m_seqID, status.getRequestID()));
      
   return status;
}

AsynchronousStatus
TunnelInterfaceImpl::requestTunnelData(WFString URL, wf_uint32 seqID, wf_uint32 startByte)
{
   DataGuiMess* dgm = 
      HttpTunnelGuiMess::CreateTunnelMess(1,
                                          seqID,
                                          URL.c_str(),
                                          startByte);

   AsynchronousStatus status = sendAsynchronousRequestToNav2(dgm);
   m_internalRequestIDHolder.insert(std::make_pair(status.getRequestID(), m_seqID));
   dgm->deleteMembers();
   delete dgm;

   return status;
   
}

void
TunnelInterfaceImpl::DataReceived(uint32 seqId,
                                  uint32 startByte,
                                  uint32 endByte,
                                  uint32 total,
                                  const char* url,
                                  const char* data,
                                  uint32 dataLength)
{
   RequestIDHolder::iterator requestInfo = m_requestIDHolder.find(seqId);
   if(requestInfo == m_requestIDHolder.end()) {
      //Request id not found in m_requestIDHolder, so we return.
      //It should never happen.
      return;
   } else {
      bool complete = true;
      //Check if we received any data.
      if(endByte == 0 && total == 0) {
         coreprintln("No data received");
         AsynchronousStatus errorStatus(requestInfo->second,
                                        NO_DATA_AVAILABLE, "", "");
         reportError(errorStatus, m_listeners.begin(), m_listeners.end());
         m_requestIDHolder.erase(requestInfo);
         InternalRequestIDHolder::iterator internalRequestInfo
            = m_internalRequestIDHolder.find(m_currentRequestID);
         if(requestInfo == m_requestIDHolder.end()) {
            // Should not happen
         } else {
            m_internalRequestIDHolder.erase(internalRequestInfo);
         }

         return;
      }
      if(endByte + 1 != total && total > endByte) {
         coreprintln("More chunk of data is needed");
         complete = false;
      } else {
         coreprintln("Last chunk!");
      }

      // remove header
      char* cleandata = const_cast<char*>(data);
      cleandata = strstr(data, HEADER_DELIMITER);

      if (cleandata == NULL) {
         // Ehm... Big problem. Set to Zero and hope for the best. 
         cleandata = const_cast<char*>(data);
      } else {
         // Add length of CRLF.
         cleandata += strlen(HEADER_DELIMITER);
      }
      
      //Send data to the listeners
      ReplyData replyData(cleandata,
                          endByte - startByte + 1,
                          total,
                          complete);
      for(ListenerCont::iterator it = m_listeners.begin();
          it != m_listeners.end(); ++it) {
         (*it)->dataReceived(requestInfo->second, replyData);
      }
      InternalRequestIDHolder::iterator internalRequestInfo
         = m_internalRequestIDHolder.find(m_currentRequestID);
      if(requestInfo == m_requestIDHolder.end()) {
      } else {
         m_internalRequestIDHolder.erase(internalRequestInfo);
      }
      
      if(!complete) {
         coreprintln("Request the rest of the data!");
         // This means that there is data missing, so we have to request the rest.
         AsynchronousStatus status =
            requestTunnelData(WFString(url), seqId, endByte+1);

         if(status.getStatusCode() != OK) {
            InternalRequestIDHolder::iterator internalRequest =
               m_internalRequestIDHolder.find(status.getRequestID());
            if(internalRequest == m_internalRequestIDHolder.end()) {
               // Should not happen
               return;
            } else {

               RequestIDHolder::iterator requestInfo =
                  m_requestIDHolder.find(seqId);
               if(requestInfo == m_requestIDHolder.end()) {
                  // Should not happen
                  return;
               } else {
                  AsynchronousStatus errorStatus(requestInfo->second,
                                                 REQUEST_FAILED, "", "");
                  reportError(errorStatus, m_listeners.begin(), m_listeners.end());
                  m_requestIDHolder.erase(requestInfo);
                  m_internalRequestIDHolder.erase(internalRequest);
                  return;
               }
      
            }
         }
         
      } else {
         coreprintln("All data downloded, remove from map");
         // Remove seqID and requestID from the map.  
         m_requestIDHolder.erase(requestInfo);

      }
   }
}

WFString
TunnelInterfaceImpl::getUIN() {

   isab::GeneralParameterMess mess(isab::GuiProtEnums::paramUserAndPassword);
   
   isab::GuiProtMess* reply = sendSynchronousRequestToNav2(&mess);

   // Check status of reply.
   AsynchronousStatus status = createAsynchStatus(
      reply, RequestID(RequestID::INVALID_REQUEST_ID));

   if (status.getStatusCode() != OK) {
      // Bad status.
      mess.deleteMembers();
      reply->deleteMembers();
      delete reply;
      return "";
   }

   isab::GeneralParameterMess* gpm =
      static_cast<isab::GeneralParameterMess*>(reply);
   WFString uin;

   if (gpm->getNumEntries() > 0 && (gpm->getStringData())[0] != NULL) {
      uin = (gpm->getStringData())[0];
   }

   // Delete members.
   mess.deleteMembers();
   reply->deleteMembers();
   delete reply;

   return uin;
   
}

WFString
TunnelInterfaceImpl::getServerInstance() {

   // Can this one really be used? According to guiparameterenums debug only.
   isab::GeneralParameterMess mess(isab::GuiProtEnums::paramServerNameAndPort);
   
   isab::GuiProtMess* reply = sendSynchronousRequestToNav2(&mess);

   // Check status of reply.
   AsynchronousStatus status = createAsynchStatus(
      reply, RequestID(RequestID::INVALID_REQUEST_ID));

   if (status.getStatusCode() != OK) {
      // Bad status.
      mess.deleteMembers();
      reply->deleteMembers();
      delete reply;
      return "";
   }

   isab::GeneralParameterMess* gpm =
      static_cast<isab::GeneralParameterMess*>(reply);
   string serverInstance;
   if (gpm->getNumEntries() > 0) {
      serverInstance = (gpm->getStringData())[0];
   }

   int pos = serverInstance.find(".");
   serverInstance = serverInstance.substr(0,pos);
   
   // Delete members.
   mess.deleteMembers();
   reply->deleteMembers();
   delete reply;

   return serverInstance.c_str();
   
}

} // End namespace WFAPI






