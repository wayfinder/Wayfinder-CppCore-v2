/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef NAVONESEARCHREQ_H
#define NAVONESEARCHREQ_H

#include "NGP.h"
#include "NavPacket.h"
// #include "Coordinate.h"
#include "NavServerComEnums.h"
#include "Common.h"
#include <vector>
#include <string>

namespace wf_ngp {

/**
 *  NAV_ONE_SEARCH_REQ 
 *
 *    This request must contain a Language of client, see Section
 *    7.1, parameter.
 *    
 *    
 *    
 *    Parameter 6802 is optional in this version of the request.
 *    The location where the search should be performed is specified
 *    by either sending parameter 6802 or parameter 6808 and 6809.
 */
class NavOneSearchReq : public isab::NavRequestPacket {
public:
   struct SearchSortingTypeEnum {
      /// Enum describing search sorting types.
      enum enum_t {
         DISTANCE_SORT = 0x00,
         ALFA_SORT = 0x01,
      };
      
      // This is for simple forward declares and casts of enums.
      SearchSortingTypeEnum():m_t(enum_t(0)) {}
      template <typename T>
      inline SearchSortingTypeEnum( T t ):m_t(enum_t(t)) {}
      operator enum_t() const { return m_t; }
      enum_t m_t;
   }; // struct SearchSortingTypeEnum
   
   struct SearchTypeEnum {
      /// Enum describing search types.
      enum enum_t {
         ALL = 0x00,
         ADDRESS = 0x01,
      };
      
      // This is for simple forward declares and casts of enums.
      SearchTypeEnum():m_t(enum_t(0)) {}
      template <typename T>
      inline SearchTypeEnum( T t ):m_t(enum_t(t)) {}
      operator enum_t() const { return m_t; }
      enum_t m_t;
   }; // struct SearchTypeEnum
   
   
   /**
    * Buffer constructor.
    * 
    * @param buff The buffer containing the data for the
    *             packet, assuming STX and length is stripped.
    * @param buffLen The length of the buffer in bytes.
    */
   NavOneSearchReq(const byte* buff, uint32 buffLen)
    : NavRequestPacket(buff, buffLen) 
   {
      uint32 index = 0;
      if(m_params.getParam(6808) != NULL) {
         m_queryLocation = m_params.getParam(6808)->getString();
      }
      if(m_params.getParam(6809) != NULL) {
         m_topRegionId = m_params.getParam(6809)->getUint32();
      }
      if(m_params.getParam(6806) != NULL) {
         m_includeInfoFields = m_params.getParam(6806)->getBool();
      }
      if(m_params.getParam(6810) != NULL) {
         m_searchType = SearchTypeEnum(m_params.getParam(6810)->getInt32());
      }
      if(m_params.getParam(6804) != NULL) {
         m_maxNbrMatches = m_params.getParam(6804)->getInt32();
      }
      if(m_params.getParam(6805) != NULL) {
         m_searchRound = m_params.getParam(6805)->getInt32();
      }
      m_searchPosition = Coordinate(m_params.getParam(6802), index);
      if(m_params.getParam(6803) != NULL) {
         m_searchRadius = m_params.getParam(6803)->getInt32();
      }
      if(m_params.getParam(6800) != NULL) {
         m_searchMatchQuery = m_params.getParam(6800)->getString();
      }
      
      { /* Array block */ 
         const isab::NParam* param = m_params.getParam(6801);
         
         uint32 arrIndex = 0;
         
         m_categoryIDs.reserve(param->getLength() / sizeof(int32));
         
         while(arrIndex < param->getLength()) {
            int32 tmp = param->getInt32(arrIndex);
            m_categoryIDs.push_back(tmp);
         }
      }
      
      if(m_params.getParam(6807) != NULL) {
         m_searchSorting = SearchSortingTypeEnum(m_params.getParam(6807)->getInt32());
      }
      index = 0; // To avoid unused variable warning
   }
   
   virtual ~NavOneSearchReq() {}
   
