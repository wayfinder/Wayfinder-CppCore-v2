/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "SearchQuery.h"

namespace WFAPI {

SearchQuery::SearchQuery(const WFString& what,
                         const WFString& where,
                         const WFString& areaID,
                         wf_uint32 topRegionID,
                         const WGS84Coordinate& position,
                         wf_uint32 startIndex,
                         wf_uint32 maxHits,
                         wf_uint32 headingID,
                         wf_uint32 round)
   : m_areaID(areaID),
     m_topRegionID(topRegionID),
     m_position(position),
     m_what(what),
     m_where(where),
     m_startIndex(startIndex),
     m_maxHits(maxHits),
     m_headingID(headingID),
     m_round(round),
     m_categoryID(WF_MAX_UINT32)
{
   
}

SearchQuery::SearchQuery(const WFString& what,
                         wf_uint32 categoryID,
                         const WGS84Coordinate& position,
                         wf_uint32 startIndex,
                         wf_uint32 maxHits,
                         wf_uint32 headingID,
                         wf_uint32 round)
   : m_topRegionID(WF_MAX_UINT32),
     m_position(position),
     m_what(what),
     m_startIndex(startIndex),
     m_maxHits(maxHits),
     m_headingID(headingID),
     m_round(round),
     m_categoryID(categoryID)
{

}

SearchQuery::SearchQuery(const WFString& what,
                         wf_uint32 categoryID,
                         wf_uint32 topRegionID,
                         wf_uint32 startIndex,
                         wf_uint32 maxHits,
                         wf_uint32 headingID,
                         wf_uint32 round)
   : m_topRegionID(topRegionID),
     m_position(),
     m_what(what),
     m_startIndex(startIndex),
     m_maxHits(maxHits),
     m_headingID(headingID),
     m_round(round),
     m_categoryID(categoryID)
{
   
}

SearchQuery::~SearchQuery()
{
   
}

const WFString&
SearchQuery::getWhat() const
{
   return m_what;
}

const WFString&
SearchQuery::getWhere() const
{
   return m_where;
}

const WFString&
SearchQuery::getAreaID() const
{
   return m_areaID;
}

wf_uint32
SearchQuery::getCategoryID() const
{
   return m_categoryID;
}

wf_uint32
SearchQuery::getTopRegionID() const
{
   return m_topRegionID;
}

const WGS84Coordinate&
SearchQuery::getPosition() const
{
   return m_position;   
}

wf_uint32
SearchQuery::getStartIndex() const
{
   return m_startIndex;
}

wf_uint32
SearchQuery::getMaxHits() const
{
   return m_maxHits;
}

wf_uint32
SearchQuery::getHeadingID() const
{
   return m_headingID;
}

wf_uint32
SearchQuery::getRound() const
{
   return m_round;    
}

void SearchQuery::setWhat(const WFString& what)
{
   m_what = what;
}

void SearchQuery::setWhere(const WFString& where)
{
   m_where = where;
}

void SearchQuery::setCategoryID(wf_uint32 categoryID)
{
   m_categoryID = categoryID;
}

void SearchQuery::setTopRegionID(wf_uint32 topRegionID)
{
   m_topRegionID = topRegionID;
}

void SearchQuery::setPosition(const WGS84Coordinate& position)
{
   m_position = position;
}

void SearchQuery::setStartIndex(wf_uint32 startIndex)
{
   m_startIndex = startIndex;
}

void SearchQuery::setMaxHits(wf_uint32 maxHits)
{
   m_maxHits = maxHits;
}

void SearchQuery::setHeadingID(wf_uint32 headingID)
{
   m_headingID = headingID;
}

} // End namespace WFAPI

