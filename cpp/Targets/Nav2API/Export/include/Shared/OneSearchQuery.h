/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_ONE_SEARCH_QUERY_H
#define WFAPI_ONE_SEARCH_QUERY_H

#include "WFAPIConfig.h"
#include "WGS84Coordinate.h"
#include "WFString.h"
#include <vector>

#define DEFAULT_MAX_SEARCH_HITS 100
namespace WFAPI {

/**
 * The possible types of searches available. Either you search for everything,
 * or you just want to match addresses.
 */
enum SearchType {
   SEARCH_FOR_EVERYTHING = 0,
   SEARCH_FOR_ADDRESSES
};

/**
 * The sorting strategy for the server to use when returning search hits.
 * Sort by distance from a given position/place, or sort alphabetically.
 */
enum SearchSorting {
      DISTANCE_SORT = 0,
      ALFA_SORT
};

/**
 * The request data for searches. This is the data that can be set
 * for a search.
 */
class OneSearchQuery
{

public:
  
   /**
    * Constructor.
    * Use this constructor when searching for a string around the users
    * position (which you provide).
    *
    * Typically you need this when using LBS system to get the users
    * coordinate, and the user wants to search for a specific street or object
    * around him.
    *
    * @param what What to Search for.
    * @param position Search hits will be sorted around this position.
    * @param searchType What type of search this is. Possible values are
    *                   "address" and "all". The type "address" is intended for
    *                   use with round 0.
    * @param round The round to search in, round 0 means that the search
    *              will be done only in internal databases, round 1 means that
    *              the search will be done in third party directories only. If
    *              set to WF_MAX_INT32 the search will be done in all
    *              directories.
    */
   OneSearchQuery(const WFString& what,
                  const WGS84Coordinate& position,
                  SearchType searchType = SEARCH_FOR_EVERYTHING,
                  wf_int32 round = WF_MAX_INT32);

   /**
    * Constructor.
    * Use this constructor when making a search for what, where and country
    * (TopRegionID).
    * Use this when you want to search for things around some arbitrary adress
    * rather than a position. This will search in all possible categories.
    *
    * @param what What to Search for.
    * @param where Used to limit the search hits to those in matching
    *              areas (City or area).
    * @param topRegionID In what region/country/state to search in.
    * @param searchType What type of search this is. Possible values are
    *                   "address" and "all". The type "address" is intended for
    *                   use with round 0.
    * @param round The round to search in, round 0 means that the search
    *              will be done only in internal databases, round 1 means that
    *              the search will be done in third party directories only. If
    *              set to WF_MAX_INT32 the search will be done in all
    *              directories.
    */
   OneSearchQuery(const WFString& what,
                  const WFString& where,
                  wf_uint32 topRegionID,
                  SearchType searchType = SEARCH_FOR_EVERYTHING,
                  wf_int32 round = WF_MAX_INT32);

   /**
    * Constructor.
    * Use this constructor when making a search for what, where and country
    * (TopRegionID).
    * Use this when you want to search for things around some arbitrary adress
    * rather than a position. This will search in the categories you specify.
    *
    * @param what What to Search for.
    * @param where Used to limit the search hits to those in matching
    *              areas (City or area).
    * @param topRegionID In what region/country/state to search in.
    * @param categoryIDs One or more specific categories to search in.
    * @param searchType What type of search this is. Possible values are
    *                   "address" and "all". The type "address" is intended for
    *                   use with round 0.
    * @param round The round to search in, round 0 means that the search
    *              will be done only in internal databases, round 1 means that
    *              the search will be done in third party directories only. If
    *              set to WF_MAX_INT32 the search will be done in all
    *              directories.
    */
   OneSearchQuery(const WFString& what,
                  const WFString& where,
                  wf_uint32 topRegionID,
                  std::vector<wf_int32> categoryIDs,
                  SearchType searchType = SEARCH_FOR_EVERYTHING,
                  wf_int32 round = WF_MAX_INT32);
   