   /**
    * NavOneSearchReq object constructor.
    * 
    * @param queryLocation The city or area to search in...
    * @param topRegionId The top region to seach in...
    * @param includeInfoFields Default is to include InfoItems use this...
    * @param searchType What we are searching for, (0x00 = all, ...
    * @param maxNbrMatches The maximum number of  matches to return...
    * @param searchRound The search round. First round 0 is the f...
    * @param searchPosition The position (MC2 format) to find matche...
    * @param searchRadius The maximum distance from the SearchPosi...
    * @param searchMatchQuery Text to find matches, if not sent or emp...
    * @param categoryIDs The categories to limit search to, not u...
    * @param searchSorting In what order to return the matches. 0x0...
    */
   NavOneSearchReq(
      const std::string& argQueryLocation,
      uint32 argTopRegionId,
      bool argIncludeInfoFields,
      const SearchTypeEnum& argSearchType,
      int32 argMaxNbrMatches,
      int32 argSearchRound,
      const Coordinate& argSearchPosition,
      int32 argSearchRadius,
      const std::string& argSearchMatchQuery,
      const std::vector< int32 >& argCategoryIDs,
      const SearchSortingTypeEnum& argSearchSorting) : 
      isab::NavRequestPacket(NGP_PROTOCOL_VERSION,
                             isab::navRequestType::NAV_ONE_SEARCH_REQ, 0, 2),
      m_queryLocation(argQueryLocation),
      m_topRegionId(argTopRegionId),
      m_includeInfoFields(argIncludeInfoFields),
      m_searchType(argSearchType),
      m_maxNbrMatches(argMaxNbrMatches),
      m_searchRound(argSearchRound),
      m_searchPosition(argSearchPosition),
      m_searchRadius(argSearchRadius),
      m_searchMatchQuery(argSearchMatchQuery),
      m_categoryIDs(argCategoryIDs),
      m_searchSorting(argSearchSorting)
   {}
   
   /**
    *  NavOneSearchReq default constructor
    *  
    *  @param queryLocation The city or area to search in..
    *  @param topRegionId The top region to seach in..
    *  @param includeInfoFields Default is to include InfoItems use this..
    *  @param searchType What we are searching for, (0x00 = all, ..
    *  @param maxNbrMatches The maximum number of  matches to return..
    *  @param searchRound The search round. First round 0 is the f..
    *  @param searchPosition The position (MC2 format) to find matche..
    *  @param searchRadius The maximum distance from the SearchPosi..
    *  @param searchMatchQuery Text to find matches, if not sent or emp..
    *  @param categoryIDs The categories to limit search to, not u..
    *  @param searchSorting In what order to return the matches. 0x0..
    */
   NavOneSearchReq() : 
      NavRequestPacket(NGP_PROTOCOL_VERSION,
                       isab::navRequestType::NAV_ONE_SEARCH_REQ, 0, 2),
      m_queryLocation(),
      m_topRegionId(),
      m_includeInfoFields(),
      m_searchType(),
      m_maxNbrMatches(),
      m_searchRound(),
      m_searchPosition(),
      m_searchRadius(),
      m_searchMatchQuery(),
      m_categoryIDs(),
      m_searchSorting()
   {}
   
   /**
    * Compare equals.
    */
   bool operator == (const NavOneSearchReq& rhs) const {
      return
         NavRequestPacket::operator == (rhs) &&
         m_queryLocation == rhs.m_queryLocation &&
         m_topRegionId == rhs.m_topRegionId &&
         m_includeInfoFields == rhs.m_includeInfoFields &&
         m_searchType == rhs.m_searchType &&
         m_maxNbrMatches == rhs.m_maxNbrMatches &&
         m_searchRound == rhs.m_searchRound &&
         m_searchPosition == rhs.m_searchPosition &&
         m_searchRadius == rhs.m_searchRadius &&
         m_searchMatchQuery == rhs.m_searchMatchQuery &&
         m_categoryIDs == rhs.m_categoryIDs &&
         m_searchSorting == rhs.m_searchSorting;
   }
   
