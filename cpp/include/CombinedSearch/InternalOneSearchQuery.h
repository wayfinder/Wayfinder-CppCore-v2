/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef ONE_SEARCH_RECORD_H
#define ONE_SEARCH_RECORD_H

#include "arch.h"
#include "NavOneSearchReq.h"
#include <vector>
#include <string>

namespace OneSearch {

typedef std::vector<int32> IntVec_t;

/**
 * Internal class that represents a search query for a One Search request,
 * Note that One Search is not OneLineSearch. One Search means that we will
 * get the search results in one result list, not based on headings like in
 * the old CombinedSearch.
 *
 * IMPORTANT: the m_searchSort and m_searchType are types from the request
 * class NavOneSearchReq. Important to notice is that when creating the
 * InternalOneSearchQuery based on the API query OneSearchQuery we do implicit
 * conversion betweeen the WFAPI::SearchType to
 * wf_ngp::NavOneSearchReq::SearchTypeEnum and between the WFAPI::SearchSorting
 * and wf_ngp::NavOneSearchReq::SearchSortingTypeEnum searchSort, hence it is
 * very important to keep the indexes in these enums the same.
 */

class InternalOneSearchQuery {
public:

   /** 
    * Default constructor, sets all members to default values. 
    */
   InternalOneSearchQuery();

   /** 
    * Copy constructor. Copies from other to this. 
    * 
    * @param other The InternalOneSearchQuery to copy from
    */
   InternalOneSearchQuery(const InternalOneSearchQuery& other);

   /** 
    * Constructor that takes parameters for all members.
    * 
    * @param what The search string specifying what to search for.
    * @param where Where to search, for example city.
    * @param categoryIds A list of category ids to search in
    * @param lat The latitude for the position to search around. In
    *            MC2Coordinate format.
    * @param lon The longituded for the position to search around. In
    *            MC2Coordinate format.
    * @param radius The radius for this search. Max radiues for search around
    *               the specified position.
    * @param searchType The type to search for. Can be ALL or ADDRESS.
    * @param maxHits Number of max hits in the result list.
    * @param round The round to search in. If MAX_INT32, all rounds will
    *              included in the search.
    * @param topRegionID The country id to search in.
    * @param searchSort How the search results should be sorted in the reply.
    *                   It can be either ALFA or DISTANCE
    * @param includeInfoFields If true the search items in the result list will
    *                          contain more detailed information then if false.
    */
   InternalOneSearchQuery(
      const std::string& what,
      const std::string& where,
      const IntVec_t& categoryIds,
      int32 lat,
      int32 lon,
      int32 radius,
      uint32 topRegionID,
      wf_ngp::NavOneSearchReq::SearchTypeEnum searchType,
      int32 maxHits,
      int32 round,
      wf_ngp::NavOneSearchReq::SearchSortingTypeEnum searchSort,
      bool includeInfoFields);

   /** 
    * Getter for the search string.
    * 
    * @return Returns the what search string.
    */
   const std::string& getWhat() const;

   /** 
    * Getter for the city/region string.
    * 
    * @return Returns the where string.
    */
   const std::string& getWhere() const;

   /** 
    * Getter for the list of category ids.
    * 
    * @return The list of catagory ids.
    */
   const IntVec_t& getCategoryIds() const;

   /** 
    * Getter for the latititude, in MC2Coordinate format
    * 
    * @return The latitude of the search position.
    */
   int32 getLat() const;

   /** 
    * Getter for the longitude, in MC2Coordinate format
    * 
    * @return The longitude of the search position.
    */
   int32 getLon() const;

   /** 
    * Getter for the search radius
    * 
    * @return The radius for the search query.
    */
   int32 getRadius() const;

   /** 
    * Getter for the country id.
    * 
    * @return The id of the country to search in.
    */
   uint32 getTopRegionId() const;

   /** 
    * Getter for the search type.
    * 
    * @return The type of this search,
    *         @see wf_ngp::NavOneSearchReq::SearchTypeEnum
    */
   wf_ngp::NavOneSearchReq::SearchTypeEnum getSearchType() const;

   /** 
    * Getter for the max number of hits in the search result list.
    * 
    * @return The max number of results for this search
    */
   int32 getMaxHits() const;

