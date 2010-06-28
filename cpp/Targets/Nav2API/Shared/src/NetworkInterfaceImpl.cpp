/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#define CORE_LOGGING_MODULE_NAME "NetworkInterfaceImpl"
#include "NetworkInterfaceImpl.h" 
#include "NetworkListener.h"
#include "Nav2APIImpl.h"

#include "GuiProtMess.h"
#include "GuiParameterEnums.h"
#include "IapData.h"
#include "NavRequestType.h"
#include "NParamBlock.h"
#include "NavServerComProtoVer.h"
#include "Nav2Logging.h"
#include "nav2util.h"
#include "ConnectionManager.h"

#include <stdlib.h>

using namespace std;

namespace WFAPI {

NetworkInterfaceImpl::NetworkInterfaceImpl(Nav2APIImpl* api)
   : InterfaceImpl(api),
     m_connMgr(NULL)
{

}

NetworkInterfaceImpl::~NetworkInterfaceImpl()
{
   delete m_connMgr;
}

void
NetworkInterfaceImpl::reportNetworkError(AsynchronousStatus status)
{
   reportError(status, m_listeners.begin(), m_listeners.end());
}

void 
NetworkInterfaceImpl::receiveAsynchronousReply(
   const isab::GuiProtMess& guiProtMess,
   RequestID requestID)
{
   // Enough with all the isab prefixing..
   using namespace isab;

   // Get the status.
   AsynchronousStatus status = createAsynchStatus(&guiProtMess, requestID);
   
   // Check status.
   if (status.getStatusCode() != OK) {
      // Bad status. Report error function for all listeners.
      reportError(status, m_listeners.begin(), m_listeners.end());
      return;
   }
   
   isab::GuiProtEnums::MessageType type = guiProtMess.getMessageType();
   switch (type) {
      case isab::GuiProtEnums::GUI_TO_NGP_DATA_REPLY:
      {
         const isab::DataGuiMess* mess = 
            static_cast<const isab::DataGuiMess*>(&guiProtMess);
         int32 type = -1;
         isab::NonOwningBuffer mess_type_data(
            const_cast<uint8*>(mess->getData()), mess->getSize());
         if (mess_type_data.remaining() >= 2) {
            type = mess_type_data.readNextUnaligned16bit();
         }

         switch (type) {
            case navRequestType::NAV_NOP_REPLY: 
            {
               // NOP
               for (ListenerCont::iterator it = m_listeners.begin();
                    it != m_listeners.end(); ++it ) {
                  (*it)->testServerConnectionReply(requestID);
               }
               break;
            }
         }

         break;
      }
      default:
         nav2log << "NetworkInterfaceImpl::receiveAsynchronousReply of "
                 << "unknown type: " << int(type) << endl;
         break;
   }
}


void
NetworkInterfaceImpl::addNetworkListener(NetworkListener* listener)
{
   m_listeners.insert(listener);
}

SynchronousStatus
NetworkInterfaceImpl::clearNetworkLinkID()
{
   isab::GeneralParameterMess iapId1(
      isab::GuiProtEnums::paramSelectedAccessPointId,
      int32(NetworkLinkID::ALWAYS_ASK));

   isab::GuiProtMess* res = sendSynchronousRequestToNav2(&iapId1);
   
   // Delete members from setgpm.
   iapId1.deleteMembers();

   // Create the AsynchronousStatus based on the message content.
   AsynchronousStatus status = createAsynchStatus( 
      res, RequestID(RequestID::INVALID_REQUEST_ID));
   
   // Delete message including members.
   res->deleteMembers();
   delete res;

   char* tmpStr = new char[2];
   sprintf( tmpStr, "%"PRIi32"", NetworkLinkID::ALWAYS_ASK);

   isab::IapData* iapData = new isab::IapData;
   iapData->m_iapID = NetworkLinkID::ALWAYS_ASK;
   iapData->m_proxyPort = NetworkLinkID::INVALID_PROXY_PORT;
   iapData->m_proxyHost = isab::strdup_new("");


   isab::GeneralParameterMess iapId2(
      isab::GuiProtEnums::paramSelectedAccessPointId2,
      (void*)iapData, 1);

   res = sendSynchronousRequestToNav2(&iapId2);
   
   // Delete members from setgpm.
   iapId2.deleteMembers();

   // Create the AsynchronousStatus based on the message content.
   status = createAsynchStatus( 
      res, RequestID(RequestID::INVALID_REQUEST_ID));
   
   // Delete message including members.
   res->deleteMembers();
   delete res;

   return status;
}

SynchronousStatus
NetworkInterfaceImpl::getKeepLinkUp(bool& val)
{
   isab::GeneralParameterMess getgpm(
      isab::GuiProtEnums::paramLinkLayerKeepAlive);

   isab::GuiProtMess* res = sendSynchronousRequestToNav2(&getgpm);
   
   // Delete members from getgpm.
   getgpm.deleteMembers();

   // Create the AsynchronousStatus based on the message content.
   AsynchronousStatus status = createAsynchStatus( 
      res, RequestID(RequestID::INVALID_REQUEST_ID));

   if (status.getStatusCode() == OK) {
      if (getgpm.getNumEntries() > 0) {
         val = getgpm.getIntegerData()[0] != 0;
      } else {
         status = AsynchronousStatus(status.getRequestID(),GENERAL_ERROR,
                                     "", "");
      }
   }

   // Delete message including members.
   res->deleteMembers();
   delete res;

   return status;
}

SynchronousStatus
NetworkInterfaceImpl::getNetworkLinkID(NetworkLinkID& networkLinkID)
{
   isab::GeneralParameterMess getgpm(
      isab::GuiProtEnums::paramSelectedAccessPointId2);
   
   isab::GuiProtMess* res = sendSynchronousRequestToNav2(&getgpm);

   const isab::GeneralParameterMess* gpm =
      static_cast<const isab::GeneralParameterMess*>(res);

   // Delete members from getgpm.
   getgpm.deleteMembers();

   // Create the AsynchronousStatus based on the message content.
   AsynchronousStatus status = createAsynchStatus( 
      res, RequestID(RequestID::INVALID_REQUEST_ID));

   if (status.getStatusCode() == OK) {
      if (gpm->getNumEntries() > 0) {
         isab::IapData* iapData = (isab::IapData*)gpm->getVoidData();
         if (iapData != NULL) {
            networkLinkID = NetworkLinkID(iapData->m_iapID,
                                          iapData->m_proxyPort,
                                          iapData->m_proxyHost);
         } else {
            networkLinkID = NetworkLinkID(NetworkLinkID::TRY_DEFAULT);
         }
         // Delete the iapData since it is new:ed by the creator of the 
         // message and is not deleted anywhere else.
         delete iapData;
      } else {
         status = AsynchronousStatus(status.getRequestID(),GENERAL_ERROR,
                                     "", "");
      }
   }

   // Delete message including members.
   res->deleteMembers();
   delete res;

   return status;
}

void
NetworkInterfaceImpl::removeNetworkListener(NetworkListener* listener)
{
   m_listeners.erase(listener);
}

SynchronousStatus
NetworkInterfaceImpl::setKeepLinkUp(bool newVal)
{
   // Send parameter to Nav2.
   isab::GeneralParameterMess setgpm(
      isab::GuiProtEnums::paramLinkLayerKeepAlive,
      int32(newVal));

   isab::GuiProtMess* res = sendSynchronousRequestToNav2(&setgpm);
   
   // Delete members from setgpm.
   setgpm.deleteMembers();

   // Create the AsynchronousStatus based on the message content.
   AsynchronousStatus status = createAsynchStatus( 
      res, RequestID(RequestID::INVALID_REQUEST_ID));
   
   // Delete message including members.
   res->deleteMembers();
   delete res;

   return status;
}

SynchronousStatus
NetworkInterfaceImpl::setNetworkLinkID(const NetworkLinkID& networkLinkID)
{

   // Send parameter to Nav2.
   AsynchronousStatus status = createOKAsynchStatus();

   isab::GeneralParameterMess setgpm(
      isab::GuiProtEnums::paramSelectedAccessPointId,
      int32(networkLinkID.getID()));
   
   isab::GuiProtMess* res = sendSynchronousRequestToNav2(&setgpm);
   
   // Delete members from setgpm.
   setgpm.deleteMembers();

   status = createAsynchStatus( 
      res, RequestID(RequestID::INVALID_REQUEST_ID));
   
   // Delete message including members.
   res->deleteMembers();
   delete res;

   isab::IapData* iapData = new isab::IapData;
   iapData->m_iapID = networkLinkID.getID();
   iapData->m_proxyPort = networkLinkID.getProxyPort();
   iapData->m_proxyHost =
      isab::strdup_new(networkLinkID.getProxyHost().c_str());
   
   isab::GeneralParameterMess setIap2(
      isab::GuiProtEnums::paramSelectedAccessPointId2,
      (void*)iapData, 1);

   res = sendSynchronousRequestToNav2(&setIap2);
   
   // Delete members from setgpm.
   setIap2.deleteMembers();

   status = createAsynchStatus( 
      res, RequestID(RequestID::INVALID_REQUEST_ID));
   
   // Delete message including members.
   res->deleteMembers();
   delete res;   

   return status;
}

AsynchronousStatus
NetworkInterfaceImpl::testServerConnection()
{
   isab::Buffer buf( 10 );
   isab::NParamBlock params;

   isab::Buffer tmpbuf( 128 );

   std::vector<byte> bbuf;

   params.writeParams(bbuf, NSC_PROTO_VER, false /* NO GZIP */);
   tmpbuf.writeNextByteArray( &bbuf.front(), bbuf.size() );
   const uint8* data = tmpbuf.accessRawData();
   uint32 size = tmpbuf.getLength();

   buf.writeNextUnaligned16bit(isab::navRequestType::NAV_NOP_REQ);

   class isab::DataGuiMess mess(isab::GuiProtEnums::GUI_TO_NGP_DATA,
                                buf.getLength(), buf.accessRawData(),
                                size, data);

   AsynchronousStatus status = sendAsynchronousRequestToNav2(&mess);

   // Clean up members.
   mess.deleteMembers();
   return status;
}

void NetworkInterfaceImpl::setConnectionManager(ConnectionManager* connMgr)
{
   m_connMgr = connMgr;
}

ConnectionManager* NetworkInterfaceImpl::getConnectionManager()
{
   return m_connMgr;
}

} // End namespace WFAPI



