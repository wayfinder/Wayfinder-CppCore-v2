/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _SEARCHIMAGESTEST_H_
#define _SEARCHIMAGESTEST_H_

#include "RegressionTest.h"
#include "SearchListener.h"
#include "Nav2Session.h"
#include "SearchInterface.h"
#include "RequestID.h"
#include "SearchHeadingArray.h"
#include "CoreDebugPrint.h"
#include "SearchCategoryArray.h"
#include "SearchInterfaceImpl.h"
#include "WFString.h"
#include <set>
#include <string>
#include "FileSystem.h"
#include "Nav2API.h"

class SearchImagesTest : public RegressionTest, public WFAPI::SearchListener
{
public:
   SearchImagesTest(Nav2Session& session, const char* name) :
      RegressionTest(name, 10000),
      m_searchInterface(session.getNav2()->getSearchInterface()),
      m_headingImagesDownloaded(false),
      m_resultImagesDownloaded(false),
      m_categoryImagesDownloaded(false),
      m_finalSearchReplyReceived(false),
      m_imgPath(session.getCategoryImageStoragePath()),
      m_searchPos(55.5949, 13.0117) // Somewhere in Malmö
   {

   }

   virtual ~SearchImagesTest() {
      m_searchInterface.removeSearchListener(this);
   }
   
   virtual void startTest() {
      m_searchInterface.addSearchListener(this);
      
      WFAPI::SearchCategoryArray categories;
      WFAPI::SynchronousStatus catStatus =
         m_searchInterface.getSearchCategories(categories);

      if(catStatus.getStatusCode() != WFAPI::OK) {
         setTestFailed(catStatus.getStatusMessage());
         return;
      }

      if(categories.empty()) {
         setTestFailed("The available search categories are empty!");
         return;
      } else {
         for( WFAPI::SearchCategoryArray::iterator itr = categories.begin();
              itr != categories.end(); itr++)
         {
            WFAPI::SearchCategory cat(*itr);
            m_imgFiles.insert(cat.getImageName());
         }
      }
      
      WFAPI::wf_uint32 categoryID = 107; // "Post office"
   
      WFAPI::SearchQuery query("",
                               categoryID,
                               m_searchPos,
                               0,
                               0,
                               10);
   
      WFAPI::AsynchronousStatus searchStatus = m_searchInterface.search(query);

      if(!(searchStatus.getStatusCode() == WFAPI::OK)) {
         setTestFailed(searchStatus.getStatusMessage());
         return;
      }

   }

   void searchReply(WFAPI::RequestID requestID,
                    const WFAPI::SearchHeadingArray& searchHeadings,
                    bool final)
   {
      if(!final) {
         return;
      }
      
      // for(WFAPI::SearchHeadingArray::const_iterator itr = searchHeadings.begin();
      //     itr != searchHeadings.end(); itr++)
      // {
      //    const WFAPI::SearchHeading& heading(*itr);

      //    // m_imgFiles.insert(heading.getImageName());

      //    const WFAPI::SearchItemArray& itemAr(heading.getItemResults());

      //    for(WFAPI::SearchItemArray::const_iterator itemItr = itemAr.begin();
      //        itemItr != itemAr.end(); itemItr++)
      //    {
      //       const WFAPI::SearchItem& searchItem(*itemItr);
            
      //       // m_imgFiles.insert(searchItem.getImageName());
      //    }
      // }
      
      m_finalSearchReplyReceived = true;
      // coreprintln("Received final search reply");
      evalPassed();
   }
   
   /**
    * @see SearchListener::headingImagesStatusUpdated
    */
   virtual void headingImagesStatusUpdated(ImageStatus currentStatus) {
      if(currentStatus != WFAPI::SearchListener::CURRENT_IMAGES_NOT_OK )
      {
         // coreprintln("Heading images OK");
         m_headingImagesDownloaded = true;
         evalPassed();         
      } else {
         // coreprintln("Heading images NOT OK");
      }
   }

   /**
    * @see SearchListener::categoryImagesStatusUpdated
    */
   virtual void categoryImagesStatusUpdated(ImageStatus currentStatus) {
      if(currentStatus != WFAPI::SearchListener::CURRENT_IMAGES_NOT_OK )
      {
         // coreprintln("Category images OK");
         m_categoryImagesDownloaded = true;
         evalPassed();         
      } else {
         // coreprintln("Category images NOt OK");
      }
   }

   /**
    * @see SearchListener::resultImagesStatusUpdated
    */
   virtual void resultImagesStatusUpdated(ImageStatus currentStatus) {
      if(currentStatus != WFAPI::SearchListener::CURRENT_IMAGES_NOT_OK )
      {
         // coreprintln("Result images OK");
         m_resultImagesDownloaded = true;
         evalPassed();         
      } else {
         // coreprintln("Result images NOT OK");
      }
   }

   virtual void searchCategoriesUpdated() {

   }
   
   virtual void error(const WFAPI::AsynchronousStatus& status); 
private:

   void verifyDelete() {
      WFAPI::SearchInterfaceImpl* searchImpl(m_searchInterface.getImpl());

      searchImpl->clearCategoryImages();
      
      for(std::set<WFAPI::WFString>::iterator itr = m_imgFiles.begin();
          itr != m_imgFiles.end(); itr++)
      {
         std::string completePath = m_imgPath;
         
         completePath += std::string(itr->c_str());
#ifdef __SYMBIAN__
         completePath += std::string(".mif");         
#else
         completePath += std::string(".png");
#endif    
         if(FileSystem::pathExists(completePath.c_str())) {

            std::string err("File that should have been deleted is not: ");
            
            err += completePath;
            
            setTestFailed(err.c_str());
            return;
         }
      }

      m_imgFiles.clear();
      
      setTestPassed();
   }
   
   void evalPassed() {
      if(m_headingImagesDownloaded && m_resultImagesDownloaded &&
         m_categoryImagesDownloaded && m_finalSearchReplyReceived)
      {
         verifyDelete();
      }
   }
   
   WFAPI::SearchInterface& m_searchInterface;

   bool m_headingImagesDownloaded;
   bool m_resultImagesDownloaded;
   bool m_categoryImagesDownloaded;
   bool m_finalSearchReplyReceived;

   std::string m_imgPath;
   
   WFAPI::WGS84Coordinate m_searchPos;

   std::set<WFAPI::WFString> m_imgFiles;

};

#endif /* _SEARCHIMAGESTEST_H_ */
