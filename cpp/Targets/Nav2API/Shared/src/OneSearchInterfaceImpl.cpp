/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

//#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE
#define CORE_LOGGING_MODULE_NAME "SEARCH_IMPL"

#include "CoreDebugPrint.h"

#include "OneSearchInterfaceImpl.h"
#include "MC2Coordinate.h"
#include "Nav2APIImpl.h"
#include "SearchListener.h"
// Std
#include <math.h> // rint
#include <string>
// Nav2
#include "OneSearchDispatcher.h"
#include "SearchStatusCode.h"
// #include "LanguageHelper.h"
#include "GuiProtMess.h"
#include "GuiProtEnums.h"
#include "HelperEnums.h"
#include "GuiProtMessageSender.h"
#include "NavRequestType.h"
#include "GuiParameterEnums.h"
#include "OneSearchQuery.h"
#include "InternalOneSearchQuery.h"

#include <set>
#include <map>

namespace WFAPI {

using namespace std;

typedef std::map<wf_uint32, GuiProtMessageReceiver*> MessageSenderMap;

/// Typedef for a set of listeners.
typedef std::set<SearchListener*> ListenerCont;
   

struct OneSearchInterfaceImpl::Impl {

   /// Message linking together a request id against a message sender.   
   MessageSenderMap m_messageReceivers;

   /**
    * Category tree dispatcher that gets a category tree and parses results.
    */
   OneSearchDispatcher* m_osDispatcher;

   /// Set of listeners.
   ListenerCont m_listeners;

   /// The path to the category images folder.
   WFString m_imagePath;
   
   /// The extension for the images to download.
   WFString m_imageExt;

   /**
    * Current search record. Contains the search data for the last
    * requested search.
    */
   OneSearch::InternalOneSearchQuery m_searchRecord;
};

OneSearchInterfaceImpl::OneSearchInterfaceImpl(Nav2APIImpl* api) :
   InterfaceImpl(api),
   m_requestID(RequestID::INVALID_REQUEST_ID)
{
   m_impl = new Impl;
   m_impl->m_imagePath = "";
   m_impl->m_imageExt = "";

   m_impl->m_osDispatcher = new OneSearchDispatcher(*this,
                                                    m_impl->m_imagePath.c_str(),
                                                    m_impl->m_imageExt.c_str());
   
}

OneSearchInterfaceImpl::~OneSearchInterfaceImpl()
{
   delete m_impl->m_osDispatcher;
   // TODO: should we clear members of type WFSring in destructor
   delete m_impl;
}

void
OneSearchInterfaceImpl::receiveAsynchronousReply(
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
      reportError(status, m_impl->m_listeners.begin(), m_impl->m_listeners.end());
      return;
   }

   // Check if GuiProtMessageReceiver is registred for receiving this
   // message.
   MessageSenderMap::iterator it =
      m_impl->m_messageReceivers.find(requestID.getID());
   if (it != m_impl->m_messageReceivers.end()) {
      // Found a registred receiver, forward the message to it.
      coreprintln_info(
         "Found the registred receiver, forward the messge to it");
      if (it->second != NULL) {
         it->second->GuiProtReceiveMessage(
            const_cast<isab::GuiProtMess*>(&guiProtMess));
      } else {
         coreprintln_warn("The found receiver was NULL so we can not send");
      }
      m_impl->m_messageReceivers.erase(it);
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
      case isab::navRequestType::NAV_ONE_SEARCH_REPLY:
      {
         // Got category tree results
         coreprintln_info("Got NAV_ONE_SEARCH_REPLY");
         std::vector<CSOneSearchMatch> searchMatches;
         m_impl->m_osDispatcher->handleSearchResult(mess, &searchMatches);
         coreprintln("Number of hits: %d", searchMatches.size());
         
         // Temporary testing
         SearchHeadingArray shArray;
         for (ListenerCont::iterator it = m_impl->m_listeners.begin();
              it != m_impl->m_listeners.end();
              ++it) {
            // TODO: new function in SearchListener for one search reply
            // (*it)->categoryTreeReceived(m_impl->m_requestID, treeUpdated);
         }
         break;
      }
      default:
         break;
      }
      break;
   }
   default:
      // Do nothing
      break;
   }
}

void
OneSearchInterfaceImpl::sendMessage(isab::GuiProtMess& message)
{
   AsynchronousStatus status(m_requestID, OK, "", "");
   if (/*m_searchState == SEARCH_REQUEST_OUTSTANDING &&*/
       m_requestID.getID() != RequestID::INVALID_REQUEST_ID) {
      // User has requested to do a search and this is not the first
      // message, we should send the m_requestID as a parameter to
      // the send request. By doing that, we will get the same request id
      // for both requests.
      status = sendAsynchronousRequestToNav2(&message, &m_requestID);
   } else {
      // User has requested to do a search and this is the first message
      // that should be sent, round 0. Or, this is an area match search.
      // Either way, we should send the message without m_requestID
      // as parameter
      status = sendAsynchronousRequestToNav2(&message);
   }
   if (status.getStatusCode() != OK) {
      // Notify the user through the error function.
      coreprintln_warn("OneSearchInterfaceImpl::sendMessage, status not OK");
      reportError(AsynchronousStatus(status.getRequestID(),
                                     UNABLE_TO_MAKE_SEARCH_REQUEST,
                                     "",""),
                  m_impl->m_listeners.begin(),
                  m_impl->m_listeners.end());
      return;
   }
   m_requestID = status.getRequestID();
   coreprintln_info("OneSearchInterfaceImpl::sendMessage");
}

