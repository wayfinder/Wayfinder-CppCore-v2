/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#define CORE_LOGGING_MODULE_NAME "FAVOURITE_REG_TEST"
#define CORE_LOGGING_MODULE_LEVEL CL_ALL 
#include "FavouriteRegTest.h"
#include "CoreDebugPrint.h"
#include "Nav2Session.h"
#include "Nav2API.h"
#include "RegressionTestErrorPrinting.h"
#include "FavouriteStatusCode.h"

#include "ItemInfoEntryArray.h"
#include "FavouriteArray.h"

#include <string>

#include <time.h>
#include <algorithm>

using namespace WFAPI;
using namespace std;

bool reqIdFound(const FavouriteRegTest::t_reqIdVec& vec, const RequestID& id)
{
   FavouriteRegTest::t_reqIdVec::const_iterator it =
      std::find(vec.begin(), vec.end(), id);
   return it != vec.end();
}

FavouriteRegTest::FavouriteRegTest(Nav2Session& session, const char* name)
   : RegressionTest(name, 50000),
     m_favouriteInterface(session.getNav2()->getFavouriteInterface()),
     m_numFavAdded(0),
     m_gotGoodReplyForSync(false),
     m_gotOkErrorMsgForSync(false)
{
   m_state = UNINITIALIZED;
}

FavouriteRegTest::~FavouriteRegTest()
{
   m_favouriteInterface.removeFavouriteListener(this); 
}

void FavouriteRegTest::startTest()
{
   m_favouriteInterface.addFavouriteListener(this);

   m_state = ADD_FAV;

   createFavourites();

   addFavourites();
}

bool FavouriteRegTest::finished()
{
   return (m_state == DONE || m_state == ERROR);
}

void
FavouriteRegTest::createFavourites()
{
   srand((unsigned) time(NULL));

   string favName;

   const int length = 10;
   
   for (int i = 0; i < length; i++) {
      favName.insert(favName.end(), (char) (rand() % ('z' - '0' + 1) + '0'));
   }
   
   Favourite fav = createFavourite(favName, favName);
   m_favTestMap.insert(make_pair(0, fav));
}

void
FavouriteRegTest::addFavourites()
{
   t_favouriteMap::iterator it = m_favTestMap.begin();
   for(; it != m_favTestMap.end(); ++it) {
      m_favouriteInterface.addFavourite(it->second);
   }
}

