/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#define CORE_LOGGING_MODULE_NAME "CS_DISP"

#include "CombinedSearch/CombinedSearchDispatcher.h"

#if defined(__linux__) || defined(IPHONE) || defined(ANDROID)
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#endif

#include "CoreDebugPrint.h"
#include <arch.h>
#include <stdio.h>
#include <ctype.h>
#ifdef __SYMBIAN32__
# include <sys/unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
# if !(defined(NAV2_CLIENT_UIQ) || defined(NAV2_CLIENT_UIQ3))
#  include <sysutil.h>
# endif
# include <f32file.h>
#elif defined UNDER_CE
# include <winbase.h>
#else
# include <unistd.h>
#endif
#include <cstdio>

#include "nav2util.h"
#include "NavServerComProtoVer.h"
#include "NavRequestType.h"
#include "GuiProt/SearchRegion.h"
#include "CombinedSearch/TmapImageHandler.h"
#include "NParam.h"
#include "SharedImageFileUtil.h"

using namespace isab;

#define MAX_REPLIES_IN_HEADING 100
#define IMG_CRC_FILENAME "imgcrc.dat"

CombinedSearchDispatcher::CombinedSearchDispatcher(
   MessageSenderInterface& messageHandler,
   MSearchResultObserver& observer,
   const char* headingAndHitImagesPath,
   const char* categoryImagePath,
   const char* imageExt)
   : m_messageHandler(messageHandler),
     m_observer(observer),
     m_useImageDimension(false),
     m_headingsDownloadStarted(false),
     m_moreHitsReq(false),
     m_searchInitiated(false),
     m_areaMatchSearch( false)
{
   m_dataHolder = new CombinedSearchDataHolder();
   m_headingAndHitImagesPath = strdup_new(headingAndHitImagesPath);
   m_categoryImagePath = strdup_new(categoryImagePath);
   m_imageExt = strdup_new(imageExt);
   m_imageHandler = new TmapImageHandler(m_messageHandler, *this);
}

CombinedSearchDispatcher::~CombinedSearchDispatcher()
{
   // First we delete the char pointers for the needed
   // search category images.
   CharVec::iterator it = m_neededCategoryImages.begin();
   while(it != m_neededCategoryImages.end()) {
      delete[] (*it);
      ++it;
   }
   m_neededCategoryImages.clear();

   // Then we delete the char pointers for the needed
   // heading images.
   it = m_neededHeadingImages.begin();
   while(it != m_neededHeadingImages.end()) {
      delete[] (*it);
      ++it;
   }
   m_neededHeadingImages.clear();

   // Don't forget the char pointers for the needed hit images!
   it = m_neededHitImages.begin();
   while(it != m_neededHitImages.end()) {
      delete[] (*it);
      ++it;
   }
   m_neededHitImages.clear();
   
   clearInfoForMatchesMap();
   delete m_dataHolder;
   delete[] m_headingAndHitImagesPath;
   delete[] m_categoryImagePath;
   delete[] m_imageExt;
   delete m_imageHandler;
}

void CombinedSearchDispatcher::requestHeadings()
{
   isab::Buffer *buf = new isab::Buffer(10);
   NParamBlock params;

   Buffer tmpbuf(128);

   std::vector< byte > bbuf;

   params.writeParams( bbuf, NSC_PROTO_VER, false /* NO GZIP */ );
   tmpbuf.writeNextByteArray( &bbuf.front(), bbuf.size() );
   const uint8* data = tmpbuf.accessRawData();
   uint32 size = tmpbuf.getLength();

   buf->writeNextUnaligned16bit( navRequestType::NAV_SEARCH_DESC_REQ );

   class DataGuiMess mess( GuiProtEnums::GUI_TO_NGP_DATA,
                           buf->getLength(), buf->accessRawData(),
                           size, data);

   m_messageHandler.sendMessage(mess);
   mess.deleteMembers();
   delete buf;
}

void CombinedSearchDispatcher::requestAdditionalInformation(
   const CharVec& searchIdList)
{
   NParamBlock params;

   // Clear the map, all the previously fetched should be removed.
   clearInfoForMatchesMap();
   
    // Set up all the params needed for the search
   for (CharVec::const_iterator it = searchIdList.begin();
        it != searchIdList.end();
        ++it) {
      params.addParam(NParam(1800, *it));
   }
   combinedSearchDispatchSearchMessage(params,
                                       navRequestType::NAV_INFO_REQ);
}

