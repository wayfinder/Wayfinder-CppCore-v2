/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "InternalOneSearchQuery.h"

using namespace std;
using namespace wf_ngp;

namespace OneSearch {

InternalOneSearchQuery::InternalOneSearchQuery() :
   m_what(), m_where(), m_lat(MAX_INT32), m_lon(MAX_INT32), m_radius(-1),
   m_topRegionID(0), m_searchType(NavOneSearchReq::SearchTypeEnum::ALL),
   m_maxHits(100), m_round(0),
   m_searchSort(NavOneSearchReq::SearchSortingTypeEnum::DISTANCE_SORT),
   m_includeInfoFields(true)
{
}

InternalOneSearchQuery::InternalOneSearchQuery(const InternalOneSearchQuery& other)
{
   init(other);
}

InternalOneSearchQuery::InternalOneSearchQuery(
   const std::string& what,
   const std::string& where,
   const IntVec_t& categoryIds,
   int32 lat,
   int32 lon,
   int32 radius,
   uint32 topRegionID,
   NavOneSearchReq::SearchTypeEnum searchType,
   int32 maxHits,
   int32 round,
   NavOneSearchReq::SearchSortingTypeEnum searchSort,
   bool includeInfoFields)
{
   init(what, where, categoryIds, lat, lon, radius, topRegionID,
        searchType, maxHits, round, searchSort, includeInfoFields);
}

const std::string&
InternalOneSearchQuery::getWhat() const
{
   return m_what;
}

const std::string&
InternalOneSearchQuery::getWhere() const
{
   return m_where;
}

const IntVec_t&
InternalOneSearchQuery::getCategoryIds() const
{
   return m_categoryIDs;
}

int32
InternalOneSearchQuery::getLat() const
{
   return m_lat;
}

int32
InternalOneSearchQuery::getLon() const
{
   return m_lon;
}

int32
InternalOneSearchQuery::getRadius() const
{
   return m_radius;
}

uint32
InternalOneSearchQuery::getTopRegionId() const
{
   return m_topRegionID;
}

NavOneSearchReq::SearchTypeEnum
InternalOneSearchQuery::getSearchType() const
{
   return m_searchType;
}

int32
InternalOneSearchQuery::getMaxHits() const
{
   return m_maxHits;
}

int32
InternalOneSearchQuery::getRound() const
{
   return m_round;
}

NavOneSearchReq::SearchSortingTypeEnum
InternalOneSearchQuery::getSearchSort() const
{
   return m_searchSort;
}

bool
InternalOneSearchQuery::getIncludeInfoFields() const
{
   return m_includeInfoFields;
}

void InternalOneSearchQuery::init(
   const std::string& what,
   const std::string& where,
   const IntVec_t& categoryIds,
   int32 lat,
   int32 lon,
   int32 radius,
   uint32 topRegionID,
   NavOneSearchReq::SearchTypeEnum searchType,
   int32 maxHits,
   int32 round,
   NavOneSearchReq::SearchSortingTypeEnum searchSort,
   bool includeInfoFields)
{
   m_what = what;
   m_where = where;
   m_categoryIDs = categoryIds;
   m_lat = lat;
   m_lon = lon;
   m_radius = radius;
   m_topRegionID = topRegionID;
   m_searchType = searchType;
   m_maxHits = maxHits;
   m_round = round;
   m_searchSort = searchSort;
   m_includeInfoFields = includeInfoFields;
}

void
InternalOneSearchQuery::init(const InternalOneSearchQuery& other)
{
   init(other.m_what,
        other.m_where,
        other.m_categoryIDs,
        other.m_lat,
        other.m_lon,
        other.m_radius,
        other.m_topRegionID,
        other.m_searchType,
        other.m_maxHits,
        other.m_round,
        other.m_searchSort,
        other.m_includeInfoFields);
}

void
InternalOneSearchQuery::swap(InternalOneSearchQuery& other)
{
   // Swap all members
   std::swap(m_what, other.m_what);
   std::swap(m_where, other.m_where);
   std::swap(m_lat, other.m_lat);
   std::swap(m_lat, other.m_lat);
   std::swap(m_radius, other.m_radius);
   std::swap(m_topRegionID, other.m_topRegionID);
   std::swap(m_searchType, other.m_searchType);
   std::swap(m_maxHits, other.m_maxHits);
   std::swap(m_round, other.m_round);
   std::swap(m_searchSort, other.m_searchSort);
   std::swap(m_includeInfoFields, other.m_includeInfoFields);
   m_categoryIDs.swap(other.m_categoryIDs);
   
}

InternalOneSearchQuery&
InternalOneSearchQuery::operator=(const InternalOneSearchQuery& other)
{
   if (this == &other) {
      return *this;
   }
   // Create temp object that is a copy of other
   InternalOneSearchQuery tmp(other);

   // Swap the temp copy with this. By doing this we have done the
   // assignment in a safe way
   tmp.swap(*this);
   
   return *this;
}

} // End namespace OneSearch


