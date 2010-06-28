/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_UPDATE_NAVIGATION_INFO_H
#define WFAPI_UPDATE_NAVIGATION_INFO_H

#include "WFAPIConfig.h"
#include "RouteAction.h"
#include "RouteCrossing.h"
#include "WFString.h"
#include "WGS84Coordinate.h"

namespace WFAPI {

/**
 * This class contains information about the next turn to make while
 * navigating. Between UpdateNavigationInfos there are 
 * UpdateNavigationDistanceInfos that updates the distance and time
 * left to the next turn.
 */
class UpdateNavigationInfo

{

public:

   /**
    * Constructor.
    *
    * @param actionAfterNextAction The action to take after the next action.
    * @param crossingAfterNextCrossing The type of crossing after next crossing.
    * @param currentStreetName The name of the current street.
    * @param distanceToGoal The distance to left to the destination.
    * @param distanceToGoalInUnit The distance left to the destination
    *                             formated in the distance unit set in settings.
    * @param distanceToNextTurn The distance to the next turn.
    * @param distanceToNextTurnInUnit The distance to the next turn formated in
    *                                 the distance unit set in settings.
    * @param distanceToTurnAfterNextTurn The distance to the turn after next
    *                                    turn, expressed from upcoming turn.
    * @param distanceToTurnAfterNextTurnInUnit The distance to the turn after next
    *                                          turn, expressed from upcoming
    *                                          turn and formated in the
    *                                          distance unit set in settings.
    * @param nextAction The action to take in the next crossing.
    * @param nextCrossing The type of the next crossing.
    * @param nextStreetName The name of the next street.
    * @param nextStreetNameAfterNextTurn The name of the next street after the
    *                                    next turn.
    * @param onTrack If user is on track.
    * @param position The position.
    * @param speed The current speed.
    * @param timeToGoal Time left to the destination.
    * @param timeToNextTurn Time left to the next turn.
    * @param exitCount The exit to take in the next turn.
    * @param exitCountForActionAfterNextAction The exit to take in the action
    *                                          after next action.
    * @param nextHighway If next street is a highway.
    * @param detour If current street is a detour.
    * @param speedcam If current street has a speed camera.
    * @param isLeftSideTraffic If it is left side traffic on the 
    *                          current street.
    * @param newCrossing True if this update means that we have passed a
    *                    crossing.
    * @param crossingNbr The number for this crossing in the route.
    */
   UpdateNavigationInfo(
      RouteAction actionAfterNextAction,
      RouteCrossing crossingAfterNextCrossing,
      const WFString& currentStreetName,
      wf_uint32 distanceToGoal,
      const WFString& distanceToGoalInUnit,
      wf_uint32 distanceToNextTurn,
      const WFString& distanceToNextTurnInUnit,
      wf_uint32 distanceToTurnAfterNextTurn,
      const WFString& distanceToTurnAfterNextTurnInUnit,
      RouteAction nextAction,
      RouteCrossing nextCrossing,
      const WFString& nextStreetName,
      const WFString& nextStreetNameAfterNextTurn,
      bool onTrack,
      WGS84Coordinate position,
      wf_uint16 speed,
      wf_uint32 timeToGoal,
      wf_uint32 timeToNextTurn,
      wf_uint32 exitCount,
      wf_uint32 exitCountForActionAfterNextAction,
      bool nextHighway,
      bool detour,
      bool speedcam,
      bool isLeftSideTraffic,
      bool newCrossing,
      wf_uint32 crossingNbr);

   /**
    * Destructor.
    */
   virtual ~UpdateNavigationInfo();

   /**
    * If currently following the route or not. If not then the
    * other information in this might not be relevant or valid.
    *
    * @return If on the route.
    */
   bool isOnTrack() const;

   /**
    * What to do at the next turn.
    *
    * @return The RouteAction for the next turn.
    */
   RouteAction getNextAction() const;

   /**
    * What type of crossing it is at the next turn.
    *
    * @return The RouteCrossing for the next turn.
    */
   RouteCrossing getNextCrossing() const;

   /**
    * The name of the street to turn into at the next turn.
    *
    * @return The next street name.
    */
   const WFString& getNextStreetName() const;

   /**
    * The name of the next street after the next turn.
    *
    * @return The next street name after the next street.
    */
   const WFString& getNextStreetNameAfterNextTurn() const;

   /**
    * The name of the current street.
    *
    * @return The current street name.
    */
   const WFString& getCurrentStreetName() const;

   /**
    * The position of the next turn.
    *
    * @return The coordinate of the next turn.
    */
   WGS84Coordinate getPosition() const;

   /**
    * The speed limit of the current road.
    *
    * @return The speed limit in km/h.
    */
   wf_uint16 getSpeed() const;

   /**
    * The time left to the destination.
    *
    * @return The time left in seconds.
    */
   wf_uint32 getTimeToGoal() const;

   /**
    * The time to the next turn starting at the prevoius turn.
    *
    * @return The time to next turn in seconds.
    */
   wf_uint32 getTimeToNextTurn() const;

   /**
    * The distance left to the destination.
    *
    * @return The distance left in meters.
    */
   wf_uint32 getDistanceToGoal() const;

   /**
    * The distance left to the destination.
    *
    * @return The distance left to the destination,
    * formated in the distance unit set in the settings interface.
    */
   const WFString& getDistanceToGoalInUnit() const;

