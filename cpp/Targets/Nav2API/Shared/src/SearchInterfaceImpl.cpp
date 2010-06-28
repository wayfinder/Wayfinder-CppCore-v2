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
#include "SearchStatusCode.h"
#include "SearchInterfaceImpl.h"
#include "FavouriteInterfaceImpl.h"
#include "MC2Coordinate.h"
#include "ItemInfoEntryArray.h"
#include "SearchItemSubTypeEnum.h"
#include "SearchHeading.h"
#include "SearchArea.h"
#include "SearchAreaArray.h"
#include "SearchAreaTypeEnum.h"
#include "SearchIDArray.h"
#include "Nav2APIImpl.h"
#include "SearchRegion.h"
#include "TopRegionArray.h"
#include "SearchStatusCode.h"
#include "SearchCategoryArray.h"
#include "WFStringArray.h"
#include "SearchHistoryArray.h"
#include "SearchDescArray.h"
#include "SearchQuery.h"
#include "SearchHeadingArray.h"
#include "SearchListener.h"

// Std
#include <math.h> // rint
#include <string>

// Nav2
//#include "GuiProt/Nav2SearchItem.h"
#include "CombinedSearchDataHolder.h"
#include "CombinedSearchDataTypes.h"
#include "CombinedSearchDispatcher.h"
#include "GuiProtMess.h"
#include "GuiProtEnums.h"
#include "HelperEnums.h"
#include "GuiProtMessageSender.h"
#include "GuiProtSearchMess.h"
#include "NavRequestType.h"
#include "GuiParameterEnums.h"
#include "GfxUtility.h"
#include "Nav2Logging.h"

namespace WFAPI {

using namespace std;

/*
 * Compare function for integers, returns the smalles number
 */
bool intCmp(int i, int j)
{
   return i < j;
}

SearchInterfaceImpl::SearchInterfaceImpl(Nav2APIImpl* api) :
   InterfaceImpl(api),
   m_imagePath(),
   m_categoryImagePath(),
   m_imageExt(),
   m_requestID(RequestID::INVALID_REQUEST_ID),
   m_outstandingRequests(0),
   m_categoryChecksum(-1)
{
   m_csDispatcher = new CombinedSearchDispatcher(*this,
                                                 *this,
                                                 m_imagePath.c_str(),
                                                 m_categoryImagePath.c_str(),
                                                 m_imageExt.c_str());
   
}

SearchInterfaceImpl::~SearchInterfaceImpl()
{
   delete m_csDispatcher;
}
   
void
SearchInterfaceImpl::receiveAsynchronousReply(
   const isab::GuiProtMess& guiProtMess,
   RequestID requestID)
{
   // Enough with all the isab prefixing..
   using namespace isab;

   // Get the status.
   AsynchronousStatus status = createAsynchStatus(&guiProtMess, requestID);
   
   // Check status.
   if (status.getStatusCode() != OK) {

      // We got an error for a search request, notify the dispatcher
      // and decrease the counter for outstanding requests.
      m_csDispatcher->searchRequestFailed();
      m_outstandingRequests--;

      // Bad status. Report error function for all listeners.

      // If this was related to an outstanding request, remove it so
      // it won't be notified by mistake in later requests  
      reportError(status, m_listeners.begin(), m_listeners.end());
      if (m_outstandingRequests == 0) {
         // Send search reply with results from valid round (the ones that
         // we have received) final flag set to true
         SearchHeadingArray shArray;
         parseSearchResults(shArray);
         for (ListenerCont::iterator it = m_listeners.begin();
              it != m_listeners.end();
              ++it) {
            (*it)->searchReply(m_requestID, shArray, true);
         }
      }
      return;
   }

   // Check if GuiProtMessageReceiver is registred for receiving this
   // message.
   MessageSenderMap::iterator it =
      m_messageReceivers.find(requestID.getID());
   if (it != m_messageReceivers.end()) {
      // Found a registred receiver, forward the message to it.
      nav2log << "Found the registred receiver, forward the messge to it" << endl;
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
      case isab::navRequestType::NAV_COMBINED_SEARCH_REPLY:
         // Got search results
         nav2log << "SIImpl, Got NAV_COMBINED_SEARCH_REPLY" << endl;
         m_csDispatcher->handleCombinedSearchResultsGot(&mess);
         break;
      case isab::navRequestType::NAV_SEARCH_DESC_REPLY:
         // Got search headings, the search dispatcher will
         // handle this
         m_csDispatcher->handleCombinedSearchHeadingsGot(&mess);
         break;
      case isab::navRequestType::NAV_INFO_REPLY:
         // Got search details, forward it to the dispatcher.
         m_csDispatcher->handleAdditionalInfoReply(&mess);
         break;
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
         // Search history was saved in the parameter file.
         handleSetSearchHistory(gpm);
      }
      break;
   }
   default:
      // Do nothing
      break;
   }
}

void
SearchInterfaceImpl::sendMessage(isab::GuiProtMess& message)
{
   AsynchronousStatus status(m_requestID, OK, "", "");
   if (m_searchState == SEARCH_REQUEST_OUTSTANDING &&
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
      nav2log << "SearchInterfaceImpl::sendMessage, status not OK" << endl;
      reportError(AsynchronousStatus(status.getRequestID(),
                                     UNABLE_TO_MAKE_SEARCH_REQUEST,
                                     "",""),
                  m_listeners.begin(),
                  m_listeners.end());
      return;
   }
   m_requestID = status.getRequestID();
   nav2log << "SearchInterfaceImpl::sendMessage, m_outstandingRequests = "
        << m_outstandingRequests << endl;
   m_outstandingRequests++;
}

