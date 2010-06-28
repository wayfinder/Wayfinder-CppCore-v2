/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_SEARCH_INTERFACE_H
#define WFAPI_SEARCH_INTERFACE_H

#include "WFAPIConfig.h"
#include "Interface.h"
#include "SearchQuery.h"
#include "OneSearchQuery.h"
#include "SearchHistoryArray.h"
#include "SearchDescArray.h"
#include "TopRegionArray.h"
#include "CategoryTreeArray.h"
#include "RequestID.h"
#include "SearchIDArray.h"
#include "AsynchronousStatus.h"


namespace WFAPI {

// Forward declarations
class SearchListener;
class Nav2APIImpl;
class SearchInterfaceImpl;
class CategoryTreeInterfaceImpl;
class OneSearchInterfaceImpl;
class SearchCategoryArray;

/**
 * Interface class for searching. Used to make searches.
 */
class SearchInterface : public Interface
{

public:

   /**
    * Constructor.
    */
   SearchInterface();

   /**
    * Destructor.
    */
   virtual ~SearchInterface();

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
    * Get the list of search categories (search headings e.g Places,
    * Addresses etc)  that where returned by the server for the
    * search request that was made. The list will only contain the
    * headings and information about the headings, to be exact:
    * name, heading number, country id and image name.
    *
    * @param searchDescList All SearchDesc found will be added to this list. 
    * @return The status of the request.
    */
   SynchronousStatus getSearchDesc(SearchDescArray& searchDescList);

   /**
    * Get the latest Top Regions.
    *
    * @param topRegionList The current top regions will be added to this list.
    * @return The status of the request.
    */
   SynchronousStatus getTopRegions(TopRegionArray& topRegionList);

   /**
    * Get the list of search categories available for searching. The
    * categories received by this function are the local categories that
    * are stored in the parameter file.
    *
    * @param searchCategoryArray The available search categories will be added
    *                            to this list.
    * @return The status of the request.
    */
   SynchronousStatus getSearchCategories(
      SearchCategoryArray& searchCategoryArray);

   /**
    * Queries the Nav2API for an updated list of categories which are valid
    * for the supplied position. Once Nav2 has obtained such a list, the
    * SearchListener will be notified via searchCategoriesUpdated. After that
    * the user can get the list by calling getSearchCategories.
    *
    * @param position The position that the categories are valid for.
    * @return The status of the request.
    */
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

   /** 
    * Performs a OneSearch request to the server. The OneSearch request means
    * that all search results will return in one list, not based on headings
    * as the old search,
    * @see SearchInterface::search(const SearchQuery& searchQuery).
    * @see OneSearchQuery class for more information of which combinations
    *      a search query can have.
    * 
    * @param searchQuery The query contains the information that should be used
    *                    when sending the search query to the server. For how
    *                    to use this please see the OneSearchQuery documentation.
    * 
    * @return The status and if status is OK the RequestID that will be
    *         returned in the SearchListeners category tree reply function.
    */
   AsynchronousStatus oneSearch(const OneSearchQuery& searchQuery);

   /**
    * This asynchronous function requests an update to the category tree
    * using a coord. Once we get a reply from server for this request
    * SearchListener::categoryTreeReceived will get called.
    * 
    *
    * @param coord For what region to request a category tree.
    * @return The status and if status is OK the RequestID that will be
    *         returned in the SearchListeners category tree reply function.
    */
   AsynchronousStatus requestCategoryTree(const WGS84Coordinate& coord);

   /** 
    * Get the root categories in the current category tree. 
    * 
    * @param rootCategoryList The root categories in the category tree will be
    * added to this list.
    * @return The status of this call. Status code will be OK if no problems
    *         occured, otherwise one of the error codes.
    */
   SynchronousStatus
   getRootCategories(CategoryTreeArray& rootCategoryList);

   /** 
    * Get the list of subcategories for the selected category.
    * The list of subcategories can be empty which means that the selected
    * category did not have any subcategories (a leaf node). 
    * 
    * @param categoryId The category id of the category you whish to retrieve
    * the subcategories for.
    * @param subCategoryList The sub categories for the specified category
    * will be added to this list.
    * @return The status of this call. Status code will be OK if no problems
    *         occured, otherwise one of the error codes.
    */
   SynchronousStatus
   getSubCategoriesOf(wf_int32 categoryId,
                      CategoryTreeArray& subCategoryList);
   
   /**
    * Internal function.
    */
   SearchInterfaceImpl* getImpl();

   /**
    * Internal function.
    */
   CategoryTreeInterfaceImpl* getCategoryTreeImpl();

   /**
    * Internal function.
    */
   OneSearchInterfaceImpl* getOneSearchImpl();

private:
   /**
    * Do not create SearchInterface get it from the Nav2API class.
    */
   SearchInterface(Nav2APIImpl* nav2APIimpl);

   /**
    * We do not allow copies of this interface.
    */
   SearchInterface(const SearchInterface&);
   const SearchInterface& operator=(const SearchInterface&);

   friend class Nav2APIImpl;

   struct impl;

   impl* m_impl;
};

} // End namespace WFAPI

#endif // End WFAPI_SEARCH_INTERFACE_H
