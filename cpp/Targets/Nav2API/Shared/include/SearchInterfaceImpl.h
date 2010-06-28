/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_SEARCH_INTERFACE_IMPL_H
#define WFAPI_SEARCH_INTERFACE_IMPL_H

#include "InterfaceImpl.h"
#include "SearchRecord.h"
#include "RequestID.h"
#include "WFString.h"
#include "MessageSenderInterface.h"
#include "SearchResultObserver.h"
#include "CombinedSearchTypedefs.h"
#include "SearchItemTypeEnum.h"
#include "SearchAreaTypeEnum.h"
#include "SearchHitsTypeEnum.h"
#include "TopRegionTypeEnum.h"
#include "CSImageDimension.h"

#include <set>
#include <vector>
#include <map>

namespace isab {
// Forward declarations
class GuiProtMess;
class SearchItem;
}

// Forward declarations
class GuiProtMessageReceiver;
class CombinedSearchCategory;
class CombinedSearchDispatcher;

namespace WFAPI {

class Nav2APIImpl;
class SearchCategoryArray;
class WFStringArray;
class TopRegionArray;
class SearchHistoryArray;
class SearchDescArray;
class SearchIDArray;
class SearchQuery;
class SearchListener;
class SearchItemArray;
class SearchAreaArray;
class SearchHeadingArray;
class WGS84Coordinate;
/**
 * Interface for search. Uses the combined search when searching.
 * Searching in different headings.
 */
class SearchInterfaceImpl : public InterfaceImpl,
                            public MessageSenderInterface,
                            public MSearchResultObserver
{
public:

   /**
    * Constructor.
    */ 
   SearchInterfaceImpl(Nav2APIImpl* api);

   /**
    * Destructor.
    */
   virtual ~SearchInterfaceImpl();
   
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
    * Implementation of abstract function from MessageSenderInterface.
    */
   //@{

   /**
    * Function for sending messages to Nav2. Will be called
    * by the dispatcher when sending search queries.
    *
    * @param message The message to be sent to Nav2/server.
    */
   virtual void sendMessage(isab::GuiProtMess& message);

   /**
    * Function for sending messages to Nav2. Will be called
    * by the dispatcher when sending search queries.
    *
    * @param message The message to be sent to Nav2/server.
    * @param messageReceiver The receiver that should receive the message.
    */
   virtual void sendMessage(
      isab::GuiProtMess& message,
      GuiProtMessageReceiver* messageReceiver);
   
   /**
    * Function for sending messages to Nav2. Will be called
    * by the dispatcher when sending search queries.
    *
    * @param params The paramblock to be sent to Nav2/server.
    * @param navRequestType The request type of the message to send.
    */
   virtual void sendMessage(const isab::NParamBlock& params,
                            uint16 navRequestType);
   //@}

   /**
    * Functions wrapped from SearchInterface.
    */
   //@{
   
   /**
    * To add a listener for searches.
    * 
    * @param listener The SearchListener to add.
    *                 It may not be deleted until it is removed by
    *                 calling removeSearchListener.
    */
   void addSearchListener(SearchListener* listener);

   /**
    * To remove a listener.
    * 
    * @param listener The SearchListener to be removed.
    */
   void removeSearchListener(SearchListener* listener);

   /**
    * Get the latest searches. Max number is 10 then the oldest search
    * history item is replaced with the new search.
    *
    * @param searchHistoryList List that will contain the latest searches
    *                          when done.
    * @return The status of this call. Status code will be OK if no problems
    *         occured, otherwise one of the error codes.
    */
   SynchronousStatus getSearchHistory(SearchHistoryArray& searchHistoryList);

   /**
    * Get the search descs.
    *
    * @param searchDescList All SearchDesc found will be added to this list. 
    * @return The status of the request.
    */
   SynchronousStatus getSearchDesc(SearchDescArray& searchDescList);

   /**
    * Get the latest Top Regions.
    *
    * @param topRegionList This list will contain the top regions when done.
    * @return The status of this call. Status code will be OK if no problems
    *         occured, otherwise one of the error codes.
    */
   SynchronousStatus getTopRegions(TopRegionArray& topRegionList);

   /**
    * Get the list of search categories available for searching.
    *
    * @param searchCategoryArray The available search categories will be added
    *                            to this list.
    * @return The status of the request.
    */
   SynchronousStatus getSearchCategories(
      SearchCategoryArray& searchCategoryArray);

   AsynchronousStatus getSearchCategoriesByPosition(const WGS84Coordinate& position);

   /**
    * This asynchronous function initiates a search using the parameter.
    *
    * @param searchQuery What to search for is in this.
    * @return The status and if status is OK the RequestID that will be
    *         returned in the SearchListeners searchReply function.
     */
   AsynchronousStatus search(const SearchQuery& searchQuery);

   /**
    * Request more search results from a heading with index as start
    * index.
    *
    * @param index Start index of the first search result to get.
    * @param maxHits Max number of hits to receive. The server might
    *                respond with fewer results but never with more.
    * @param heading The heading id of the heading to request more hits
    *                from.
    * @return The status and if status is OK the RequestID that will be
    *         returned in the SearchListeners searchReply function.
    */
   AsynchronousStatus requestNextSearchResults(wf_uint32 index,
                                               wf_uint32 maxHits,
                                               wf_uint32 heading);

   /**
    * Use this function when a search result list of a category contains
    * area matches.
    *
    * @param areaID The id of the area to use for search.
    * @param heading The heading to search in.
    * @param areaName The name of the area to search in.
    * @return The status and if status is OK the RequestID that will be
    *         returned in the SearchListeners searchReply function.
    */
   AsynchronousStatus extendedSearch(const WFString& areaID,
                                     const WFString& areaName,
                                     wf_uint32 heading);
   
   /**
    * This asynchronous function initiates retrieving details for the
    * identifiers in the parameter.
    *
    * @param searchIDArray .
    * @return The status and if status is OK the RequestID that will be
    *         returned in the SearchListeners searchDetailsReply function.
    */
   AsynchronousStatus searchDetails(const SearchIDArray& searchIDArray);
   //@}

   /**
    * Functions from MSearchResultObserver
    */
   //@{
   
   /**
    * Call to the observer from the search module when search headings
    * have been received from the server. 
    *
    */
   void SearchHeadingsReceived();


   void HeadingImagesStatusUpdated(ImageStatus currentStatus);
   void CategoryImagesStatusUpdated(ImageStatus currentStatus);
   void ResultImagesStatusUpdated(ImageStatus currentStatus);
   
   /**
    * Call to the observer from the search module when search
    * results has been received and total number of hits has
    * been calculated.
    */
   virtual void TotalNbrHitsReceived(
      const std::vector<CombinedSearchCategory*>& categories);

   /**
    * Call to the observer from the search module when search
    * results has been received and parsed.
    */
   void SearchResultReceived();

   /**
    * Call to the observer from the search module when search
    * results has been received and parsed, a reply to a request
    * to the search module for fetching more search results for 
    * a given heading.
    */
   void MoreSearchResultReceived();

   /**
    * Call to the observer from the search module when search
    * results has been received and parsed, a reply to a request
    * to the search module for doing a area match search.
    */
   void AreaMatchSearchResultReceived();

   /**
    * Call to the observer when additional information has been
    * parsed.
    */
    void AdditionalInfoReceived(
       const MSearchResultObserver::idToAdditionalInfoMap& additionalInfoMap);
   //@}

   /**
    * Sets the image path. Make sure this path exists, if not no images will
    * be stored.
    *
    * @param imagePath The path to where to store the images fetched from
    *                  the server.
    */
   void setImagePath(const WFString& imagePath);

   /**
    * Sets the image path for the search category icons. Make sure this
    * path exists, if not no images will be stored.
    *
    * @param categoryImagePath The path to where to store the images
    *                          fetched from the server.
    */
   void setCategoryImagePath(const WFString& categoryImagePath);

   /**
    * Sets the image extension. 
    *
    * @param imageExt The image extension to use.
    */
   void setImageExt(const WFString& imageExt);

   /** 
    * Sets the image dimension.
    * 
    * @param imageDimension The image dimension to use.
    */
   void setImageDimension(CombinedSearch::CSImageDimension imageDimension);

   /**
    * Called by Nav2APIImpl when the top region has changed. We notify
    * the listeners that the top region list has been updated.
    */
   void handleSetTopRegionList();

   /**
    * Called by Nav2APIImpl when the category checksum has been received
    * from the server. There will always be one call during startup, during
    * startup we store the categoryChecksum in m_categoryChecksum for
    * later comparison.
    * If a the checksum has changed we clear the stored category icons
    * and requests the new icon list from the paramter file and then tells
    * CombinedSearchDispatcher to download the files.
    *
    * @param categoryChecksum The categoryChecksum to compare with.
    */
   void handleSetCategoryChecksum(wf_int32 categoryChecksum);
   
   void notifyCategoriesUpdated();

   /**
    * Clears all the category images. Used for testing.
    */ 
   void clearCategoryImages();
private:

   /**
    * A reply from the parameter module when a search history
    * has been added. Parameter paramSearchStrings has been sent
    * in message to nav2 containing a search history item.
    *
    * @param guiProtMess Message containing the data that has been
    *                    stored in the paramter file. This data is
    *                    sent to the CombinedSearchDataHolder for caching.
    */
   void handleSetSearchHistory(const isab::
                               GeneralParameterMess& guiProtMess);

   /**
    * When user makes a search we call this function for storing the
    * parameters used for the search. A message is sent to nav2 that
    * stores it in the parameter file.
    */
   void handleSaveSearchHistory();

   /**
    * Internal function, converts a vector with isab::SearchItem
    * to a vector with WFAPI::SearchItem.
    *
    * @param searchItemVec The vector to convert.
    * @param itemArray The vector to convert to.
    */
   void convertToSearchItemsArray(const CSSIVec_t& searchItemVec,
                                  SearchItemArray& itemArray);

   /**
    * Internal function, converts a vector with isab::SearchItem
    * to a vector with WFAPI::SearchArea.
    *
    * @param searchItemVec The vector to convert.
    * @param areaArray The vector to convert to.
    */
   void convertToAreaItemsArray(const CSSIVec_t& searchItemVec,
                                SearchAreaArray& areaArray);

   /**
    * Internal function, converts from server enum to
    * WFAPI::SearchItemType value.
    *
    * @param type The type to convert.
    * @return The converted value, in WFAPI::SearchItemType.
    */
   SearchItemType convertToSearchItemType(wf_uint16 type);

   /**
    * Internal function, converts from server enum to
    * WFAPI::SearchAreaType value.
    *
    * @param type The type to convert.
    * @return The converted value, in WFAPI::SearchAreaType.
    */
   SearchAreaType convertToSearchAreaType(wf_uint16 type);
   
   /**
    * Internal function, converts from server enum to
    * WFAPI::TopRegionType value.
    *
    * @param type The type to convert.
    * @return The converted value, in WFAPI::TopRegionType.
    */
   TopRegionType convertToTopRegionType(wf_uint32 type);

   /**
    * Internal function, converts from int to
    * WFAPI::SearchHitsType value.
    *
    * @param type The type to convert.
    * @return The converted value, in WFAPI::SearchHitsType.
    */
   SearchHitsType convertToSearchHitsType(wf_uint32 type);
   
   /**
    * Parses a list with region names into a comma seperated string.
    *
    * @param regions The RegionList to parse.
    * @return A comma seperated string.
    */
   WFString parseSearchRegions(const isab::SearchItem::RegionList& regions);

   /**
    * Get the param values for the given param id, the returned
    * parameter message should contain an int32*.
    *
    * @param paramID The parameter id to fetch.
    * @param intVec A vector containing int32, the data received by the
    *               get parameter call. Will contain the date when
    *               the function returns.
    */
   SynchronousStatus getParam(wf_uint16 paramID,
                              std::vector<wf_int32>& intVec);

   /**
    * Get the param values for the given param id, the returned
    * parameter message should contain an char**.
    *
    * @param paramID The parameter id to fetch.
    * @param stringArray A vector containing WFStrings, the data received by
    *                    the get parameter call. Will contain the date when
    *                    the function returns.
    */
   SynchronousStatus getParam(wf_uint16 paramID,
                              WFStringArray& stringArray);

   /**
    * Convenience function that calculates the distance between two
    * coordinates.
    *
    * @param startCoord The start point for the calculation.
    * @param endCoord The end point for the calculation.
    * @return The distance between the two supplied positions in meter.
    */
   wf_uint32 calculateDistance(const WGS84Coordinate& startCoord,
                               const WGS84Coordinate& endCoord);


   /**
    * Utility function for parsing search items when getting
    * a result from the search module.
    *
    * @param shArray The array that will contain the results on return.
    */
   void parseSearchResults(SearchHeadingArray& shArray);

   /**
    * Enum that is a mirror of a server enum. Used for converting
    * from server value to WFAPI values.
    */
   enum ServerSearchItemType {
      street   = 0x01,
      poi      = 0x02,
      misc     = 0x03,
      category = 0x04,
      persons  = 0x05,
   };
   
   /**
    * Enum that is a mirror of a server enum. Used for converting
    * from server value to WFAPI values.
    */
   enum ServerTopRegionType {
      country = 0,
      state = 1,
      internationalRegion = 2,
      metaregion = 3,
   };

   /**
    * Current state of the interface.
    */
   enum searchState {
      IDLE,
      SEARCH_REQUEST_OUTSTANDING,
      AREA_MATCH_REQUEST_OUTSTANDING
   };

   /// Typedef for a set of listeners.
   typedef std::set<SearchListener*> ListenerCont;

   /**
    * Set of listeners.
    */
   ListenerCont m_listeners;

   /// Current state of this class.
   searchState m_searchState;

   /// The path to the image folder.
   WFString m_imagePath;

   /// The path to the category images folder.
   WFString m_categoryImagePath;
   
   /// The extension for the images to download.
   WFString m_imageExt;

   /**
    * Search dispatcher that performs search and parses search
    * requests.
    */
   CombinedSearchDispatcher* m_csDispatcher;

   /**
    * Request id of the current request.
    * NOTE: This means we, theoratically, could
    * get a new request before the old one is handled. We would the
    * get in a state where the UI never can finish his first request.
    * This can be solved using a map, and mapping a request against
    * a requestID .
    */
   RequestID m_requestID;

   /**
    * Number of outstanding requests. A search contains normaly of two
    * requests, one for round 0 and one round 1.
    */
   wf_uint32 m_outstandingRequests;

   /**
    * Current search record. Contains the search data for the last
    * requested search.
    */
   SearchRecord m_searchRecord;

   /// Message linking together a request id against a message sender.
   typedef std::map<wf_uint32, GuiProtMessageReceiver*> MessageSenderMap;
   MessageSenderMap m_messageReceivers;

   /// Map linking together a top region id with the name of that top region.
   typedef std::map<wf_uint32, WFString> Uint32StringMap;
   Uint32StringMap m_topRegionList;

   /// The checksum for the currently stored categories.
   wf_int32 m_categoryChecksum;
};

} // End namespace WFAPI

#endif // end WFAPI_SEARCH_INTERFACE_IMPL_H