   /**
    *  NavOneSearchReq serialization function
    *  @param param The param that will be filled with data.
    */ 
   virtual void serialize(isab::NParamBlock& block) const {
      using namespace isab;
      NParam& tmpQueryLocation = block.addParam(NParam(6808));
      tmpQueryLocation.addString(m_queryLocation.c_str());
      NParam& tmpTopRegionId = block.addParam(NParam(6809));
      tmpTopRegionId.addUint32(m_topRegionId);
      NParam& tmpIncludeInfoFields = block.addParam(NParam(6806));
      tmpIncludeInfoFields.addBool(m_includeInfoFields);
      NParam& tmpSearchType = block.addParam(NParam(6810));
      tmpSearchType.addInt32(m_searchType);
      NParam& tmpMaxNbrMatches = block.addParam(NParam(6804));
      tmpMaxNbrMatches.addInt32(m_maxNbrMatches);
      NParam& tmpSearchRound = block.addParam(NParam(6805));
      tmpSearchRound.addInt32(m_searchRound);
      NParam& tmpSearchPosition = block.addParam(NParam(6802));
      m_searchPosition.serialize(tmpSearchPosition);
      NParam& tmpSearchRadius = block.addParam(NParam(6803));
      tmpSearchRadius.addInt32(m_searchRadius);
      NParam& tmpSearchMatchQuery = block.addParam(NParam(6800));
      tmpSearchMatchQuery.addString(m_searchMatchQuery.c_str());
      NParam& tmpCategoryIDs = block.addParam(NParam(6801));
      
      for(unsigned int j = 0; j < m_categoryIDs.size(); j++) {
         tmpCategoryIDs.addInt32(m_categoryIDs[j]);
      }
      NParam& tmpSearchSorting = block.addParam(NParam(6807));
      tmpSearchSorting.addInt32(m_searchSorting);
   }
   
   // Object getters
   /// @return QueryLocation
   const std::string& getQueryLocation() const { return m_queryLocation; }
   
   /// @return TopRegionId
   uint32 getTopRegionId() const { return m_topRegionId; }
   
   /// @return IncludeInfoFields
   bool getIncludeInfoFields() const { return m_includeInfoFields; }
   
   /// @return SearchType
   const SearchTypeEnum& getSearchType() const { return m_searchType; }
   
   /// @return MaxNumberMatches
   int32 getMaxNbrMatches() const { return m_maxNbrMatches; }
   
   /// @return SearchRound
   int32 getSearchRound() const { return m_searchRound; }
   
   /// @return SearchPosition
   const Coordinate& getSearchPosition() const { return m_searchPosition; }
   
   /// @return SearchRadius
   int32 getSearchRadius() const { return m_searchRadius; }
   
   /// @return SearchMatchQuery
   const std::string& getSearchMatchQuery() const { return m_searchMatchQuery; }
   
   /// @return CategoryIDs
   const std::vector< int32 >& getCategoryIDs() const { return m_categoryIDs; }
   
   /// @return SearchSorting
   const SearchSortingTypeEnum& getSearchSorting() const { return m_searchSorting; }
   
private:
   /// The city or area to search in
   std::string m_queryLocation;
   /// The top region to seach in
   uint32 m_topRegionId;
   /// Default is to include InfoItems use this to turn it off if needed
   bool m_includeInfoFields;
   /// What we are searching for, (0x00 = all, 0x01 = address). The type 'address' is intended for use with round 0. If not sent 'all' is used.
   SearchTypeEnum m_searchType;
   /// The maximum number of  matches to return.
   int32 m_maxNbrMatches;
   /// The search round. First round 0 is the fast round, round 1 is external search and slow.
   int32 m_searchRound;
   /// The position (MC2 format) to find matches around.
   Coordinate m_searchPosition;
   /// The maximum distance from the SearchPosition to find matches at.
   int32 m_searchRadius;
   /// Text to find matches, if not sent or empty it is not used.
   std::string m_searchMatchQuery;
   /// The categories to limit search to, not used if not sent or empty.
   std::vector< int32 > m_categoryIDs;
   /// In what order to return the matches. 0x00 distance_sort, 0x01 alfa_sort
   SearchSortingTypeEnum m_searchSorting;
};

} // namespace wf_ngp
#endif // NAVONESEARCHREQ_H
