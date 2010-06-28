/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "InvalidInputSearchTest.h"
#include "CoreDebugPrint.h"
#include "Nav2Session.h"
#include "Nav2API.h"
#include "RegressionTestErrorPrinting.h"

#include <string>

#define ERR_FUNC_NAME "error"
#define SEARCH_REPLY_FUNC_NAME "searchReply"

using namespace WFAPI;

InvalidInputSearchTest::InvalidInputSearchTest(Nav2Session& session,
                                               const char* name)
   : RegressionTest(name, 50000),
     m_curSearchIndex(0),
     m_searchInterface(session.getNav2()->getSearchInterface())
{
   m_state = UNINITIALIZED;
}

InvalidInputSearchTest::~InvalidInputSearchTest()
{
   m_searchInterface.removeSearchListener(this);
}

void InvalidInputSearchTest::startTest()
{
   m_state = SEARCHING_FOR;
   m_searchInterface.addSearchListener(this);

   // Set up all the searches that should be done
   setupSearchQueries();

   // Trigger the first search
   doSearch();   
}

void InvalidInputSearchTest::searchReply(WFAPI::RequestID requestID,
                                         const WFAPI::SearchHeadingArray& searchHeadings,
                                         bool final)
{
   bool ret = nonTickedRepliesExists(SEARCH_REPLY_FUNC_NAME,
                                     m_currentSearchTest.resultPolicy);

   if (!ret) {
      if (m_currentSearchTest.resultPolicy.numberRepliesRequired !=
          m_currentSearchTest.resultPolicy.numberRepliesGotten) {
         setTestFailed("Missmatch between number of required replies"
                       " and number of gotten replies");
      } else {
         m_curSearchIndex++;
         m_state = DONE;
         if (checkIfDone()) {
            // We are done with the test
            setTestPassed();
         } else {
            doSearch();
         }
      }
   }    
}

void InvalidInputSearchTest::error(const WFAPI::AsynchronousStatus& status)
{
   bool ret = nonTickedRepliesExists(ERR_FUNC_NAME,
                                     m_currentSearchTest.resultPolicy);
   if (ret) {
      
   }

   if (!ret) {
      m_curSearchIndex++;
      m_state = DONE;
      if (checkIfDone()) {
         // We are done with the test
         setTestPassed();
      } else {
         doSearch();
      }
   }
}

const char* InvalidInputSearchTest::getError()
{
   return "ERROR";
}

void InvalidInputSearchTest::setupSearchQueries()
{
   WGS84Coordinate validPos(55.5949, 13.0117); // Somewhere in Malmö
   SearchQuery query("#$",
                     WF_MAX_UINT32,
                     validPos);

   SearchTest test;
   test.query = query;
   test.name = "Invalid input test";
   // ResultPolicy resultPolicy;
   test.resultPolicy.numberRepliesRequired = 3;
   test.resultPolicy.numberRepliesGotten = 0;
   test.resultPolicy.policyVec.push_back(
      ResultPolicy::ResultType(ERR_FUNC_NAME, false));
   test.resultPolicy.policyVec.push_back(
      ResultPolicy::ResultType(ERR_FUNC_NAME, false));
   test.resultPolicy.policyVec.push_back(
      ResultPolicy::ResultType(SEARCH_REPLY_FUNC_NAME, false));
//   test.resultPolicy
   m_searchTestMap[invalidInputTest1] = test;
}

bool
InvalidInputSearchTest::nonTickedRepliesExists(WFAPI::WFString funcName,
                                               ResultPolicy& curTest)
{
   curTest.numberRepliesGotten++;
   bool foundNonTicked = false;
   bool ticked = false;
   ResultPolicy::t_policyVec::iterator it = curTest.policyVec.begin();
   for (; it < curTest.policyVec.end(); ++it) {
      if (it->function == funcName && it->ticked == false && ticked == false) {
         // foundNonTicked = true;
         it->ticked = true;
         ticked = true;
         //break;
      } else if (it->ticked == false) {
         foundNonTicked = true;
      }
   }
   return foundNonTicked;
}

bool InvalidInputSearchTest::checkIfDone()
{
   if (finished()) {
      if (m_state == ERROR) {
         setTestFailed("Some error has occured");
      } else {
         setTestPassed();
      }
      return true;
   }
   return false;
    
}

bool InvalidInputSearchTest::finished()
{
   return m_curSearchIndex >= searchTestFinished &&
      (m_state == DONE || m_state == ERROR);    
}

void InvalidInputSearchTest::doSearch()
{
   if (checkIfDone()) {
      // Test is done
      return;
   }
   m_state = SEARCHING_FOR;
   
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
