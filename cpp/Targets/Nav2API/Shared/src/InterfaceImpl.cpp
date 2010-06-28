/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "InterfaceImpl.h"
#include "WFAPISync.h"
#include "RequestID.h"
#include "Nav2APIImpl.h"
// Nav2
#include "Monitor.h"
#include "Buffer.h"
#include "NParamBlock.h"
#include "NavServerComProtoVer.h"
// GuiProt
#include "GuiProtMess.h"
#include "GuiProtEnums.h"

namespace WFAPI {

InterfaceImpl::InterfaceImpl(Nav2APIImpl* api)
   :  m_monitor(new isab::Monitor()), m_replyData(NULL), m_api(api)
{
}

InterfaceImpl::~InterfaceImpl()
{
   delete m_monitor;
}

isab::GuiProtMess*
InterfaceImpl::sendSynchronousRequestToNav2(isab::GuiProtMess* guiProtMess,
                                            RequestID* requestID)
{
   WFAPISync sync(m_monitor);
   RequestID req = m_api->nav2Request(guiProtMess, this, true, requestID);

   m_monitor->wait();
   // Here receiveSynchronousReply is called, eventually
   isab::GuiProtMess* res = m_replyData;
   m_replyData = NULL;

   return res;
}

AsynchronousStatus
InterfaceImpl::sendAsynchronousRequestToNav2(isab::GuiProtMess* guiProtMess,
                                             RequestID* requestID)
{
   WFAPISync sync(m_monitor);
   RequestID req = m_api->nav2Request(guiProtMess, this, false, requestID);

   AsynchronousStatus status(req, OK, "", "");
   return status;
}

AsynchronousStatus
InterfaceImpl::sendAsynchronousNGPRequestToNav2(
   const isab::NParamBlock& params,
   wf_uint16 navRequestType,
   RequestID* requestID)
{
   isab::Buffer buf(10);

   isab::Buffer tmpbuf(128);

   std::vector<byte> bbuf;

   params.writeParams(bbuf, NSC_PROTO_VER, false /* NO GZIP */);
   tmpbuf.writeNextByteArray(&bbuf.front(), bbuf.size());
   const uint8* data = tmpbuf.accessRawData();
   uint32 size = tmpbuf.getLength();

   buf.writeNextUnaligned16bit(navRequestType);

   isab::DataGuiMess mess(isab::GuiProtEnums::GUI_TO_NGP_DATA,
                          buf.getLength(), buf.accessRawData(),
                          size, data, 2 );

   AsynchronousStatus status = sendAsynchronousRequestToNav2( 
      &mess, requestID);

   mess.deleteMembers();

   return status;
}

void
InterfaceImpl::receiveSynchronousReply(isab::GuiProtMess* guiProtMess,
                                       RequestID requestID)
{
   WFAPISync sync(m_monitor);
   m_replyData = guiProtMess;

   m_monitor->notify();
}

AsynchronousStatus 
InterfaceImpl::createOKAsynchStatus(RequestID requestID)
{
   return AsynchronousStatus(requestID, OK, "", "");
}

AsynchronousStatus 
InterfaceImpl::createAsynchStatus(const isab::GuiProtMess* message,
                                  RequestID requestID)
{
   isab::GuiProtEnums::MessageType type = message->getMessageType();
   switch( type ) {
      case isab::GuiProtEnums::MESSAGETYPE_ERROR:
      {
         const isab::ErrorMess* errMess = 
            static_cast<const isab::ErrorMess*>(message);
         return AsynchronousStatus(
               requestID,
               m_api->nav2ErrorNumberToStatusCode(errMess->getErrorNumber()),
               errMess->getErrorString(),
               errMess->getErrorURL(),
               errMess->getErrorData());
      }
      
      case isab::GuiProtEnums::REQUEST_FAILED:
      {
         const isab::RequestFailedMess* rf = 
            static_cast<const isab::RequestFailedMess*>(message);
         return AsynchronousStatus(
               requestID,
               m_api->nav2ErrorNumberToStatusCode(rf->getErrorNbr()),
               rf->getErrorString(),
               rf->getErrorURL(),
               rf->getErrorData());

      }
      default:
         // Send OK status.
         return AsynchronousStatus(
            requestID, OK, "", "");
   }
}

Nav2APIImpl*
InterfaceImpl::getApi()
{
   return m_api;
}

} // End namespace WFAPI
