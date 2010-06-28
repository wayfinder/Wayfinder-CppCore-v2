/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


//#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE
#define CORE_LOGGING_MODULE_NAME "CATEGORY_TREE_DISPATCHER"

#include "CategoryTreeDispatcher.h"
#include "TmapImageHandler.h"
#include "nav2util.h"
#include "NavServerComProtoVer.h"
#include "GuiProt/GuiProtMess.h"
#include "NParam.h"
#include "CategoryTree.h"

#include "NavLocalCategoryTreeReq.h"
#include "NavLocalCategoryTreeReply.h"

#include "SharedImageFileUtil.h"
#include <vector>
#include <string>

#include "CoreDebugPrint.h"

using namespace isab;

typedef std::vector<const char*> CharVec;

struct CategoryTreeDispatcher::Impl
{
   CharVec m_neededImagesVec;

   /**
    * Handles downloading of images connected to headings
    * and search results
    */
   TmapImageHandler* m_imageHandler;

   /// Path to shared folder containing images for the search categories.
   char* m_categoryImagePath;

   /// Extension of the images to download (s60v3 uses mif, uiq3 uses png)
   char* m_imageExt;

   /// The dimension for downloaded images.
   CombinedSearch::CSImageDimension m_imageDimension;

   /// If to download images with specified dimension.
   bool m_useImageDimension;

   /**
    * The crc from a previous NAV_LOCAL_CATEGORY_REPLY. Send
    * empty attribute if no crc is available.
    */
   std::string m_categoryTreeCrc;

   /**
    * Indicates that the crc sent in the request matched the
    * latest version. No new category tree is sent when this
    * parameter is true.
    */
   bool m_crcOK;

   /// The category tree containing the category tree received from the server
   CategoryTreeImpl::CategoryTree m_categoryTree;
};

CategoryTreeDispatcher::CategoryTreeDispatcher(
   MessageSenderInterface& messageHandler,
   const char* categoryImagePath, 
   const char* imageExt)
  : m_messageHandler(messageHandler)
{
   m_impl = new CategoryTreeDispatcher::Impl;
   m_impl->m_categoryImagePath = strdup_new(categoryImagePath);
   m_impl->m_imageExt = strdup_new(imageExt);
   m_impl->m_useImageDimension = true;
   m_impl->m_imageHandler = new TmapImageHandler(m_messageHandler, *this);
   m_impl->m_crcOK = false;
}

CategoryTreeDispatcher::~CategoryTreeDispatcher()
{
   delete m_impl->m_categoryImagePath;
   delete m_impl->m_imageExt;
   delete m_impl->m_imageHandler;
   delete m_impl;
}

void CategoryTreeDispatcher::ImageReceived(const char* imageName)
{
   // We start by checking if the image is a requested category image,
   // and if it is the last one in the needed category image set we
   // do an appropriate callback.

   if (SharedImageFileUtil::checkAllImagesReceived(m_impl->m_neededImagesVec,
                                                   imageName)) {
      // All images downloaded.
   }
}

void CategoryTreeDispatcher::ImageReceived(const char* imageName,
                                           const uint8* imageBuffer,
                                           uint32 imageBufferLength)
{
}

void CategoryTreeDispatcher::ImageReceivedError(const char* imageName)
{
    
}

void
CategoryTreeDispatcher::setCategoryImagePath(const char* categoryImagePath)
{
   if (m_impl->m_categoryImagePath) {
      delete[] m_impl->m_categoryImagePath;
   }
   m_impl->m_categoryImagePath = strdup_new(categoryImagePath);
}

void
CategoryTreeDispatcher::setImageExt(const char* imageExt)
{
   if (m_impl->m_imageExt) {
      delete[] m_impl->m_imageExt;
   }
   m_impl->m_imageExt = strdup_new(imageExt);
}

