/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "OneSearchDispatcher.h"

//#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE
#define CORE_LOGGING_MODULE_NAME "CATEGORY_TREE_DISPATCHER"

#include "TmapImageHandler.h"
#include "nav2util.h"
#include "NavServerComProtoVer.h"
#include "GuiProt/GuiProtMess.h"
#include "NParam.h"

#include "NavOneSearchReq.h"
#include "Buffer.h"
#include "SharedImageFileUtil.h"
#include <vector>
#include <string>

#include "CoreDebugPrint.h"

using namespace isab;

// Typedef for a vector containing pointers to const char
typedef std::vector<const char*> CharVec;

struct OneSearchDispatcher::Impl
{
   CharVec m_neededImagesVec;

   /**
    * Handles downloading of images connected to headings
    * and search results
    */
   TmapImageHandler* m_imageHandler;

   /// Path to shared folder containing images for the search results.
   char* m_imagePath;

   /// Extension of the images to download (s60v3 uses mif, uiq3 uses png)
   char* m_imageExt;

   /// The dimension for downloaded images.
   CombinedSearch::CSImageDimension m_imageDimension;
   
   /// If to download images with specified dimension.
   bool m_useImageDimension;

};

OneSearchDispatcher::OneSearchDispatcher(
   MessageSenderInterface& messageHandler,
   const char* imagePath, 
   const char* imageExt)
   : m_messageHandler(messageHandler)
{
   m_impl = new OneSearchDispatcher::Impl;
   m_impl->m_imagePath = strdup_new(imagePath);
   m_impl->m_imageExt = strdup_new(imageExt);
   m_impl->m_useImageDimension = false;
   m_impl->m_imageHandler = new TmapImageHandler(m_messageHandler, *this);
}

OneSearchDispatcher::~OneSearchDispatcher()
{
   delete m_impl->m_imagePath;
   delete m_impl->m_imageExt;
   delete m_impl->m_imageHandler;
   delete m_impl;
}

void
OneSearchDispatcher::sendSearchRequest(
   OneSearch::InternalOneSearchQuery& searchRecord)
{
   // Maybe we should not allow requesting another tree
   // before the first one has been received and handled.
   // Could be blocked here.
   NParamBlock params;

   // The position to search in is based on either the param
   // parameter 6802 (position)  is optional in this version of the request.
   // The location where the search should be performed is specified
   // by either sending parameter 6802 (position) or parameter
   // 6808 (city to search in) and 6809 (top region id, country id).

   std::vector<int32> roundVec;
   roundVec.reserve(2);
   if (searchRecord.getRound() == MAX_INT32) {
      // This means both rounds
      roundVec.push_back(0);
      roundVec.push_back(1);
   } else {
      roundVec.push_back(searchRecord.getRound());
   }

   // IMPORTANT: The conversion between WFAPI::SearchType to
   // wf_ngp::NavOneSearchReq::SearchTypeEnum and the conversion
   // between WFAPI::SearchSorting to
   // wf_ngp::NavOneSearchReq::SearchSortingTypeEnum which means
   // that the indexes in both of these enums need to match
   std::vector<int32> categoryIds;
   NavOneSearchReq msg(searchRecord.getWhere(),
                       searchRecord.getTopRegionId(),
                       searchRecord.getIncludeInfoFields(),
                       searchRecord.getSearchType(),
                       searchRecord.getMaxHits(),
                       searchRecord.getRound(),
                       Coordinate(searchRecord.getLat(),
                                  searchRecord.getLon()),
                       searchRecord.getRadius(),
                       searchRecord.getWhat(),
                       searchRecord.getCategoryIds(),
                       searchRecord.getSearchSort());
                       
   msg.serialize(params);

   // Sorry but I need to set the round in the param block for all
   // rounds in the roundVec
   NParam* round = const_cast<NParam*>(params.getParam(6805));
   for (std::vector<int32>::const_iterator it = roundVec.begin();
        it != roundVec.end();
        ++it) {

      // Set the round and send the request
      *round = NParam(6805, *it);
      sendMessage(params, navRequestType::NAV_ONE_SEARCH_REQ);
   }   
}

