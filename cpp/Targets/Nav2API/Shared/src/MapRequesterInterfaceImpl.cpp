/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "MapRequesterInterfaceImpl.h"
#include "Nav2APIImpl.h"
#include "HttpClientConnectionListener.h"
#include "NetworkInterfaceImpl.h"

#include "GuiProtMess.h"
#include "GuiProtEnums.h"
#include "Nav2Logging.h"

#define CORE_LOGGING_MODULE_NAME "MapRequesterInterfaceImpl"
#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE
#include "CoreDebugPrint.h"

using namespace isab;
using namespace std;

namespace WFAPI {


MapRequesterInterfaceImpl::MapRequesterInterfaceImpl(Nav2APIImpl* api) :
   InterfaceImpl(api)
{
   m_listener = NULL;
   m_networkInterface = NULL;
}

MapRequesterInterfaceImpl::~MapRequesterInterfaceImpl()
{

}

void 
MapRequesterInterfaceImpl::receiveAsynchronousReply(
      const isab::GuiProtMess& guiProtMess,
      RequestID requestID)
{
   // Enough with all the isab prefixing..
   using namespace isab;

   // Get the status.
   AsynchronousStatus status = createAsynchStatus(&guiProtMess, requestID);
   
   isab::GuiProtEnums::MessageType type = guiProtMess.getMessageType();
   switch (type) {
      case GuiProtEnums::GET_MULTI_VECTOR_MAP_REPLY:
      {
         if (m_listener != NULL) {
            const DataGuiMess* reply = 
               static_cast<const DataGuiMess*> (&guiProtMess);

            if (status.getStatusCode() == OK) {
               nav2log << "MRIImpl::receiveAsynchronousReply(" 
                         << (char*) reply->getData() << ")" << endl;
               // Everything is OK:
               m_listener->bytesReceived(
                     requestID.getID(),     //id
                     200,                   //http-request status
                     reply->getData(),      //data
                     0,                     //startoffset,
                     reply->getSize(),      //nbrBytes 
                     reply->getSize());     //contentLength
            } else {
               // Error.
               m_listener->bytesReceived(
                     requestID.getID(), //id
                     900,               //very invalid http-request status. 
                     NULL,              //data
                     -1,                //startoffset,
                     -1,                //nbrBytes 
                     -1);               //contentLength
            }
         }
      } break;
      default:
         if (status.getStatusCode() == OK) {
            nav2log << "MRIImpl::receiveAsynchronousReply with OK status of"
               "type: "<< (int) type << endl;
         } else {
            coreprintln_error("Received asynchronous reply with error: %s\n",
                              status.getStatusMessage().c_str());
            if(m_networkInterface) {
               m_networkInterface->reportNetworkError(status);
            }
         }
         break;
   }
}

int 
MapRequesterInterfaceImpl::requestOne(const char* descr)
{
   isab::GenericGuiMess request(isab::GuiProtEnums::GET_VECTOR_MAP, descr);
   
   AsynchronousStatus status = sendAsynchronousRequestToNav2(&request);
   
   request.deleteMembers();
   
   if (status.getStatusCode() == OK) {
      return status.getRequestID().getID();
   } else {
      return -1;
   }
}

int 
MapRequesterInterfaceImpl::requestMany(const uint8* buf, int nbrBytes)
{
   nav2log << "MRIImpl::requestMany(" << (char*) buf 
             << ")" << endl;
   isab::DataGuiMess request(isab::GuiProtEnums::GET_MULTI_VECTOR_MAP, 
                             nbrBytes, buf);
   
   AsynchronousStatus status = sendAsynchronousRequestToNav2(&request);
   
   request.deleteMembers();
   
   if (status.getStatusCode() == OK) {
      return status.getRequestID().getID();
   } else {
      return -1;
   }
}

void 
MapRequesterInterfaceImpl::setListener(HttpClientConnectionListener* listener)
{
   m_listener = listener;
}

void MapRequesterInterfaceImpl::setNetworkInterfaceImpl(NetworkInterfaceImpl* netInterface)
{
   m_networkInterface = netInterface;
}

} // End namespace WFAPI

