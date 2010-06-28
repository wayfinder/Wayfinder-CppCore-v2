/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "OneSearchQuery.h"

namespace WFAPI {

OneSearchQuery::OneSearchQuery(const WFString& what,
                                  const WGS84Coordinate& position,
                                  SearchType searchType,
                                  wf_int32 round)
{
   WFString where;
   std::vector<wf_int32> categoryIDs;
   wf_int32 radius = 0;
   wf_uint32 topRegionID = 0;

   init(what,
        where,
        categoryIDs,
        position,
        radius,
        topRegionID,
        SEARCH_FOR_EVERYTHING, //searchType
        DEFAULT_MAX_SEARCH_HITS, // maxhits
        round);
}

OneSearchQuery::OneSearchQuery(const WFString& what,
                               const WFString& where,
                               wf_uint32 topRegionID,
                               SearchType searchType,
                               wf_int32 round)
{
   std::vector<wf_int32> categoryIDs;
   WGS84Coordinate position(180,180);
   wf_int32 radius = 0;
   
   init(what,
        where,
        categoryIDs,
        position,
        radius,
        topRegionID,
        searchType,
        DEFAULT_MAX_SEARCH_HITS, // maxhits
        round);
}

OneSearchQuery::OneSearchQuery(const WFString& what,
                               const WFString& where,
                               wf_uint32 topRegionID,
                               std::vector<wf_int32> categoryIDs,
                               SearchType searchType,
                               wf_int32 round)
{
   WGS84Coordinate position(180,180);
   wf_int32 radius = 0;

   init(what,
        where,
        categoryIDs,
        position,
        radius,
        topRegionID,
        searchType,
        DEFAULT_MAX_SEARCH_HITS, // maxhits
        round);
}

OneSearchQuery::OneSearchQuery(const WFString& what,
                               const WFString& where,
                               std::vector<wf_int32> categoryIDs,
                               const WGS84Coordinate& position,
                               wf_int32 radius,
                               wf_uint32 topRegionID,
                               SearchType searchType,
                               wf_int32 maxHits,
                               wf_int32 round,
                               SearchSorting searchSorting,
                               bool includeInfoFields
                               )
   : m_what(what),
     m_where(where),
     m_categoryIDs(categoryIDs),
     m_position(position),
     m_radius(radius),
     m_topRegionID(topRegionID),
     m_searchType(searchType),
     m_maxHits(maxHits),
     m_round(round),
     m_searchSorting(searchSorting),
     m_includeInfoFields(includeInfoFields)
{
   
}

void
OneSearchQuery::init(const WFString& what,
                     const WFString& where,
                     std::vector<wf_int32> categoryIDs,
                     const WGS84Coordinate& position,
                     wf_int32 radius,
                     wf_uint32 topRegionID,
                     SearchType searchType,
                     wf_int32 maxHits,
                     wf_int32 round,
                     SearchSorting searchSorting,
                     bool includeInfoFields)
{
   m_what = what;
   m_where = where;
   m_categoryIDs = categoryIDs;
   m_position = position;
   m_radius = radius;
   m_topRegionID = topRegionID;
   m_searchType = searchType;
   m_maxHits = maxHits;
   m_round = round;
   m_searchSorting = searchSorting;
   m_includeInfoFields = includeInfoFields;
}


OneSearchQuery::~OneSearchQuery()
{
   
}

const WFString&
OneSearchQuery::getWhat() const
{
   return m_what;
}

const WFString&
OneSearchQuery::getWhere() const
{
   return m_where;
}

const std::vector<wf_int32>&
OneSearchQuery::getCategoryIDs() const
{
   return m_categoryIDs;
}

const WGS84Coordinate&
OneSearchQuery::getPosition() const
{
   return m_position;   
}

wf_int32
OneSearchQuery::getRadius() const
{
   return m_radius;
}

wf_uint32
OneSearchQuery::getTopRegionID() const
{
   return m_topRegionID;
}

SearchType
OneSearchQuery::getSearchType() const
{
   return m_searchType;
}

wf_int32
OneSearchQuery::getMaxHits() const
{
   return m_maxHits;
}

wf_int32
OneSearchQuery::getRound() const
{
   return m_round;    
}

SearchSorting OneSearchQuery::getSearchSorting() const
{
   return m_searchSorting;
}

bool OneSearchQuery::getIncludeInfoFields() const
{
   return m_includeInfoFields;
}

} // End namespace WFAPI