void CombinedSearchDispatcher::dispatchAreaMatchSearch(SearchRecord& sr, uint32 heading)
{
   if (m_searchInitiated) {
      return;
   }
   m_searchInitiated = true;

   NParamBlock params;

    // Set up all the params needed for the search
   params.addParam( NParam( 1201, sr.m_areaName ) );
   params.addParam( NParam( 1202, sr.m_areaId ) );

   // Add the search string.
   params.addParam( NParam( 1203, sr.m_searchString ) );
   
   params.addParam( NParam( 1205, sr.m_regionId ) );
   params.addParam( NParam( 5600, uint32( 0 ) ) );
   params.addParam( NParam( 5601, uint32( 1 ) ) );
   params.addParam( NParam( 5602, heading ) );
   params.addParam( NParam( 5603, uint32( MAX_REPLIES_IN_HEADING ) ) );

   if ( sr.m_categoryId != MAX_UINT32) { 
      // If the user selected a category add it to the param
      params.addParam( NParam( 5605, uint16(sr.m_categoryId) )  );
   }

   NParam& p = params.addParam( NParam( 1000 ) );
   p.addInt32( sr.m_origLat );
   p.addInt32( sr.m_origLon );
   p.addUint16( uint16(-1) );

   m_round = 1;
   m_moreHitsReq = false;
   m_areaMatchSearch = true;
   combinedSearchDispatchSearchMessage(
      params, navRequestType::NAV_COMBINED_SEARCH_REQ);
}

void CombinedSearchDispatcher::combinedSearchStart(SearchRecord& sr)
{   
   if (m_searchInitiated) {
      // If we already have an outstanding search request dont search again.
      return;
   }
   m_searchInitiated = true;

   if ( !m_headingsDownloadStarted ) {
      requestHeadings();
   } else {
      if (m_neededHeadingImages.size() == 0 ) {
         // We do a callback indicating that the images are ok to use.
         m_observer.HeadingImagesStatusUpdated(
            MSearchResultObserver::CURRENT_IMAGES_OK);
      }
   }

   m_dataHolder->clearCombinedSearchCategoryResults();
   m_dataHolder->clearCombinedSearchResults();
   clearInfoForMatchesMap();
   
   m_dataHolder->setSearchRecord(sr);
   NParamBlock params;

   // Set up all the params needed for the search
   params.addParam( NParam( 1201, sr.m_areaName ) );
   params.addParam( NParam( 1202, sr.m_areaId ) );
   
   // Add the search string.
   params.addParam( NParam( 1203, sr.m_searchString ) );
   
   params.addParam( NParam( 1205, sr.m_regionId ) );
   params.addParam( NParam( 5600, uint32( sr.m_index ) ) );
   params.addParam( NParam( 5602, sr.m_heading ) );
   params.addParam( NParam( 5603, sr.m_maxHits ) );

   if ( sr.m_categoryId != MAX_UINT32) { 
      // If the user selected a category we add it to the param
      params.addParam( NParam( 5605, uint16(sr.m_categoryId) )  );
   }
   
   NParam& p = params.addParam( NParam( 1000 ) );

   p.addInt32( sr.m_origLat );
   p.addInt32( sr.m_origLon );
   p.addUint16( uint16(-1) );

   m_moreHitsReq = false;
   m_areaMatchSearch = false;

   std::vector<uint32> roundVec;
   roundVec.reserve(2);
   if (sr.m_round == MAX_UINT32) {
      // This means both rounds
      roundVec.push_back(0);
      roundVec.push_back(1);
   } else {
      roundVec.push_back(sr.m_round);
   }

   std::vector<uint32>::const_iterator it = roundVec.begin();
   m_round = *it;
   m_maxRound = roundVec.back();
   NParam& search_round = params.addParam(NParam(5601, uint32(0)));
   for(; it != roundVec.end(); ++it) {
      search_round = NParam( 5601, *it );
      combinedSearchDispatchSearchMessage(
         params, navRequestType::NAV_COMBINED_SEARCH_REQ);
   }   
}

void CombinedSearchDispatcher::requestMoreHits(uint32 index,
                                               uint32 heading,
                                               uint32 maxHits) 
{
   NParamBlock params;
   // Get the reference to the stored search record
   SearchRecord& searchRecord = m_dataHolder->getSearchRecord();
   searchRecord.setIndex(index);
   searchRecord.setMaxHits(maxHits);
   searchRecord.setHeading(heading);
   
   // Set up all the params needed for the search
   params.addParam( NParam( 1201, searchRecord.m_areaName ) );
   params.addParam( NParam( 1202, searchRecord.m_areaId ) );

   // Add the search string.
   params.addParam( NParam( 1203, searchRecord.m_searchString ) );

   params.addParam( NParam( 1205, searchRecord.m_regionId ) );
   params.addParam( NParam( 5600, index ));
   params.addParam( NParam( 5601, uint32( 0 ) ) );
   params.addParam( NParam( 5602, heading ) );
   params.addParam( NParam( 5603, maxHits ) );

   if ( searchRecord.m_categoryId != MAX_UINT32) { 
      // If the user selected a category we add it to the param
      params.addParam( NParam( 5605, uint16(searchRecord.m_categoryId)));
   }

   NParam& p = params.addParam( NParam( 1000 ) );
   
   p.addInt32( searchRecord.m_origLat );
   p.addInt32( searchRecord.m_origLon );
   p.addUint16( uint16(-1) );
   
   m_round = 1;
   m_maxRound = m_round;
   m_moreHitsReq = true;
   m_areaMatchSearch = false;
   combinedSearchDispatchSearchMessage(
      params, navRequestType::NAV_COMBINED_SEARCH_REQ);
}