void
SearchInterfaceImpl::sendMessage(
   isab::GuiProtMess& message,
   GuiProtMessageReceiver* messageReceiver)
{
   nav2log << "SearchInterfaceImpl::sendMessage with receiver, pre send"
           << endl; 
   AsynchronousStatus status = sendAsynchronousRequestToNav2(&message);
   nav2log << "SearchInterfaceImpl::sendMessage with receiver, post send. Status = "
        << status.getStatusCode() << endl; 
   if (status.getStatusCode() == OK) {
      // Both the CombinedSearchDispatcher and the TmapImageHandler uses
      // this function to send messages, we need to store the request id
      // and the receiver so that we know who to forward the reply to later.
      m_messageReceivers.insert(make_pair(status.getRequestID().getID(),
                                          messageReceiver));
   } else {
      nav2log << "SearchInterfaceImpl::sendMessage with receiver, status not OK"
              << endl;
      reportError(AsynchronousStatus(status.getRequestID(),
                                     UNABLE_TO_MAKE_SEARCH_REQUEST,
                                     "",""),
                  m_listeners.begin(),
                  m_listeners.end());
      return;
   }
}

void SearchInterfaceImpl::sendMessage(const isab::NParamBlock& params,
                                            uint16 navRequestType)
{
   // Implement when needed.
}

void
SearchInterfaceImpl::addSearchListener(SearchListener* listener)
{
   m_listeners.insert(listener);
}

void
SearchInterfaceImpl::removeSearchListener(SearchListener* listener)
{
   m_listeners.erase(listener);
}

SynchronousStatus
SearchInterfaceImpl::getSearchHistory(SearchHistoryArray& searchHistoryList)
{
   // Simply get the cached search history from the CombinedSearchDataHolder
   const CSHiDeque_t& historyList = m_csDispatcher->
      getCombinedSearchDataHolder().getSearchHistory();
   
   for (CSHiDeque_t::const_iterator it = historyList.begin();
        it != historyList.end();
        ++it) {
      searchHistoryList.push_back(SearchHistoricInput(
                                     (*it)->GetSearchString(),
                                     (*it)->GetCityString(),
                                     (*it)->GetIntCountryId(),
                                     (*it)->GetCategoryId()));
   }
   return createOKAsynchStatus();
}

SynchronousStatus
SearchInterfaceImpl::getSearchDesc(SearchDescArray& headingList)
{
   CSCatVec_t categoryVec =
      m_csDispatcher->getCombinedSearchCategoryResults();
   for (CSCatVec_t::const_iterator it = categoryVec.begin();
        it != categoryVec.end();
        ++it) {
      CombinedSearchCategory& cat = **it;
      headingList.push_back(SearchDesc(WFString(cat.getName()),
                                       cat.getHeadingNo(),
                                       cat.getTopRegionId(),
                                       WFString(cat.getImageName())));
   }
   return createOKAsynchStatus();
}

SynchronousStatus
SearchInterfaceImpl::getTopRegions(TopRegionArray& topRegionList)
{
   nav2log << "SearchInterfaceImpl::getTopRegions, first line" << endl;
   //Request the favorites lists
   //isab::GenericGuiMess message(isab::GuiProtEnums::GET_TOP_REGION_LIST);
   isab::GeneralParameterMess message(isab::GuiProtEnums::paramTopRegionList);
   RequestID requestID(RequestID::INVALID_REQUEST_ID);
   nav2log << "SearchInterfaceImpl::getTopRegions, pre get tr request" << endl;
   isab::GuiProtMess* returnMess = sendSynchronousRequestToNav2(&message,
                                                                &requestID);
   nav2log << "SearchInterfaceImpl::getTopRegions, post tr request" << endl;
   AsynchronousStatus status = createAsynchStatus(returnMess, requestID);
   if (status.getStatusCode() != OK) {
      returnMess->deleteMembers();
      delete returnMess;
      return AsynchronousStatus(status.getRequestID(),
                                UNABLE_TO_GET_TOP_REGIONS,
                                "","");
   }
   nav2log << "SearchInterfaceImpl::getTopRegions, pre parse return mess" << endl;
   isab::GeneralParameterMess* topRegionMess =
      static_cast<isab::GeneralParameterMess*>(returnMess);
   isab::NonOwningBuffer tmp(
      const_cast<uint8*>(topRegionMess->getBinaryData()),
      topRegionMess->getNumEntries());
   nav2log << "SearchInterfaceImpl::getTopRegions, post parse return mess" 
           << endl;
   tmp.setReadPos(0);
   m_topRegionList.clear();
   isab::TopRegionList trList(tmp);
   for (wf_int32 i = 0; i < trList.size(); ++i) {
      const isab::TopRegion* region = trList.getRegion(i);
      topRegionList.push_back(
         TopRegion(WFString(region->getName()),
                   region->getId(),
                   convertToTopRegionType(region->getType())));

      // Store the top region in our map with the id as the key and the
      // name as the value.
      m_topRegionList.insert(make_pair(region->getId(), region->getName()));
      nav2log << "SearchInterfaceImpl::getTopRegions, adding region: "
           << region->getName() << endl;
   }
   returnMess->deleteMembers();
   delete returnMess;
   nav2log << "SearchInterfaceImpl::getTopRegions, pre return" << endl;
   return status;
}

