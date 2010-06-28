/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "SearchTester.h"
#include "SearchInterface.h"
#include "Favourite.h"
#include "FavouriteArray.h"
#include "WGS84Coordinate.h"
#include "WFString.h"
#include "ItemInfoEnums.h"
#include "DistanceUnit.h"
#include "SearchCategoryArray.h"
#include <stdlib.h>
#include <string.h>
#include <set>
#include <iostream>


using namespace std;
using namespace WFAPI;

SearchTester::SearchTester(SearchInterface& searchInterface)
   : m_searchInterface(searchInterface)
{
   addCommandOptions();
   m_searchInterface.addSearchListener(this);
}
                   
/**
 * Destructor.
 */
SearchTester::~SearchTester()
{

}

bool
SearchTester::handleInput(const char* input, bool& goBack)
{
   // Continue until user selects back
   StringMap::const_iterator it = m_commandOptions.find(input);
   if (it == m_commandOptions.end()) {
      return false;
   }
   switch((*it).second.m_commandType) {
  case help:  
      printHelp();
      break;
   case cmdSearch:
      search();
      break;
   case cmdSearchByPosition:
      searchByPos();
      break;
   case cmdSearchDefault:
      searchTest();
      break;
   case cmdListAll:
      listAll();
      break;
   case cmdSearchHeadings:
      listHeadings();
      break;
   case cmdSearchResults:
      listResults();
      break;
   case cmdGetDetails:
      getDetailsForItems();
      break;
   case cmdGetMoreResults:
      getMoreSearchResults();
      break;
   case cmdGetSearchHistory:
      getSearchHistory();
      break;
   case cmdGetSearchCategories:
      searchCategories();
      break;
   case back:
      goBack = true;
      break;
   default:
      cout << "Command not handled here, please try again or select b to go back." << endl;
      return false;
   }
   return true;
}



void SearchTester::searchCategoriesUpdated()
{
   std::cout << "Search categories was updated: " << std::endl;

   SearchCategoryArray categories;
   
   m_searchInterface.getSearchCategories(categories);
   
   for(SearchCategoryArray::const_iterator itr = categories.begin();
       itr != categories.end(); itr++)
   {
      SearchCategory c = *itr;
      
      std::cout << "Category name: " << (*itr).getName()
                << " id: " << (*itr).getId()
                << " intId: " << (*itr).getIntId() << std::endl;
   }
   
}

void
SearchTester::searchCategories()
{
   // SearchCategoryArray array;
   // SynchronousStatus s = m_searchInterface.getSearchCategories(array);

   // std::cout << "Available categories: " << std::endl;
   
   // for(SearchCategoryArray::iterator itr = array.begin(); itr != array.end();
   //     itr++)
   // {
   //    std::cout << "Category name: " << (*itr).getName()
   //              << " id: " << (*itr).getId()
   //              << " intId: " << (*itr).getIntId() << std::endl;
   // }

   static bool inSweden = true;

   WGS84Coordinate coord(0, 0);
   
   if(inSweden) {
      std::cout << "Testing in malmo" << std::endl;
      coord = WGS84Coordinate(55.5956, 13.0112);
   } else  {
      std::cout << "Testing invalid coordinates" << endl;
      coord = WGS84Coordinate(WF_MAX_UINT32, WF_MAX_UINT32);
   }//  else {
   //    std::cout << "Testing in jacksonville" << std::endl;
   //    coord = WGS84Coordinate(30.3273, -81.6569);
   // }
   
   AsynchronousStatus status =
      m_searchInterface.getSearchCategoriesByPosition(coord);

   inSweden = !inSweden;
}

void
SearchTester::search()
{
   TopRegionArray topRegionArray;
   SynchronousStatus status =
      m_searchInterface.getTopRegions(topRegionArray);

   if (status.getStatusCode() != OK) {
      cout << "Get top regions failed with code: " << status.getStatusCode()
           << endl;
      return;
   }
   
   string what = "";
   cout << "What: ";
   getline(cin, what);        

   string where = "";
   cout << "Where: ";
   getline(cin, where);

   cout << "List of top regions:" << endl;
   wf_uint32 i = 0;
   for (TopRegionArray::const_iterator it = topRegionArray.begin();
        it != topRegionArray.end();
        ++it, ++i) {
      cout << i << " - " << it->getName().c_str() << endl;
   }
   cout << "Please enter the number of the country you want to search in:"
        << endl;
   string country = "";
   getline(cin, country);
   wf_int32 countryIndex = atoi(country.c_str());
   if (countryIndex < wf_int32(topRegionArray.size())) {
      TopRegion region = topRegionArray.at(countryIndex);
      SearchQuery query(what.c_str(), where.c_str(),
                        "", region.getID(), WGS84Coordinate());
      m_searchInterface.search(query);
   } else {
      cout << "Invalid country index!" << endl;
   }
}

