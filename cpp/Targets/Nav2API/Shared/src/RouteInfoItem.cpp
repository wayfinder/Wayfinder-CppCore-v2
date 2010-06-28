/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "RouteInfoItem.h"

namespace WFAPI {


RouteInfoItem::RouteInfoItem()
{

}

RouteInfoItem::RouteInfoItem(const RouteAction& action,
                             const WGS84Coordinate& coordinate,
                             const RouteCrossing& crossing,
                             wf_uint32 distanceToGoal,
                             wf_uint32 distanceToNextTurn,
                             wf_int32 timeToGoal,
                             const WFString& streetName,
                             bool isLeftSideTraffic,
                             bool isHighWay,
                             bool hasSpeedCameraLandmark,
                             const WFString& speedCameraInfo,
                             bool hasDetourLandmark,
                             const WFString& detourInfo,
                             wf_uint32 exitCount) :
   m_action(action),
   m_coordinate(coordinate),
   m_crossing(crossing),
   m_distanceToGoal(distanceToGoal),
   m_distanceToNextTurn(distanceToNextTurn),
   m_timeToGoal(timeToGoal),
   m_streetName(streetName),
   m_isLeftSideTraffic(isLeftSideTraffic),
   m_isHighWay(isHighWay),
   m_hasSpeedCameraLandmark(hasSpeedCameraLandmark),
   m_speedCameraInfo(speedCameraInfo),
   m_hasDetourLandmark(hasDetourLandmark),
   m_detourInfo(detourInfo),
   m_exitCount(exitCount)
{

}

RouteInfoItem::~RouteInfoItem()
{

}

RouteAction 
RouteInfoItem::getAction() const
{
   return m_action;
}

WGS84Coordinate 
RouteInfoItem::getCoordinate() const
{
   return m_coordinate;
}

RouteCrossing 
RouteInfoItem::getCrossing() const
{
   return m_crossing;
}

wf_uint32 
RouteInfoItem::getDistanceToGoal() const
{
   return m_distanceToGoal;
}

wf_uint32 
RouteInfoItem::getDistanceToNextTurn() const
{  
   return m_distanceToNextTurn;
}

wf_int32 
RouteInfoItem::getTimeToGoal() const
{  
   return m_timeToGoal;
}

const WFString& 
RouteInfoItem::getStreetName() const
{
   return m_streetName;
}

bool
RouteInfoItem::getIfLeftSideTraffic() const
{
   return m_isLeftSideTraffic;
}

bool
RouteInfoItem::getIfHighWay() const
{
   return m_isHighWay;
}

bool
RouteInfoItem::getIfHasSpeedCameraLandmark() const
{
   return m_hasSpeedCameraLandmark;
}

const WFString&
RouteInfoItem::getSpeedCameraInfo() const
{
   return m_speedCameraInfo;
}

const WFString&
RouteInfoItem::getDetourInfo() const
{
   return m_detourInfo;
}

bool
RouteInfoItem::getIfHasDetourLandmark() const
{
   return m_hasDetourLandmark;
}

wf_uint32
RouteInfoItem::getExitCount() const
{
   return m_exitCount; 
}

} // End namespace WFAPI
