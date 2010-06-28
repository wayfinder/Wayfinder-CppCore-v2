/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef COMBINED_SEARCH_DISPATCHER_H
#define COMBINED_SEARCH_DISPATCHER_H

#include "arch.h"
#include "GuiProt/GuiProtMessageHandler.h"
#include "GuiProt/GuiProtMess.h"
#include "SearchResultObserver.h"
#include "SearchRecord.h"
#include "NParamBlock.h"
#include "CombinedSearchDataHolder.h"
#include "TmapImageObserver.h"
#include "MessageSenderInterface.h"
#include "CSImageDimension.h"

class TmapImageHandler;

/**
 * Class that handles all combined searches. Triggeres download
 * for missing images. It downloads categories, triggeres search
 * and can request more search hits from the server.
 * @class TmapImageHandler
 */
class CombinedSearchDispatcher : 
   public TmapImageObserver
{
 public:

   typedef std::vector<const char*> CharVec;
   
   /**
    * Creates a CombinedSearchDispatcher.
    * @param messageHandler The message handler needed for sending 
    *                       messages to Nav2.
    * @param observer The observer that will receive all the
    *                 search results when handled.
    * @param headingAndHitImagesPath The path to store the heading and search
    *                                hit images in.
    * @param categoryImagePath The path to store the category images in.
    * @param imageExt The extension for the image filenames.
    */
   CombinedSearchDispatcher( MessageSenderInterface& messageHandler,
                             MSearchResultObserver& observer,
                             const char* headingAndHitImagesPath, 
                             const char* categoryImagePath, 
                             const char* imageExt );

   /**
    * Destructor.
    */
   virtual ~CombinedSearchDispatcher();

   /**
    * Send message to get the headings, the message
    * will be received by AppUi that will pass it forward
    * to handleCombinedSearchCategoriesGot.
    */
   void requestHeadings();

   /**
    * Send a message containing one or more ids of search items
    * to get detailed information about.
    *
    * @param searchIdList A vector containing ids of search items.
    */
   void requestAdditionalInformation(const CharVec& searchIdList);
   
   /**
    * Creates a search message, the search is a area match search.
    * @param sr, the SearchRecord containing information what to search for.
    * @param heading, the id of the heading to perform the area match search for.
    */
   void dispatchAreaMatchSearch(SearchRecord& sr, uint32 heading);

   /**
    * Creates the search message to be sent to Nav2 and further 
    * to the server. Calls combinedSearchDispatchSearchMessage
    * that will send the message twice, one for round one and
    * one for round two.
    * @param searchRecord Structure holding the search conditions.
    */  
   void combinedSearchStart(SearchRecord& sr);

   /**
    * Called from the gui when more hits needs to be
    * requested from the server.
    * @param index, The search items start index, the first item 
    *               to be downloaded.
    * @param heading, The heading that contains the search hits
    *                 to be requested from the server.
    */ 
   void requestMoreHits(uint32 index,
                        uint32 heading,
                        uint32 maxHits = 25);

   /** 
    * Called from message handler (AppUi) when a search failed.
    */
   void searchRequestFailed();

   /**
    * Returns the global data holder for combined search, the returned reference
    * is const, hence no changes will be allowed.
    * @return m_dataHolder The global instance of the CombinedSearchDataHolder, the
    *                      return value is const.
    */
   const CombinedSearchDataHolder& getCombinedSearchDataHolder() const; 

   /**
    * Returns the global data holder for combined search.
    * @return m_dataHolder The global instance of the CombinedSearchDataHolder
    */
   CombinedSearchDataHolder& getCombinedSearchDataHolder(); 

   /**
    * Calls CombinedSearchDataHolder::getCombinedSearchCategories
    * @return Stored map containing all downloaded categories.
    */
   const std::map<uint32, CombinedSearchCategory*>& getCombinedSearchCategories();

   /**
    * Calls CombinedSearchDataHolder::getCombinedSearchResults
    * @return Stored vector containing all search hits for the latest search.
    */
   const std::vector<CombinedSearchCategory*>& getCombinedSearchResults();

   /**
    * Calls CombinedSearchDataHolder::getCombinedSearchCategoryResults
    * @return Stored vector containing all category results and the
    *         search results.
    */
   const std::vector<CombinedSearchCategory*>& getCombinedSearchCategoryResults();

   /**
    * Function that will be called by the receiver of NAV_SEARCH_DESC_REPLY
    * reply. Usually this is AppUi. This function needs to check if
    * all images for all headings is downloaded, if not this must
    * be done.
    * @param mess The message containing all the received headings.
    */
   void handleCombinedSearchHeadingsGot(const isab::DataGuiMess *mess);

   /**
    * Function that will be called by the receiver of NAV_INFO_REPLY
    * reply. The message is parsed and the information about each
    * search item is stored in a map (m_infoForMatchesMap) with the
    * search item id as key and an AdditionalInfo as data.
    * 
    * @param mess The message containing detailed information about requested
    *             search items.
    */
   void handleAdditionalInfoReply(const isab::DataGuiMess *mess);

   /**
    * Called from WayfinderAppUi::ReceiveMessage when server 
    * responded to a search request. 
    * @param mess, The DataGuiMess containing all search hits and
    *              the categories.
    */
   void handleCombinedSearchResultsGot(const isab::DataGuiMess *mess);

   /**
    * Returns current round for this search.
    * @return Current round.
    */
   uint16 getCurrentRound() const;

   /**
    * Returns the number of outstanding rounds that we are
    * waiting response on.
    *
    * @return The number of outstanding rounds/requests.
    */
   uint16 getOutstandingRounds() const;

   /**
    * Sets the image path for heading and search hit images. Make sure
    * this path exists, there is a bug in TmapImageHandler that crashes
    * TcpThread if no directory exits.
    *
    * @param imagePath The path to where to store the images fetched from
    *                  the server.
    */
   void setHeadingsAndHitsImagePath(const char* imagePath);

   /**
    * Sets the image path for the category images. Make sure this path
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
    * @param imageDimension The dimension for images.
    */
   void setImageDimension(CombinedSearch::CSImageDimension imageDimension);

   /**
    * Downloads images if the don't already exist in the m_categoryImagePath
    *
    * @param iconList The vector containing all the image names to be
    *                 downloaded.
    */
   void getCategoryIcons(CharVec& iconList);

   /**
    * Erases all the icons stored in the m_categoryImagePath
    */
   void clearCategoryIcons();
public: // From TmapImageObserver

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

private:
   void processHeading(const NParam* hdng, const NParam* rgns,
                       std::vector<isab::SearchItem>& topHitItems);

   bool processSearchItem(isab::NonOwningBuffer* bff,
                          uint32 type,
                          uint32 topHits,
                          int& topHitsAdded,
                          std::vector<isab::SearchItem>& items,
                          std::vector<isab::SearchRegion *>& regions,
                          std::vector<isab::SearchItem>& topHitItems);
      
   void processSearchItem(isab::NonOwningBuffer* bff);
   
   /**
    * Sends a search message to Nav2.
    * 
    * @param params NParamBlock that contains the search conditions.
    * @param final True if this is the final round, false if more messages
    *              is to be sent, that is more rounds.
    */
   void combinedSearchDispatchSearchMessage(isab::NParamBlock& params,
                                            uint16 navRequestType);

   /**
    * Iterates through the search results, just for a quick
    * calculation of how many search hits was received.
    * @param mess The DataGuiMess that contains all search hits.
    */
   void calcTotalNbrHits(const isab::DataGuiMess* mess);

   /**
    * Downloads all images specified in the supplied vector.
    * 
    * @param neededImages A vector containing the names of the images
    *                     to download.
    */
   void downloadNeededImages(const CharVec& neededImages);

   /**
    * Compares the crc that arrives from the server with the one that we
    * have stored on file.
    *
    * @param serverCrc The server crc to compare with.
    */
   bool compareIconCrc( const char* serverCrc );

   /**
    * Stores the server crc to file.
    *
    * @param serverCrc The crc to store.
    */
   void saveIconCrc( const char* serverCrc );

   /**
    * Removes all the icon files from disc.
    *
    * @param imagePath The path where the images are stored.
    */
   void clearStoredIcons(const char* imagePath);

   /**
    * Clears the m_infoForMatchesMap.
    */
   void clearInfoForMatchesMap();
   
 protected:

   /// Used to send messages to the server
   MessageSenderInterface& m_messageHandler;

   /// The observer that gets callbacks on its requests
   MSearchResultObserver& m_observer;

   /// Container that holds all search results etc
   CombinedSearchDataHolder* m_dataHolder;

   /// Handles downloading of images connected to headings
   /// and search results
   TmapImageHandler* m_imageHandler;

   /// Vector holding search category images that needs to be downloaded.
   CharVec m_neededCategoryImages;

   /// Vector holding heading images that needs to be downloaded.
   CharVec m_neededHeadingImages;

   /// Vector holding search hit images that needs to be downloaded.
   CharVec m_neededHitImages;
   
   /// Path to shared folder holding all images for the combined search
   char* m_headingAndHitImagesPath;

   /// Path to shared folder containing images for the search categories.
   char* m_categoryImagePath;
   
   /// Extension of the images to download (s60v3 uses mif, uiq3 uses png)
   char* m_imageExt;

   /// Dimension for images that will be downloaded.
   CombinedSearch::CSImageDimension m_imageDimension;

   /// If to download images with specified dimension.
   bool m_useImageDimension;

   /// Flag that is true if categories has been downloaded, if so there is no
   /// need to download them again
   bool m_headingsDownloadStarted;

   /// If 0, first round. If 1, second round. Images are not triggered to be
   /// downloaded until both rounds are received
   uint16 m_round;

   /// Holds the number of rounds/requests that has been requested.
   uint16 m_maxRound;

   /// Just a flag so that we know if requester asked for a search or just
   /// for a downloadMorHits. Different callback functions is called depending
   /// on this flag.
   bool m_moreHitsReq;

   /// Tells us if there exist any outstanding requests already sent to the
   /// server. If so, no more searches are allowed until we receive a message
   /// for that search
   bool m_searchInitiated;

   /// Flag indicating if the current search request was a area match
   /// search or not.
   bool m_areaMatchSearch;

   /// The map of SearchItem ids that we have additional information for.
   MSearchResultObserver::idToAdditionalInfoMap m_infoForMatchesMap;
};

#endif
