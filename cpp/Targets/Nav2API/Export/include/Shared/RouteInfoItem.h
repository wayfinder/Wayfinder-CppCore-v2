/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_ROUTE_INFO_ITEM_H
#define WFAPI_ROUTE_INFO_ITEM_H

#include "WFAPIConfig.h"
#include "RouteAction.h"
#include "RouteCrossing.h"
#include "WGS84Coordinate.h"
#include "WFString.h"

namespace WFAPI {

/**
 * Information about a route turn in an itinerary.
 */
class RouteInfoItem
{

public:

   /**
    * Constructor.
    */
   RouteInfoItem();

   /**
    * Constructor with parameters.
    *
    * @param action The RouteAction at the turn.
    * @param coordinate The position of the turn.
    * @param crossing The RouteCrossing at turn.
    * @param distanceToGoal Distance to goal.
    * @param distanceToNextTurn Distance to next turn.
    * @param timeToGoal Time left from this turn to goal, in seconds.
    * @param streetName The name of the street.
    * @param isLeftSideTraffic If it is left side traffic on the current street.
    * @param isHighWay If the current street is a highway.
    * @param hasSpeedCameraLandmark If the current road has a speed camera
    * @param speedCameraInfo Information about the speed camera if one exists.
    * @param hasDetourLandmark If the current road is a detour.
    * @param detourInfo Information about the detour if one exists.
    * @param exitCount The exit to take in the next turn.
    */
   RouteInfoItem(const RouteAction& action,
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
                 wf_uint32 exitCount);

   /**
    * Destructor.
    */
   virtual ~RouteInfoItem();

   /**
    * What to do at the turn.
    *
    * @return The RouteAction for the turn.
    */
   RouteAction getAction() const;

   /**
    * The position of the turn.
    *
    * @return The coordinate of the turn.
    */
   WGS84Coordinate getCoordinate() const;

   /**
    * What type of crossing it is at the turn.
    *
    * @return The RouteCrossing for the turn.
    */
   RouteCrossing getCrossing() const;

   /**
    * The distance left to the destination at the turn.
    *
    * @return The distance left in meters.
    */
   wf_uint32 getDistanceToGoal() const;

   /**
    * The distnace to the next turn starting at the turn.
    *
    * @return The distance to next turn in meters.
    */
   wf_uint32 getDistanceToNextTurn() const;

   /**
    * The time to goal from this crossing
    *
    * @return The time to goal from this crossing, in seconds
    */
   wf_int32 getTimeToGoal() const;

   /**
    * The name of the street to turn into.
    *
    * @return The street name.
    */
   const WFString& getStreetName() const;

   /**
    * If the current street has left side traffic.
    *
    * @return If the street has left side traffic.
    */
   bool getIfLeftSideTraffic() const;

   /**
    * If the current street is a high way.
    *
    * @return If the street is a highway.
    */
   bool getIfHighWay() const;

   /**
    * If the current street has a speed camera landmark
    *
    * @return If the street has a speed camera landmark.
    */
   bool getIfHasSpeedCameraLandmark() const;

   /**
    * Get information about speed camera landmark.
    * Will return empty string if no information is available.
    *
    * @return Information about the speed camera landmark.
    */ 
   const WFString& getSpeedCameraInfo() const;

   /**
    * If the current street is a detour.
    *
    * @return If the street has a detour landmark.
    */
   bool getIfHasDetourLandmark() const;

   /**
    * Get information about detour landmark. Information is only available in
    * the beginning of a detour and in the end of a detour. Will return empty
    * string if no information is available.
    *
    * @return Information about the detour landmark.
    */
   const WFString& getDetourInfo() const;

   /**
    * Get which exit to take when making a turn.
    * Use this for example when needing to know which exit to take in an
    * roundabout.
    *
    * @return The exit to take.
    */
   wf_uint32 getExitCount() const;
   
private:

   /// The RouteAction at the turn.
   RouteAction m_action;

   /// The position of the turn.
   WGS84Coordinate m_coordinate;

   /// The RouteCrossing at turn.
   RouteCrossing m_crossing;

   /// Distance to goal.
   wf_uint32 m_distanceToGoal;

   /// Distance to next turn.
   wf_uint32 m_distanceToNextTurn;

   /// Time to goal from this turn
   wf_int32 m_timeToGoal;

   /// The name of the street.
   WFString m_streetName;

   /// If it is left side traffic on the current street.
   bool m_isLeftSideTraffic;

   /// If current street is a high way
   bool m_isHighWay;

   /// If current street has a speed camera.
   bool m_hasSpeedCameraLandmark;

   /// Information about the speedCamera on this road.
   WFString m_speedCameraInfo;

   /// If current street has a detour landmark.
   bool m_hasDetourLandmark;

   /// Information about the detour landmark on this road.
   WFString m_detourInfo;

   /// Which exit to take at the turn.
   wf_uint32 m_exitCount;

};

} // End namespace WFAPI

#endif // End WFAPI_ROUTE_INFO_ITEM_H