void
OneSearchDispatcher::sendMessage(NParamBlock& params, uint16 navRequestType)
{
   isab::Buffer buf(10);

   isab::Buffer tmpbuf(128);

   std::vector<byte> bbuf;
   params.writeParams(bbuf, NSC_PROTO_VER, false);
   tmpbuf.writeNextByteArray(&bbuf.front(), bbuf.size());

   const uint8* data = tmpbuf.accessRawData();
   uint32 size = tmpbuf.getLength();

   buf.writeNextUnaligned16bit(navRequestType);

   isab::DataGuiMess mess(isab::GuiProtEnums::GUI_TO_NGP_DATA,
                          buf.getLength(), buf.accessRawData(),
                          size, data, 2 );
   
   m_messageHandler.sendMessage(mess);
   mess.deleteMembers();
}

void
OneSearchDispatcher::handleSearchResult(
   const isab::DataGuiMess& mess,
   std::vector<CSOneSearchMatch>* searchMatches)
{
   NParamBlock pb(mess.getAdditionalData(),
                  mess.getAdditionalSize(),
                  NSC_PROTO_VER);

   int32 nbrOfMatches = pb.getParam(6900)->getInt32();
   coreprintln("We got %d matches!", nbrOfMatches);
//   int32 totalNbrOfMatches = pb.getParam(6901)->getInt32();
//   coreprintln("We got total %d matches!", totalNbrOfMatches);

   const NParam* searchStringTable = pb.getParam(6902);
   const NParam* areaTable = pb.getParam(6903);
   const NParam* infoItemTable = pb.getParam(6904);
   const NParam* matchesTable = pb.getParam(6905);

   uint32 byteOffset = 0;
   for (int i = 0; i < nbrOfMatches ; i++) {
      CSOneSearchMatch curMatch;

      int32 nameIndex = matchesTable->incGetInt32(byteOffset);
      curMatch.name = searchStringTable->getString(nameIndex);
      
      int32 idIndex = matchesTable->incGetInt32(byteOffset);
      curMatch.id = searchStringTable->getString(idIndex);
      
      int32 locationIndex = matchesTable->incGetInt32(byteOffset);
      curMatch.location = searchStringTable->getString(locationIndex);
      
      curMatch.latitude = matchesTable->incGetInt32(byteOffset);
      curMatch.longitude = matchesTable->incGetInt32(byteOffset);
      curMatch.type = matchesTable->incGetUint8(byteOffset);
      curMatch.subType = matchesTable->incGetUint16(byteOffset);
      
      int32 categoryImageIndex = matchesTable->incGetInt32(byteOffset);
      curMatch.categoryImage =
         searchStringTable->getString(categoryImageIndex);

      if(m_impl->m_useImageDimension &&
         strcmp(m_impl->m_imageExt, ".png") == 0) {
         SharedImageFileUtil::checkAndAddImageName(m_impl->m_neededImagesVec,
                                                   curMatch.categoryImage.c_str(),
                                                   m_impl->m_imagePath,
                                                   m_impl->m_imageExt,
                                                   m_impl->m_imageDimension);
      } else {
         SharedImageFileUtil::checkAndAddImageName(m_impl->m_neededImagesVec,
                                                   curMatch.categoryImage.c_str(),
                                                   m_impl->m_imagePath,
                                                   m_impl->m_imageExt);
      }
      
      int32 providerImageIndex = matchesTable->incGetInt32(byteOffset);
      curMatch.providerImage =
         searchStringTable->getString(providerImageIndex);
      if(m_impl->m_useImageDimension &&
         strcmp(m_impl->m_imageExt, ".png") == 0) {
         SharedImageFileUtil::checkAndAddImageName(m_impl->m_neededImagesVec,
                                                   curMatch.providerImage.c_str(),
                                                   m_impl->m_imagePath,
                                                   m_impl->m_imageExt,
                                                   m_impl->m_imageDimension);
      } else {
         SharedImageFileUtil::checkAndAddImageName(m_impl->m_neededImagesVec,
                                                   curMatch.providerImage.c_str(),
                                                   m_impl->m_imagePath,
                                                   m_impl->m_imageExt);
      }
      
      int32 brandImageIndex = matchesTable->incGetInt32(byteOffset);
      curMatch.brandImage = searchStringTable->getString(brandImageIndex);
      if(m_impl->m_useImageDimension && strcmp(m_impl->m_imageExt, ".png") == 0) {
         SharedImageFileUtil::checkAndAddImageName(m_impl->m_neededImagesVec,
                                                   curMatch.brandImage.c_str(),
                                                   m_impl->m_imagePath,
                                                   m_impl->m_imageExt,
                                                   m_impl->m_imageDimension);
      } else {
         SharedImageFileUtil::checkAndAddImageName(m_impl->m_neededImagesVec,
                                                   curMatch.brandImage.c_str(),
                                                   m_impl->m_imagePath,
                                                   m_impl->m_imageExt);
      }
      
      curMatch.flags = matchesTable->incGetUint8(byteOffset);
      
      uint8 nbrCategories = matchesTable->incGetUint8(byteOffset);
      uint8 nbrAreas = matchesTable->incGetUint8(byteOffset);
      uint8 nbrDetailedItems = matchesTable->incGetUint8(byteOffset);

      for(int catIdx = 0; catIdx < nbrCategories; catIdx++) {
         uint32 catId = matchesTable->incGetInt32(byteOffset);
         curMatch.categories.push_back(catId);
      }

      for(int aI = 0; aI < nbrAreas; aI++) {
         CSOneSearchMatchArea curArea;
         uint32 areaIndex = matchesTable->incGetInt32(byteOffset);
         
         curArea.areaType = areaTable->incGetUint16(areaIndex);
         
         int32 areaNameIndex = areaTable->incGetInt32(areaIndex);
         curArea.areaName = searchStringTable->getString(areaNameIndex);
               
         int32 areaIdIndex = areaTable->incGetInt32(areaIndex);
         curArea.areaId = searchStringTable->getString(areaIdIndex);
      
         curMatch.areas.push_back(curArea);
      }

      for(int dI = 0; dI < nbrDetailedItems; dI++) {
         CSOneSearchMatchDetailedItem curItem;
         
         uint32 detailedItemIndex = matchesTable->incGetInt32(byteOffset);
         curItem.type = infoItemTable->incGetUint16(detailedItemIndex);
               
         curItem.contentType = infoItemTable->incGetUint8(detailedItemIndex);
               
         int32 keyIndex = infoItemTable->incGetInt32(detailedItemIndex);
         curItem.key = searchStringTable->getString(keyIndex);
               
         int32 valueIndex = infoItemTable->incGetInt32(detailedItemIndex);
         curItem.value = searchStringTable->getString(valueIndex);
               curMatch.detailedItems.push_back(curItem);
      }
      searchMatches->push_back(curMatch);
   }

   downloadNeededImages();

}