void
CombinedSearchDispatcher::combinedSearchDispatchSearchMessage(
   NParamBlock& params, uint16 navRequestType)
{
   isab::Buffer *buf = new isab::Buffer(10);

   Buffer tmpbuf(128);

   std::vector<byte> bbuf;

   params.writeParams(bbuf, NSC_PROTO_VER, false /* NO GZIP */);
   tmpbuf.writeNextByteArray( &bbuf.front(), bbuf.size() );
   const uint8* data = tmpbuf.accessRawData();
   uint32 size = tmpbuf.getLength();

   buf->writeNextUnaligned16bit(navRequestType);

   class DataGuiMess mess( GuiProtEnums::GUI_TO_NGP_DATA,
                           buf->getLength(), buf->accessRawData(),
                           size, data, 2 );

   m_messageHandler.sendMessage(mess);

   mess.deleteMembers();
   delete buf;
}

void
CombinedSearchDispatcher::handleCombinedSearchHeadingsGot(
   const isab::DataGuiMess *mess)
{   
   NParamBlock params(mess->getAdditionalData(), 
                      mess->getAdditionalSize(),
                      NSC_PROTO_VER);


   const NParam *param = params.getParam(5900);

   isab::NonOwningBuffer *bff =
      new isab::NonOwningBuffer(const_cast<uint8*>(param->getBuff()),
                                param->getLength());

   std::map<uint32, CombinedSearchCategory*> headings;

   while( bff->remaining() >= 10 ) {
      CombinedSearchCategory* tmp = new CombinedSearchCategory( bff );
      headings.insert( std::make_pair( tmp->getHeadingNo(), tmp ) );
   }


   bool clearedImages = false;
   const NParam *crcParam = params.getParam(28);
   if (crcParam) {
      const char* crc = crcParam->getString();
      // Compare server crc with the stored crc in file.
      if ( !compareIconCrc(crc) ) {
         // We do a callback indicating that the heading images are NOT ok
         // to use until downloaded.
         m_observer.HeadingImagesStatusUpdated(
            MSearchResultObserver::CURRENT_IMAGES_NOT_OK);

         // If any resultimages were stored in the directory, they
         // will be cleared too.
         m_observer.ResultImagesStatusUpdated(
            MSearchResultObserver::CURRENT_IMAGES_NOT_OK);
         
         // Remove all files in csicons dir if crc is not the same.
         // NOTE: Maybe hit images should be saved.
         clearStoredIcons(m_headingAndHitImagesPath);

         clearedImages = true;
         // Write new crc txt file to csicons dir.
         saveIconCrc( crc );
      }
   }

   // NOTE: Maybe hit images should be saved.

   CharVec::iterator it = m_neededHeadingImages.begin();
   while(it != m_neededHeadingImages.end()) {
      // Delete images
      delete[] (*it);
      ++it;
   }
   m_neededHeadingImages.clear();
   m_neededHeadingImages.reserve( headings.size() );

   std::map<uint32, CombinedSearchCategory*>::const_iterator cit;
   for( cit = headings.begin(); cit != headings.end(); ++cit ) {
      CombinedSearchCategory* cat = cit->second;
      // Check if all images are downloaded, if not download them.
      // Check if file already exists.
      const char* imageName = cat->getImageName();

      if(m_useImageDimension && strcmp(m_imageExt, ".png") == 0) {
         SharedImageFileUtil::checkAndAddImageName(m_neededHeadingImages,
                                                   imageName,
                                                   m_headingAndHitImagesPath,
                                                   m_imageExt,
                                                   m_imageDimension);
      } else {
         SharedImageFileUtil::checkAndAddImageName(m_neededHeadingImages,
                                                   imageName,
                                                   m_headingAndHitImagesPath,
                                                   m_imageExt);
      }
   }

   if (m_neededHeadingImages.size() == 0 && !clearedImages){      
      // We do a callback indicating that the images are ok to use.
      m_observer.HeadingImagesStatusUpdated(
         MSearchResultObserver::CURRENT_IMAGES_OK);
   } else {
      // If the images are cleared, we will already have notified the
      // observer that the current images are not OK.
      // No need to repeat ourselves.
      if (!clearedImages){
         // We do a callback indicating that the images are NOT ok
         // to use until downloaded.
         m_observer.HeadingImagesStatusUpdated(
            MSearchResultObserver::CURRENT_IMAGES_NOT_OK);
      }

      // Request a download of needed images
      downloadNeededImages(m_neededHeadingImages);
   }

   m_headingsDownloadStarted = true;
   
   // Store the map in the data holder, setCombinedSearchCategories
   // calls swap and takes ownership
   m_dataHolder->setCombinedSearchCategories(headings);

   m_observer.SearchHeadingsReceived();
   
   delete bff;
}