SynchronousStatus
SearchInterfaceImpl::getSearchCategories(
   SearchCategoryArray& searchCategoryArray)
{
   WFStringArray nameArray;
   SynchronousStatus status = getParam(GuiProtEnums::paramCategoryNames,
                                       nameArray);
   
   if (status.getStatusCode() != OK) {
      nav2log << "Error retreiving category names, statusCode: "
              << status.getStatusCode() << endl;
      return status;
   }

   WFStringArray idArray;
   idArray.reserve(nameArray.size());
   status = getParam(GuiProtEnums::paramCategoryIds,
                     idArray);

   if (status.getStatusCode() != OK) {
      nav2log << "Error retreiving category ids, statusCode: "
              << status.getStatusCode() << endl;
      return status;
   }

   WFStringArray iconArray;
   iconArray.reserve(nameArray.size());
   status = getParam(GuiProtEnums::paramCategoryIcons,
                     iconArray);

   if (status.getStatusCode() != OK) {
      nav2log << "Error retreiving category icons, statusCode: "
              << status.getStatusCode() << endl;
      return status;
   }

   typedef std::vector<wf_int32> intVec;
   intVec intIdVec;
   intIdVec.reserve(nameArray.size());
   status = getParam(GuiProtEnums::paramCategoryIntIds,
                     intIdVec);

   if (status.getStatusCode() != OK) {
      nav2log << "Error retreiving category int ids, statusCode: "
              << status.getStatusCode() << endl;
      return status;
   }

   // Create an array with the different sizes of the category vectors.
   // Most of the times they will probably have the same size, however
   // we need to plan for them not having the same sizes.
   wf_int sizes[] = { nameArray.size(), idArray.size(),
                      iconArray.size(), intIdVec.size() };
   
   size_t arraySize = sizeof(sizes) / sizeof(sizes[0]);

   // Get the smallest number of elements from the size of array.
   wf_int* numElements = std::min_element(sizes, sizes + arraySize, intCmp);

   std::vector<const char*> iconList;
   for (wf_int i = 0; i < *numElements; ++i) {
      const char* iconName = iconArray[i].c_str();
      searchCategoryArray.push_back(SearchCategory(WFString(nameArray[i]),
                                                   WFString(idArray[i]),
                                                   intIdVec[i],
                                                   WFString(iconName)));
      iconList.push_back(iconName);
   }

   m_csDispatcher->getCategoryIcons(iconList);
   return status;
}

AsynchronousStatus
SearchInterfaceImpl::getSearchCategoriesByPosition(
   const WGS84Coordinate& position)
{   
   Nav2Coordinate nav(position);

   ParameterSyncMess* getParameterSyncMess = new
      ParameterSyncMess(nav.nav2lat,nav.nav2lon);
   
   AsynchronousStatus status =
      sendAsynchronousRequestToNav2(getParameterSyncMess);
   
   getParameterSyncMess->deleteMembers();
   delete getParameterSyncMess;
   
   return AsynchronousStatus(RequestID(RequestID::INVALID_REQUEST_ID),
                             status.getStatusCode(),
                             status.getStatusMessage(),
                             status.getStatusURL());
}

AsynchronousStatus
SearchInterfaceImpl::search(const SearchQuery& searchQuery)
{
   if (m_searchState != IDLE) {
      // If this happens there can be an idea to return an AsynchronousStatus
      // with the status set to busy.
   }
   m_searchState = SEARCH_REQUEST_OUTSTANDING;
   

   Nav2Coordinate coord(searchQuery.getPosition());
   
   m_searchRecord = SearchRecord(searchQuery.getTopRegionID(),
                                 searchQuery.getAreaID().c_str(),
                                 searchQuery.getWhere().c_str(),
                                 searchQuery.getWhat().c_str(),
                                 searchQuery.getCategoryID(),
                                 coord.nav2lat,
                                 coord.nav2lon);
   m_searchRecord.setRound(searchQuery.getRound());

   m_requestID = RequestID(RequestID::INVALID_REQUEST_ID);
   m_csDispatcher->combinedSearchStart(m_searchRecord);
   if (m_requestID.getID() == RequestID::INVALID_REQUEST_ID) {
      return AsynchronousStatus(m_requestID,
                                UNABLE_TO_INITIATE_SEARCH,
                                "", "");
   } else {
      handleSaveSearchHistory();
      return createOKAsynchStatus(m_requestID);
   }
}

AsynchronousStatus
SearchInterfaceImpl::requestNextSearchResults(wf_uint32 index,
                                              wf_uint32 maxHits,
                                              wf_uint32 heading)
{
   if (m_searchState != IDLE) {
      // If this happens there can be an idea to return an AsynchronousStatus
      // with the status set to busy.
   }
   m_searchState = SEARCH_REQUEST_OUTSTANDING;
   m_searchRecord.setIndex(index);
   m_searchRecord.setMaxHits(maxHits);
   m_searchRecord.setHeading(heading);   

   nav2log << "SIImpl::requestNextSearchResults with data: " << endl
        << "index = " << index << endl
        << "maxHits = " << maxHits << endl
        << "heading = " << heading << endl;
   m_requestID = RequestID(RequestID::INVALID_REQUEST_ID);
   m_csDispatcher->requestMoreHits(index, heading, maxHits);
   if (m_requestID.getID() == RequestID::INVALID_REQUEST_ID) {
      return AsynchronousStatus(m_requestID,
                                UNABLE_TO_REQUEST_NEXT_SEARCH_RESULTS,
                                "", "");
   } else {
      return createOKAsynchStatus(m_requestID);
   } 
}

AsynchronousStatus
SearchInterfaceImpl::extendedSearch(const WFString& areaID,
                                    const WFString& areaName,
                                    wf_uint32 heading)
{
   m_searchRecord.setAreaId(areaID.c_str());
   m_searchRecord.setAreaName(areaName.c_str());
   m_searchRecord.setHeading(heading);
   m_csDispatcher->dispatchAreaMatchSearch(m_searchRecord, heading);
   return createOKAsynchStatus(m_requestID);
}

