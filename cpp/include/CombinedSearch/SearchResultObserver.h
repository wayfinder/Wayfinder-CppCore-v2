/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef SEARCH_RESULT_OBSERVER_H
#define SEARCH_RESULT_OBSERVER_H

#include "arch.h"
#include <vector>
#include <map>
#include "CombinedSearchDataTypes.h"
#include "GuiProt/AdditionalInfo.h"

class MSearchResultObserver
{
public:

   struct strCmp {
      bool operator()(const char* s1, const char* s2) const {
         return strcmp(s1, s2) < 0;
      }
      bool operator()(char* s1, char* s2) const {
         return strcmp(s1, s2) < 0;
      }
      bool operator()(const char* s1, char* s2) const {
         return strcmp(s1, s2) < 0;
      }
      bool operator()(char* s1, const char* s2) const {
         return strcmp(s1, s2) < 0;
      }
   };
   
   typedef std::vector<isab::AdditionalInfo*> AdditionalInfoVec;
   
   typedef std::map<char*, AdditionalInfoVec, strCmp > idToAdditionalInfoMap;

   enum ImageStatus {
      /// The current images are ok to be used.
      CURRENT_IMAGES_OK,
      /// The current images are not ok and will be deleted.
      CURRENT_IMAGES_NOT_OK,
      /// The images have been updated and are ok to use.
      UPDATED_IMAGES_OK
   };
   
   virtual ~MSearchResultObserver() { }
   
   /**
    * Call to the observer from the search module when headings
    * have been received from the server. 
    */
   virtual void SearchHeadingsReceived() = 0;

   /**
    * Called when the statis for heading images has been updated.
    * @param currentStatus The updated status.
    */
   virtual void HeadingImagesStatusUpdated(ImageStatus currentStatus) = 0;

   /**
    * Called when the statis for category images has been updated.
    * @param currentStatus The updated status.
    */
   virtual void CategoryImagesStatusUpdated(ImageStatus currentStatus) = 0;

   /**
    * Called when the statis for result images has been updated.
    * @param currentStatus The updated status.
    */
   virtual void ResultImagesStatusUpdated(ImageStatus currentStatus) = 0;

   /**
    * Call to the observer from the search module when search
    * results has been received and total number of hits has
    * been calculated.
    */
   virtual void TotalNbrHitsReceived(const std::vector<CombinedSearchCategory*>& categories) = 0;

   /**
    * Call to the observer from the search module when search
    * results has been received and parsed.
    */
   virtual void SearchResultReceived() = 0;

   /**
    * Call to the observer from the search module when search
    * results has been received and parsed, a reply to a request
    * to the search module for fetching more search results for 
    * a given heading.
    */
   virtual void MoreSearchResultReceived() = 0;

   /**
    * Call to the observer from the search module when search
    * results has been received and parsed, a reply to a request
    * to the search module for doing a area match search.
    */
   virtual void AreaMatchSearchResultReceived() = 0;

   /**
    * Call to the observer when additional information has been
    * parsed.
    */
   virtual void AdditionalInfoReceived(
      const idToAdditionalInfoMap& additionalInfoMap) = 0;
};

#endif