void
SearchTester::searchByPos()
{
   string what = "";
   cout << "What: ";
   getline(cin, what);

   string categoryID = "";
   cout << "CategoryID: ";
   getline(cin, categoryID);        
   wf_uint32 cid = atoi(categoryID.c_str());
   if(cid == 0)
      cid = WF_MAX_UINT32;
   SearchQuery query(what.c_str(), cid,
                     WGS84Coordinate(55.5934, 13.016));
   m_searchInterface.search(query);

}

void
SearchTester::searchTest()
{
   
   cout << "Searching for Rest in stockholm, Sweden" << endl;
   m_searchInterface.search(SearchQuery("Rest", "", "", 1, WGS84Coordinate()));
   // m_searchInterface.search(SearchQuery("Rest", "", "", 1,
   //                                      WGS84Coordinate(),
   //                                      0, 25, WF_MAX_UINT32, 0));
}


void
SearchTester::error(const AsynchronousStatus& status)
{
   printErrorStatus(&status, "SearchTester");
}

void
SearchTester::searchDetailsReply(RequestID requestID,
                                 const SearchItemArray& searchItemArray)
{
   cout << "********** SearchTester::searchDetailsReply ***********" << endl;
   for (SearchItemArray::const_iterator it = searchItemArray.begin();
        it != searchItemArray.end();
        ++it) {
      cout << "Name: " << it->getName().c_str() << endl;
      ItemInfoArray infoArray = it->getAdditionalInformation();
      WFString normalPrefix(" ");
      WFString hiddenPrefix("   Hidden ");
      for (ItemInfoArray::const_iterator sit = infoArray.begin();
           sit != infoArray.end();
           ++sit) {
         const WFString* prefix = &normalPrefix;
         if (sit->getInfoType() == DONT_SHOW ||
             sit->getInfoType() == SPECIAL_FLAG) {
            prefix = &hiddenPrefix;
         }
         cout << *prefix << "Key:   " << sit->getKey().c_str() << endl;
         cout << *prefix << "Value: " << sit->getVal().c_str() << endl;
         cout << *prefix << "Type:  " << sit->getInfoType() << endl;
      }
      cout << endl;
   }
}

void
SearchTester::searchReply(RequestID requestID,
                          const SearchHeadingArray& searchHeadings,
                          bool final)
{
   if (!final) {
      cout << "Got search reply but it is not final, please wait!" << endl;
      return;
   } else {
      cout << "Got final search reply!" << endl;
      m_searchResList = searchHeadings;
   }
}

void
SearchTester::nextSearchReply(RequestID requestID,
                              const SearchItemArray& searchItemArray,
                              wf_uint32 heading)
{
   // Take these hits as current ones
   for (SearchHeadingArray::iterator it = m_searchResList.begin();
        it != m_searchResList.end();
        ++it) {
      if (it->getHeadingID() == heading) {
         // We found the correct heading, now we need to put the
         // searchItemArray in that heading.
         // But first update the current position in the hits.
         it->setCurrentPosition(it->getCurrentPosition() + 
                                it->getItemResults().size());
         it->setItemResults(searchItemArray);
         break;
      }
   }
   cout << "***************** Next search reply *******************" << endl;
   for (SearchItemArray::const_iterator it = searchItemArray.begin();
        it != searchItemArray.end();
        ++it) {
      cout << "Name: " << it->getName().c_str() << endl;
      cout << "Location name: " << it->getLocationName().c_str()
           << endl << endl;
   }
}

void
SearchTester::extendedSearchReply(RequestID requestID,
                                  const SearchItemArray& searchItemArray,
                                  wf_uint32 heading)
{
   cout << "***************** Extended search reply *******************" << endl;
   for (SearchHeadingArray::iterator it = m_searchResList.begin();
        it != m_searchResList.end();
        ++it) {
      if (it->getHeadingID() == heading) {
         // We found the correct heading, now we need to put the
         // searchItemArray in that heading.
         it->setItemResults(searchItemArray);
         // We also need to set the type of hits to the correct type now.
         it->setTypeOfHits(SEARCH_RESULTS);
         break;
      }
   }
   for (SearchItemArray::const_iterator it = searchItemArray.begin();
        it != searchItemArray.end();
        ++it) {
      cout << "Name: " << it->getName().c_str() << endl;
      cout << "Location name: " << it->getLocationName().c_str()
           << endl << endl;
   }
}
 