void
CombinedSearchDispatcher::handleAdditionalInfoReply(
   const isab::DataGuiMess *mess)
{
   NParamBlock params(mess->getAdditionalData(), 
                      mess->getAdditionalSize(),
                      NSC_PROTO_VER);
   
   std::vector<const NParam*> additionalParams;
   params.getAllParams(1900, additionalParams);
   //typedef std::map< char*, vector<AdditionalInfo> > idToAdditionalInfoMap;
   //idToAdditionalInfoMap m_infoForMatchesMap;
   
   for (size_t i = 0; i < additionalParams.size(); ++i) {
      // Get data
      uint32 pos = 0;
      const char* idStr = additionalParams[i]->incGetString(pos);
      byte nbrTuples = additionalParams[i]->incGetByte(pos);
      std::vector<AdditionalInfo*> data;
      for (byte j = 0; j < nbrTuples; ++j) {
         byte type = additionalParams[i]->incGetByte(pos);
         const char* key = additionalParams[i]->incGetString(pos);
         const char* value = additionalParams[i]->incGetString(pos);
         data.push_back(new AdditionalInfo(key, value, type));
      }
      m_infoForMatchesMap.insert(make_pair(strdup_new(idStr), data));
   }

   m_observer.AdditionalInfoReceived(m_infoForMatchesMap);
}

bool CombinedSearchDispatcher::compareIconCrc( const char* serverCrc )
{   
   char filename[] = IMG_CRC_FILENAME;
   char* completeName = new char[strlen(m_headingAndHitImagesPath)
                                 + strlen(filename) + 1];
   strcpy(completeName, m_headingAndHitImagesPath);
   strcat(completeName, filename);
   FILE* crcFile = fopen(completeName, "r");
   if (crcFile != NULL) {
      fseek(crcFile, 0, SEEK_END);
      int fileSize = ftell(crcFile);

      fseek( crcFile, 0, SEEK_SET);

      char* storedCrc = new char[fileSize + 1];
      fread(storedCrc, 1, fileSize, crcFile);
      fclose(crcFile);
      storedCrc[fileSize] = '\0';
      bool equalCrc = (strcmp(storedCrc, serverCrc) == 0);
      delete[] storedCrc;
      delete[] completeName;
      if (equalCrc) {
         return true;
      } else {
         return false;
      }
   } else {
      delete[] completeName;
      return false;
   }
}

void CombinedSearchDispatcher::saveIconCrc( const char* serverCrc )
{
   char filename[] = IMG_CRC_FILENAME;
   char* completeName = new char[strlen(m_headingAndHitImagesPath) +
                                 strlen(filename) + 1];
   strcpy(completeName, m_headingAndHitImagesPath);
   strcat(completeName, filename);
   FILE* crcFile = fopen(completeName, "w");
   if (crcFile != NULL) {
      fwrite(serverCrc, 1, strlen(serverCrc), crcFile);
      fclose(crcFile);
   }
   delete[] completeName;
}