   /**
    * The distance to the next turn starting at the prevoius turn.
    *
    * @return The distance to next turn in meters.
    */
   wf_uint32 getDistanceToNextTurn() const;

   /**
    * The distance to the next turn starting at the prevoius turn.
    *
    * @return The distance to next turn,
    * formated in the distance unit set in the settings interface.
    */
   const WFString& getDistanceToNextTurnInUnit() const;

   /**
    * The distance to the turn after next turn from the next turn.
    *
    * @return The distance to the turn after next turn in meters.
    */
   wf_uint32 getDistanceToTurnAfterNextTurn() const;

   /**
    * The distance to the turn after next turn from the next turn.
    *
    * @return The distance to the turn after next turn,
    * formated in the distance unit set in the settings interface.
    */
   const WFString& getDistanceToTurnAfterNextTurnInUnit() const;

   /**
    * What to do at the turn after the next turn.
    *
    * @return The RouteAction for the next turn after next turn.
    */
   RouteAction getActionAfterNextAction() const;

   /**
    * What type of crossing it is at the turn after the next turn.
    *
    * @return The RouteCrossing for the turn after next turn.
    */
   RouteCrossing getCrossingAfterNextCrossing() const;

   /**
    * Get the exit count for the upcoming crossing.
    * 
    * @return The exit count for the upcoming crossing. 
    */
   wf_uint32 getExitCount() const;

   /**
    * Get the exit count for the action after next action.
    * 
    * @return The exit count for action after next action. 
    */
   wf_uint32 getExitCountForActionAfterNextAction() const;

    /**
    * If the next road segment coming up is a highway.
    *
    * @return If a highway is coming up next.
    */
   bool getNextHighway() const;

    /**
    * If the road segment coming up is a detour.
    * Might be caused by roadwork for instance.
    *
    * @return If we're driving on a detour.
    */
   bool getDetour() const;

    /**
    * If the road segment coming up has speedcams on it.
    *
    * @return If there are speedcams coming up.
    */
   bool getSpeedcam() const;

   /**
    * If it is left side traffic on the current street.
    *
    * @return If the street has left side traffic.
    */
   bool getIfLeftSideTraffic() const;

   /**
    * This function should be used when checking if the update is due
    * to a new crossing or not. An UpdateNavigationInfo could be sent also
    * when passing into a new segment and therefore this function should
    * be used when updating for example an Itinerary view.
    *
    * @return True if this UpdateNavigationInfo contains a new crossing.
    */
   bool getIsNewCrossing() const;

   /**
    * Get the crossing number of this crossing.
    *
    * @return The crossing number for this crossing 
    */
   wf_uint32 getCrossingNbr() const;

   /**
    * Assignment operator.
    *
    * @param rhs The object we should assign values from.
    */ 
   UpdateNavigationInfo& operator=(const UpdateNavigationInfo& rhs);

private:

   /// The RouteAction after next turn.
   RouteAction m_actionAfterNextAction;

   /// The RouteCrossing after next turn.
   RouteCrossing m_crossingAfterNextCrossing;

   /// The name of the current street.
   WFString m_currentStreetName;

   /// Distance to goal. In meters.
   wf_uint32 m_distanceToGoal;

   /// Distance to goal. In the the distance unit that is set.
   WFString m_distanceToGoalInUnit;
   
   /// Distance to next turn. In meters.
   wf_uint32 m_distanceToNextTurn;

   /// Distance to next turn. In the the distance unit that is set.
   WFString m_distanceToNextTurnInUnit;

   /// Distance to turn after next turn from next turn. In meters.
   wf_uint32 m_distanceToTurnAfterNextTurn;

   /*
    * Distance to turn after next turn from next turn.
    * Formated according to the distance unit that is set in settings.
    */
   WFString m_distanceToTurnAfterNextTurnInUnit;

   /// The RouteAction at next turn.
   RouteAction m_nextAction;

   /// The RouteCrossing at next turn.
   RouteCrossing m_nextCrossing;

   /// The name of the next street.
   WFString m_nextStreetName;

   /// The name of the next street after the next crossing.
   WFString m_nextStreetNameAfterNextTurn;

   /// If on track.
   bool m_onTrack;

   /// The position of the next turn.
   WGS84Coordinate m_position;

   /// The road speed. In km/h.
   wf_uint16 m_speed;

   /// The time to goal. In seconds.
   wf_uint32 m_timeToGoal;

   /// The time to next turn. In seconds.
   wf_uint32 m_timeToNextTurn;

   /// Exit Count. Holds the exit to take in the next crossing.
   wf_uint32 m_exitCount;

   /// Holds the exit to take in the action after next action.
   wf_uint32 m_exitCountForActionAfterNextAction;

   /// If the next road segment is a highway.
   bool m_nextHighway;

   /// If the current segment is a detour.
   bool m_detour;

   /// If the current route has speedcams on it.
   bool m_speedcam;

   /// If it is left side traffic on the current street.
   bool m_isLeftSideTraffic;

   /**
    * True if this instance contains a new crossing, is false if for
    * example the update only is an update since we passed into a new
    * road segment.
    */
   bool m_newCrossing;

   /// The number for this crossing in the route.
   wf_uint32 m_crossingNbr;
};

} // End namespace WFAPI

#endif // End WFAPI_UPDATE_NAVIGATION_INFO_H

