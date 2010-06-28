/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "UpdateNavigationInfo.h"


namespace WFAPI {

UpdateNavigationInfo::UpdateNavigationInfo(
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
   wf_uint32 crossingNbr)
      : m_actionAfterNextAction(actionAfterNextAction),
        m_crossingAfterNextCrossing(crossingAfterNextCrossing),
        m_currentStreetName(currentStreetName),
        m_distanceToGoal(distanceToGoal),
        m_distanceToGoalInUnit(distanceToGoalInUnit),
        m_distanceToNextTurn(distanceToNextTurn),
        m_distanceToNextTurnInUnit(distanceToNextTurnInUnit),
        m_distanceToTurnAfterNextTurn(distanceToTurnAfterNextTurn),
        m_distanceToTurnAfterNextTurnInUnit(distanceToTurnAfterNextTurnInUnit),
        m_nextAction(nextAction),
        m_nextCrossing(nextCrossing),
        m_nextStreetName(nextStreetName),
        m_nextStreetNameAfterNextTurn(nextStreetNameAfterNextTurn),
        m_onTrack(onTrack),
        m_position(position),
        m_speed(speed),
        m_timeToGoal(timeToGoal),
        m_timeToNextTurn(timeToNextTurn),
        m_exitCount(exitCount),
        m_exitCountForActionAfterNextAction(exitCountForActionAfterNextAction),
        m_nextHighway(nextHighway),
        m_detour(detour),
        m_speedcam(speedcam),
        m_isLeftSideTraffic(isLeftSideTraffic),
        m_newCrossing(newCrossing),
        m_crossingNbr(crossingNbr)
{
}

UpdateNavigationInfo::~UpdateNavigationInfo()
{
}

bool
UpdateNavigationInfo::isOnTrack() const
{
   return m_onTrack;
}

RouteAction
UpdateNavigationInfo::getNextAction() const
{
   return m_nextAction;
}

RouteCrossing
UpdateNavigationInfo::getNextCrossing() const
{
   return m_nextCrossing;
}

const WFString&
UpdateNavigationInfo::getNextStreetName() const
{
   return m_nextStreetName;
}

const WFString&
UpdateNavigationInfo::getNextStreetNameAfterNextTurn() const
{
   return m_nextStreetNameAfterNextTurn;
}

const WFString&
UpdateNavigationInfo::getCurrentStreetName() const
{
   return m_currentStreetName;
}

WGS84Coordinate
UpdateNavigationInfo::getPosition() const
{
   return m_position;
}

wf_uint16
UpdateNavigationInfo::getSpeed() const
{
   return m_speed;
}

wf_uint32
UpdateNavigationInfo::getTimeToGoal() const
{
   return m_timeToGoal;
}

wf_uint32
UpdateNavigationInfo::getTimeToNextTurn() const
{
   return m_timeToNextTurn;
}

wf_uint32
UpdateNavigationInfo::getDistanceToGoal() const
{
   return m_distanceToGoal;
}

const WFString&
UpdateNavigationInfo::getDistanceToGoalInUnit() const
{
   return m_distanceToGoalInUnit;
}

wf_uint32
UpdateNavigationInfo::getDistanceToNextTurn() const
{
   return m_distanceToNextTurn;
}

const WFString&
UpdateNavigationInfo::getDistanceToNextTurnInUnit() const
{
   return m_distanceToNextTurnInUnit;
}

wf_uint32
UpdateNavigationInfo::getDistanceToTurnAfterNextTurn() const
{
   return m_distanceToTurnAfterNextTurn;
}

const WFString&
UpdateNavigationInfo::getDistanceToTurnAfterNextTurnInUnit() const
{
   return m_distanceToTurnAfterNextTurnInUnit;
}

RouteAction
UpdateNavigationInfo::getActionAfterNextAction() const
{
   return m_actionAfterNextAction;
}

RouteCrossing
UpdateNavigationInfo::getCrossingAfterNextCrossing() const
{
   return m_crossingAfterNextCrossing;
}

wf_uint32
UpdateNavigationInfo::getExitCount() const
{
   return m_exitCount;
}

wf_uint32
UpdateNavigationInfo::getExitCountForActionAfterNextAction() const
{
   return m_exitCountForActionAfterNextAction;
}

bool
UpdateNavigationInfo::getNextHighway() const
{
   return m_nextHighway;
}

bool
UpdateNavigationInfo::getDetour() const
{
   return m_detour;
}

bool
UpdateNavigationInfo::getSpeedcam() const
{
   return m_speedcam;
}

bool
UpdateNavigationInfo::getIfLeftSideTraffic() const 
{
   return m_isLeftSideTraffic;
}

bool
UpdateNavigationInfo::getIsNewCrossing() const
{
   return m_newCrossing;
}

wf_uint32
UpdateNavigationInfo::getCrossingNbr() const
{
   return m_crossingNbr;
}

UpdateNavigationInfo&
UpdateNavigationInfo::operator=(const UpdateNavigationInfo& rhs)
{
   if (this == &rhs) {
      // Handle self assignment
      return *this;
   }
   
   m_actionAfterNextAction = rhs.m_actionAfterNextAction;
   m_crossingAfterNextCrossing = rhs.m_crossingAfterNextCrossing;
   m_currentStreetName = rhs.m_currentStreetName;
   m_distanceToGoal = rhs.m_distanceToGoal;
   m_distanceToGoalInUnit = rhs.m_distanceToGoalInUnit;
   m_distanceToNextTurn = rhs.m_distanceToNextTurn;
   m_distanceToNextTurnInUnit = rhs.m_distanceToNextTurnInUnit;
   m_distanceToTurnAfterNextTurn = rhs.m_distanceToTurnAfterNextTurn;
   m_distanceToTurnAfterNextTurnInUnit = rhs.m_distanceToTurnAfterNextTurnInUnit;
   m_nextAction = rhs.m_nextAction;
   m_nextCrossing = rhs.m_nextCrossing;
   m_nextStreetName = rhs.m_nextStreetName;
   m_nextStreetNameAfterNextTurn = rhs.m_nextStreetNameAfterNextTurn;
   m_onTrack = rhs.m_onTrack;
   m_position = rhs.m_position;
   m_speed = rhs.m_speed;
   m_timeToGoal = rhs.m_timeToGoal;
   m_timeToNextTurn = rhs.m_timeToNextTurn;
   m_exitCount = rhs.m_exitCount;
   m_nextHighway = rhs.m_nextHighway;
   m_detour = rhs.m_detour;
   m_speedcam = rhs.m_speedcam;
   m_isLeftSideTraffic = rhs.m_isLeftSideTraffic;
   m_newCrossing = rhs.m_newCrossing;
   m_crossingNbr = rhs.m_crossingNbr;
   
   return *this;
}

} // End namespace WFAPI