void
SearchTester::topRegionsChanged()
{
   
}

void
SearchTester::totalNbrOfHitsReply(RequestID requestID,
                                  SearchHeadingArray searchHeadingArray)
{
   cout << "======== totalNumberOfHitsReceived ========" << endl;
   for (SearchHeadingArray::iterator it = searchHeadingArray.begin();
        it != searchHeadingArray.end();
        ++it) {
      cout << "Heading: " << it->getName();
      cout << "   Hits: " << it->getTotalNbrHits() << endl;
   }
   cout << "============================================" << endl;
}

void
SearchTester::listAll()
{
   for (SearchHeadingArray::const_iterator it = m_searchResList.begin();
        it != m_searchResList.end();
        ++it) {
      cout << "Heading: " << it->getName().c_str() << endl;
      cout << "-------------- List of search results ---------------" << endl;
      const SearchItemArray& itemArray = it->getItemResults();
      for (SearchItemArray::const_iterator sit = itemArray.begin();
           sit != itemArray.end();
           ++sit) {
         cout << "Name: " << sit->getName().c_str() << endl;
         cout << "Location name: " << sit->getLocationName().c_str() << endl;
         cout << "Distance (m): " << sit->getDistance() << endl;
         cout << "Distance KM: " << sit->getDistance(KM) << endl;
         cout << "Distance miles: " << sit->getDistance(MILES_FEET) << endl;
         cout << "Distance yards: " << sit->getDistanceInYards() << endl;
         cout << "Distance feet: " << sit->getDistanceInFeet()
              << endl << endl;
      }
   }
}

void
SearchTester::listHeadings()
{
   wf_uint32 index = 1;
   for (SearchHeadingArray::const_iterator it = m_searchResList.begin();
        it != m_searchResList.end();
        ++it, ++index) {
      cout << index <<  " - " << it->getName().c_str()
           << " [" << it->getTotalNbrHits() << "]" << endl;
   }
}

void
SearchTester::listResults()
{
   listHeadings();
   cout << "Enter the index of the heading you want to display results from: ";
   string input = "";
   getline(cin, input);
   wf_uint32 index = atoi(input.c_str());

   if ((index - 1) < m_searchResList.size()) {
      SearchHeading heading = m_searchResList[index - 1];
      const SearchItemArray& itemArray = heading.getItemResults();

      if (heading.getTypeOfHits() == AREA_MATCHES) {
         // Area matches
         const SearchAreaArray& areaArray = heading.getAreaResults();
         wf_uint32 idx = 1;
         for (SearchAreaArray::const_iterator it = areaArray.begin();
              it != areaArray.end();
              ++it, ++idx) {
            cout << idx << " - Name: " << it->getName().c_str() << endl;
         }

         cout << "Enter the index of the area you want to use for the extended search: ";
         input.clear();
         getline(cin, input);
         wf_uint32 areaIndex = atoi(input.c_str());
         areaIndex--;
         if (areaIndex >= areaArray.size()) {
            cout << "Invalid index." << endl;
            return;
         }
         SearchArea searchItem = areaArray[areaIndex];
         m_searchInterface.extendedSearch(searchItem.getID(),
                                          searchItem.getName(),
                                          heading.getHeadingID());
      } else {
         // Search items, display them
         wf_uint32 idx = 1;
         for (SearchItemArray::const_iterator it = itemArray.begin();
              it != itemArray.end();
              ++it, ++idx) {
            cout << idx << " - Name: " << it->getName().c_str() << endl;
            cout << idx << " - Location name: "
                 << it->getLocationName().c_str() << endl << endl;
         }
      }
   } else {
      cout << "Invalid index!" << endl;
   }
}

