/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#define CORE_LOGGING_MODULE_NAME "SEARCH_OVERFLOW_TEST"

#include "SearchOverflowTest.h"
#include "SearchCategoryArray.h"
#include "CoreDebugPrint.h"
#include "Nav2Session.h"
#include "Nav2API.h"
#include "RegressionTestErrorPrinting.h"

using namespace WFAPI;

#define NUM_SEARCH_HITS 25
#define SEARCH_HEADING 0
// #define SOFT_DEBUG_PRINTS
// #define SOFT_DEBUG_PRINT_RESULTS

SearchOverflowTest::SearchOverflowTest(Nav2Session& session,
                                       const char* name) :
   RegressionTest(name, 50000),
   m_searchInterface(session.getNav2()->getSearchInterface())
{
   m_state = UNINITIALIZED;
   m_curIndex = 0;
   m_totalHits = 0;
   m_prevDist = 0;
   m_errorString = NULL;
}


SearchOverflowTest::~SearchOverflowTest()
{
   m_searchInterface.removeSearchListener(this);
}

void SearchOverflowTest::startTest()
{
#ifdef SOFT_DEBUG_PRINTS
   coreprintln("SearchOverflowTest::startTest()");
#endif
   m_state = SEARCHING_FOR;
   m_searchInterface.addSearchListener(this);

   wf_uint32 categoryID = 107; // "Post office"
   WGS84Coordinate pos(55.5949, 13.0117); // Somewhere in Malmö
   
   SearchQuery query("",
                     categoryID,
                     pos,
                     0,
                     0, NUM_SEARCH_HITS);
   
   AsynchronousStatus searchStatus = m_searchInterface.search(query);

   if(!(searchStatus.getStatusCode() == OK)) {
      setTestFailed(searchStatus.getStatusMessage());
   }
}

bool SearchOverflowTest::finished()
{
   return m_state == DONE || m_state == ERROR; 
}

void
SearchOverflowTest::searchReply(RequestID requestID,
                                const SearchHeadingArray& searchHeadings,
                                bool final)
{
#ifdef SOFT_DEBUG_PRINTS
   coreprintln("SearchOverflowTest::searchReply()");
#endif
   if (!final) {
      return;
   }
   if (searchHeadings.empty()) {
      m_errorString = "No search headings found";
      m_state = ERROR;
      return;
   }

   const SearchHeading& sHeading = searchHeadings[SEARCH_HEADING];
      
#ifdef SOFT_DEBUG_PRINT_RESULTS
   unsigned int i = 0;
      
   for(SearchItemArray::const_iterator itr = sHeading.getItemResults().begin();
       itr != sHeading.getItemResults().end(); itr++)
   {
      const SearchItem& s = *itr;

      int diffPrev = m_prevDist - s.getDistanceFromSearchPos();

      coreprintln("   -  [%d] - %s, d: %d m.",
                  i++,
                  s.getName().c_str(), s.getDistanceFromSearchPos());
         
      m_prevDist = s.getDistance();
   }
#endif

   m_results.push_back(sHeading.getItemResults());
   
   m_totalHits = sHeading.getTotalNbrHits();
   m_curIndex = sHeading.getItemResults().size();
      
   m_state = REQUESTING_UNTIL_END;

   // sleep(1);
      
   m_searchInterface.requestNextSearchResults(m_curIndex,
                                              NUM_SEARCH_HITS,
                                              SEARCH_HEADING);
}

void
SearchOverflowTest::nextSearchReply(RequestID requestID,
                                    const SearchItemArray& searchItemArray,
                                    wf_uint32 heading)
{
   if(m_state == ERROR) {
      coreprintln("- got NSR when an error previously occured.");
      return;
   }
   
   unsigned int vecIndex = m_curIndex / NUM_SEARCH_HITS;

   if(vecIndex >= m_results.size()) {

      // A set of results that has not been seen before has now arrived.
      // We go over the previous set of results and see if any item in this
      // new set is the same as any in the previous

      for(std::vector<SearchItemArray>::iterator itr = m_results.begin();
          itr != m_results.end(); itr++)
      {
         SearchItemArray s(*itr);
         for(SearchItemArray::iterator resItr = s.begin();
             resItr != s.end(); resItr++)
         {
            const SearchItem& resItem = *resItr;
            for(SearchItemArray::const_iterator newItr = searchItemArray.begin();
                newItr != searchItemArray.end(); newItr++)
            {
               const SearchItem& newItem = *newItr;

               if(resItem.getID() == newItem.getID()) {
                  setTestFailed("Duplicate SearchItems were discovered "
                                "in the results");
                  m_state = ERROR;
                  return;
               }
            }
         }
      }      
      m_results.push_back(searchItemArray);
   } else {
      if(searchItemArray.size() != m_results[vecIndex].size()) {
         setTestFailed("A SearchItemArray for a specific index was "
                       "different in size the second time when stepping "
                       "backwards in the search results.");
         m_state = ERROR;
         return;
      }
   }

#ifdef SOFT_DEBUG_PRINT_RESULTS
   coreprintln("- got NSR with %d hits (%d total hits). Start index = %d.",
               searchItemArray.size(), m_totalHits, m_curIndex);

   unsigned int i = m_curIndex;

   for(SearchItemArray::const_iterator itr = searchItemArray.begin();
       itr != searchItemArray.end(); itr++)
   {
      const SearchItem& s = *itr;

      int diffPrev = m_prevDist - s.getDistanceFromSearchPos();

      coreprintln("  -  [%d] - %s, d: %d m.",
                  i++, s.getName().c_str(), s.getDistanceFromSearchPos());
         
      m_prevDist = s.getDistance();
   }
#endif
   
   if(m_state == REQUESTING_UNTIL_END) {
      m_curIndex += NUM_SEARCH_HITS;
   
      if(m_curIndex > m_totalHits) {
         m_curIndex -= NUM_SEARCH_HITS * 2;
         m_state = REQUESTING_BACK_ONE;

         m_searchInterface.requestNextSearchResults(m_curIndex,
                                                    NUM_SEARCH_HITS,
                                                    SEARCH_HEADING);
         
#ifdef SOFT_DEBUG_PRINTS
         coreprintln("- Requesting prev. NEW INDEX: %d", m_curIndex);
#endif

      } else {
         
#ifdef SOFT_DEBUG_PRINTS
         coreprintln("- Requesting forw. until end. NEW INDEX: %d",
                     m_curIndex);
#endif
         // sleep(1);
         
         m_searchInterface.requestNextSearchResults(m_curIndex,
                                                    NUM_SEARCH_HITS,
                                                    SEARCH_HEADING);
      }
   } else if(m_state == REQUESTING_BACK_ONE) {
      
#ifdef SOFT_DEBUG_PRINTS
      coreprintln("- Requesting forward one step. NEW INDEX = %d",
                  m_curIndex);
#endif

      m_state = REQUESTING_FORWARD_END;
      m_curIndex += NUM_SEARCH_HITS;
      // sleep(1);
      m_searchInterface.requestNextSearchResults(m_curIndex,
                                                 NUM_SEARCH_HITS,
                                                 SEARCH_HEADING);
      
   } else if(m_state == REQUESTING_FORWARD_END) {
      setTestPassed();
   }
}

void SearchOverflowTest::error(const WFAPI::AsynchronousStatus& status)
{
   dumpError(status);
   
#ifdef SOFT_DEBUG_PRINTS
   coreprintln("SearchOverflowTest::error()");
#endif
   setTestFailed(status.getStatusMessage());
   m_state = ERROR; 
}
