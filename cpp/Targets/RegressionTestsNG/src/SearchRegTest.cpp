/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#define CORE_LOGGING_MODULE_NAME "SEARCH_REG_TEST"
//#define CORE_LOGGING_MODULE_LEVEL CL_ALL 
#include "SearchRegTest.h"
#include "CoreDebugPrint.h"
#include "Nav2Session.h"
#include "Nav2API.h"
#include "RegressionTestErrorPrinting.h"

#include <string>

//#define SOFT_DEBUG_PRINTS

using namespace WFAPI;

#define NUM_SEARCH_HITS 25
#define SEARCH_HEADING 0
#define NUM_SEARCHES 2
#define MAX_ROUND 2

SearchRegTest::SearchRegTest(Nav2Session& session,
                             const char* name) :
   RegressionTest(name, 50000),
   m_curSearchIndex(0),
   m_currRound(0),
   m_searchInterface(session.getNav2()->getSearchInterface())
{
   m_state = UNINITIALIZED;
   m_errorString = NULL;
}

SearchRegTest::~SearchRegTest()
{
   m_searchInterface.removeSearchListener(this);
}

void SearchRegTest::startTest()
{
   m_state = SEARCHING_FOR;
   m_searchInterface.addSearchListener(this);

   // Set up all the searches that should be done
   setupSearchQueries();

   // Trigger the first search
   doSearch();
}

void SearchRegTest::setupSearchQueries()
{
   WGS84Coordinate validPos(55.5949, 13.0117); // Somewhere in Malmö
   WGS84Coordinate invalidPos; // Default pos
   
   SearchQuery query("Bar",
                     WF_MAX_UINT32,
                     invalidPos);

   SearchTest test;
   test.query = query;
   test.name = "First bad position test";
   test.valid = false;
   test.shouldContainResults = false;
   
   // The first two tests is bad tests that contains a
   // 180, 180 coordinate, these tests will trigger an error.
   m_searchTestMap[positionWhatSearchTest1] = test;

   test.name = "Second bad position test";
   m_searchTestMap[positionWhatSearchTest2] = test;

   // This test contains a valid position, hence it should be a valid test
   // that gives good search hits.
   test.query.setPosition(validPos);
   test.name = "First good position test";
   test.valid = true;
   test.shouldContainResults = true;
   m_searchTestMap[positionWhatSearchTest3] = test;

   // Next test is a category search based on a coordinate. This test should be
   // a valid test
   wf_uint32 categoryID = 107; // "Post office"
   SearchQuery catQuery("Post",
                        categoryID,
                        validPos);
   test.query = catQuery;
   test.name = "Category search by position, post, postal office (107)";
   test.valid = true;
   m_searchTestMap[positionCategorySearchTest1] = test;
}

void SearchRegTest::doSearch()
{
   if (checkIfDone()) {
      // Test is done
      return;
   }
   m_state = SEARCHING_FOR;
   m_currRound = 0;
   
   t_searchTestMap::const_iterator it =
      m_searchTestMap.find(SearchTestId(m_curSearchIndex));
   if (it == m_searchTestMap.end()) {
      // This could be used later for knowing when all tests are done.
      setTestFailed("Invalid test id");
      return;
   }

   // For convenience, keep the current search in m_currentSearchTest.
   // This prevents us from getting the current search test from the map.
   m_currentSearchTest = (*it).second;
   AsynchronousStatus searchStatus =
      m_searchInterface.search(m_currentSearchTest.query);

   if(searchStatus.getStatusCode() != OK) {
      coreprintln_error("%s, search returned error: %d",
                        __FUNCTION__, searchStatus.getStatusCode());
      setTestFailed(searchStatus.getStatusMessage());
   } else {
      coreprintln_info("----- [Starting search: %s] -----",
                       m_currentSearchTest.name.c_str());
   }
}

bool SearchRegTest::checkIfDone()
{
   if (finished()) {
      if (m_state == ERROR) {
         setTestFailed(m_errorString);
      } else {
         setTestPassed();
      }
      return true;
   }
   return false;
}

bool SearchRegTest::finished()
{
   return m_curSearchIndex >= searchTestFinished &&
      (m_state == DONE || m_state == ERROR);
}

void
SearchRegTest::searchReply(RequestID requestID,
                           const SearchHeadingArray& searchHeadings,
                           bool final)
{
   m_currRound++;
   if (!final) {
      return;
   }
   

   if (m_currentSearchTest.shouldContainResults && searchHeadings.empty()) {
      // The test should contain results but it doesnt, the test has failed.
      coreprintln_error("Search heading is empty");
      m_state = ERROR;
      std::string stringName = m_currentSearchTest.name.c_str();
      std::string errorString = "Search Test: " + stringName + 
         "failed due to no results was found";
      setTestFailed(errorString.c_str());
   } else if (!searchHeadings.empty()) {
      // The search should contain results and it did, display the search results.
#ifdef SOFT_DEBUG_PRINTS
      printSearchResults(searchHeadings);
#endif
      m_state = DONE;
   }

   m_curSearchIndex++;
   doSearch();
}

void SearchRegTest::error(const WFAPI::AsynchronousStatus& status)
{
   m_currRound++;
#ifdef SOFT_DEBUG_PRINTS
   dumpError(status);
#endif
   
   if (m_currRound < MAX_ROUND) {
      // coreprintln_info("%s, we are waiting for more replies from the server",
      //                  __FUNCTION__);
   } else if (m_currRound == MAX_ROUND && m_curSearchIndex != searchTestFinished) {
      // coreprintln_info("We have more searches to do, triggering a new search");
      m_curSearchIndex++;
      doSearch();
   } else {
      setTestFailed(status.getStatusMessage());
      m_state = ERROR;
   }
}

void SearchRegTest::printSearchResults(const SearchHeadingArray& searchHeadings)
{
   for (SearchHeadingArray::const_iterator it = searchHeadings.begin();
        it != searchHeadings.end();
        ++it) {
      wf_int32 i = 0;
      for(SearchItemArray::const_iterator itr = it->getItemResults().begin();
          itr != it->getItemResults().end(); itr++)
      {
         coreprintln("   -  [%d] - %s, d: %d m.",
                     i++,
                     itr->getName().c_str(), itr->getDistanceFromSearchPos());
      }
   }
}