Favourite
FavouriteRegTest::createFavourite(string name, string desc)
{
   ItemInfoArray additionalInfo;
   additionalInfo.push_back(ItemInfoEntry(WFString("Text"),
                                          WFString("bla bla"),
                                          TEXT));
   additionalInfo.push_back(ItemInfoEntry(WFString("URL"),
                                          WFString("www.idg.se"),
                                          URL));
   additionalInfo.push_back(ItemInfoEntry(WFString("Phone number"),
                                          WFString("8955847 584"),
                                          PHONE_NUMBER));
   /*additionalInfo.push_back(ItemInfoEntry(WFString("Image url"),
                                          WFString("www.enbild.se/bild.jpg"),
                                          IMAGE_URL));*/
   additionalInfo.push_back(ItemInfoEntry(WFString("Email_address"),
                                          WFString("hej@hej.se"),
                                          EMAIL_ADDRESS));
   additionalInfo.push_back(ItemInfoEntry(WFString("Long desc"),
                                          WFString("bla bla bla"),
                                          LONG_DESCRIPTION));
   additionalInfo.push_back(ItemInfoEntry(WFString("Dont show"),
                                          WFString("secret information"),
                                          DONT_SHOW));
   additionalInfo.push_back(ItemInfoEntry(WFString("Short info"),
                                          WFString("bla"),
                                          SHORT_INFO));
   additionalInfo.push_back(ItemInfoEntry(WFString("Address"),
                                          WFString("Baravagen 1"),
                                          ADDRESS));
   additionalInfo.push_back(ItemInfoEntry(WFString("House number"),
                                          WFString("1"),
                                          HOUSE_NUMBER));
   additionalInfo.push_back(ItemInfoEntry(WFString("PARSED_ZIP_CODE"),
                                          WFString("parsed zip code"),
                                          PARSED_ZIP_CODE));
   additionalInfo.push_back(ItemInfoEntry(WFString("COMPLETE_ZIP"),
                                          WFString("complete zip"),
                                          COMPLETE_ZIP));
   additionalInfo.push_back(ItemInfoEntry(WFString("PARSED_ZIP_AREA"),
                                          WFString("parsed zip area"),
                                          PARSED_ZIP_AREA));
   additionalInfo.push_back(ItemInfoEntry(WFString("FULL_ADDRESS"),
                                          WFString("full address"),
                                          FULL_ADDRESS));
   additionalInfo.push_back(ItemInfoEntry(WFString("CITYPART"),
                                          WFString("citypart"),
                                          CITYPART));
   additionalInfo.push_back(ItemInfoEntry(WFString("SUPPLIER"),
                                          WFString("supplier"),
                                          SUPPLIER));
   additionalInfo.push_back(ItemInfoEntry(WFString("POST_ADDRESS"),
                                          WFString("post adress"),
                                          POST_ADDRESS));
   additionalInfo.push_back(ItemInfoEntry(WFString("POST_ZIP_AREA"),
                                          WFString("post zip area"),
                                          POST_ZIP_AREA));
   additionalInfo.push_back(ItemInfoEntry(WFString("POST_ZIP_CODE"),
                                          WFString("post zip code"),
                                          POST_ZIP_CODE));
   additionalInfo.push_back(ItemInfoEntry(WFString("NEIGHBORHOOD"),
                                          WFString("neighborhood"),
                                          NEIGHBORHOOD));

   return Favourite(Favourite::INVALID_FAVOURITE_ID,
                    WGS84Coordinate(45,45),
                    name.c_str(),
                    desc.c_str(),
                    additionalInfo);
}

void FavouriteRegTest::favouritesChanged() 
{
   m_numFavAdded++;
   if(m_numFavAdded == m_favTestMap.size()) {
      m_state = SYNC_FAV;
      m_allReqId.push_back(m_favouriteInterface.syncFavourites().getRequestID());
   }
}

void FavouriteRegTest::favouritesSynced(RequestID requestID)
{
   if (!reqIdFound(m_allReqId, requestID)) {
      return;
   }
   if(m_state == SYNC_FAV) {
      checkSyncedFavourites();
   } else if (m_state == SYNC_FAV_ONGOING) {
      startSyncTest();
   } else if(m_state == START_SYNC_TEST) {
      if (reqIdFound(m_reqIdVec, requestID)) {
         m_gotGoodReplyForSync = true;
      }
      if (checkIfSyncTestIsDone()) {
         m_state = DONE;
         setTestPassed();
      }
   } else if(m_state == CHECK_DONE) {
      m_state = DONE;
      setTestPassed();
   }
}

