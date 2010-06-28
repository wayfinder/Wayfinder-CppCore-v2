/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_SEARCH_LISTENER_H
#define WFAPI_SEARCH_LISTENER_H

#include "Listener.h"
#include "SearchHeadingArray.h"

namespace WFAPI {

class SearchCategoryArray;

/**
 * Listener for Search events. The abstract functions are the
 * callback functions that is called for the respective asynchronous
 * function call in SearchInterface.
 *
 * Create a subclass and implement the callbacks and add a 
 * subclass object to the SearchInterface to start getting
 * search callbacks.
 *
 * The error function, declared in the super class Listener, will be called
 * for all requests that returns with an error.
 */
class SearchListener : public Listener
{
   
public:

   /**
    * This enum describes different statuses for image sets.
    * It is used in the ...ImageStatusUpdated callback functions.
    */
   enum ImageStatus {
      /// The current images are ok to be used.
      CURRENT_IMAGES_OK,
      /// The current images are not ok and will be deleted.
      CURRENT_IMAGES_NOT_OK,
      /// The images have been updated and are ok to use.
      UPDATED_IMAGES_OK
   };
   
   /**
    * Constructor.
    */
   SearchListener();
   
   /**
    * Destructor.
    */
   virtual ~SearchListener();

   /**
    * A reply for a search details.
    *
    * @param requestID The identification of the search details request
    *                  sent from the SearchInterface.
    * @param searchItemArray The SearchItems with the details.
    */
   virtual void searchDetailsReply(RequestID requestID,
                                   const SearchItemArray& searchItemArray) {}
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
                            bool final) {}

   /**
    * Contains a reply for an issued search of categories.
    */
   virtual void searchCategoriesUpdated() {}
   
   /**
    * A reply for a request for more search results for a specific
    * heading.
    *
    * @param requestID The identification of the search request
    *                  sent from the SerchInterface.
    * @param searchItemArray List of the requested search results.
    * @param heading The heading that the search results belongs to,
    *                it is the same heading as the caller requested
    *                more search results for.
    */
   virtual void nextSearchReply(RequestID requestID,
                                const SearchItemArray& searchItemArray,
                                wf_uint32 heading) {}

   /**
    * A reply for a extended search request.
    *
    * @param requestID The identification of the search request
    *                  sent from the SerchInterface.
    * @param searchItemArray List of the requested search results.
    * @param heading The heading that the search results belongs to,
    *                it is the same heading as the caller requested
    *                more search results for.
    */
   virtual void extendedSearchReply(RequestID requestID,
                                    const SearchItemArray& searchItemArray,
                                    wf_uint32 heading) {}
   
   /**
    * Called when the current top regions has been changed, for example
    * after a end user has bought access to a new country. 
    */
   virtual void topRegionsChanged() {}

   /**
    * A reply with the total number of hits
    *
    * @param requestID The identification of the search request sent from the
    *                  the SearchInterface.
    * @param searchHeadingArray A list that contains the total number of hits
    *                           for each heading.
    */ 
   virtual void totalNbrOfHitsReply(RequestID requestID,
                                    SearchHeadingArray searchHeadingArray) {}
   /**
    * This function will be called after a search to indicate the current
    * status of the search heading images.
    *
    * @param currentStatus The current status of the images.
    */
   virtual void headingImagesStatusUpdated(ImageStatus currentStatus) {}

   /**
    * This function will be called after a search to indicate the current
    * status of the search category images.
    *
    * @param currentStatus The current status of the images.
    */
   virtual void categoryImagesStatusUpdated(ImageStatus currentStatus) {}

   /**
    * This function will be called after a search to indicate the current
    * status of the search result images.
    *
    * @param currentStatus The current status of the images.
    */
   virtual void resultImagesStatusUpdated(ImageStatus currentStatus) {}

   /**
    * A reply to the category tree update request.
    * This does not necesarily mean that the category tree has been updated
    * since there might not be any other category tree for the coordinates
    * supplied in the request.
    *
    * @param requestID The identification of the category tree request
    *                  sent from the SearchInterface.
    * @param treeUpdated If this is true it means that we have received an
    *                    updated version of the category tree.
    *                    False if the category tree is up to date.
    */
   virtual void categoryTreeReceived(RequestID requestID,
                                     bool treeUpdated) {}
};

} // End namespace WFAPI

#endif // End WFAPI_SEARCH_LISTENER_H
