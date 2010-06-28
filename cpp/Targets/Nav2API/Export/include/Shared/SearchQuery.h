/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_SEARCH_QUERY_H
#define WFAPI_SEARCH_QUERY_H

#include "WFAPIConfig.h"
#include "WGS84Coordinate.h"
#include "WFString.h"

namespace WFAPI {


/**
 * The request data for searches. This is the data that can be set
 * for a search.
 */
class SearchQuery
{

public:
  
   /**
    * Constructor.
    *
    * @param what What to Search for.
    * @param where Used to limit the search hits to those in matching areas.
    * @param areaID An id of an SearchArea from a prevoius search that
    *               has been selected by an end user.
    * @param topRegionID In what region/country/state to search in.
    * @param position Search hits will be sorted around this position.
    * @param startIndex The wanted index of the first match in the reply,
    *                   used to get more hits for a search.
    * @param maxHits The maximum number of hits wanted starting at
    *                startIndex.
    * @param headingID Set if to get hits in a specific heading.
    * @param round The round to search in, round 0 means that the search
    *              will be done only in internal databases, round 1 means that
    *              the search will be done in third party directories only. If
    *              set to WF_MAX_UINT32 the search will be done in all
    *              directories.
    */
   SearchQuery(const WFString& what,
               const WFString& where,
               const WFString& areaID,
               wf_uint32 topRegionID,
               const WGS84Coordinate& position,
               wf_uint32 startIndex = 0,
               wf_uint32 maxHits = 25,
               wf_uint32 headingID = WF_MAX_UINT32,
               wf_uint32 round = WF_MAX_UINT32);

   /**
    * Constructor.
    * Use this constructor when making a search around the users position.
    *
    * @param what What to Search for.
    * @param categoryID The category to search within instead of search
    *                   using a string. Use WF_MAX_UINT32 if not to search by
    *                   categoryID.
    * @param position Search hits will be sorted around this position.
    * @param startIndex The wanted index of the first match in the reply,
    *                   used to get more hits for a search.
    * @param maxHits The maximum number of hits wanted starting at
    *                startIndex.
    * @param headingID Set if to get hits in a specific heading.
    * @param round The round to search in, round 0 means that the search
    *              will be done only in internal databases, round 1 means that
    *              the search will be done in third party directories only. If
    *              set to WF_MAX_UINT32 the search will be done in all
    *              directories.
    */
   SearchQuery(const WFString& what,
               wf_uint32 categoryID,
               const WGS84Coordinate& position,
               wf_uint32 startIndex = 0,
               wf_uint32 maxHits = 25,
               wf_uint32 headingID = WF_MAX_UINT32,
               wf_uint32 round = WF_MAX_UINT32);

   /**
    * Constructor
    * Use this constructor when making a search in a category with
    * a heading id, that is not a search with a gps coordinate.
    * When making a search in a category around a given postion,
    * use the constructor above instead.
    * 
    * @param what What to Search for.
    * @param categoryID The category to search within instead of search
    *                   using a string. Use WF_MAX_UINT32 if not to search by
    *                   categoryID.
    * @param topRegionID In what region/country/state to search in.
    * @param startIndex The wanted index of the first match in the reply,
    *                   used to get more hits for a search.
    * @param maxHits The maximum number of hits wanted starting at
    *                startIndex.
    * @param headingID Set if to get hits in a specific heading.
    * @param round The round to search in, round 0 means that the search
    *              will be done only in internal databases, round 1 means that
    *              the search will be done in third party directories only. If
    *              set to WF_MAX_UINT32 the search will be done in all
    *              directories.
    */
   SearchQuery(const WFString& what,
               wf_uint32 categoryID,
               wf_uint32 topRegionID,
               wf_uint32 startIndex = 0,
               wf_uint32 maxHits = 25,
               wf_uint32 headingID = WF_MAX_UINT32,
               wf_uint32 round = WF_MAX_UINT32);

   /**
    * Destructor.
    */
   virtual ~SearchQuery();

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
    * Get the area id from this search.
    *
    * @return The area id.
    */
   const WFString& getAreaID() const;

   /**
    * Get the category id from this search.
    *
    * @return The category id.
    */
   wf_uint32 getCategoryID() const;
   
   /**
    * Get the top region id from this search.
    *
    * @return The top region id.
    */
   wf_uint32 getTopRegionID() const;

   /**
    * Get the position from this search.
    *
    * @return The position.
    */
   const WGS84Coordinate& getPosition() const;

   /**
    * Get the start index from this search.
    *
    * @return The start index.
    */
   wf_uint32 getStartIndex() const;

   /**
    * Get the max number of hits for this search.
    *
    * @return The max number of hits.
    */
   wf_uint32 getMaxHits() const;

   /**
    * Get the heading id for this search.
    *
    * @return The heading id.
    */
   wf_uint32 getHeadingID() const;

   /**
    * Get the round set for this search.
    *
    * @return The round set for this search query.
    */
   wf_uint32 getRound() const;
   
   /**
    * Sets the what string.
    *
    * @param what The new what string.
    */
   void setWhat(const WFString& what);

   /**
    * Sets the where string.
    *
    * @param where The new where string.
    */
   void setWhere(const WFString& where);

   /**
    * Sets the category id.
    *
    * @param categoryID The category ID.
    */
   void setCategoryID(wf_uint32 categoryID);

   /**
    * Sets the top region id.
    *
    * @param topRegionID The new top region id.
    */
   void setTopRegionID(wf_uint32 topRegionID);

   /**
    * Sets the position.
    *
    * @param position The new position.
    */
   void setPosition(const WGS84Coordinate& position);

   /**
    * Sets the start index.
    *
    * @param startIndex The new start index.
    */
   void setStartIndex(wf_uint32 startIndex);

   /**
    * Sets the max number of hits.
    *
    * @param maxHits The new max number of hits.
    */
   void setMaxHits(wf_uint32 maxHits);

   /**
    * Sets the heading id.
    *
    * @param headingID The new heading id.
    */
   void setHeadingID(wf_uint32 headingID);
   
private:

   /// ID of a SearchArea, the search will be done in this area.
   WFString m_areaID;
   
   /**
    * In what region/country/state the search will be done.
    */
   wf_uint32 m_topRegionID;

   /**
    * Search hits will be sorted around this position.
    * If no areaID is given, this position will be usead as search area.
    */
   WGS84Coordinate m_position;

   /// A WFString that holds what to search for.
   WFString m_what;

   /**
    * A WFString that holds where to search. Used to pick which search hits
    * that are interesting.
    */
   WFString m_where;

   /// The wanted index of the first match in the reply.
   wf_uint32 m_startIndex;

   /// The maximum number of hits wanted.
   wf_uint32 m_maxHits;

   /// The wanted heading.
   wf_uint32 m_headingID;

   /// The round to search in for this search.
   wf_uint32 m_round;

   /// The ID of the category to search within.
   wf_uint32 m_categoryID;
};

} // End namespace WFAPI

#endif // End WFAPI_SEARCH_QUERY_H
