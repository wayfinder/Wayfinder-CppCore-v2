/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_SEARCH_ITEM_H
#define WFAPI_SEARCH_ITEM_H

#include "WFAPIConfig.h"
#include "SearchItemTypeEnum.h"
#include "SearchItemSubTypeEnum.h"
#include "ItemInfoEntryArray.h"
#include "WGS84Coordinate.h"
#include "DistanceUnit.h"

namespace WFAPI {


/**
 * A search match for an item.
 */
class SearchItem
{

public:

   /**
    * Constructor.
    *
    * @param name The name of the match.
    * @param id The identification string of the match, can be used
    *           to get more details about it.
    * @param type The type of item this match is.
    * @param subType If the SearchItem type is POI this is the type
    *                of Point Of Interest.
    * @param distance The distance this match is from the position used in
    *                 the search.
    * @param locationName The names of the area(s) that the match is
    *                     located in.
    * @param position The coordinate that the match is at.
    * @param imageName The name of the icon that represents this match.
    * @param advert If this match is an advertisement.
    * @param additionalInformation More details about a search item
    *                              is only filled in if the SearchItem
    *                              is from a searchDetails reply.
    * @param distanceFromSearchPos The distance from the position that was
    *                              used when doing the search.
    */
   SearchItem(const WFString& name, const WFString& id, 
              SearchItemType type, SearchItemSubType subType, 
              wf_uint32 distance,
              const WFString& locationName,
              const WGS84Coordinate& position,
              const WFString& imageName, 
              bool advert,
              const ItemInfoArray& additionalInformation,
              wf_uint32 distanceFromSearchPos = WF_MAX_UINT32);

   /**
    * Copy constructor.
    */
   SearchItem(const SearchItem& searchItem);

   /**
    * Destructor.
    */
   virtual ~SearchItem();

   /**
    * Get the name.
    * 
    * @return The name of the match.
    */
   const WFString& getName() const;

   /**
    * Get the id.
    * 
    * @return The id of the match.
    */
   const WFString& getID() const;

   /**
    * Get the locationName.
    * 
    * @return The locationName of the match.
    */
   const WFString& getLocationName() const;

   /**
    * Get the type.
    * 
    * @return The type of match.
    */
   SearchItemType getType() const;

   /**
    * Get the sub type, is only set if the type is POI.
    * 
    * @return The sub type of the match.
    */
   SearchItemSubType getSubType() const;

   /**
    * Get the distance from the current position when the search was done.
    * An invalid distance will be returned as WF_MAX_UINT32.
    * 
    * @return The distance in meters.
    */
   wf_uint32 getDistance() const;

   /**
    * Get the distance from the current position when the search was done.
    * An invalid distance will be returned as WF_MAX_UINT32.
    * 
    * @return The distance in feet.
    */
   wf_uint32 getDistanceInFeet() const;

   /**
    * Get the distance from the current position when the search was done.
    * An invalid distance will be returned as WF_MAX_UINT32.
    * 
    * @return The distance in yards.
    */
   wf_uint32 getDistanceInYards() const;

   /**
    * Get the distance from the current position when the search was done
    * as a string. An invalid distance will be returned as an empty string.
    * 
    * @param distanceUnit The unit in which to receive the distance in.
    * @return The distance from the searched position returned as a WFString.
    */
   WFString getDistance(DistanceUnit distanceUnit) const;
   
   /**
    * Get the distance from the position used when doing the search, if such
    * was provided. An invalid distance will be returned as WF_MAX_UINT32.
    * 
    * @return The distance in meters.
    */
   wf_uint32 getDistanceFromSearchPos() const;

   /**
    * Get the distance from the position used when doing the search, if such
    * was provided. The result will be returned as a parsed string.
    * An invalid distance will be returned as an empty string.
    * 
    * @param distanceUnit The unit in which to receive the distance in.
    * @return The distance from the searched position returned as a WFString.
    */
   WFString getDistanceFromSearchPos(DistanceUnit distanceUnit) const;

   /**
    * Get the distance from from the position that was used when searching, if
    * such exists. The distance will be returned as distance in feet.
    * An invalid distance will be returned as WF_MAX_UINT32.
    *     
    * @return The distance in feet.
    */
   wf_uint32 getDistanceFromSearchPosInFeet() const;

