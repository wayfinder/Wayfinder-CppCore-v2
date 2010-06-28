/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef CATEGORY_TREE_DISPATCHER_H
#define CATEGORY_TREE_DISPATCHER_H

#include "arch.h"
#include "TmapImageObserver.h"
#include "MessageSenderInterface.h"
#include "SearchResultObserver.h"
#include "NavServerComEnums.h"

#include "CategoryTreeTypedefs.h"
#include "CSImageDimension.h"

class TmapImageHandler;

class CategoryTreeDispatcher : 
   public TmapImageObserver
{
public:

   /**
    * Creates a CategoryTreeDispatcher.
    *
    * TODO: Add new observer that can be notified when categories
    * has been parsed
    * @param messageHandler The message handler needed for sending 
    *                       messages to Nav2.
    * @param observer The observer that will receive all the
    *                 search results when handled.
    * @param categoryImagePath The path to store the category images in.
    * @param imageExt The extension for the image filenames.
    */
   CategoryTreeDispatcher(MessageSenderInterface& messageHandler,
                          const char* categoryImagePath, 
                          const char* imageExt);

   /**
    * Destructor
    */
   virtual ~CategoryTreeDispatcher();
   
public:
   /**
    * Sets the image path for the category tree images. Make sure this path
    * exists, if not no images will be stored.
    *
    * @param categoryImagePath The path to where to store the images
    *                          fetched from the server.
    */   
   void setCategoryImagePath(const char* categoryImagePath);
   
   /**
    * Sets the image extension, will not add dot '.' between filename and
    * extension so if there should be one it must be in the imageExt.
    *
    * @param imageExt The image extension to use.
    */
   void setImageExt(const char* imageExt);

   /** 
    * Sets the dimension of the images that will be downloaded.
    * 
    * @param imageDimension The dimension of the images to download.
    */
   void setImageDimension(CombinedSearch::CSImageDimension imageDimension);
   
   /**
    * Fetches the category tree from server if necessary.
    *
    * @param clientLang The language the client uses.
    * @param lat The latitude to get the category tree for.
    * @param lon The longitude to get the category tree for.
    * @param categoryTreeCrc The CRC of the stored category tree.
    */
   void getCategoryTree(isab::NavServerComEnums::languageCode clientLang,
                        int32 lat, int32 lon);   

   /** 
    * Handles the reply containing the category tree.
    * 
    * @param data The data containing the category tree.
    * @param size The size of the data.
    * 
    * @return True if we got a new crc from the server, this means that
    *         we got a new category tree.
    *         False if server replied with the same crc as we have stored.
    */
   bool handleCategoryTreeGot(const uint8* data, uint32 size);

   /** 
    * Called by the TmapImageHandler when requested image
    * is downloaded
    * @param imageName, The name of the image just downloaded.
    */
   void ImageReceived(const char* imageName);

   /**
    * Function gets called by TmapImageHandler
    * when a requested image is downloaded and will be returned as a Buffer.
    * 
    * @param imageName, The name of the downloaded image
    * @param imageBuffer The downloaded image buffer.
    * @param imageBufferLength Length of the downloaded buffer.
    */
   virtual void ImageReceived(const char* imageName,
                              const uint8* imageBuffer,
                              uint32 imageBufferLength);

   /**
    * Called by the TmapImageHandler when something went
    * wrong with the download.
    * @param imageName, The name of the image that failed to be downloaded.
    */
   void ImageReceivedError(const char* imageName);
   
   /** 
    * Simple get function to get the root categories from the category tree.
    * 
    * @param rootCategories  The vector that will be populated with categories.
    */
   void
   getRootCategories(CategoryTreeImpl::CTCatElemVec_t& rootCategories);

   /** 
    * Simple get function for the subcategories of a specified category id.
    * 
    * @param categoryId The id to get the subcategories for.
    * @param categories The vector that will be populated with subcategories.
    */
   void getSubCategoriesOf(int32 categoryId,
                           CategoryTreeImpl::CTCatElemVec_t& categories);

protected:
   void downloadNeededImages();
   
private:
   /// Used to send messages to the server
   MessageSenderInterface& m_messageHandler;

   /**
    * Opaque pointer for data containment
    */
   struct Impl;
   Impl* m_impl;
};

#endif
