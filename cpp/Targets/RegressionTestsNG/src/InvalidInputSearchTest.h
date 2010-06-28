/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _INVALID_INPUT_SEARCH_TEST_H_
#define _INVALID_INPUT_SEARCH_TEST_H_


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
class InvalidInputSearchTest : public WFAPI::SearchListener,
                               public RegressionTest
{
public:
   enum state_t { UNINITIALIZED,
                  SEARCHING_FOR,
                  ERROR,
                  DONE };
   
   InvalidInputSearchTest(Nav2Session& session, const char* name);

   virtual ~InvalidInputSearchTest();

   
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
      invalidInputTest1 = 0,
      searchTestFinished
   };

   struct ResultPolicy {

      /**
       * Number of replies from the SearchInterface that is required
       * to say that the search is ok
       */
      WFAPI::wf_uint16 numberRepliesRequired;

      /// Number of replies that we have gotten so far
      WFAPI::wf_uint16 numberRepliesGotten;

      /**
       * Small struct that represents a function call. We will have
       * function name and if it is ticked, that is if it has been
       * called or not.
       */
      struct ResultType {
         ResultType(WFAPI::WFString function, bool ticked) :
            function(function), ticked(ticked) {}
         WFAPI::WFString function;
         bool ticked;
      };

      /**
       * Vector of ResultType, we can add which functions that
       * has been called or not
       */
      typedef std::vector<ResultType> t_policyVec;
      t_policyVec policyVec;
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
         name("")
         {}

      /// The query containing the search question
      WFAPI::SearchQuery query;

      /// Name of the test
      WFAPI::WFString name;

      /// The result policy for this search test.
      ResultPolicy resultPolicy;
   };


protected:

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

   /**
    * Checks if there is unticked functions that we are
    * waiting for or if we are ready with this search.
    *
    * @param funcNam The name of the function that was called.
    * @param curTest The ResultPolicy of the current test.
    * @return True if non ticked function calls was found, false if
    *         none where found.
    */
   bool nonTickedRepliesExists(WFAPI::WFString funcName,
                               ResultPolicy& curTest);

private:

   /// The current search that is being testd.
   SearchTest m_currentSearchTest;

   /// Current index for the search.
   WFAPI::wf_uint32 m_curSearchIndex;

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

#endif /* _INVALID_INPUT_SEARCH_TEST_H_ */