   /**
    * Get the distance from from the position that was used when searching, if
    * such exists. The distance will be returned as distance in yards.
    * An invalid distance will be returned as WF_MAX_UINT32.
    * 
    * @return The distance in yards.
    */
   wf_uint32 getDistanceFromSearchPosInYards() const;


   /**
    * Get the distance from the specified position.
    * An invalid distance will be returned as WF_MAX_UINT32.
    *
    * @param pos The position to measure the distance to. 
    * @return The distance in meters.
    */
   wf_uint32 getDistanceFromPos(const WGS84Coordinate& pos) const;

   /**
    * Get the distance from the specified position The result will be returned
    * as a parsed string. An invalid distance will be returned as an empty string.
    *
    * @param distanceUnit The unit in which to receive the distance in. 
    * @param pos The position to measure the distance to. 
    * @return The distance from the searched position returned as a WFString.
    */
   WFString getDistanceFromPos(DistanceUnit distanceUnit,
                               const WGS84Coordinate& pos) const;

   /**
    * Get the distance from from the specified position. The distance will be
    * returned as distance in feet. An invalid distance will be returned as
    * WF_MAX_UINT32.
    *
    * @param pos The position to measure the distance to. 
    * @return The distance in feet.
    */
   wf_uint32 getDistanceFromPosInFeet(const WGS84Coordinate& pos) const;

   /**
    * Get the distance from from the specified position. The distance will be
    * returned as distance in yards. An invalid distance will be returned as
    * WF_MAX_UINT32.
    *
    * @param pos The position to measure the distance to. 
    * @return The distance in yards.
    */
   wf_uint32 getDistanceFromPosInYards(const WGS84Coordinate& pos) const;

   /**
    * Get the position.
    * 
    * @return The coordinate of the match.
    */
   const WGS84Coordinate& getPosition() const;

   /**
    * Get the imageName.
    * 
    * @return The imageName.
    */
   const WFString& getImageName() const;

   /**
    * Get if the match is advertisement.
    * 
    * @return If the match is advertisement.
    */
   bool getAdvert() const;

   /**
    * Get the additional information.
    *
    * @return Reference to the additional information.
    */
   const ItemInfoArray& getAdditionalInformation() const;

   /**
    * Assignment operator.
    *
    * @param other The SearchItem to be copied.
    * @return The copied SearchItem, a reference to this.
    */
   SearchItem& operator=(const SearchItem& other);

private:

   /**
    * Internal function that calculates the distance based on
    * a given distance unit and parses the answer to a string.
    *
    * @param distance The distance to be converted.
    * @param calculatedDistance Will contain the resulting parsed string
    *        when the function is done.
    * @param distanceUnit The distance unit to use for the calculation.
    */
   void calcDistance(wf_uint32 distance,
                     WFString& calculatedDistance,
                     DistanceUnit distanceUnit) const;

   
   /**
    * Copy function that copies data from other to this.
    *
    * @param other The SearchItem to copy.
    */
   void init(const SearchItem& other);

   /// Name of the search item.
   WFString m_name;

   /// Id of the search item.
   WFString m_id;

   /// Type of the search item, poi, misc etc.
   SearchItemType m_type;

   /// More detailed type, airport, hospital etc.
   SearchItemSubType m_subType;

   /**
    * Distance from current location, is set when providing coordinates
    * in the search query.
    */
   wf_uint32 m_distance;

   /// String describing the place, for example New York etc.
   WFString m_locationName;

   /// Position of this search item.
   WGS84Coordinate m_position;

   /// Name of the image for this item. Has been downloaded when searching.
   WFString m_imageName;

   /// True if this item is a advertisement and not a real search hit.
   bool m_advert;

   /**
    * List of all the additional information. SearchInterface::searchDetails
    * should be called with the id of this SearchItem when wanting this
    * information.
    */
   ItemInfoArray m_additionalInformation;
   
   /**
    * Distance from the position that was used when performing the search,
    * this is NOT the distance from the current position.
    */
   wf_uint32 m_distanceFromSearchPos;   
};

} // End namespace WFAPI

#endif // End WFAPI_SEARCH_ITEM_H