   /**
    * Constructor.
    * Use this constructor if you really know what you are doing, and have very
    * specific needs that the above constructors cannot fulfill.
    *
    * Warning: The location where the search should be performed is specified
    * by either using parameter ("position"), or ("where" and
    * "topRegionID"). Exactly one of these two combinations must be used.
    * 
    * @param what What to Search for. Can be an empty string.
    * @param where Used to limit the search hits to those in matching
    *              areas (City or area). Can be an empty string.
    * @param categoryIDs The category to search within instead of search
    *                   using a string. Use WF_MAX_UINT32 if not to search by
    *                   categoryIDs.
    * @param position The position to find matches around. Ignored if set to
    *                 (180,180)
    * @param radius The maximum distance from the position to find
    *                     matches To ignore this value, set it to 0.
    * @param topRegionID In what region/country/state to search. Pass an empty
    *                    list to ignore this parameter.
    * @param maxHits The maximum number of hits wanted.
    * @param round The round to search in, round 0 means that the search
    *              will be done only in internal databases, round 1 means that
    *              the search will be done in third party directories only. If
    *              set to WF_MAX_INT32 the search will be done in all
    *              directories.
    * @param searchSorting In what order to return the matches, see SearchSorting
    *                      enumeration.
    * @param includeInfoFields set if to include InfoItems in each search
    *                          result item
    * @param searchType What type of search this is. Possible values are
    *                   "address" and "all". The type "address" is intended for
    *                   use with round 0.
    */
   OneSearchQuery(const WFString& what,
                  const WFString& where,
                  std::vector<wf_int32> categoryIDs,
                  const WGS84Coordinate& position,
                  wf_int32 radius,
                  wf_uint32 topRegionID,
                  SearchType searchType = SEARCH_FOR_EVERYTHING,
                  wf_int32 maxHits = DEFAULT_MAX_SEARCH_HITS,
                  wf_int32 round = WF_MAX_INT32,
                  SearchSorting searchSorting = DISTANCE_SORT,
                  bool includeInfoFields = true
                  );

   /**
    * Destructor.
    */
   virtual ~OneSearchQuery();

   /**
    * Get the what string from this search.
    *
    * @return The what string.
    */
   const WFString& getWhat() const;

   /**
    * Get the where string from this search.
    *
    * @return The where string.
    */
   const WFString& getWhere() const;

   /**
    * Get the categories ID from this search.
    *
    * @return The categories ID.
    */
   const std::vector<wf_int32>& getCategoryIDs() const;
   
   /**
    * Get the position from this search.
    *
    * @return The position.
    */
   const WGS84Coordinate& getPosition() const;

   /**
    * Get the radius for the search matches.
    *
    * @return The radius in meters.
    */
   wf_int32 getRadius() const;
   
   /**
    * Get the top region id from this search.
    *
    * @return The top region id.
    */
   wf_uint32 getTopRegionID() const;

   /**
    * Get the search type used for this search.
    *
    * @return The search type.
    */
   SearchType getSearchType() const;

   /**
    * Get the max number of hits for this search.
    *
    * @return The max number of hits.
    */
   wf_int32 getMaxHits() const;

   /**
    * Get the round set for this search.
    *
    * @return The round set for this search query.
    */
   wf_int32 getRound() const;

   /**
    * Get the sorting strategy used for this search.
    *
    * @return The sorting strategy.
    */
   SearchSorting getSearchSorting() const;

   /** Get if we want to include the info fields in the search result
    *
    * @return if we want to include the info field
    */
   bool getIncludeInfoFields() const;

protected:

   /// Internal function
   void init(const WFString& what,
             const WFString& where,
             std::vector<wf_int32> categoryIDs,
             const WGS84Coordinate& position,
             wf_int32 radius,
             wf_uint32 topRegionID,
             SearchType searchType = SEARCH_FOR_EVERYTHING,
             wf_int32 maxHits = DEFAULT_MAX_SEARCH_HITS,
             wf_int32 round = WF_MAX_INT32,
             SearchSorting searchSorting = DISTANCE_SORT,
             bool includeInfoFields = true
      );
   
private:

   /// A WFString that holds what to search for.
   WFString m_what;

   /**
    * A WFString that holds where to search. Used to pick which search hits
    * that are interesting.
    */
   WFString m_where;

   /// The ID of the category to search within.
   std::vector<wf_int32> m_categoryIDs;

   /// Search hits will be sorted around this position.
   WGS84Coordinate m_position;

   /// The radius in meter to use when searching around a position
   wf_int32 m_radius;

   /// In what region/country/state the search will be done.
   wf_uint32 m_topRegionID;

   /// the type of search to do, eg. search for only addresses or everything.
   SearchType m_searchType;

   /// The maximum number of hits wanted.
   wf_int32 m_maxHits;

   /// The round to search in for this search.
   wf_int32 m_round;

   /// The sorting strategy for the server to use when returning the hits.
   SearchSorting m_searchSorting;

   /// if to include the info fields in the search hits.
   bool m_includeInfoFields;

};

} // End namespace WFAPI

#endif // End WFAPI_SEARCH_QUERY_H
