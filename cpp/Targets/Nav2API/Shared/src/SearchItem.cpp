/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "SearchItem.h"
#include "WFAPIConfig.h"
#include "SearchItemTypeEnum.h"
#include "SearchItemSubTypeEnum.h"
#include "ItemInfoEntryArray.h"
#include "WGS84Coordinate.h"
#include "Nav2Logging.h"
#include "DistancePrintingPolicy.h"
#include "GfxUtility.h"
#include "MC2Coordinate.h"
#include <cstdio>

#include <math.h> // rint

namespace WFAPI {

SearchItem::SearchItem(const WFString& name, const WFString& id, 
                       SearchItemType type, SearchItemSubType subType, 
                       wf_uint32 distance,
                       const WFString& locationName,
                       const WGS84Coordinate& position,
                       const WFString& imageName, 
                       bool advert,
                       const ItemInfoArray& additionalInformation,
                       wf_uint32 distanceFromSearchPos)
   : m_name(name), m_id(id), m_type(type), m_subType(subType),
     m_distance(distance), m_locationName(locationName),
     m_position(position), m_imageName(imageName), m_advert(advert),
     m_additionalInformation(additionalInformation),
     m_distanceFromSearchPos(distanceFromSearchPos)
{
   
}
wf_uint32
calculateDistance(const WFAPI::WGS84Coordinate& startCoord,
                  const WFAPI::WGS84Coordinate& endCoord)
{
   if (startCoord.isValid() && endCoord.isValid()){
      return wf_uint32(rint(GfxUtility::rtP2Pdistance_linear(startCoord,
                                                             endCoord)));
   } else {
      return WF_MAX_UINT32;
   }

}



SearchItem::SearchItem(const SearchItem& searchItem)
{
   init(searchItem);
}

SearchItem::~SearchItem()
{
   
}

const WFString&
SearchItem::getName() const
{
   return m_name;   
}

const WFString&
SearchItem::getID() const
{
   return m_id;
}

const WFString&
SearchItem::getLocationName() const
{
   return m_locationName;
}

SearchItemType
SearchItem::getType() const
{
   return m_type;
}

SearchItemSubType
SearchItem::getSubType() const
{
   return m_subType;
}

wf_uint32
SearchItem::getDistance() const
{
   if(!(const_cast<WGS84Coordinate*>(&m_position)->isValid())) {
      return WF_MAX_UINT32;
   }
   return m_distance;
}

wf_uint32
SearchItem::getDistanceInFeet() const
{

   if (m_distance == WF_MAX_UINT32){

      return WF_MAX_UINT32;

   } else {
      return isab::DistancePrintingPolicy::meterToFeet(m_distance);      

   }


}

wf_uint32
SearchItem::getDistanceInYards() const
{

   if (m_distance == WF_MAX_UINT32){

      return WF_MAX_UINT32;

   } else {
      return isab::DistancePrintingPolicy::meterToYards(m_distance);

   }


}

WFString
SearchItem::getDistance(DistanceUnit distanceUnit) const
{

   if (m_distance == WF_MAX_UINT32){
      return "";

   }
   WFString calcedDistance;
   calcDistance(m_distance, calcedDistance, distanceUnit);
   return calcedDistance;
}

wf_uint32
SearchItem::getDistanceFromSearchPos() const
{
   if(!const_cast<WGS84Coordinate*>(&m_position)->isValid()) {
      return WF_MAX_UINT32;
   }
   return m_distanceFromSearchPos;
}

WFString
SearchItem::getDistanceFromSearchPos(DistanceUnit distanceUnit) const
{

   if (m_distanceFromSearchPos == WF_MAX_UINT32){
      return "";

   }
   WFString calcedDistance;
   calcDistance(m_distanceFromSearchPos, calcedDistance, distanceUnit);
   return calcedDistance;
}

wf_uint32
SearchItem::getDistanceFromSearchPosInFeet() const
{

   if (m_distanceFromSearchPos == WF_MAX_UINT32){

      return WF_MAX_UINT32;

   } else {
      return isab::DistancePrintingPolicy::meterToFeet(m_distanceFromSearchPos);

   }


}

wf_uint32
SearchItem::getDistanceFromSearchPosInYards() const
{

   if (m_distanceFromSearchPos == WF_MAX_UINT32){

      return WF_MAX_UINT32;

   } else {
      return isab::DistancePrintingPolicy::meterToYards(m_distanceFromSearchPos);

   }


}


wf_uint32
SearchItem::getDistanceFromPos(const WGS84Coordinate& pos) const
{
   return calculateDistance(pos, getPosition());
}

WFString
SearchItem::getDistanceFromPos(DistanceUnit distanceUnit,
                               const WGS84Coordinate& pos) const
{
   wf_uint32 dist = calculateDistance(pos, getPosition());

   if (dist == WF_MAX_UINT32){
      return "";
   }
   WFString calcedDistance;
   calcDistance(dist, calcedDistance, distanceUnit);
   return calcedDistance;
}


wf_uint32
SearchItem::getDistanceFromPosInFeet(const WGS84Coordinate& pos) const
{
   wf_uint32 dist = calculateDistance(pos, getPosition());
   if (dist == WF_MAX_UINT32){
      return WF_MAX_UINT32;
   } else {
      return isab::DistancePrintingPolicy::meterToFeet(dist);
   }
}

wf_uint32
SearchItem::getDistanceFromPosInYards(const WGS84Coordinate& pos) const
{
   wf_uint32 dist = calculateDistance(pos, getPosition());
   if (dist == WF_MAX_UINT32){
      return WF_MAX_UINT32;
   } else {
      return isab::DistancePrintingPolicy::meterToYards(dist);
   }
}


const WGS84Coordinate&
SearchItem::getPosition() const
{
   return m_position;
}

const WFString&
SearchItem::getImageName() const
{
   return m_imageName;
}

bool
SearchItem::getAdvert() const
{
   return m_advert;
}

const ItemInfoArray&
SearchItem::getAdditionalInformation() const
{
   return m_additionalInformation;
}

void
SearchItem::calcDistance(wf_uint32 distance,
                         WFString& calculatedDistance,
                         DistanceUnit distanceUnit) const
{
   char* res = NULL;
   using namespace isab;
   switch(distanceUnit) {
      case KM:
      {
         res =  
            DistancePrintingPolicy::
            convertDistanceMetric(distance,
                                  DistancePrintingPolicy::Round,
                                  true);
         break;
      }
      case MILES_FEET:
      {
         res = 
            DistancePrintingPolicy::
            convertDistanceImperialFeet(distance,
                                        DistancePrintingPolicy::Round,
                                        true);
         break;
      }
      case MILES_YARD:
      {
         res =  
            DistancePrintingPolicy::
            convertDistanceImperialYards(distance,
                                         DistancePrintingPolicy::Round,
                                         true);
         break;
      }
   }
   //WFString str(res);
   calculatedDistance = res;
   delete[] res;
   //return str;
}

SearchItem&
SearchItem::operator=(const SearchItem& other)
{
   if (this != &other) {
      init(other);
   }
   return *this;
}

void
SearchItem::init(const SearchItem& other)
{
   m_name = other.getName();
   m_id = other.getID(); 
   m_type = other.getType();
   m_subType = other.getSubType(); 
   m_distance = other.getDistance();
   m_locationName = other.getLocationName();
   m_position = other.getPosition();
   m_imageName = other.getImageName(); 
   m_advert = other.getAdvert();
   m_additionalInformation = other.getAdditionalInformation();
   m_distanceFromSearchPos = other.getDistanceFromSearchPos();
}

} // End namespace WFAPI