AsynchronousStatus
SearchInterfaceImpl::searchDetails(const SearchIDArray& searchIDArray)
{
   CombinedSearchDispatcher::CharVec idVec;
   idVec.reserve(searchIDArray.size());
   for (SearchIDArray::const_iterator it = searchIDArray.begin();
        it != searchIDArray.end();
        ++it) {
      idVec.push_back(it->c_str());
   }
   m_csDispatcher->requestAdditionalInformation(idVec);
   if (m_requestID.getID() == RequestID::INVALID_REQUEST_ID) {
      return AsynchronousStatus(m_requestID,
                                UNABLE_TO_RETRIEVE_SEARCH_DETAILS,
                                "", "");
   } else {
      return createOKAsynchStatus(m_requestID);
   }
}
void SearchInterfaceImpl::SearchHeadingsReceived(){
   nav2log << "SearchInterfaceImpl::SearchHeadingsReceived: " << endl;
   m_outstandingRequests--;
}

void
SearchInterfaceImpl::HeadingImagesStatusUpdated(
   MSearchResultObserver::ImageStatus currentStatus)
{
   WFAPI::SearchListener::ImageStatus curStatus =
      WFAPI::SearchListener::CURRENT_IMAGES_NOT_OK;
   nav2log << "SearchInterfaceImpl::HeadingImagesStatusUpdated: ";
   switch (currentStatus)
   {
      case MSearchResultObserver::CURRENT_IMAGES_OK:
         curStatus = WFAPI::SearchListener::CURRENT_IMAGES_OK;
         nav2log << "images are OK to use.";
         break;
      case MSearchResultObserver::CURRENT_IMAGES_NOT_OK:
         curStatus = WFAPI::SearchListener::CURRENT_IMAGES_NOT_OK;
         nav2log << "images are not OK to use.";
         break;
      case MSearchResultObserver::UPDATED_IMAGES_OK:
         curStatus = WFAPI::SearchListener::UPDATED_IMAGES_OK;
         nav2log << "images are updated and OK to use.";
         break;
   }

   nav2log << endl;
   for (ListenerCont::iterator it = m_listeners.begin();
        it != m_listeners.end();
        ++it) {
      (*it)->headingImagesStatusUpdated(curStatus);
   }   
   
}
void
SearchInterfaceImpl::CategoryImagesStatusUpdated(
   MSearchResultObserver::ImageStatus currentStatus)
{
   WFAPI::SearchListener::ImageStatus curStatus =
      WFAPI::SearchListener::CURRENT_IMAGES_NOT_OK;

   nav2log << "SearchInterfaceImpl::CategoryImagesStatusUpdated: ";
   switch (currentStatus)
   {
      case MSearchResultObserver::CURRENT_IMAGES_OK:
         curStatus = WFAPI::SearchListener::CURRENT_IMAGES_OK;
         nav2log << "images are OK to use.";
         break;
      case MSearchResultObserver::CURRENT_IMAGES_NOT_OK:
         curStatus = WFAPI::SearchListener::CURRENT_IMAGES_NOT_OK;
         nav2log << "images are not OK to use.";
         break;
      case MSearchResultObserver::UPDATED_IMAGES_OK:
         curStatus = WFAPI::SearchListener::UPDATED_IMAGES_OK;
         nav2log << "images are updated and OK to use.";
         break;
   }

   nav2log << endl;
   for (ListenerCont::iterator it = m_listeners.begin();
        it != m_listeners.end();
        ++it) {
      (*it)->categoryImagesStatusUpdated(curStatus);
   }   
   
}


void
SearchInterfaceImpl::ResultImagesStatusUpdated(
   MSearchResultObserver::ImageStatus currentStatus)
{
   WFAPI::SearchListener::ImageStatus curStatus =
      WFAPI::SearchListener::CURRENT_IMAGES_NOT_OK;

   nav2log << "SearchInterfaceImpl::ResultImagesStatusUpdated: ";
   switch (currentStatus)
   {
      case MSearchResultObserver::CURRENT_IMAGES_OK:
         curStatus = WFAPI::SearchListener::CURRENT_IMAGES_OK;
         nav2log << "images are OK to use.";
         break;
      case MSearchResultObserver::CURRENT_IMAGES_NOT_OK:
         curStatus = WFAPI::SearchListener::CURRENT_IMAGES_NOT_OK;
         nav2log << "images are not OK to use.";
         break;
      case MSearchResultObserver::UPDATED_IMAGES_OK:
         curStatus = WFAPI::SearchListener::UPDATED_IMAGES_OK;
         nav2log << "images are updated and OK to use.";
         break;
   }

   nav2log << endl;
   for (ListenerCont::iterator it = m_listeners.begin();
        it != m_listeners.end();
        ++it) {
      (*it)->resultImagesStatusUpdated(curStatus);
   }   
   
}

void
SearchInterfaceImpl::TotalNbrHitsReceived(
   const std::vector<CombinedSearchCategory*>& categories)
{
   nav2log << "SearchInterfaceImpl::TotalNbrHitsReceived: " << endl;

   SearchHeadingArray searchHeadingArray;
   std::vector<CombinedSearchCategory*>::const_iterator it;
   for (it = categories.begin(); it != categories.end(); ++it) {

      searchHeadingArray.push_back(SearchHeading(
                                      convertToSearchHitsType(
                                         (*it)->getTypeOfHits()),
                                      (*it)->getImageName(),
                                      (*it)->getName(),
                                      (*it)->getHeadingNo(),
                                      (*it)->getTotalNbrHits()));
   }

   for (ListenerCont::iterator it = m_listeners.begin();
        it != m_listeners.end();
        ++it) {
      (*it)->totalNbrOfHitsReply(m_requestID, searchHeadingArray);
   }
}