void FavouriteRegTest::startSyncTest()
{
   // We have already synced favourites in the normal way so
   // if we got to this point we know that the sync works.
   // We are now going to test to call sync multiple times
   // so we can check that we get the correct error.
   m_state = START_SYNC_TEST;
   m_reqIdVec.clear();
   RequestID firstStatus =
      m_favouriteInterface.syncFavourites().getRequestID();
   m_reqIdVec.push_back(firstStatus);
   m_allReqId.push_back(firstStatus);
   RequestID secondStatus =
      m_favouriteInterface.syncFavourites().getRequestID();
   m_reqIdVec.push_back(secondStatus);
   m_allReqId.push_back(secondStatus);
}
void FavouriteRegTest::checkSyncedFavourites()
{
   if(m_state == ERROR) {
      //setTestFailed();
   } else {
      FavouriteArray favouriteArray;
      wf_uint32 totalFavouriteCount = 0;

      // Get the current favourites
      m_favouriteInterface.getFavourites(0, WF_MAX_UINT16, totalFavouriteCount,
                                         favouriteArray);
      

      t_favouriteMap::iterator itm = m_favTestMap.begin();
      for(; itm != m_favTestMap.end(); ++itm) {
         bool foundMatch = false;
         for (FavouriteArray::iterator it = favouriteArray.begin(); 
              it != favouriteArray.end(); ++it) {
            
            if(it->getName() == itm->second.getName()) {

               // found a match!
               foundMatch = true;
               
               if (!(it->getInformationArray().empty()) &&
                   !(itm->second.getInformationArray().empty())) {
                  ItemInfoArray::const_iterator itemIt = 
                     it->getInformationArray().begin();
                  ItemInfoArray::const_iterator itemItm = 
                     itm->second.getInformationArray().begin();
                  for (; itemIt != it->getInformationArray().end() &&
                          itemItm != itm->second.getInformationArray().end() ;
                       ++itemIt, ++itemItm) {
                     if(itemIt->getKey() == itemItm->getKey() &&
                        itemIt->getVal() == itemItm->getVal() &&
                        itemIt->getInfoType() == itemItm->getInfoType()) {
                     } else {
                        m_state = ERROR;
                        setTestFailed(
                           "Favourite not returned correct from server");
                        
                     }
                  }
               } else if((!(it->getInformationArray().empty()) &&
                          (itm->second.getInformationArray().empty()))
                         || ((it->getInformationArray().empty()) &&
                             !(itm->second.getInformationArray().empty()))) {
                  m_state = ERROR;
                  setTestFailed(
                     "Synced favourite or created favourite did not contain addinfo.");
               }
            }
         }
         if(!foundMatch) {
            // Did not found created favourite in synced list.
            m_state = ERROR;
            setTestFailed("Did not found created favourite in synced list.");
         }
      }
      
      //Delete favs after
      for (FavouriteArray::iterator it = favouriteArray.begin(); 
           it != favouriteArray.end(); ++it) {
         m_favouriteInterface.deleteFavourite(it->getID());
      }
      if(m_state != ERROR) {
         m_state = SYNC_FAV_ONGOING;
         m_allReqId.push_back(m_favouriteInterface.syncFavourites().getRequestID());
      }
   }
}

void FavouriteRegTest::error(const WFAPI::AsynchronousStatus& status)
{
   if (m_state == START_SYNC_TEST) {
      t_reqIdVec::const_iterator it = std::find(m_reqIdVec.begin(),
                                           m_reqIdVec.end(),
                                           status.getRequestID());
      if (it != m_reqIdVec.end() &&
          status.getStatusCode() == FAVOURITE_SYNC_ALREADY_IN_PROGRESS) {
         // We got the correct error code for this request
         m_gotOkErrorMsgForSync = true;
         if (checkIfSyncTestIsDone()) {
            setTestPassed();
         }
      } else {
         m_state = ERROR;
         setTestFailed("Got unkown error message for sync already in progress");
      }
   } else {
      m_state = ERROR;
      setTestFailed(status.getStatusMessage());
   }
}

void
FavouriteRegTest::printFavourite(Favourite fav)
{
   coreprintln("================== Favourite ==================");
   coreprintln("Name: %s", fav.getName().c_str());
   coreprintln("ID: %u", fav.getID());
   coreprintln("Position: (%f, %f)",
               fav.getPosition().latDeg,
               fav.getPosition().lonDeg);
   coreprintln("Description: %s", fav.getDescription().c_str());

   if (!(fav.getInformationArray().empty())) {
      for (ItemInfoArray::const_iterator itemIt = 
              fav.getInformationArray().begin(); 
           itemIt != fav.getInformationArray().end(); ++itemIt) {
         coreprintln(" Extra key: %s", itemIt->getKey().c_str());
         coreprintln(" Extra val: %s", itemIt->getVal().c_str());
         coreprintln(" Extra type: %u", itemIt->getInfoType());
      }
   }
   coreprintln("===============================================");
}

bool
FavouriteRegTest::checkIfSyncTestIsDone()
{
   return (m_gotGoodReplyForSync && m_gotOkErrorMsgForSync);
}
