/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
 
#ifndef WFAPI_SEARCH_STATUS_CODE_H
#define WFAPI_SEARCH_STATUS_CODE_H

#include "WFAPIConfig.h"
#include "StatusCode.h"

namespace WFAPI {

/**
 * Enumeration for search interface specific error codes that can be received.
 */
   enum SearchStatusCode
   {
      /// General internal problem regarding the search.
      GENERAL_SEARCH_ERROR = START_SEARCH_STATUS_CODE + 1,

      /// An error occured when trying to send a search request to nav2.
      UNABLE_TO_MAKE_SEARCH_REQUEST,

      /// An error occured when trying to initiate a search.
      UNABLE_TO_INITIATE_SEARCH,

      /// An error occured when requesting the next search results.
      UNABLE_TO_REQUEST_NEXT_SEARCH_RESULTS,

      /// Unable to retrive details about a specific search.
      UNABLE_TO_RETRIEVE_SEARCH_DETAILS,

      /// Unable to receive more search results.
      UNABLE_TO_RECEIVE_MORE_SEARCH_RESULTS,

      /// Unable to receive results for the area match search.
      UNABLE_TO_RECEIVE_AREA_MATCH_SEARCH_RESULTS,

      /// An error occured when trying to obtain the top regions.
      UNABLE_TO_GET_TOP_REGIONS,

      /// We have outstanding search requests and therefore not able to
      /// handle the search request. Please wait and try again.
      OUTSTANDING_SEARCH_REQUEST,

      /// Problem when retreiving the category iconS.
      UNABLE_TO_GET_CATEGORY_ICONS,

      /// There were no categories stored on disk 
      NO_CATEGORIES_IN_STORED_PARAM_FILE,

      /// Unable to fetch the category tree from server.
      UNABLE_TO_GET_CATEGORY_TREE,
   };

} // End namespace WFAPI

#endif // End WFAPI_SEARCH_STATUS_CODE_H