void
CategoryTreeDispatcher::setImageDimension(
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

void CategoryTreeDispatcher::getCategoryTree(
   isab::NavServerComEnums::languageCode clientLang,
   int32 lat, int32 lon)
{
   // Maybe we should not allow requesting another tree
   // before the first one has been received and handled.
   // Could be blocked here.
   const char* categoryTreeCrc = m_impl->m_categoryTreeCrc.c_str();
   NParamBlock params;
   NavLocalCategoryTreeReq msg(clientLang, 1,
                               Coordinate(lat, lon),
                               categoryTreeCrc);
   msg.serialize(params);
   m_messageHandler.sendMessage(
      params,
      isab::navRequestType::NAV_LOCAL_CATEGORY_TREE_REQ);
}

bool
CategoryTreeDispatcher::handleCategoryTreeGot(const uint8* data,
                                              uint32 size)
{
   NParamBlock pb(data, size, NSC_PROTO_VER);

   const isab::NParam* crcOkParam = pb.getParam(6505);
   if (crcOkParam != NULL) {
      m_impl->m_crcOK = crcOkParam->getBool();
   }
   coreprintln_info("m_impl->m_crcOK = %d", m_impl->m_crcOK);
   if (m_impl->m_crcOK == true) {
      // We should not need to do any parsing here, maybe notify API that
      // no update is needed.
      return false;
   }
   
   // Get the crc param, we need this param for getting the crc of this
   // category tree
   const isab::NParam* crcParam = pb.getParam(6500);
   if (crcParam != NULL) {
      m_impl->m_categoryTreeCrc = crcParam->getString();
      coreprintln_info("m_impl->m_categoryTreeCrc = %s",
                       m_impl->m_categoryTreeCrc.c_str());
   }
   
   // Get the NParam for the lookup table
   const isab::NParam* param = pb.getParam(6503);
         
   // Get the NParam for the category table
   const isab::NParam* categoryParam = pb.getParam(6502);

   // Get the NParam for the string table
   const isab::NParam* stringParam = pb.getParam(6504);  

   if (param == NULL || categoryParam == NULL || stringParam == NULL) {
      // The message did not contain any category tree data, so we can
      // not continue.
      return false;
   }
   
   // Index for stepping through the params.
   uint32 arrIndex = 0;

   // Get the sub categories for the root node
   uint16 nbrSubCats = categoryParam->incGetUint16(arrIndex);
   coreprintln_info("Root - CategoryTable - nbrSubCats = %d", nbrSubCats);

   // Get all the categories that should be on the root level and
   // ad each and one of them as a root category to the CategoryTree
   for(uint32 index = 0; index < nbrSubCats; ++index) {

      // Get the offset for a root category, the offset will then be used
      // for indexing in the categoryParam for getting the category id of
      // this root category
      int32 subCategoryOffset = categoryParam->incGetInt32(arrIndex);
      coreprintln_info("Root - CategoryTable - subCatOffset = %d", subCategoryOffset);

      int32 subCategoryId = categoryParam->getInt32(uint16(subCategoryOffset));
      coreprintln_info("Root - CategoryTable - subCategoryId = %d", subCategoryId);

      // Add the id as a root category to the CategoryTree
      m_impl->m_categoryTree.addRootCategory(subCategoryId);
   }

   // Reset the index variable
   arrIndex = 0;

   while(arrIndex < param->getLength()) {
      // Just read past the category id, we will get it later
      param->incGetInt32(arrIndex);

      // Get the position for the next category, this index (byte offset)
      // will then be used for indexing in the categoryParam for getting
      // a category
      uint32 catParamIndex = param->incGetInt32(arrIndex);
      
      // Get all the data for this category
      int32 categoryId = categoryParam->incGetInt32(catParamIndex);

      // Here we get the byte offset for the name in the string table
      uint32 categoryNameOffset = categoryParam->incGetInt32(catParamIndex);

      // Use the index (byte offset) to get the name of the category from
      // the string table
      const char* categoryName = stringParam->getString(categoryNameOffset);
      
      // Here we get the byte offset for the image name in the string table
      uint32 categoryImageOffset = categoryParam->incGetInt32(catParamIndex);

      // Use the index (byte offset) to get the name of the category image from
      // the string table
      const char* categoryImage = stringParam->getString(categoryImageOffset);
      if(m_impl->m_useImageDimension && strcmp(m_impl->m_imageExt, ".png") == 0) {
         SharedImageFileUtil::checkAndAddImageName(m_impl->m_neededImagesVec,
                                                   categoryImage,
                                                   m_impl->m_categoryImagePath,
                                                   m_impl->m_imageExt,
                                                   m_impl->m_imageDimension);
      } else {
         SharedImageFileUtil::checkAndAddImageName(m_impl->m_neededImagesVec,
                                                   categoryImage,
                                                   m_impl->m_categoryImagePath,
                                                   m_impl->m_imageExt);
      }

      // Get number of sub categories for this category
      uint16 nbrSubCats = categoryParam->incGetUint16(catParamIndex);
      coreprintln_info("CategoryTable - nbrSubCats = %d", nbrSubCats);
      
      // Create a local vector here and reserve space for the sub categories
      std::vector<int32> subCategoryIds(nbrSubCats);

      // For each sub category, get the byte offset in the categorypParam
      // then use this offset in the categoryParam to get the id of the
      // sub category
      for(uint32 index = 0; index < nbrSubCats; ++index) {

         int32 subCategoryOffset = categoryParam->incGetInt32(catParamIndex);
         coreprintln_info("CategoryTable - subCatOffset = %d", subCategoryOffset);

         int32 subCategoryId = categoryParam->getInt32(subCategoryOffset);
         coreprintln_info("CategoryTable - subCategoryId = %d", subCategoryId);

         // Add the id of the sub category to the vector
         subCategoryIds[index] = subCategoryId;
      }
      
      CategoryTreeImpl::CategoryTreeElem* elem =
         new CategoryTreeImpl::CategoryTreeElem(categoryId,
                                                categoryName,
                                                categoryImage,
                                                subCategoryIds);
      m_impl->m_categoryTree.addCategory(elem);
   }
   
   downloadNeededImages();
   return true;
}

void
CategoryTreeDispatcher::getRootCategories(
   CategoryTreeImpl::CTCatElemVec_t& rootCategories)
{
   m_impl->m_categoryTree.getRootCategories(rootCategories);
}

void
CategoryTreeDispatcher::getSubCategoriesOf(
   int32 categoryId, CategoryTreeImpl::CTCatElemVec_t& categories)
{
   m_impl->m_categoryTree.getSubCategoriesOf(categoryId, categories);
}

void CategoryTreeDispatcher::downloadNeededImages()
{
   coreprintln_info("CategoryTreeDispatcher::downloadNeededImages, "
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