void
OneSearchInterfaceImpl::sendMessage(isab::GuiProtMess& message,
                                       GuiProtMessageReceiver* messageReceiver)
{
   coreprintln_info(
      "OneSearchInterfaceImpl::sendMessage with receiver, pre send");

   AsynchronousStatus status = sendAsynchronousRequestToNav2(&message);
   coreprintln_info("OneSearchInterfaceImpl::sendMessage with receiver"
                    ", post send. Status = %d", status.getStatusCode());
   if (status.getStatusCode() == OK) {
      // Both the CombinedSearchDispatcher and the TmapImageHandler uses
      // this function to send messages, we need to store the request id
      // and the receiver so that we know who to forward the reply to later.
      m_impl->m_messageReceivers.insert(make_pair(status.getRequestID().getID(),
                                                  messageReceiver));
   } else {
      coreprintln_warn(
         "OneSearchInterfaceImpl::sendMessage with receiver, status not OK");

      reportError(AsynchronousStatus(status.getRequestID(),
                                     UNABLE_TO_MAKE_SEARCH_REQUEST,
                                     "",""),
                  m_impl->m_listeners.begin(),
                  m_impl->m_listeners.end());
      return;
   }
}

void OneSearchInterfaceImpl::sendMessage(const isab::NParamBlock& params,
                                            uint16 navRequestType)
{
   AsynchronousStatus status(m_requestID, OK, "", "");
   status = sendAsynchronousNGPRequestToNav2(params, navRequestType);
   if (status.getStatusCode() != OK) {
      // Notify the user through the error function.
      coreprintln_warn("OneSearchInterfaceImpl::sendMessage, status not OK");
      reportError(AsynchronousStatus(status.getRequestID(),
                                     UNABLE_TO_MAKE_SEARCH_REQUEST,
                                     "",""),
                  m_impl->m_listeners.begin(),
                  m_impl->m_listeners.end());
      return;
   }
   m_requestID = status.getRequestID();
   coreprintln_info("OneSearchInterfaceImpl::sendMessage");
}

void
OneSearchInterfaceImpl::addSearchListener(SearchListener* listener)
{
   m_impl->m_listeners.insert(listener);
}

void
OneSearchInterfaceImpl::removeSearchListener(SearchListener* listener)
{
   m_impl->m_listeners.erase(listener);
}

void
OneSearchInterfaceImpl::setImagePath(const WFString& imagePath)
{
   m_impl->m_imagePath = imagePath;
   m_impl->m_osDispatcher->setImagePath(m_impl->m_imagePath.c_str());
}

void
OneSearchInterfaceImpl::setImageExt(const WFString& imageExt)
{
   // We want '.' first in our image extension, at least CS want it.
   m_impl->m_imageExt = (std::string(".") + imageExt.c_str()).c_str();
   m_impl->m_osDispatcher->setImageExt(m_impl->m_imageExt.c_str());
}

void
OneSearchInterfaceImpl::setImageDimension(
   CombinedSearch::CSImageDimension imageDimension)
{
   m_impl->m_osDispatcher->setImageDimension(imageDimension);
}

AsynchronousStatus
OneSearchInterfaceImpl::sendSearchRequest(const OneSearchQuery& searchQuery)
{
   MC2Coordinate mc2Coord(searchQuery.getPosition());

   using namespace OneSearch;
   InternalOneSearchQuery sr(searchQuery.getWhat().c_str(),
                             searchQuery.getWhere().c_str(),
                             searchQuery.getCategoryIDs(),
                             mc2Coord.lat,
                             mc2Coord.lon,
                             searchQuery.getRadius(),
                             searchQuery.getTopRegionID(),
                             searchQuery.getSearchType(),
                             searchQuery.getMaxHits(),
                             searchQuery.getRound(),
                             searchQuery.getSearchSorting(),
                             searchQuery.getIncludeInfoFields());

   m_impl->m_searchRecord = sr;
   m_requestID = RequestID(RequestID::INVALID_REQUEST_ID);
   m_impl->m_osDispatcher->sendSearchRequest(m_impl->m_searchRecord);
   if (m_requestID.getID() == RequestID::INVALID_REQUEST_ID) {
      return AsynchronousStatus(m_requestID,
                                UNABLE_TO_GET_CATEGORY_TREE,
                                "", "");
   } else {
      return createOKAsynchStatus(m_requestID);
   } 
}

} // End namespace WFAPI
