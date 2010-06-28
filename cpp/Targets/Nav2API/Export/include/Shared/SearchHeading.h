/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_SEARCH_HEADING_H
#define WFAPI_SEARCH_HEADING_H

#include "WFAPIConfig.h"
#include "SearchItemArray.h"
#include "SearchAreaArray.h"
#include "WFString.h"
#include "SearchHitsTypeEnum.h"

namespace WFAPI {

/**
 * A search heading describes a set of search matches.
 */
class SearchHeading
{

public:

   /**
    * Constructor.
    *
    * @param typeOfHits The type of results in this heading, SEARCH_RESULTS
    *                   for items and AREA_MATCHES for areas.
    * @param itemResults The item results. Contains the hits if typeOfHits is 0.
    * @param areaResults The area results. Contains the hits if typeOfHits is 1.
    * @param imageName The name of the image for this heading.
    * @param name The name of this heading.
    * @param headingID The identifier of this heading.
    * @param topRegionID If this heading is limited to a specific region this
    *                    value is the top region id of that region. If not
    *                    limited to a region this value is WF_MAX_UINT32.
    * @param totalNbrHits The total number of hits for this heading that can
    *                     be retrieved using the current SearchQuery.
    */
   SearchHeading(SearchHitsType typeOfHits,
                 const SearchItemArray& itemResults,
                 const SearchAreaArray& areaResults,
                 const WFString& imageName,
                 const WFString& name,
                 wf_uint32 headingID,
                 wf_uint32 topRegionID,
                 wf_uint32 totalNbrHits);

   /**
    * Constructor. This constructor is used for handling the total number of
    * hits for search results. Objects created with this constructor does NOT
    * contain and search results.
    *
    * @param typeOfHits The type of results in this heading, SEARCH_RESULTS
    *                   for items and AREA_MATCHES for areas.
    * @param imageName The name of the image for this heading.
    * @param name The name of this heading.
    * @param headingID The identifier of this heading.
    * @param totalNbrHits The total number of hits for this heading that can
    *                     be retrieved using the current SearchQuery.
    */
   SearchHeading(SearchHitsType typeOfHits,
                 const WFString& imageName,
                 const WFString& name,
                 wf_uint32 headingID,
                 wf_uint32 totalNbrHits);
   
   /**
    * Copy constructor.
    */
   SearchHeading(const SearchHeading& searchHeading);

   /**
    * Destructor.
    */
   virtual ~SearchHeading();
   
   /**
    * If there is items or areas in this heading.
    * 
    * @return The type of hits for this heading. Enum can be found
    *         in SearchHitsTypeEnum.h.
    */
   SearchHitsType getTypeOfHits() const;

   /**
    * The item matches.
    *
    * @return The SearchItemArray with the SearchItems.
    */
   const SearchItemArray& getItemResults() const;

   /**
    * The area matches.
    *
    * @return The SearchAreaArray with the SearchAreas.
    */
   const SearchAreaArray& getAreaResults() const;

   /**
    * Get the image name for this heading.
    *
    * @return The image name string.
    */
   const WFString& getImageName() const;

   /**
    * Get the name for this heading.
    *
    * @return The name string.
    */
   const WFString& getName() const;

   /**
    * Get the heading ID.
    *
    * @return The heading's id.
    */
   wf_uint32 getHeadingID() const;

   /**
    * Get the TopRegionID of this heading.
    *
    * @return The TopRegion id.
    */
   wf_uint32 getTopRegionID() const;

   /**
    * Get the total number of matches, for the type of hits 
    * in this heading, see getTypeOfHits.
    *
    * @return The total number of hits for this heading.
    */
   wf_uint32 getTotalNbrHits() const;

   /** 
    * Get the current position in the heading, this value is only to help
    * the UI keep track of the position in the heading. 
    * The initial value is 0.
    *
    * @return The value of current position in the heading.
    */
   wf_uint32 getCurrentPosition() const;

   /**
    * If there is items or areas in this heading.
    * 
    * @param typeOfHits The type of hits for this heading. SEARCH_RESULTS for
    *                   search results and AREA_MATCHES for area matches.
    */
   void setTypeOfHits(SearchHitsType typeOfHits);

   /**
    * The item matches.
    *
    * @param searchItemArray The SearchItemArray with the SearchItems.
    */
   void setItemResults(const SearchItemArray& searchItemArray);

   /**
    * The area matches.
    *
    * @param areaResults The new SearchAreaArray with the SearchAreas.
    */
   void setAreaResults(const SearchAreaArray& areaResults);

   /**
    * Set the image name for this heading.
    *
    * @param imageName The new image name string.
    */
   void setImageName(const WFString& imageName);

   /**
    * Set the name for this heading.
    *
    * @param name The new name string.
    */
   void setName(const WFString& name);

   /**
    * Set the heading ID.
    *
    * @param headingID The new heading's id.
    */
   void setHeadingID(wf_uint32 headingID);

   /**
    * Set the TopRegionID of this heading.
    *
    * @param topRegionID The new TopRegion id value.
    */
   void setTopRegionID(wf_uint32 topRegionID);

   /**
    * Set the total number of matches, for the type of hits 
    * in this heading, see setTypeOfHits.
    *
    * @param totalNbrHits The new value for total number of hits in 
    *                     this heading.
    */
   void setTotalNbrHits(wf_uint32 totalNbrHits);

   /**
    * Set the current position in the heading, this value has no
    * effect on the heading but is only to help the UI keep track of
    * the position in the heading. The initial value is 0.
    *
    * @param currentPosition The new value of current position in the heading.
    */
   void setCurrentPosition(wf_uint32 currentPosition);

   /**
    *   Assignment operator.
    */
   SearchHeading& operator=(const SearchHeading& other);
   
private:

   /**
    * Initialise this using another SearchHeading.
    *
    * @param other The other serach heading.
    */
   void init(const SearchHeading& other);

   /// The type of hits, 0 for items and 1 for areas.
   SearchHitsType m_typeOfHits;

   /// The array of SearchItems.
   SearchItemArray m_searchItemArray;

   /// The array of SearchAreas.
   SearchAreaArray m_searchAreaArray;

   /// The name of the image for this heading.
   WFString m_imageName;

   /// The name of this heading.
   WFString m_name;

   /// The id of this heading.
   wf_uint32 m_headingID;

   /// The top region for this heading.
   wf_uint32 m_topRegionID;

   /// The total number of hits.
   wf_uint32 m_totalNbrHits;

   /// The current position in the hits, used by the UI.
   wf_uint32 m_currentPosition;
};


} // End namespace WFAPI

#endif // End WFAPI_SEARCH_HEADING_H