void
SearchTester::getDetailsForItems()
{
   cout << "Get detailes for items" << endl;

   wf_uint32 index = 1;
   for (SearchHeadingArray::const_iterator it = m_searchResList.begin();
        it != m_searchResList.end();
        ++it) {
      const SearchItemArray& itemArray = it->getItemResults();
      for (SearchItemArray::const_iterator sit = itemArray.begin();
           sit != itemArray.end();
           ++sit, ++index) {
         cout << index << ". " << sit->getName().c_str() << endl;
      }
   }
   cout << "Enter the numbers for which you want detailed information seperated by comma without any spaces: " << endl;
   string input = "";
   getline(cin, input);
   char* pch;
   pch = strtok(const_cast<char*>(input.c_str()),",");
   set<wf_int> indexSet;
   while (pch != NULL) {
      cout << pch << endl;
      indexSet.insert(atoi(pch));
      pch = strtok (NULL, ",");
   }
   SearchIDArray idArray;
   index = 1;
   for (SearchHeadingArray::const_iterator it = m_searchResList.begin();
        it != m_searchResList.end();
        ++it) {
      const SearchItemArray& itemArray = it->getItemResults();
      for (SearchItemArray::const_iterator sit = itemArray.begin();
           sit != itemArray.end();
           ++sit, ++index) {
         std::set<wf_int>::const_iterator indexIt = indexSet.find(index);
         if (indexIt != indexSet.end()) {
            cout << "Adding item: " << sit->getID().c_str() << ", name: "
                 << sit->getName().c_str() << endl; 
            idArray.push_back(sit->getID());
         }
      }
   }
   AsynchronousStatus status = m_searchInterface.searchDetails(idArray);
   cout << "Request for search details returned status code: "
        << status.getStatusCode() << endl;
}

void
SearchTester::getMoreSearchResults()
{
   listHeadings();
   cout << "Enter the index of the heading you want to request more hits from: ";
   wf_uint32 index = 0;
   cin >> index;
   if ((index - 1) < m_searchResList.size()) {
      SearchHeading heading = m_searchResList[index - 1];
      wf_uint32 startIndex = heading.getCurrentPosition() + 
         heading.getItemResults().size();
      if (startIndex < heading.getTotalNbrHits()) {
         m_searchInterface.requestNextSearchResults(startIndex,
                                                    25,
                                                    heading.getHeadingID());
      } else {
         cout << "At end of list!" << endl;
      }
   } else {
      cout << "Invalid index!" << endl;
   }
}

void
SearchTester::getSearchHistory()
{
   SearchHistoryArray searchHistoryList;
   SynchronousStatus status =
      m_searchInterface.getSearchHistory(searchHistoryList);

   if (status.getStatusCode() != OK) {
      cout << "When getting search history, error: " << status.getStatusCode()
           << endl;
   } else {
      cout << "****************** Search History received *******************"
           << endl;
      for (SearchHistoryArray::const_iterator it = searchHistoryList.begin();
           it != searchHistoryList.end();
           ++it) {
         cout << "What: " << it->getWhat().c_str() << endl;
         cout << "Where: " << it->getWhere().c_str() << endl;
         cout << "Top region id: " << it->getTopRegionID() << endl;
         cout << "Category: " << it->getCategoryID() << endl << endl;
      }
   }
}
      
void 
SearchTester::addCommandOptions()
{
   m_commandOptions.
      insert(make_pair("h",
                       CommandHolder("Displays this help menu.",
                                     help)));
   m_commandOptions.
      insert(make_pair("s", 
                       CommandHolder("Do search",
                                     cmdSearch)));
   m_commandOptions.
      insert(make_pair("t", 
                       CommandHolder("Trigger test search",
                                     cmdSearchDefault)));
   m_commandOptions.
      insert(make_pair("a", 
                       CommandHolder("List all search results",
                                     cmdListAll)));
   m_commandOptions.
      insert(make_pair("c", 
                       CommandHolder("List headings from search", 
                                     cmdSearchHeadings)));
   m_commandOptions.
      insert(make_pair("r", 
                       CommandHolder("List search results", 
                                     cmdSearchResults)));
   m_commandOptions.
      insert(make_pair("d", 
                       CommandHolder("Get details of search results", 
                                     cmdGetDetails))),
   m_commandOptions.
      insert(make_pair("p", 
                       CommandHolder("Get a list of previous searches", 
                                     cmdGetSearchHistory))),
   m_commandOptions.
      insert(make_pair("n", 
                       CommandHolder("Request more hits from a heading", 
                                     cmdGetMoreResults))),
   m_commandOptions.
      insert(make_pair("b", 
                       CommandHolder("Back",
                                     back)));
   m_commandOptions.
      insert(make_pair("i", 
                       CommandHolder("Search by position",
                                     cmdSearchByPosition)));
   m_commandOptions.
      insert(make_pair("j", 
                       CommandHolder("Get search categories by position",
                                     cmdGetSearchCategories)));
}

void SearchTester::headingImagesStatusUpdated(ImageStatus currentStatus)
{
   std::cout << "HeadingImagesStatusUpdated !" << std::endl;
}

void SearchTester::categoryImagesStatusUpdated(ImageStatus currentStatus)
{
   std::cout << "HeadingImagesStatusUpdated !" << std::endl;
}

void SearchTester::resultImagesStatusUpdated(ImageStatus currentStatus)
{
   std::cout << "ResultImagesStatusUpdated !"  << std::endl;
}
