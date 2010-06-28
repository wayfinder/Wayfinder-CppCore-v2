/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "FavouriteInterfaceImpl.h"
#include "FavouriteListener.h"
#include "FavouriteArray.h"
#include "Nav2APIImpl.h"
#include "SynchronousStatus.h"
#include "AsynchronousStatus.h"

#include "ItemInfoEntry.h"
#include "ItemInfoEnums.h"

#include "MC2Coordinate.h"

// Include GuiProt
#include "GuiProtMess.h"
#include "GuiProtFavoriteMess.h"
#include "GuiProtEnums.h"
#include "AdditionalInfo.h"
#include "Favorite.h"

#include "Nav2Logging.h"

#include <algorithm>

using namespace isab;
using namespace std;

namespace WFAPI {

FavouriteInterfaceImpl::FavouriteInterfaceImpl(Nav2APIImpl* api)
   : InterfaceImpl(api)
{
   
}
   
FavouriteInterfaceImpl::~FavouriteInterfaceImpl()
{

}

void
FavouriteInterfaceImpl::receiveAsynchronousReply(const isab::GuiProtMess&
                                                 guiProtMess,
                                                 RequestID requestID)
{
   AsynchronousStatus status = createAsynchStatus(&guiProtMess, requestID);

   // Check status if OK.
   if (status.getStatusCode() != OK) {
      // Bad status. Report error function for all listeners.
      reportError(status, m_listeners.begin(), m_listeners.end());
      return;
   }

   isab::GuiProtEnums::MessageType type = guiProtMess.getMessageType();
   switch (type) {
      case isab::GuiProtEnums::SYNC_FAVORITES_REPLY:
      {
         for(ListenerCont::iterator it = m_listeners.begin();
             it != m_listeners.end(); ++it) {
            (*it)->favouritesSynced(requestID);
         }
         
      } break;
      case isab::GuiProtEnums::GET_FAVORITES_REPLY:
      case isab::GuiProtEnums::FAVORITES_CHANGED:
      {
         for(ListenerCont::iterator it = m_listeners.begin();
             it !=  m_listeners.end(); it++) {
            (*it)->favouritesChanged();
         }
               
      } break;
      default:
         nav2log << "FavouriteInterfaceImpl::receiveAsynchronousReply" <<
            " of unknown type: " << (int) type << endl;
         // Unsolicited reply. We ignore these for now.
         break;
   }
}
   
SynchronousStatus
FavouriteInterfaceImpl::addFavourite(const Favourite& favourite)
{
   using namespace isab;

   // Creating an isab::Favorite from the WFAPI::Favourite
   Nav2Coordinate coord(favourite.getPosition());
   Favorite nav2Favorite(static_cast<int32>(coord.nav2lat),
                         static_cast<int32>(coord.nav2lon),
                         favourite.getName().c_str(),
                         "",
                         favourite.getDescription().c_str()
                         ,"","");

   /**
    * Copying the ItemInfoArray from WFAPI:Favourite and adds every
    * ItemInfoEntry as AdditionalInfo to the isab::Favorite.
    */
   if(!favourite.getInformationArray().empty()) {
      
      ItemInfoArray::const_iterator end =
         favourite.getInformationArray().end();
      for(ItemInfoArray::const_iterator it =
             favourite.getInformationArray().begin();
          it != end; it++) {
         nav2Favorite.addAdditionalInfo(new AdditionalInfo(
                                           it->getKey().c_str(),
                                           it->getVal().c_str(),
                                           itemInfoTypeToNav2AdditionalInfoType(
                                              it->getInfoType())));
      }
   }
      
   AddFavoriteMess* addFavouriteMsg = new AddFavoriteMess(&nav2Favorite);

   AsynchronousStatus status = sendAsynchronousRequestToNav2(addFavouriteMsg);

   /*
    * Clean up. 
    * While Favorit is sent in to the constructor,
    * deleteMembers is not called.
    * addFavouriteMsg->deleteMembers();
    */
   delete addFavouriteMsg;
   
   if (status.getStatusCode() != OK) {
      // Bad status, something went wrong.
      return SynchronousStatus(status.getStatusCode(),
                               status.getStatusMessage(),
                               status.getStatusURL());
   }

   // Everything OK. 
   return SynchronousStatus(OK,
                            status.getStatusMessage(),
                            status.getStatusURL());
}

SynchronousStatus
FavouriteInterfaceImpl::addCurrentPositionAsFavourite(const WFString& name,
                                                      const WFString& description,
                                                      ItemInfoArray& info)
{
   Favourite fav(Favourite::INVALID_FAVOURITE_ID,
                 getApi()->getCurrentPosition(),
                 name,
                 description,
                 info);
   return addFavourite(fav);
}

void
FavouriteInterfaceImpl::addFavouriteListener(FavouriteListener* listener)
{
   m_listeners.insert(listener);
}

SynchronousStatus
FavouriteInterfaceImpl::deleteFavourite(wf_uint32 id)
{
   using namespace isab;

   GenericGuiMess* removeFavouriteMsg = 
      new GenericGuiMess(GuiProtEnums::REMOVE_FAVORITE, uint32(id));

   AsynchronousStatus status =
      sendAsynchronousRequestToNav2(removeFavouriteMsg);
   
   if (status.getStatusCode() != OK) {
      // Bad status, something went wrong.
      return SynchronousStatus(status.getStatusCode(),
                               status.getStatusMessage(),
                               status.getStatusURL());
   }

   // Clean up removeFavouriteMsg.
   removeFavouriteMsg->deleteMembers();
   delete removeFavouriteMsg;
   
   // Everything OK. Delete members of the reply message and return OK.
   return SynchronousStatus(OK,
                            status.getStatusMessage(),
                            status.getStatusURL());

}


SynchronousStatus
FavouriteInterfaceImpl::getFavourites(wf_uint32 startIndex, 
                                      wf_uint32 count,
                                      wf_uint32& totalFavouriteCount,
                                      FavouriteArray& favouriteArray)
{
   using namespace isab;

   
   uint16 endFavNum = std::min(wf_uint32(MAX_UINT16), startIndex + count);
   
   GenericGuiMess* getFavouritesMsg = 
      new GenericGuiMess(GuiProtEnums::GET_FAVORITES_ALL_DATA,
                         uint16(startIndex),
                         endFavNum);

   GuiProtMess* replyMsg =
      sendSynchronousRequestToNav2(getFavouritesMsg);
   
   // Get the status.
   AsynchronousStatus status =
      createAsynchStatus(replyMsg,RequestID::INVALID_REQUEST_ID);
   
   // Clean up getFavouritesMsg   
   getFavouritesMsg->deleteMembers();
   delete getFavouritesMsg;
   
   // Check status.
   if (status.getStatusCode() != OK) {
      // Bad status, something went wrong. Clean up and return.
      replyMsg->deleteMembers();
      delete replyMsg;
      return SynchronousStatus(status.getStatusCode(),
                               status.getStatusMessage(),
                               status.getStatusURL());   
   }
   
   GetFavoritesAllDataReplyMess* getFavouritesReplyMsg =
      (static_cast<GetFavoritesAllDataReplyMess*>(replyMsg));

   // Iterators for going thrue the vector with favorites returned from Nav2.
   std::vector<Favorite*>::const_iterator itEnd =
      getFavouritesReplyMsg->getFavorites()->end();

   // Iterators for going thrue the additional info in each favorite.
   std::vector<AdditionalInfo*>::const_iterator infoVectIt;
   std::vector<AdditionalInfo*>::const_iterator infoVectItEnd;
   ItemInfoArray itemInfoArray;

   for(std::vector<Favorite*>::const_iterator it =
          getFavouritesReplyMsg->getFavorites()->begin();
       it != itEnd; it++) {
      if(!((*it)->getInfos().empty())) {
         infoVectIt = (*it)->getInfos().begin();
         infoVectItEnd = (*it)->getInfos().end();
         itemInfoArray.clear();
         
         for(;infoVectIt != infoVectItEnd; infoVectIt++) {
            itemInfoArray.push_back(
               ItemInfoEntry((*infoVectIt)->getKey(),
                             (*infoVectIt)->getValue(),
                             nav2AdditionalInfoTypeToItemInfoType(
                                GuiProtEnums::AdditionalInfoType(
                                   (*infoVectIt)->getType()))));
         }
      }

      /*
       * Create a WFAPI::Favourite from the Nav2::Favorite and add it
       * to the  outparameter favouriteArray.
       */ 
      favouriteArray.push_back(Favourite((*it)->getID(),
                                         Nav2Coordinate((*it)->getLat(),
                                                        (*it)->getLon()),
                                         WFString((*it)->getName()),
                                         WFString((*it)->getDescription()),
                                         itemInfoArray));
   
   }
   // Set out parameter totalFavouriteCount.
   totalFavouriteCount = getFavouritesReplyMsg->getTotalNbrOfFavorites();

   // OK so far, clean up and return.
   replyMsg->deleteMembers();
   delete replyMsg;

   return SynchronousStatus(status.getStatusCode(),
                            status.getStatusMessage(),
                            status.getStatusURL());

}

void
FavouriteInterfaceImpl::removeFavouriteListener(FavouriteListener* listener)
{
   m_listeners.erase(listener);
}

AsynchronousStatus
FavouriteInterfaceImpl::syncFavourites()
{
   
   using namespace isab;
   
   GenericGuiMess* syncFavouriteMsg =
      new GenericGuiMess(GuiProtEnums::SYNC_FAVORITES);

   AsynchronousStatus status =
      sendAsynchronousRequestToNav2(syncFavouriteMsg);

   // Clean up before return.
   syncFavouriteMsg->deleteMembers();
   delete syncFavouriteMsg;
   
   return status;
      
}

ItemInfoType
FavouriteInterfaceImpl::nav2AdditionalInfoTypeToItemInfoType(
   isab::GuiProtEnums::AdditionalInfoType infoType)
{
   switch (infoType) {
      case isab::GuiProtEnums::email:
         return EMAIL_ADDRESS;  
      case isab::GuiProtEnums::phone_number:
      case isab::GuiProtEnums::mobile_phone:
         return PHONE_NUMBER;
      case isab::GuiProtEnums::url:
      case isab::GuiProtEnums::wap_url:
         return URL;
      case isab::GuiProtEnums::image_url:
         return IMAGE_URL;
      case isab::GuiProtEnums::long_description:
         return LONG_DESCRIPTION;
      case isab::GuiProtEnums::dont_show:
         return DONT_SHOW;
      case isab::GuiProtEnums::special_flag:
         return SPECIAL_FLAG;
      case isab::GuiProtEnums::short_info:
         return SHORT_INFO;
      case isab::GuiProtEnums::vis_address:
         return ADDRESS;
      case isab::GuiProtEnums::vis_house_nbr:
         return HOUSE_NUMBER;
      case isab::GuiProtEnums::vis_zip_code:
         return PARSED_ZIP_CODE;
      case isab::GuiProtEnums::vis_complete_zip:
         return COMPLETE_ZIP;
      case isab::GuiProtEnums::Vis_zip_area:
         return PARSED_ZIP_AREA;
      case isab::GuiProtEnums::vis_full_address:
         return FULL_ADDRESS;
      case isab::GuiProtEnums::citypart:
         return CITYPART;
      case isab::GuiProtEnums::supplier:
         return SUPPLIER;
      case isab::GuiProtEnums::post_address:
         return POST_ADDRESS;
      case isab::GuiProtEnums::post_zip_area:
         return POST_ZIP_AREA;
      case isab::GuiProtEnums::post_zip_code:
         return POST_ZIP_CODE;
      case isab::GuiProtEnums::neighborhood:
         return NEIGHBORHOOD;
      default:
         return TEXT;
   }
}

isab::GuiProtEnums::AdditionalInfoType
FavouriteInterfaceImpl::itemInfoTypeToNav2AdditionalInfoType(
   ItemInfoType infoType)
{
   switch (infoType) {
   case EMAIL_ADDRESS:
      return isab::GuiProtEnums::email;  
   case PHONE_NUMBER:
      return isab::GuiProtEnums::phone_number;
   case URL:
      return isab::GuiProtEnums::url;
   case IMAGE_URL:
      return isab::GuiProtEnums::image_url;
   case LONG_DESCRIPTION:
      return isab::GuiProtEnums::long_description;
   case DONT_SHOW:
      return isab::GuiProtEnums::dont_show;
   case SPECIAL_FLAG:
      return isab::GuiProtEnums::special_flag;
   case SHORT_INFO:
      return isab::GuiProtEnums::short_info;
   case ADDRESS:
      return isab::GuiProtEnums::vis_address;
   case HOUSE_NUMBER:
      return isab::GuiProtEnums::vis_house_nbr;
   case PARSED_ZIP_CODE:
      return isab::GuiProtEnums::vis_zip_code;
   case COMPLETE_ZIP:
      return isab::GuiProtEnums::vis_complete_zip;
   case PARSED_ZIP_AREA:
      return isab::GuiProtEnums::Vis_zip_area;
   case FULL_ADDRESS:
      return isab::GuiProtEnums::vis_full_address;
   case CITYPART:
      return isab::GuiProtEnums::citypart;
   case SUPPLIER:
      return isab::GuiProtEnums::supplier;
   case POST_ADDRESS:
      return isab::GuiProtEnums::post_address;
   case POST_ZIP_AREA:
      return isab::GuiProtEnums::post_zip_area;
   case POST_ZIP_CODE:
      return isab::GuiProtEnums::post_zip_code;
   case NEIGHBORHOOD:
      return isab::GuiProtEnums::neighborhood;
   default:
      return isab::GuiProtEnums::text;
   }
}

} // End namespace WFAPI