void
SearchInterfaceImpl::SearchResultReceived()
{
   m_outstandingRequests--;
   nav2log << "SearchInterfaceImpl::SearchResultReceived, m_outstandingRequests = "
        << m_outstandingRequests << endl;

   bool final = false;
   if (m_outstandingRequests == 0
       /*m_outstandingRequests == 0*/) {
      // Got the final search reply for the search.
      final = true;
   }

   SearchHeadingArray shArray;
   parseSearchResults(shArray);
   
   for (ListenerCont::iterator it = m_listeners.begin();
        it != m_listeners.end();
        ++it) {
      (*it)->searchReply(m_requestID, shArray, final);
   }
}

void
SearchInterfaceImpl::parseSearchResults(SearchHeadingArray& shArray)
{
   const CSCatVec_t& categories =
      m_csDispatcher->getCombinedSearchResults();

   nav2log << "SearchInterfaceImpl::SearchResultReceived, nbr searches: "
           << categories.size() << endl;
   
   shArray.reserve(categories.size());
   
   for (CSCatVec_t::const_iterator it = categories.begin();
        it != categories.end();
        ++it) {
      CombinedSearchCategory& cat = **it;
      SearchItemArray itemArray;
      SearchAreaArray areaArray;
      if (cat.getTypeOfHits() == 0) {
         // The category contains search results
         // Copy all the search items in the current category
         // to the itemArray.
         convertToSearchItemsArray(cat.getResults(), itemArray);         
      } else {
         // Copy all the area matches from the current category to
         // the areArray
         convertToAreaItemsArray(cat.getResults(), areaArray);
      }
      // Copy all the categories to the search heading array
      shArray.push_back(SearchHeading(
                           convertToSearchHitsType(cat.getTypeOfHits()),
                           itemArray,
                           areaArray,
                           WFString(cat.getImageName()),
                           WFString(cat.getName()),
                           cat.getHeadingNo(),
                           cat.getTopRegionId(),
                           cat.getTotalNbrHits()));
   }
}

void
SearchInterfaceImpl::MoreSearchResultReceived()
{
   m_outstandingRequests--;
   nav2log << "SearchInterfaceImpl::MoreSearchResultReceived" << endl;
   wf_uint32 index = m_searchRecord.m_index;
   wf_uint32 maxHits = m_searchRecord.m_maxHits;

   // Get the CombinedSearchCategory for the heading that was
   // requested.
   const CombinedSearchCategory* cat = m_csDispatcher->
      getCombinedSearchDataHolder().getCombinedSearchResults(
         m_searchRecord.m_heading);
   if (cat == NULL) {
      AsynchronousStatus errorStatus(m_requestID,
                                     UNABLE_TO_RECEIVE_MORE_SEARCH_RESULTS,
                                     "", "");
      for (ListenerCont::iterator it = m_listeners.begin();
           it != m_listeners.end();
           ++it) {
         (*it)->error(errorStatus);
      }
      return;
   }

   // Got the correct heading, now we need to send the new
   // search matches to the listeners. We start from the
   // index that was sent to us in the request and try to get
   // maxHits number of hits.
   const CSSIVec_t& searchItemVec = cat->getResults();
   SearchItemArray itemArray;
   for (wf_uint32 i = index; i < index + maxHits; ++i) {
      if (i < searchItemVec.size()) {
         isab::SearchItem item = searchItemVec[i];
         Nav2Coordinate itemPos(item.getLat(),
                                item.getLon());
         itemArray.push_back(
            SearchItem(item.getName(),
                       item.getID(),
                       convertToSearchItemType(item.getType()),
                       SearchItemSubType(item.getSubType()),
                       calculateDistance(getApi()->getCurrentPosition(),
                                         itemPos),
                       parseSearchRegions(item.getRegionList()),
                       itemPos,
                       item.getImageName(), item.getAdvert(),
                       ItemInfoArray(),
                       calculateDistance(Nav2Coordinate(
                                            m_searchRecord.m_origLat,
                                            m_searchRecord.m_origLon),
                                         itemPos)));
      }
   }
   for (ListenerCont::iterator it = m_listeners.begin();
        it != m_listeners.end();
        ++it) {
      (*it)->nextSearchReply(
         m_requestID, itemArray, m_searchRecord.m_heading);
   }
}

void
SearchInterfaceImpl::AreaMatchSearchResultReceived()
{
   nav2log << "SearchInterfaceImpl::Area matches received" << endl;

   m_outstandingRequests--;

   // Get the CombinedSearchCategory for the heading that was
   // requested.
   const CombinedSearchCategory* cat = m_csDispatcher->
      getCombinedSearchDataHolder().getCombinedSearchResults(
         m_searchRecord.m_heading);
   if (cat == NULL) {
      AsynchronousStatus errorStatus(m_requestID,
                                     UNABLE_TO_RECEIVE_AREA_MATCH_SEARCH_RESULTS,
                                     "", "");
      for (ListenerCont::iterator it = m_listeners.begin();
           it != m_listeners.end();
           ++it) {
         (*it)->error(errorStatus);
      }
      return;
   }

   // Got the correct heading, now we need to send the new
   // search matches to the listeners. 
   const CSSIVec_t& searchItemVec = cat->getResults();
   SearchItemArray itemArray;
   for (CSSIVec_t::const_iterator it = searchItemVec.begin();
        it != searchItemVec.end();
        ++it) {
      Nav2Coordinate itemPos(it->getLat(),
                             it->getLon());
      itemArray.push_back(
         SearchItem(it->getName(),
                    it->getID(),
                    convertToSearchItemType(it->getType()),
                    SearchItemSubType(it->getSubType()),
                    calculateDistance(getApi()->getCurrentPosition(),
                                      itemPos),
                    parseSearchRegions(it->getRegionList()),
                    itemPos,
                    it->getImageName(),
                    it->getAdvert(),
                    ItemInfoArray(),
                    calculateDistance(Nav2Coordinate(m_searchRecord.m_origLat,
                                                     m_searchRecord.m_origLon),
                                      itemPos)));
   }
   for (ListenerCont::iterator it = m_listeners.begin();
        it != m_listeners.end();
        ++it) {
      (*it)->extendedSearchReply(
         m_requestID, itemArray, m_searchRecord.m_heading);
   }
   nav2log << "SearchInterfaceImpl::AreaMatchSearchResultReceived" << endl;
}