void CombinedSearchDispatcher::clearStoredIcons(const char* imagePath)
{
#ifdef _WIN32_WCE
   //TODO: Add funky implementation here.

   std::string searchQuery( imagePath );
   searchQuery += (searchQuery.empty()
               || (searchQuery[searchQuery.size()-1] != '\\'
                   && searchQuery[searchQuery.size()-1] != ':')) ? "\\*" : "*";
   
   WIN32_FIND_DATA data;

   HANDLE fileHandle;

   WCHAR wideSearchQuery[MAX_PATH];
   WCHAR widePath[MAX_PATH];
   WCHAR wideExt[MAX_PATH];
   WCHAR tmpFileName[MAX_PATH];

   MultiByteToWideChar(CP_ACP, 0, m_imageExt, -1, wideExt, MAX_PATH);
   MultiByteToWideChar(CP_ACP, 0, searchQuery.c_str(), -1, wideSearchQuery, MAX_PATH);
   MultiByteToWideChar(CP_ACP, 0, imagePath, -1, widePath, MAX_PATH);
   
   if ( (fileHandle = ::FindFirstFile( wideSearchQuery, &data ))
        == INVALID_HANDLE_VALUE )
   {
      //Invalid path
      fileHandle = 0;
      return;
   }
   
   do {
      // Current file
      if ((wcscmp(data.cFileName, L".") == 0 ||
           wcscmp(data.cFileName, L"..") == 0)) {
         continue;
      }
      // get the image files
      if (wcsstr(data.cFileName, wideExt)) {
         if ( data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
            // This is a directory and not a file, do nothing.
            continue;
         } else {
            wsprintf(tmpFileName, L"%s%s", widePath, data.cFileName);
            DeleteFile(tmpFileName);
         }
      }
      
   } while(FindNextFile(fileHandle, &data) != 0);

   FindClose(fileHandle);
   
   return;
   
#else
   //open image dir
   DIR* pDir = opendir(imagePath);
   struct dirent* pDirentStruct;
   struct stat st;
   if (pDir != NULL) {
      while( (pDirentStruct = readdir(pDir)) ) {
         //skip if find . and ..
         if ((strcmp(pDirentStruct->d_name, ".") == 0 ||
              strcmp(pDirentStruct->d_name, "..") == 0)) {
            continue;
         }
         // get the image files
         if (strstr(pDirentStruct->d_name, m_imageExt)) {
            char* filename = pDirentStruct->d_name;
            char* completeName = 
               new char[strlen(imagePath) + strlen(filename) + 1];
            strcpy(completeName, imagePath);
            strcat(completeName, filename);
            int statret = stat(completeName, &st);
            if (statret != -1 && S_ISDIR(st.st_mode)) {
               // This is a directory not a file, do nothing.
            } else {
               // This is a img file, delete it.
               remove(completeName);
            }
            delete[] completeName;
         }
      }
      closedir(pDir);
   }
#endif
}

void CombinedSearchDispatcher::clearInfoForMatchesMap()
{
   for (MSearchResultObserver::idToAdditionalInfoMap::iterator it =
           m_infoForMatchesMap.begin();
        it != m_infoForMatchesMap.end(); ++it) {
      delete[] it->first;
      for (MSearchResultObserver::AdditionalInfoVec::iterator sit =
              it->second.begin();
           sit != it->second.end(); ++sit) {
         delete (*sit);
      }
   }
   m_infoForMatchesMap.clear();
}

void CombinedSearchDispatcher::calcTotalNbrHits(const isab::DataGuiMess* mess)
{
   const std::map<uint32, CombinedSearchCategory*>& cats = m_dataHolder->getCombinedSearchCategories();

   isab::NParamBlock params(mess->getAdditionalData(), mess->getAdditionalSize(), NSC_PROTO_VER);
   int categories = cats.size();
   uint32 headNo = 0;
   for( int a = 0; a < categories; ++a ) {
      // Iterate through all categegories received from NAV_SEARCH_DESC_REPLY
      // Get the heading from the parameter
      const NParam *hdng = params.getParam(5700 + a * 2);
      if( hdng ) {
         isab::NonOwningBuffer* bff = new isab::NonOwningBuffer( const_cast<uint8 *>( hdng->getBuff() ),
                                                                 hdng->getLength() );
         // Store the current read pos, since bff doesnt copy the data 
         // we dont want to break the buffer for later reading.
         uint32 startReadPos = bff->getReadPos();

         // Get the heading, neede for mapping in the data holder         
         headNo = bff->readNext32bit();
         uint32 type = bff->readNext32bit();
         uint32 totalNbrHits = 0;   
         uint32 topHits = 0;
         // Ignore, we just want number of hits
         bff->readNext32bit(); 
         // Get total number or hits
         totalNbrHits = bff->readNext32bit(); 
         // Get the number of top hits for this heading
         topHits = bff->readNext32bit();
         if (m_areaMatchSearch) {
            m_dataHolder->replaceSearchCategoryResults(headNo, type, totalNbrHits, topHits);
         } else {
            // Store the heading and the total number of hits in the data holder
            m_dataHolder->setSearchCategoryResults(headNo, type, totalNbrHits, topHits);
         }
         // Reset the read pos in the buffer
         bff->setReadPos(startReadPos);
         delete bff;
      }
   }
   // Report to the observer, no search hits are actually processed here, only
   // number of hits for each heading is processed and reported. Processing of 
   // search results will be done a bit later.
   if (!m_areaMatchSearch) {
      m_observer.TotalNbrHitsReceived(m_dataHolder->getCombinedSearchCategoryResults());
   }
}

