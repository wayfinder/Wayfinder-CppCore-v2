/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _SEARCHREGTEST_H_
#define _SEARCHREGTEST_H_

#include "SearchListener.h"
#include "SearchInterface.h"
#include "RegressionTest.h"

#include <map>

class Nav2Session;

/**
 * Basically what this calls does is to perfrom a number of different
 * search tests. It starts out with searches that is invalid. We
 * know that these searches will result in calls to the error function.
 * This is a good way to see if this is handled or not. So after the two
 * initial bad searches we do two good searchs in two ways.
 *
 * This class could be divided into one base class that only triggers tests
 * and a number of sub classes that contais different tests and different
 * conditions for setting the test to passed on failed.
 */
class SearchRegTest : public WFAPI::SearchListener,
                      public RegressionTest
{
public:
   enum state_t { UNINITIALIZED,
                  SEARCHING_FOR,
                  ERROR,
                  DONE };
   
   SearchRegTest(Nav2Session& session, const char* name);

   virtual ~SearchRegTest();

   /**
    * Starts the test.
    */
   virtual void startTest();

   /**
    * Checks if this test if finished or not.
    *
    * @return True if done, false if not.
    */
   bool finished();

   /**
    * From SearchInterface, called when a search reply is received
    * from the server.
    *
    * @param requestID The requestID for the search request.
    * @param searchHeadings The list of search results.
    * @param final True if the reply is for the last round or not.
    */
   virtual void searchReply(WFAPI::RequestID requestID,
                            const WFAPI::SearchHeadingArray& searchHeadings,
                            bool final);

   /**
    * From SearchInterface, called when a search has triggered
    * an error in some way.
    *
    * @param status The status containing the error message.
    */
   virtual void error(const WFAPI::AsynchronousStatus& status);

   /**
    * Returns the current error.
    */
   const char* getError();

   /**
    * Enum containing indexes for the different searches that should
    * be done in this search test.
    */
   enum SearchTestId {
      positionWhatSearchTest1 = 0,
      positionWhatSearchTest2,
      positionWhatSearchTest3,
      positionCategorySearchTest1,
      searchTestFinished
   };

   /**
    * Container for a search test. Contains a search query
    * for this search and some other conditions.
    */
   struct SearchTest {

      /**
       * Default constructor, needed since the WFAPI::SearchQuery don't
       * have a default constructor, hence the query member needs to
       * be initialized.
       */
      SearchTest() :
         query(WFAPI::SearchQuery("", 0, WFAPI::WGS84Coordinate())),
         name(""),
         valid(true),
         shouldContainResults(true)
         {}

      /// The query containing the search question
      WFAPI::SearchQuery query;

      /// Name of the test
      WFAPI::WFString name;

      /**
       * If this test should return valid answers or if it
       * is ok if the test generates an error.
       */
      bool valid;

      /// If this test should contain hits or not.
      bool shouldContainResults;
   };

protected:

   /**
    * Print the search results.
    *
    * @param searchHeadings The list containing the search results.
    */
   void printSearchResults(const WFAPI::SearchHeadingArray& searchHeadings);

   /**
    * This function actually triggers a search request. It also checks
    * if the search is to be treated as done.
    */
   void doSearch();

   /**
    * This function sets up all the different search request that
    * should be included in this search test.
    */
   void setupSearchQueries();

   /**
    * Checks if all tests are done or not
    *
    * @return True if done, false if not.
    */
   bool checkIfDone();

private:

   /// The current search that is being testd.
   SearchTest m_currentSearchTest;

   /// Current index for the search.
   WFAPI::wf_uint32 m_curSearchIndex;

   /// Keeps track of the current round for the search
   WFAPI::wf_uint32 m_currRound;

   /// The search interface
   WFAPI::SearchInterface& m_searchInterface;

   /// Keeps track of the current state
   state_t m_state;

   /// The error string when something has gone wrong
   const char* m_errorString;

   /// Typedef for a map containing a search inded and a search test
   typedef std::map<WFAPI::wf_int, SearchTest> t_searchTestMap;

   /**
    * A map containing the search tests that should be done
    * with an index as key.
    */
   t_searchTestMap m_searchTestMap;
};

#endif /* _SEARCHOVERFLOWTEST_H_ */