void
SearchInterfaceImpl::AdditionalInfoReceived(
   const MSearchResultObserver::idToAdditionalInfoMap& additionalInfoMap)
{
   m_outstandingRequests--;
   nav2log << "SearchInterfaceImpl::AdditionalInfoReceived" << endl;
   const CSCatVec_t& searchResults =
      m_csDispatcher->getCombinedSearchResults();
   
   wf_uint32 nbrFoundElements = 0;
   wf_uint32 nbrTotalElements = additionalInfoMap.size();

   SearchItemArray searchItemArray;
   searchItemArray.reserve(nbrTotalElements);

   for (CSCatVec_t::const_iterator catIt = searchResults.begin();
        catIt != searchResults.end(); ++catIt) {
      // Get the vector containing all the search results for
      // the current heading
      const CSSIVec_t& searchResVec = (*catIt)->getResults();
      for (CSSIVec_t::const_iterator srIt = searchResVec.begin();
           srIt != searchResVec.end(); ++srIt) {
         MSearchResultObserver::idToAdditionalInfoMap::
            const_iterator findIt = additionalInfoMap.find(
               const_cast<char*>(srIt->getID()));
         if (findIt != additionalInfoMap.end()) {
            // We found a match

            // Copy all the information from the additionalInfoMap
            // to the intemInfoArray
            ItemInfoArray itemInfoArray;
            MSearchResultObserver::AdditionalInfoVec infoVec = findIt->second;
            for (MSearchResultObserver::AdditionalInfoVec::
                    const_iterator infoIt = infoVec.begin();
                 infoIt != infoVec.end(); ++infoIt) {
               itemInfoArray.push_back(
                  ItemInfoEntry(
                     (*infoIt)->getKey(),
                     (*infoIt)->getValue(),
                     FavouriteInterfaceImpl::
                     nav2AdditionalInfoTypeToItemInfoType(
                        GuiProtEnums::AdditionalInfoType(
                           (*infoIt)->getType()))));
            }

            Nav2Coordinate itemPos((*srIt).getLat(),
                                   (*srIt).getLon());
            // Create a WFAPI::SearchItem based on the found isab::SearchItem
            // and put it into the itemArray.
            searchItemArray.push_back(
               SearchItem((*srIt).getName(),
                          (*srIt).getID(),
                          convertToSearchItemType((*srIt).getType()),
                          SearchItemSubType((*srIt).getSubType()),
                          calculateDistance(getApi()->getCurrentPosition(),
                                            itemPos),
                          parseSearchRegions((*srIt).getRegionList()),
                          itemPos,
                          (*srIt).getImageName(),
                          (*srIt).getAdvert(),
                          itemInfoArray,
                          calculateDistance(Nav2Coordinate(
                                               m_searchRecord.m_origLat,
                                               m_searchRecord.m_origLon),
                                            itemPos)));
            nbrFoundElements++;
         }
         if (nbrFoundElements == nbrTotalElements) {
            // We have found all the items, no need to search anymore
            break;
         }
      }
      if (nbrFoundElements == nbrTotalElements) {
         // We have found all the items, no need to search anymore
         break;
      }
   }
   
   for (ListenerCont::iterator it = m_listeners.begin();
        it != m_listeners.end();
        ++it) {
      (*it)->searchDetailsReply(m_requestID, searchItemArray);
   }
}

void 
SearchInterfaceImpl::setImagePath(const WFString& imagePath)
{
   m_imagePath = imagePath;
   m_csDispatcher->setHeadingsAndHitsImagePath(m_imagePath.c_str());
}

void
SearchInterfaceImpl::setCategoryImagePath(const WFString& categoryImagePath)
{
   m_categoryImagePath = categoryImagePath;
   m_csDispatcher->setCategoryImagePath(m_categoryImagePath.c_str());
}

void
SearchInterfaceImpl::setImageExt(const WFString& imageExt)
{
   // We want '.' first in our image extension, at least CS want it.
   m_imageExt = (std::string(".") + imageExt.c_str()).c_str();
   m_csDispatcher->setImageExt(m_imageExt.c_str());
}

void
SearchInterfaceImpl::setImageDimension(
   CombinedSearch::CSImageDimension imageDimension)
{
   m_csDispatcher->setImageDimension(imageDimension);
}