bool
CombinedSearchDispatcher::processSearchItem(
   isab::NonOwningBuffer* bff,
   uint32 type,
   uint32 topHits,
   int& topHitsAdded,
   std::vector<isab::SearchItem>& items,
   std::vector<isab::SearchRegion *>& regions,
   std::vector<isab::SearchItem>& topHitItems)
{   
   uint16 searchType = 0;
   long lat = 0;
   long lon = 0;
   int distance = -1;
   uint8 subType = 0;
   uint8 advert = 0;
   
   if (type == 0) {
      // Type is a searh hit
      // Search type, SearchItemType enum
      searchType = bff->readNext8bit();
      // Sub type, SearchItemsSubType enum
      subType = bff->readNext8bit();
      // Determines if advertisement or not
      advert = bff->readNext8bit();
      // Internal id of the match
   } else if (type == 1) {
      searchType = bff->readNextUnaligned16bit();
   }
   const char *id = bff->getNextCharString();
   if( id ) {               
      const char *name = bff->getNextCharString();
               
      // skip image name since SearchItem does not contain it
      const char *imageName = bff->getNextCharString();  
      if (type == 0) {
         lat = bff->readNextUnaligned32bit();
         lon = bff->readNextUnaligned32bit();
      }
      int rgn_cnt = bff->readNext8bit();
      SearchRegion** rgns_temp = 0;
      if( rgn_cnt > 0 ) {
         rgns_temp = new SearchRegion*[ rgn_cnt ];
         for( int c = 0; c < rgn_cnt; ++c ) {
            int index = bff->readNextUnaligned16bit();
                      
            if( ( (int)regions.size() > index ) && ( index >= 0 ) ) { 
               rgns_temp[ c ] = regions[ index ];
            } else {
               --c;
               --rgn_cnt;
            }
         }
      } else {
         // Just to make sure we don't send any negative numbers when
         // creating the searchitem
         rgn_cnt = 0;
      }
      if (topHitsAdded < (int)topHits) {
         // Add the top hit to the vector
         topHitItems.push_back( SearchItem( name, id, searchType,
                                            subType, distance, 
                                            rgns_temp, rgn_cnt,
                                            lat, lon, 2, 
                                            imageName, advert ) );
         topHitsAdded++;
      }

      items.push_back( SearchItem( name, id, searchType, subType,
                                   distance, rgns_temp, rgn_cnt,
                                   lat, lon, 2, 
                                   imageName, advert ) );

      if (rgns_temp) {
         delete[] rgns_temp;
      }
      // We add this image to be downloaded, if we need to.
      if(m_useImageDimension && strcmp(m_imageExt, ".png") == 0) {
         SharedImageFileUtil::checkAndAddImageName(m_neededHitImages,
                                                   imageName,
                                                   m_headingAndHitImagesPath,
                                                   m_imageExt,
                                                   m_imageDimension);
      } else {
         SharedImageFileUtil::checkAndAddImageName(m_neededHitImages,
                                                   imageName,
                                                   m_headingAndHitImagesPath,
                                                   m_imageExt);
      }
   } else {
      return false;
   }

   return true;
}

void
CombinedSearchDispatcher::processHeading(const NParam* hdng, const NParam* rgns,
                                         std::vector<isab::SearchItem>& topHitItems)
{
   // Create a buffer that will contain all heading data.
   isab::NonOwningBuffer* bff =
      new isab::NonOwningBuffer( const_cast<uint8 *>( hdng->getBuff() ),
                                 hdng->getLength() );

   std::vector<isab::SearchItem> items;
   std::vector<isab::SearchRegion *> regions;
          
   if( rgns ) {
      isab::NonOwningBuffer *rgn_bff = new isab::NonOwningBuffer(
         const_cast<uint8 *>( rgns->getBuff() ),
         rgns->getLength() );

      while( rgn_bff->remaining() ){
         regions.push_back( new isab::SearchRegion( rgn_bff ) );
      }

      delete rgn_bff;
   }

   uint32 headNo = bff->readNext32bit();
   uint32 type = bff->readNext32bit();
   uint32 totalNbrHits = 0;
   uint32 topHits = 0;
   char* advertHitTitle = NULL;
   char* normHitTitle = NULL;
   // Index of first match, skipped since SearchItem does not 
   // a member for storing this
   bff->readNext32bit(); 
   // Get the total number of hits
   totalNbrHits = bff->readNext32bit();
   // Get the number of top hits for this heading
   topHits = bff->readNext32bit();
   // Get the advert title
   advertHitTitle = bff->getNextCharStringAlloc();
   // Get the normal heading title
   normHitTitle = bff->getNextCharStringAlloc();
   int topHitsAdded = 0;
   //          if( type == 0 ) {

   
   while( bff->remaining() ) {
      bool keepGoing = processSearchItem(bff, type, topHits, topHitsAdded,
                                         items, regions, topHitItems);

      if(!keepGoing) {
         break;
      }
   }
   
   if (m_moreHitsReq) {
      m_dataHolder->appendSearchResults(headNo, items);
   } else if (m_areaMatchSearch) {
      // Got the results from an area match search, replace existing
      // category.
      m_dataHolder->replaceSearchResults(headNo, 
                                         type, 
                                         totalNbrHits, 
                                         topHits, 
                                         advertHitTitle,
                                         normHitTitle,
                                         items);
   } else {
      m_dataHolder->setSearchResults(headNo, 
                                     type,
                                     totalNbrHits, 
                                     topHits,
                                     advertHitTitle,
                                     normHitTitle,
                                     items);
   }

   // Delete title strings since they have been allocated
   delete[] advertHitTitle;
   delete[] normHitTitle;

   std::vector<isab::SearchRegion *>::iterator a = regions.begin();
   std::vector<isab::SearchRegion *>::iterator end = regions.end();
   while( a != end ) {
      delete ( * ( a++ ) );  
   }

   delete bff;
}

