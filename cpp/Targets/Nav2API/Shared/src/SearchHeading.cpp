/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "SearchHeading.h"

namespace WFAPI {

SearchHeading::SearchHeading(SearchHitsType typeOfHits,
                             const SearchItemArray& itemResults,
                             const SearchAreaArray& areaResults,
                             const WFString& imageName,
                             const WFString& name,
                             wf_uint32 headingID,
                             wf_uint32 topRegionID,
                             wf_uint32 totalNbrHits)
   : m_typeOfHits(typeOfHits),
     m_searchItemArray(itemResults),
     m_searchAreaArray(areaResults),
     m_imageName(imageName),
     m_name(name),
     m_headingID(headingID),
     m_topRegionID(topRegionID),
     m_totalNbrHits(totalNbrHits),
     m_currentPosition(0)
{
}

SearchHeading::SearchHeading(SearchHitsType typeOfHits,
                             const WFString& imageName,
                             const WFString& name,
                             wf_uint32 headingID,
                             wf_uint32 totalNbrHits)
   : m_typeOfHits(typeOfHits),
     m_imageName(imageName),
     m_name(name),
     m_headingID(headingID),
     m_topRegionID(WF_MAX_UINT32),
     m_totalNbrHits(totalNbrHits),
     m_currentPosition(0)
{
}


SearchHeading::SearchHeading(const SearchHeading& searchHeading)
{
   init(searchHeading);
}

SearchHeading::~SearchHeading()
{
   
}

SearchHitsType
SearchHeading::getTypeOfHits() const
{
   return m_typeOfHits;
}

const SearchItemArray&
SearchHeading::getItemResults() const
{
   return m_searchItemArray;
}

const SearchAreaArray&
SearchHeading::getAreaResults() const
{
   return m_searchAreaArray;
}

const WFString&
SearchHeading::getImageName() const
{
   return m_imageName;
}

const WFString&
SearchHeading::getName() const
{
   return m_name;
}

wf_uint32
SearchHeading::getHeadingID() const
{
   return m_headingID;
}

wf_uint32
SearchHeading::getTopRegionID() const
{
   return m_topRegionID;
}

wf_uint32
SearchHeading::getTotalNbrHits() const
{
   return m_totalNbrHits;
}

wf_uint32
SearchHeading::getCurrentPosition() const
{
   return m_currentPosition;
}

void
SearchHeading::setTypeOfHits(SearchHitsType typeOfHits)
{
   m_typeOfHits = typeOfHits;
}

void
SearchHeading::setItemResults(const SearchItemArray& searchItemArray)
{
   m_searchItemArray = searchItemArray;

}

void
SearchHeading::setAreaResults(const SearchAreaArray& areaResults)
{
   m_searchAreaArray = areaResults;

}

void
SearchHeading::setImageName(const WFString& imageName)
{
   m_imageName = imageName;
}

void
SearchHeading::setName(const WFString& name)
{
   m_name = name;
}

void
SearchHeading::setHeadingID(wf_uint32 headingID)
{
   m_headingID = headingID;
}

void
SearchHeading::setTopRegionID(wf_uint32 topRegionID)
{
   m_topRegionID = topRegionID;
}

void
SearchHeading::setTotalNbrHits(wf_uint32 totalNbrHits)
{
   m_totalNbrHits = totalNbrHits;
}

void
SearchHeading::setCurrentPosition(wf_uint32 currentPosition)
{
   m_currentPosition = currentPosition;
}

SearchHeading&
SearchHeading::operator=(const SearchHeading& other)
{
   if (this == &other) {
      // Handle self assignment
      return *this;
   }
   init(other);
   return *this;
}

void
SearchHeading::init(const SearchHeading& other)
{
   m_typeOfHits = other.getTypeOfHits();
   m_searchItemArray = other.getItemResults();
   m_searchAreaArray = other.getAreaResults();
   m_imageName = other.getImageName();
   m_name = other.getName();
   m_headingID = other.getHeadingID();
   m_topRegionID = other.getTopRegionID();
   m_totalNbrHits = other.getTotalNbrHits();
   m_currentPosition = other.getCurrentPosition();   
}

} // End namespace WFAPI