void
SearchInterfaceImpl::handleSetSearchHistory(
   const isab::GeneralParameterMess& gpm)
{
   nav2log << "SIIml::handleSetSearchHistory" << endl;
   CombinedSearchDataHolder& dataHolder = 
      m_csDispatcher->getCombinedSearchDataHolder();

   /* Search history. */
   NonOwningBuffer buf(gpm.getBinaryData(),
                       gpm.getNumEntries());
   
   /* Get version. */
   int16 version = buf.readNext16bit();
   if (version == SEARCH_HISTORY_VERSION_NUMBER) {
      /* OK. */
      int16 numEntries = buf.readNext16bit();
      dataHolder.clearCombinedSearchHistory();
      for (int16 i = 0 ; i < numEntries ; i++) {
         // Read 6-tuples of searchstring, housenum, citystring, cityid, 
         // countrystring, countryid and categoryid.
         const char *ss = buf.getNextCharString();
         buf.getNextCharString(); //housenum not used anymore, just move position in buffer
         const char *cis = buf.getNextCharString();
         buf.getNextCharString(); //cityindex not used anymore, just move position in buffer
         const char *cos = buf.getNextCharString();
         const char *coi = buf.getNextCharString();
         uint32 cai = buf.readNext32bit();
         // Add it to the dataholder.
         dataHolder.addSearchHistoryItem(ss, cis, cos, coi, cai);
      }
   } else {
      /* Unknown search history version. */
      /* The search history will be rewritten when the user */
      /* does his next search. */
   }
}

void
SearchInterfaceImpl::handleSaveSearchHistory()
{
   CombinedSearchDataHolder& dataHolder = 
      m_csDispatcher->getCombinedSearchDataHolder();

   Uint32StringMap::const_iterator it = m_topRegionList.find(
      m_searchRecord.m_regionId);

   WFString countryName;
   if (it == m_topRegionList.end()) {
      // We did not find, this should not happen.
      nav2log << "Problem locating the top region id in m_topRegionList!"
           << endl;
   } else {
      countryName = it->second;
   }

   char strCountryId[15];
   sprintf(strCountryId, "%u", m_searchRecord.m_regionId);
   dataHolder.addSearchHistoryItem(m_searchRecord.m_searchString,
                                   m_searchRecord.m_areaName,
                                   countryName.c_str(),
                                   strCountryId,
                                   m_searchRecord.m_categoryId);

   Buffer *buf = new Buffer(1280);

   uint32 numEntries = dataHolder.getNumSearchHistoryItems();
   buf->writeNext16bit(SEARCH_HISTORY_VERSION_NUMBER); /* Version. */
   buf->writeNext16bit(numEntries);
   for (uint32 i = numEntries; i > 0; i--) {
      SearchHistoryItem* shi = dataHolder.getSearchHistoryItem(i-1);
      if (shi) {
         buf->writeNextCharString(shi->GetSearchString());
         buf->writeNextCharString(shi->GetHouseNum());
         buf->writeNextCharString(shi->GetCityString());
         buf->writeNextCharString(shi->GetCityId());
         buf->writeNextCharString(shi->GetCountryString());
         buf->writeNextCharString(shi->GetCountryId());
         buf->writeNext32bit(shi->GetCategoryId());
      }
   }
   const uint8 *rawdata = buf->accessRawData();
   int32 len = buf->getLength();

   GeneralParameterMess gpm(GuiProtEnums::paramSearchStrings,
                            (uint8*)rawdata, len);

   buf->releaseData();
   sendAsynchronousRequestToNav2(&gpm);
   /* Don't delete data, it's owned by the buffer. */

   gpm.deleteMembers();
   delete buf;
}

void
SearchInterfaceImpl::handleSetTopRegionList()
{
   for (ListenerCont::iterator it = m_listeners.begin();
        it != m_listeners.end();
        ++it) {
      (*it)->topRegionsChanged();
   }
}

void
SearchInterfaceImpl::convertToSearchItemsArray(
   const CSSIVec_t& searchItemVec, SearchItemArray& itemArray)
{
   itemArray.reserve(searchItemVec.size());
   for (CSSIVec_t::const_iterator it = searchItemVec.begin();
        it != searchItemVec.end();
        ++it) {
      isab::SearchItem item = *it;
      ItemInfoArray itemInfoArray;
      Nav2Coordinate itemPos(item.getLat(), item.getLon());
      itemArray.push_back(
         SearchItem(item.getName(),
                    item.getID(),
                    convertToSearchItemType(item.getType()),
                    SearchItemSubType(item.getSubType()),
                    calculateDistance(getApi()->getCurrentPosition(),
                                      itemPos),
                    parseSearchRegions(item.getRegionList()),
                    itemPos,
                    item.getImageName(),
                    item.getAdvert(),
                    itemInfoArray,
                    calculateDistance(Nav2Coordinate(m_searchRecord.m_origLat,
                                                     m_searchRecord.m_origLon),
                                      itemPos)));
   }
}

void
SearchInterfaceImpl::convertToAreaItemsArray(const CSSIVec_t& searchItemVec,
                                             SearchAreaArray& areaArray)
{
   areaArray.reserve(searchItemVec.size());
   for (CSSIVec_t::const_iterator it = searchItemVec.begin();
        it != searchItemVec.end();
        ++it) {
      isab::SearchItem item = *it;
      areaArray.push_back(
         SearchArea(item.getName(),
                    item.getID(),
                    convertToSearchAreaType(item.getType())));
   }   
}

SearchItemType
SearchInterfaceImpl::convertToSearchItemType(wf_uint16 type)
{
   switch (ServerSearchItemType(type)) {
   case street:
      return STREET;
   case poi:
      return POI;
   case category:      
   case misc:
      return MISC;
   case persons:
      return PERSONS;
   }
   return MISC;
}

SearchAreaType
SearchInterfaceImpl::convertToSearchAreaType(wf_uint16 type)
{
   switch (GuiProtEnums::RegionType(type)) {
   case GuiProtEnums::cityPart:
      return CITY_PART;
   case GuiProtEnums::city:
      return CITY;
   case GuiProtEnums::municipal:
      return MUNICIPAL;
   case GuiProtEnums::zipcode:
      return ZIP_CODE;
   case GuiProtEnums::zipArea:
      return ZIP_AREA;
   default:
      return CITY;
   }
}