void
CombinedSearchDispatcher::handleCombinedSearchResultsGot(
   const isab::DataGuiMess *mess)
{   
   if (!m_moreHitsReq) {
      // Get number of hits for each heading and report to the observer,
      // processing of search hits will be done after this.
      calcTotalNbrHits(mess);
   }
   const std::map<uint32, CombinedSearchCategory*>& headings =
      m_dataHolder->getCombinedSearchCategories();

   // Create the param that will contain the actuall search hits
   isab::NParamBlock params(mess->getAdditionalData(),
                            mess->getAdditionalSize(), NSC_PROTO_VER);

   const NParam* titleParam = params.getParam(6000);
   
   if (titleParam) {
      // Create a buffer that will the two titles, advertisement 
      // title and normal heading title.
      isab::NonOwningBuffer* titleBuf = new isab::NonOwningBuffer(
         const_cast<uint8 *>( titleParam->getBuff() ),
         titleParam->getLength() );
      
      m_dataHolder->setAdvertHeadingTitle(titleBuf->getNextCharString());
      m_dataHolder->setNormHeadingTitle(titleBuf->getNextCharString());
      delete titleBuf;
   }
   
   // Vector that will hold all top hits found in all the headings
   std::vector<isab::SearchItem> topHitItems;
   int nbrHeadings = headings.size();
   for( int a = 0; a < nbrHeadings; ++a ) {
      // Iterate through all headings received from NAV_SEARCH_DESC_REPLY
      // Get the heading from the parameter
      const NParam *hdng = params.getParam(5700 + a * 2);
      // Get the regions for current heading
      const NParam *rgns = params.getParam(5701 + a * 2);
      if( hdng ) {
         processHeading(hdng, rgns, topHitItems);
      }
   }
   
   // Set the top hits to the data holder. 
   m_dataHolder->setTopHitResults( topHitItems );
   
   if (m_round == m_maxRound ){
      m_searchInitiated = false;
      if (m_neededHitImages.size() == 0){
         // The search is complete and no result images
         // need to be downloaded.
         m_observer.ResultImagesStatusUpdated(
            MSearchResultObserver::CURRENT_IMAGES_OK);
      } else {
         // Images must be downloaded before they can be used.
         m_observer.ResultImagesStatusUpdated(
            MSearchResultObserver::CURRENT_IMAGES_NOT_OK);
         downloadNeededImages(m_neededHitImages);
      }
   }
   m_round++;

   // This could be used if we want the gui to wait until both round one and
   // two are finished, if the gui should be updated for each round this
   // should not be used.
   if (m_moreHitsReq) {
      m_observer.MoreSearchResultReceived();
   } else if (m_areaMatchSearch) {
      // Call to the observer that received reply on area match search request.
      m_observer.AreaMatchSearchResultReceived();
   } else {
      m_observer.SearchResultReceived();
   }
}

void CombinedSearchDispatcher::downloadNeededImages(const CharVec& imageNames)
{
   if( imageNames.size() ) {
      // Iterate through the vector and download all the needed images.
      CharVec::const_iterator it =
         imageNames.begin();
      while( it != imageNames.end() ) {
         if(m_useImageDimension && strcmp(m_imageExt, ".png") == 0) {
            m_imageHandler->FetchImageUsingNewTMapInterface((*it), false);
         } else {
            m_imageHandler->FetchImageUsingOldTMapInterface((*it), false);
         }
         ++it;
      }
   }
}

void CombinedSearchDispatcher::ImageReceived(const char* imageName)
{
   // We start by checking if the image is a requested category image,
   // and if it is the last one in the needed category image set we
   // do an appropriate callback.
   if (SharedImageFileUtil::checkAllImagesReceived(m_neededCategoryImages,
                                                   imageName)) {
      // Got all images, notify observer
      m_observer.CategoryImagesStatusUpdated(
         MSearchResultObserver::UPDATED_IMAGES_OK);
   }

   // Next we check if the image is a requested heading image,
   // and if it is the last one in the needed heading image set we
   // do an appropriate callback.
   if (SharedImageFileUtil::checkAllImagesReceived(m_neededHeadingImages,
                                                   imageName)) {
      // Got all images, notify observer
      m_observer.HeadingImagesStatusUpdated(
         MSearchResultObserver::UPDATED_IMAGES_OK);
   }

   // Last but not least we check if the image is a requested hit image,
   // and if it is the last one in the needed hit image set we
   // do an appropriate callback.
   if (SharedImageFileUtil::checkAllImagesReceived(m_neededHitImages,
                                                   imageName)) {
      // Got all images, notify observer
      m_observer.ResultImagesStatusUpdated(
         MSearchResultObserver::UPDATED_IMAGES_OK);
   }
}

