/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef ONE_SEARH_DISPATCHER_H
#define ONE_SEARH_DISPATCHER_H

#include "arch.h"
#include "TmapImageObserver.h"
#include "MessageSenderInterface.h"
#include "SearchResultObserver.h"
#include "NavServerComEnums.h"
#include "InternalOneSearchQuery.h"
#include "CSImageDimension.h"
#include "NParamBlock.h"

class TmapImageHandler;

/**
 * Struct that contains information about a 
 */
struct CSOneSearchMatchDetailedItem {
   /// The type of detailed item
   uint16 type;
   /// The content type of detailed item.
   uint16 contentType;
   /// The key of the detailed item
   std::string key;
   /// The value of the detailed item.
   std::string value;
};

/**
 * Struct that contains information about an area bound to a search match.
 */
struct CSOneSearchMatchArea {
   /// The type of search area.
   uint16 areaType;
   /// The name of the search area-
   std::string areaName;
   /// The id of the search area.
   std::string areaId;
};

/** 
 * Struct that contains data about a search match.
 */
struct CSOneSearchMatch {
   /// The name of the search match.
   std::string name;
   /// The id of the search match.
   std::string id;
   /// The location of the match as a string.
   std::string location;
   /// The latitude of the position of the search match.
   int32 latitude;
   /// The longitude of the position of the search match.
   int32 longitude;
   /// The type of search match.
   uint8 type;
   /// The subtype of search match
   uint16 subType;
   /// The image name for the search match category.
   std::string categoryImage;
   /// The image name of the search match provider.
   std::string providerImage;
   /// The image name of the search match brand.
   std::string brandImage;
   /// Set if more information is available.
   uint8 flags;
   /// Contains the categoriesID for all categories that the search match
   /// belongs to.
   std::vector<uint32> categories;
   /// Contains the areas the search match are located in.
   std::vector<CSOneSearchMatchArea> areas;
   /// Contains detailed information about the search match.
   std::vector<CSOneSearchMatchDetailedItem> detailedItems;
};

class OneSearchDispatcher : 
   public TmapImageObserver
{
public:

   /**
    * Creates a OneSearchDispatcher.
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
   OneSearchDispatcher(MessageSenderInterface& messageHandler,
                       const char* imagePath, 
                       const char* imageExt);

   /**
    * Destructor
    */
   virtual ~OneSearchDispatcher();
   
public:
   /**
    * Sets the image path for the category tree images. Make sure this path
    * exists, if not no images will be stored.
    *
    * @param categoryImagePath The path to where to store the images
    *                          fetched from the server.
    */   
   void setImagePath(const char* categoryImagePath);
   
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
    * Creates the NAV_ONE_SEARCH_REQ and sends the request to server.
    * Note that one request per round will be sent to server.
    *
    * @param searchRecord The search record containing the data to put in the
    *                     search request
    */
   void sendSearchRequest(OneSearch::InternalOneSearchQuery& searchRecord);

   /**
    * Handles the reply containing the search results.
    *
    * @param mess The data message containing the search results.
    * @param searchMatches The vector to where the search matches will be
    *                      stored.
    */
   void handleSearchResult(const isab::DataGuiMess& mess,
                           std::vector<CSOneSearchMatch>* searchMatches);
   
   
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
   

protected:

   /**
    * Internaln convenience function that will check which
    * images that should be downloaded and start the download
    * of these.
    */
   void downloadNeededImages();

    /** 
    * Internal function for sending a request via the message sender
    * interface to server.
    * 
    * @param params Param block containing the data.
    * @param navRequestType The type of request.
    */
   void sendMessage(isab::NParamBlock& params, uint16 navRequestType);

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