TopRegionType
SearchInterfaceImpl::convertToTopRegionType(wf_uint32 type)
{
   switch(ServerTopRegionType(type)) {
   case country:
      return COUNTRY;
   case state:
      return STATE;
   case internationalRegion:
   case metaregion:
      return INTERNATIONAL_REGION;
   }
   return COUNTRY;
}

SearchHitsType
SearchInterfaceImpl::convertToSearchHitsType(wf_uint32 type)
{
   switch(type) {
   case 0:
      return SEARCH_RESULTS;
   case 1:
      return AREA_MATCHES;
   }
   return SEARCH_RESULTS;
}

WFString
SearchInterfaceImpl::parseSearchRegions(
   const isab::SearchItem::RegionList& regions)
{
   //char* tmpRegionString = new char[100];
   std::string tmpRegionString;
   tmpRegionString.reserve(50);
   for (isab::SearchItem::RegionList::const_iterator it = regions.begin();
        it != regions.end();
        ++it) {
      if (it != regions.begin()) {
         tmpRegionString += ", ";
      }
      tmpRegionString += (*it)->getName();
   }
   return WFString(tmpRegionString.c_str());
}

SynchronousStatus
SearchInterfaceImpl::getParam(wf_uint16 paramID,
                              std::vector<wf_int32>& intVec)
{
   isab::GeneralParameterMess mess(paramID);

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

   // Done get value
   isab::GeneralParameterMess* gpm =
      static_cast<isab::GeneralParameterMess*>(reply);

   if(gpm->getParamId() == isab::GuiProtEnums::paramInvalid) {
      mess.deleteMembers();
      reply->deleteMembers();
      delete reply;
      
      return SynchronousStatus(NO_CATEGORIES_IN_STORED_PARAM_FILE,
                               "No categories were loaded in memory, this is "
                               "probably due to an incorrect parameter file. ",
                               "");
   }
   
   wf_int32 numEntries = gpm->getNumEntries();
   wf_int32* data = gpm->getIntegerData();
   
   for (wf_int32 i = 0; i < numEntries; ++i) {
      intVec.push_back(data[i]);
   }

   // Delete members.
   mess.deleteMembers();
   reply->deleteMembers();
   delete reply;
   
   return createOKAsynchStatus();   
}


SynchronousStatus
SearchInterfaceImpl::getParam(wf_uint16 paramID,
                              WFStringArray& stringArray)
{
   isab::GeneralParameterMess mess(paramID);

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

   // Done get value
   isab::GeneralParameterMess* gpm =
      static_cast<isab::GeneralParameterMess*>(reply);

   if(gpm->getParamId() == isab::GuiProtEnums::paramInvalid) {
      mess.deleteMembers();
      reply->deleteMembers();
      delete reply;
      
      return SynchronousStatus(NO_CATEGORIES_IN_STORED_PARAM_FILE,
                               "No categories were loaded in memory, this is "
                               "probably due to an incorrect parameter file. ",
                               "");
   }

   wf_int32 numEntries = gpm->getNumEntries();
   char** data = gpm->getStringData();
   
   for (wf_int32 i = 0; i < numEntries; ++i) {
      if(data[i]) {
         stringArray.push_back(WFString(data[i]));         
      }
   }

   // Delete members.
   mess.deleteMembers();
   reply->deleteMembers();
   delete reply;
   
   return createOKAsynchStatus();
}

void
SearchInterfaceImpl::handleSetCategoryChecksum(wf_int32 categoryChecksum)
{
   // Check if we have an new checksum, first run (during startup), the
   // m_categoryChecksum will be -1
   if (m_categoryChecksum != -1 && m_categoryChecksum != categoryChecksum) {
      
      // Whipe currently stored category images
      m_csDispatcher->clearCategoryIcons();

      WFStringArray iconArray;
      SynchronousStatus status = getParam(GuiProtEnums::paramCategoryIcons,
                                           iconArray);

      if (status.getStatusCode() == OK) {

         // Ok getting the category icons from the param file,
         // now we need to ask the CombinedSearchDispatcher to handle
         // fetching the icons.
         std::vector<const char*> iconList;
         iconList.reserve(iconArray.size());
         for (WFStringArray::const_iterator it = iconArray.begin(),
                 end = iconArray.end();
              it != end; ++it) {
            iconList.push_back(it->c_str());
         }

         m_csDispatcher->getCategoryIcons(iconList);
      } else {
         reportError(AsynchronousStatus(RequestID::INVALID_REQUEST_ID,
                                        UNABLE_TO_GET_CATEGORY_ICONS,
                                        "", ""),
                                        m_listeners.begin(),
                                        m_listeners.end());
      }
   }
   m_categoryChecksum = categoryChecksum;
}

void SearchInterfaceImpl::notifyCategoriesUpdated()
{
   for(ListenerCont::iterator itr = m_listeners.begin();
       itr != m_listeners.end(); itr++)
   {
      (*itr)->searchCategoriesUpdated();
   }
}

wf_uint32
SearchInterfaceImpl::calculateDistance(const WGS84Coordinate& startCoord,
                                       const WGS84Coordinate& endCoord)
{
   if (startCoord.isValid() && endCoord.isValid()){
      return wf_uint32(rint(GfxUtility::rtP2Pdistance_linear(startCoord,
                                                             endCoord)));
   } else {
      return WF_MAX_UINT32;
   }

}


void WFAPI::SearchInterfaceImpl::clearCategoryImages()
{
   m_csDispatcher->clearCategoryIcons();    
}

// wf_uint32
// SearchInterfaceImpl::calculateDistance(const Nav2Coordinate& startCoord,
//                                        const Nav2Coordinate& endCoord)
// {
// }

} // End namespace WFAPI




