/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

// #define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE
#define CORE_LOGGING_MODULE_NAME "SEARCH_IMPL"

#include "CoreDebugPrint.h"

#include "CoreDebugPrint.h"

#include "CategoryTreeInterfaceImpl.h"
#include "MC2Coordinate.h"
#include "Nav2APIImpl.h"
#include "WFStringArray.h"
#include "SearchListener.h"
// Std
#include <math.h> // rint
#include <string>
// Nav2
#include "CategoryTreeDispatcher.h"
#include "SearchStatusCode.h"
#include "LanguageHelper.h"
#include "GuiProtMess.h"
#include "GuiProtEnums.h"
#include "HelperEnums.h"
#include "GuiProtMessageSender.h"
#include "NavRequestType.h"
#include "GuiParameterEnums.h"
#include "GfxUtility.h"
#include "Nav2Logging.h"
//#include "CategoryTree.h"
#include "CategoryTreeElem.h"
#include "CategoryTreeElement.h"
#include "CategoryTreeArray.h"

using namespace CategoryTreeImpl;

namespace WFAPI {

using namespace std;

CategoryTreeInterfaceImpl::CategoryTreeInterfaceImpl(Nav2APIImpl* api) :
   InterfaceImpl(api),
   m_categoryImagePath(),
   m_imageExt(),
   m_requestID(RequestID::INVALID_REQUEST_ID),
   m_categoryCrc(-1)
{
   m_ctDispatcher = new CategoryTreeDispatcher(*this,
                                               m_categoryImagePath.c_str(),
                                               m_imageExt.c_str());
   
}

CategoryTreeInterfaceImpl::~CategoryTreeInterfaceImpl()
{
   delete m_ctDispatcher;
}

void
CategoryTreeInterfaceImpl::receiveAsynchronousReply(
   const isab::GuiProtMess& guiProtMess,
   RequestID requestID)
{
   // Enough with all the isab prefixing..
   using namespace isab;

   // Get the status.
   AsynchronousStatus status = createAsynchStatus(&guiProtMess, requestID);
   
   // Check status.
   if (status.getStatusCode() != OK) {
      // We got an error for a ct request, notify the dispatcher.
      //m_ctDispatcher->searchRequestFailed();

      // Bad status. Report error function for all listeners. 
      reportError(status, m_listeners.begin(), m_listeners.end());
      return;
   }

   // Check if GuiProtMessageReceiver is registred for receiving this
   // message.
   MessageSenderMap::iterator it =
      m_messageReceivers.find(requestID.getID());
   if (it != m_messageReceivers.end()) {
      // Found a registred receiver, forward the message to it.
      nav2log << "Found the registred receiver, forward the messge to it"
              << endl;
      if (it->second != NULL) {
         it->second->GuiProtReceiveMessage(
            const_cast<isab::GuiProtMess*>(&guiProtMess));
      } else {
         nav2log << "The found receiver was NULL so we can not send" << endl;
      }
      m_messageReceivers.erase(it);
      return;
   }
   
   isab::GuiProtEnums::MessageType type = guiProtMess.getMessageType();
   
   switch (type) {
   case isab::GuiProtEnums::GUI_TO_NGP_DATA_REPLY: {
      const isab::DataGuiMess& mess =
         static_cast<const isab::DataGuiMess&>(guiProtMess);
      
      int32 type = -1;
      NonOwningBuffer dataMessageType(
         const_cast<uint8 *>(mess.getData()), mess.getSize());
         
      if (dataMessageType.remaining() >= 2) {
         type = dataMessageType.readNextUnaligned16bit();
      }
      switch (type) {
      case isab::navRequestType::NAV_LOCAL_CATEGORY_TREE_REPLY:
      {
         // Got category tree results
         nav2log << "CTImpl, Got NAV_LOCAL_CATEGORY_TREE_REPLY" << endl;
         bool treeUpdated =
            m_ctDispatcher->handleCategoryTreeGot(mess.getAdditionalData(),
                                                  mess.getAdditionalSize());
         if (treeUpdated) {
            // Category tree updated so store the new binary blob of
            // category tree in param file.
            SynchronousStatus syncStatus =
               storeCachedCategoryTree(mess.getAdditionalData(),
                                       mess.getAdditionalSize());
         }
         
         for (ListenerCont::iterator it = m_listeners.begin();
              it != m_listeners.end();
              ++it) {
            (*it)->categoryTreeReceived(m_requestID, treeUpdated);
         }
         break;
      }
      default:
         break;
      }
      break;
   }
   case isab::GuiProtEnums::PARAMETERS_SYNC_REPLY:
      break;
   case isab::GuiProtEnums::SET_GENERAL_PARAMETER: {
      const isab::GeneralParameterMess& gpm =
         static_cast<const isab::GeneralParameterMess&>(guiProtMess);
      if (gpm.getParamId() == isab::GuiProtEnums::paramSearchStrings) {
         // Category tree or CRC was saved in the parameter file.
         //handleSetSearchHistory(gpm);
      }
      break;
   }
   default:
      // Do nothing
      break;
   }
}

void
CategoryTreeInterfaceImpl::sendMessage(isab::GuiProtMess& message)
{
   AsynchronousStatus status(m_requestID, OK, "", "");
   status = sendAsynchronousRequestToNav2(&message);
   if (status.getStatusCode() != OK) {
      // Notify the user through the error function.
      nav2log << "CategoryTreeInterfaceImpl::sendMessage, status not OK" << endl;
      reportError(AsynchronousStatus(status.getRequestID(),
                                     UNABLE_TO_MAKE_SEARCH_REQUEST,
                                     "",""),
                  m_listeners.begin(),
                  m_listeners.end());
      return;
   }
   m_requestID = status.getRequestID();
   nav2log << "CategoryTreeInterfaceImpl::sendMessage" << endl;
}

void
CategoryTreeInterfaceImpl::sendMessage(isab::GuiProtMess& message,
                                       GuiProtMessageReceiver* messageReceiver)
{
   nav2log << "CategoryTreeInterfaceImpl::sendMessage with receiver, pre send"
           << endl; 
   AsynchronousStatus status = sendAsynchronousRequestToNav2(&message);
   nav2log << "CategoryTreeInterfaceImpl::sendMessage with receiver, post send. Status = "
           << status.getStatusCode() << endl; 
   if (status.getStatusCode() == OK) {
      // Both the CombinedSearchDispatcher and the TmapImageHandler uses
      // this function to send messages, we need to store the request id
      // and the receiver so that we know who to forward the reply to later.
      m_messageReceivers.insert(make_pair(status.getRequestID().getID(),
                                          messageReceiver));
   } else {
      nav2log << "CategoryTreeInterfaceImpl::sendMessage with receiver, status not OK"
              << endl;
      reportError(AsynchronousStatus(status.getRequestID(),
                                     UNABLE_TO_MAKE_SEARCH_REQUEST,
                                     "",""),
                  m_listeners.begin(),
                  m_listeners.end());
      return;
   }
}

void CategoryTreeInterfaceImpl::sendMessage(const isab::NParamBlock& params,
                                            uint16 navRequestType)
{
   AsynchronousStatus status(m_requestID, OK, "", "");
   status = sendAsynchronousNGPRequestToNav2(params, navRequestType);
   if (status.getStatusCode() != OK) {
      // Notify the user through the error function.
      nav2log << "CategoryTreeInterfaceImpl::sendMessage, status not OK" << endl;
      reportError(AsynchronousStatus(status.getRequestID(),
                                     UNABLE_TO_MAKE_SEARCH_REQUEST,
                                     "",""),
                  m_listeners.begin(),
                  m_listeners.end());
      return;
   }
   m_requestID = status.getRequestID();
   nav2log << "CategoryTreeInterfaceImpl::sendMessage" << endl;
}

void
CategoryTreeInterfaceImpl::addSearchListener(SearchListener* listener)
{
   m_listeners.insert(listener);
}

void
CategoryTreeInterfaceImpl::removeSearchListener(SearchListener* listener)
{
   m_listeners.erase(listener);
}

void
CategoryTreeInterfaceImpl::setCategoryImagePath(const WFString& catImagePath)
{
   m_categoryImagePath = catImagePath;
   m_ctDispatcher->setCategoryImagePath(m_categoryImagePath.c_str());
}

void
CategoryTreeInterfaceImpl::setImageExt(const WFString& imageExt)
{
   // We want '.' first in our image extension, at least CS want it.
   m_imageExt = (std::string(".") + imageExt.c_str()).c_str();
   m_ctDispatcher->setImageExt(m_imageExt.c_str());
}

void
CategoryTreeInterfaceImpl::setImageDimension(
   CombinedSearch::CSImageDimension imageDimension)
{
   m_ctDispatcher->setImageDimension(imageDimension);
}

AsynchronousStatus
CategoryTreeInterfaceImpl::requestCategoryTree(const WGS84Coordinate& coord)
{
   // First get the cached category tree from param files, since the CRC in 
   // params might show that there is no other category tree on server.
   SynchronousStatus status = getCachedCategoryTree();
   
   m_requestID = RequestID(RequestID::INVALID_REQUEST_ID);
   Nav2Coordinate nav2Coord(coord);
   isab::NavServerComEnums::languageCode langCode =
      WFAPI::LanguageHelper::textLanguageToNav2(
         getApi()->getSessionData()->getTextLanguage());
   m_ctDispatcher->getCategoryTree(langCode,
                                   nav2Coord.nav2lat, nav2Coord.nav2lon);
   if (m_requestID.getID() == RequestID::INVALID_REQUEST_ID) {
      return AsynchronousStatus(m_requestID,
                                UNABLE_TO_GET_CATEGORY_TREE,
                                "", "");
   } else {
      return createOKAsynchStatus(m_requestID);
   } 
}

SynchronousStatus
CategoryTreeInterfaceImpl::getRootCategories(
   CategoryTreeArray& rootCategoryList)
{
   // Simply get the cached root categories.
   CTCatElemVec_t rootList;
   m_ctDispatcher->getRootCategories(rootList);
   if (rootList.empty()) {
      // No category tree in memory, so try to request last
      // cached category tree from params.
      SynchronousStatus status = getCachedCategoryTree();
      // Try to get root categories again, if it is still empty we dont have
      // any cached category tree so we return empty to the caller.
      m_ctDispatcher->getRootCategories(rootList);
   }
   
   for (CTCatElemVec_t::const_iterator it = rootList.begin();
        it != rootList.end();
        ++it) {
      rootCategoryList.push_back(CategoryTreeElement(
                                    (*it)->getId(),
                                    (*it)->getName(),
                                    (*it)->getImageName()));
   }
   return createOKAsynchStatus();
}

SynchronousStatus
CategoryTreeInterfaceImpl::getSubCategoriesOf(
   wf_int32 categoryId,
   CategoryTreeArray& subCategoryList)
{
   // To see if we have an in memory category tree.
   CTCatElemVec_t rootList;
   m_ctDispatcher->getRootCategories(rootList);
   if (rootList.empty()) {
      // No category tree in memory, so try to request last
      // cached category tree from params.
      SynchronousStatus status = getCachedCategoryTree();
   }
   
   // Simply get the cached sub categories.
   CTCatElemVec_t categoryList;
   m_ctDispatcher->getSubCategoriesOf(categoryId, categoryList);
   
   for (CTCatElemVec_t::const_iterator it = categoryList.begin();
        it != categoryList.end();
        ++it) {
      subCategoryList.push_back(CategoryTreeElement(
                                   (*it)->getId(),
                                   (*it)->getName(),
                                   (*it)->getImageName()));
   }
   return createOKAsynchStatus();
}

SynchronousStatus
CategoryTreeInterfaceImpl::storeCachedCategoryTree(const uint8* data,
                                                   uint32 size)
{
   // Store category tree in params.
   isab::GeneralParameterMess
      gpm(uint16(isab::GuiProtEnums::paramCategoryTree),
          const_cast<uint8*>(data), size);
            
   isab::GuiProtMess* reply = sendSynchronousRequestToNav2(&gpm);

   // This could be made asynch...
   //AsynchronousStatus cacheStatus = sendAsynchronousRequestToNav2(&gpm);

   // Check status of reply.
   AsynchronousStatus status = createAsynchStatus(
      reply, RequestID(RequestID::INVALID_REQUEST_ID));
            
   if (status.getStatusCode() != OK) {
      // Bad status.
      gpm.deleteMembers();
      reply->deleteMembers();
      delete reply;
      return status;
   }

   // Delete members.
   reply->deleteMembers();
   delete reply;
   
   return createOKAsynchStatus();
}

SynchronousStatus
CategoryTreeInterfaceImpl::getCachedCategoryTree()
{
   // Get category tree from params.
   isab::GeneralParameterMess mess(isab::GuiProtEnums::paramCategoryTree);
   isab::GuiProtMess* reply = sendSynchronousRequestToNav2(&mess);

   // Check status of reply.
   AsynchronousStatus status = createAsynchStatus(
      reply, RequestID(RequestID::INVALID_REQUEST_ID));

   if (status.getStatusCode() != OK) {
      // Bad status.
      mess.deleteMembers();
      reply->deleteMembers();
      delete reply;
      return status;
   }

   isab::GeneralParameterMess* gpm =
      static_cast<isab::GeneralParameterMess*>(reply);

   if (gpm->getBinaryData() != NULL && gpm->getNumEntries() > 0) {
      m_ctDispatcher->handleCategoryTreeGot(gpm->getBinaryData(),
                                            gpm->getNumEntries());
   }
   
   // Delete members.
   mess.deleteMembers();
   reply->deleteMembers();
   delete reply;

   return createOKAsynchStatus();
}

} // End namespace WFAPI