void
CombinedSearchDispatcher::ImageReceived(const char* imageName,
                                        const uint8* imageBuffer,
                                        uint32 imageBufferLength)
{
   // We will not use this for now. All the images are downloaded
   // to file instead of to buffer.
}

void CombinedSearchDispatcher::ImageReceivedError(const char* imageName)
{
   // No error handling for the moment, if the image failed to be dowloaded
   // we will display the fall back image.
   ImageReceived(imageName);
}

void CombinedSearchDispatcher::searchRequestFailed()
{
   // We got a report from the message receiver that one search request
   // has failed. We need to check if this is the only or last outstanding
   // request. We also need to increment the m_round since we wont get a
   // call to a proper search reply function.

   if (m_round >= m_maxRound) {
      // Message receiver got a request_failed
      m_searchInitiated = false;      
   }
   m_round++;
}

const CombinedSearchDataHolder&
CombinedSearchDispatcher::getCombinedSearchDataHolder() const
{
   return *m_dataHolder;
}

CombinedSearchDataHolder&
CombinedSearchDispatcher::getCombinedSearchDataHolder() 
{
   return *m_dataHolder;
}

const std::map<uint32, CombinedSearchCategory*>&
CombinedSearchDispatcher::getCombinedSearchCategories() 
{
   return m_dataHolder->getCombinedSearchCategories();
}

const std::vector<CombinedSearchCategory*>&
CombinedSearchDispatcher::getCombinedSearchResults() 
{
   return m_dataHolder->getCombinedSearchResults();
}

const std::vector<CombinedSearchCategory*>&
CombinedSearchDispatcher::getCombinedSearchCategoryResults()
{
   return m_dataHolder->getCombinedSearchCategoryResults();
}

uint16 CombinedSearchDispatcher::getCurrentRound() const
{
   return m_round;
}

uint16 CombinedSearchDispatcher::getOutstandingRounds() const
{
   return m_maxRound - m_round;
}

void
CombinedSearchDispatcher::setHeadingsAndHitsImagePath(const char* imagePath)
{
   if (m_headingAndHitImagesPath) {
      delete[] m_headingAndHitImagesPath;
   } 
   m_headingAndHitImagesPath = strdup_new(imagePath);
}

void
CombinedSearchDispatcher::setCategoryImagePath(const char* categoryImagePath)
{
   if (m_categoryImagePath) {
      delete[] m_categoryImagePath;
   } 
   m_categoryImagePath = strdup_new(categoryImagePath);   
}

void
CombinedSearchDispatcher::setImageExt(const char* imageExt)
{
   if (m_imageExt) {
      delete[] m_imageExt;
   } 
   m_imageExt = strdup_new(imageExt);
}

void
CombinedSearchDispatcher::setImageDimension(
   CombinedSearch::CSImageDimension imageDimension)
{
   m_imageDimension = imageDimension;
   if(imageDimension.width != 0 && imageDimension.height != 0) {
      m_useImageDimension = true;
   } else {
      // hmmm... we got invalid image dimensions, lets not use them
      m_useImageDimension = false;
   }
}

void
CombinedSearchDispatcher::getCategoryIcons(CharVec& iconList)
{
   for (CharVec::const_iterator it = iconList.begin(), end = iconList.end();
        it != end; ++it) {
      if(m_useImageDimension && strcmp(m_imageExt, ".png") == 0) {
         SharedImageFileUtil::checkAndAddImageName(m_neededCategoryImages,
                                                   *it, m_categoryImagePath,
                                                   m_imageExt,
                                                   m_imageDimension);
      } else {
         SharedImageFileUtil::checkAndAddImageName(m_neededCategoryImages,
                                                   *it, m_categoryImagePath,
                                                   m_imageExt);
      }
   }
   
   if (m_neededCategoryImages.size() == 0){
      // We don't need to download any category images!
      m_observer.CategoryImagesStatusUpdated(
         MSearchResultObserver::CURRENT_IMAGES_OK);
      
   } else {
      // We need to download images before they can be used.
      m_observer.CategoryImagesStatusUpdated(
         MSearchResultObserver::CURRENT_IMAGES_NOT_OK);
      downloadNeededImages(m_neededCategoryImages);   
   }
}

void
CombinedSearchDispatcher::clearCategoryIcons()
{
   m_observer.CategoryImagesStatusUpdated(
      MSearchResultObserver::CURRENT_IMAGES_NOT_OK);
   clearStoredIcons(m_categoryImagePath);
}


