/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef SEARCH_TESTER_H
#define SEARCH_TESTER_H

#include "SearchListener.h"
#include "Nav2API.h"
#include "RequestID.h"
#include "WFStringArray.h"
#include "SearchItemArray.h"
#include "SearchHeadingArray.h"
#include "CommandOption.h"
#include "GenericTester.h"
#include <vector>

using namespace WFAPI;

/**
 * Test class that implements route and navigation testing.
 */
class SearchTester : public GenericTester,
                     public SearchListener
{

public:
   
   /**
    * Constructor, adds itself as listener to FavouriteInterface
    *
    * @param favouriteInterface The favourite interface which to add
    *                           ourself as listener.
    */
   SearchTester(SearchInterface& searchInterface);
                   
   /**
    * Destructor.
    */
   ~SearchTester();

   void searchCategories();
   
   /**
    * Performes a search. The user has to fill in what, where and country.
    */
   void search();

   /**
    * Performes a search. The user has to fill in what, search is made by using
    * the users position.
    */
   void searchByPos();
   
   /**
    * Performs a search test with hard coded parameters.
    */
   void searchTest();

   /**
    * Lists all search results from all headings.
    */
   void listAll();

   /**
    * Lists all headings and the number of results in each of them.
    */
   void listHeadings();

   /**
    * Lists the results from a heading, user will be prompted to input
    * the index of which heading to display the results for.
    */
   void listResults();

   /**
    * Get details of search items. The user will get a list of search
    * items and the he/she can enter indexes seperated by comma to get
    * detailed information about these.
    */
   void getDetailsForItems();

   /**
    * Requests more search results from a specific heading.
    */
   void getMoreSearchResults();

   /**
    * Returns the search history.
    */
   void getSearchHistory();

public: // From base class

   /**
    * Called when an error has occured.
    */
   virtual void error(const AsynchronousStatus& status);

   /**
    * A reply for a search details.
    *
    * @param requestID The identification of the search details request
    *                  sent from the SearchInterface.
    * @param searchItemArray The SearchItems with the details.
    */
   virtual void searchDetailsReply(RequestID requestID,
                                   const SearchItemArray& searchItemArray);
   /**
    * A reply for a search.
    *
    * @param requestID The identification of the search request
    *                  sent from the SearchInterface.
    * @param searchHeadings The reply headings with the search results.
    * @param final If this is the final reply or there will be more calls
    *              following this one. The same RequestID will be used for
    *              all the replies.
    */
   virtual void searchReply(RequestID requestID,
                            const SearchHeadingArray& searchHeadings,
                            bool final);

   /**
    * A reply for a request for more search results for a specific
    * heading.
    *
    * @param requestID The identification of the search request
    *                  sent from the SerchInterface.
    * @param searchItemArray List of the requested search results.
    * @param headnig The heading that the search results belongs to,
    *                it is the same heading as the caller requested
    *                more search results for.
    */
   virtual void nextSearchReply(RequestID requestID,
                                const SearchItemArray& searchItemArray,
                                wf_uint32 heading);

   /**
    * Contains a reply for an issued search of categories.
    *
    * @param requestID The identification of the search request
    *                  sent from the SearchInterface.
    * @param categories The resulting categories.
    *
    */
   virtual void searchCategoriesUpdated();
   
   /**
    * A reply for a extended search request.
    *
    * @param requestID The identification of the search request
    *                  sent from the SerchInterface.
    * @param searchItemArray List of the requested search results.
    * @param headnig The heading that the search results belongs to,
    *                it is the same heading as the caller requested
    *                more search results for.
    */
   virtual void extendedSearchReply(RequestID requestID,
                                    const SearchItemArray& searchItemArray,
                                    wf_uint32 heading);

   /**
    * Called when the current top regions has been changed, for example
    * after a end user has bought access to a new country. 
    */
   virtual void topRegionsChanged();


   /**
    * A reply with the total number of hits
    *
    * @param requestID The identification of the search request sent from the
    *                  the SearchInterface.
    * @param searchHeadingArray A list that contains the total number of hits
    *                           for each heading.
    */ 
   virtual void totalNbrOfHitsReply(RequestID requestID,
                                    SearchHeadingArray searchHeadingArray);
      
   /**
    * Handle the input and return true if it was consumed.
    *
    * @param input The input to handle.
    * @param goBack Out parameter. Will be set to true if the
    *               input indicated that the tester should go
    *               out of scope.
    * @return True if the input was handled, false otherwise.
    */
   virtual bool handleInput(const char* input, bool& goBack);

   /**
    * This function will be called after a search to indicate the current
    * status of the search heading images.
    *
    * @param currentStatus The current status of the images.
    */
   virtual void headingImagesStatusUpdated(ImageStatus currentStatus);

   /**
    * This function will be called after a search to indicate the current
    * status of the search category images.
    *
    * @param currentStatus The current status of the images.
    */
   virtual void categoryImagesStatusUpdated(ImageStatus currentStatus);

   /**
    * This function will be called after a search to indicate the current
    * status of the search result images.
    *
    * @param currentStatus The current status of the images.
    */
   virtual void resultImagesStatusUpdated(ImageStatus currentStatus);

private:

   /**
    * Adds the command options valid for this class. The storage is a map
    * where the key is a String and the value is a CommandHolder.
    * The CommandHolder holds the actual command to perform so by finding
    * in the map after user input we can map this against a real command.
    */
   void addCommandOptions();

   /// the SearchInterface
   SearchInterface& m_searchInterface;

   /// Map containing all the menu options for this class.
   StringMap m_searchCommandOptions;

   /// List of all the search results
   SearchHeadingArray m_searchResList;
};

#endif