void
OneSearchDispatcher::setImagePath(const char* imagePath)
{
   if (m_impl->m_imagePath) {
      delete[] m_impl->m_imagePath;
   } 
   m_impl->m_imagePath = strdup_new(imagePath);   
}

void
OneSearchDispatcher::setImageExt(const char* imageExt)
{
   if (m_impl->m_imageExt) {
      delete[] m_impl->m_imageExt;
   } 
   m_impl->m_imageExt = strdup_new(imageExt);
}

void
OneSearchDispatcher::setImageDimension(
   CombinedSearch::CSImageDimension imageDimension)
{

   m_impl->m_imageDimension = imageDimension;
   if(imageDimension.width != 0 && imageDimension.height != 0) {
      m_impl->m_useImageDimension = true;
   } else {
      // hmmm... we got invalid image dimensions, lets not use them
      m_impl->m_useImageDimension = false;
   }
}


void
OneSearchDispatcher::ImageReceived(const char* imageName)
{
   coreprintln("imageReceived name: %s", imageName);
   // We start by checking if the image is a requested category image,
   // and if it is the last one in the needed category image set we
   // do an appropriate callback.

   if (SharedImageFileUtil::checkAllImagesReceived(m_impl->m_neededImagesVec,
                                                   imageName)) {
      // All images downloaded.
   }
}

void
OneSearchDispatcher::ImageReceived(const char* imageName,
                                        const uint8* imageBuffer,
                                        uint32 imageBufferLength)
{
}

void
OneSearchDispatcher::ImageReceivedError(const char* imageName)
{
    
}


void
OneSearchDispatcher::downloadNeededImages()
{
   coreprintln_info("OneSearchDispatcher::downloadNeededImages, "
                    "about to download %d image files",
                    m_impl->m_neededImagesVec.size());
   if( m_impl->m_neededImagesVec.size() ) {
      // Iterate through the vector and download all the needed images.
      CharVec::const_iterator it =
         m_impl->m_neededImagesVec.begin();
      while( it != m_impl->m_neededImagesVec.end() ) {
         if(m_impl->m_useImageDimension && strcmp(m_impl->m_imageExt, ".png") == 0){
            m_impl->m_imageHandler->FetchImageUsingNewTMapInterface((*it),
                                                                    false);
         } else {
            m_impl->m_imageHandler->FetchImageUsingOldTMapInterface((*it),
                                                                    false);
         }

         ++it;
      }
   }
}