   /** 
    * Getter for the round to search in. For all round search this should
    * be set to MAX_INT32
    * 
    * @return The round to search in.
    */
   int32 getRound() const;

   /** 
    * Getter for the search sort.
    * 
    * @return The way the search results should be sorted in the result list,
    *         @see wf_ngp::NavOneSearchReq::SearchSortingTypeEnum
    */
   wf_ngp::NavOneSearchReq::SearchSortingTypeEnum getSearchSort() const;

   /** 
    * Getter for the flag that enables disables extended information
    * in the search result list. If true the search results will
    * contain more detailed information then if false
    * 
    * 
    * @return True if extended information is enabled, false if not.
    */
   bool getIncludeInfoFields() const;

   /** 
    * Assignment operator, assigns the values from other
    * to this.
    *
    * @param other The InternalOneSearchQuery to assign the data from.
    * 
    * @return A reference to the new assigned member.
    */
   InternalOneSearchQuery& operator= (const InternalOneSearchQuery& other);

private: // private functions

   /** 
    * Internal convenience function that initializes the members based on
    * another InternalOneSearchQuery object.
    * 
    * @param other The InternalOneSearchQuery to copy.
    */
   void init(const InternalOneSearchQuery& other);

   /** 
    * Internal convenience function that initializes the members based
    * on the parameters.
    * 
    * @param what The search string specifying what to search for.
    * @param where Where to search, for example city.
    * @param categoryIds A list of category ids to search in
    * @param lat The latitude for the position to search around. In
    *            MC2Coordinate format.
    * @param lon The longituded for the position to search around. In
    *            MC2Coordinate format.
    * @param radius The radius for this search. Max radiues for search around
    *               the specified position.
    * @param searchType The type to search for. Can be ALL or ADDRESS.
    * @param maxHits Number of max hits in the result list.
    * @param round The round to search in. If MAX_INT32, all rounds will
    *              included in the search.
    * @param topRegionID The country id to search in.
    * @param searchSort How the search results should be sorted in the reply.
    *                   It can be either ALFA or DISTANCE
    * @param includeInfoFields If true the search items in the result list will
    *                          contain more detailed information then if false.
    */
   void init(
      const std::string& what,
      const std::string& where,
      const IntVec_t& categoryIds,
      int32 lat,
      int32 lon,
      int32 radius,
      uint32 topRegionID,
      wf_ngp::NavOneSearchReq::SearchTypeEnum searchType,
      int32 maxHits,
      int32 round,
      wf_ngp::NavOneSearchReq::SearchSortingTypeEnum searchSort,
      bool includeInfoFields);

   /** 
    * Internal swap function, used for exception safe assigment
    * in assigment operator.
    * 
    * @param other The object to swap values from.
    */   
   void swap(InternalOneSearchQuery& other);

private:

   /// A string that holds what to search for.
   std::string m_what;

   /**
    * A string that holds where to search. Used to pick which search hits
    * that are interesting.
    */
   std::string m_where;

   /// The ID of the category to search within.
   IntVec_t m_categoryIDs;

   /**
    * The latitude of the search position in MC2Coordinate format.
    * Search hits will be sorted around this position.
    * If no areaID is given, this position will be usead as search area.
    */
   int32 m_lat;

   /**
    * The longitude of the search position in MC2Coordinate format.
    * Search hits will be sorted around this position.
    * If no areaID is given, this position will be usead as search area.
    */
   int32 m_lon;
   
   /// The radius in meter to use when searching around a position
   int32 m_radius;
   
   /**
    * In what region/country/state the search will be done.
    */
   uint32 m_topRegionID;

   /// the type of search to do, eg. search for only addresses or everything.
   wf_ngp::NavOneSearchReq::SearchTypeEnum m_searchType;

   /// The maximum number of hits wanted.
   int32 m_maxHits;

   /// The round to search in for this search.
   int32 m_round;

   /// The sorting strategy for the server to use when returning the hits.
   wf_ngp::NavOneSearchReq::SearchSortingTypeEnum m_searchSort;

   /// if to include the info fields in the search hits.
   bool m_includeInfoFields;

}; // End class OneSearchQuery

} // End namespace OneSearch

#endif // ONE_SEARCH_RECORD_H
