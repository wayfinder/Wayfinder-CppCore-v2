/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_UPDATE_NAVIGATION_DISTANCE_INFO_H
#define WFAPI_UPDATE_NAVIGATION_DISTANCE_INFO_H

#include "WFAPIConfig.h"
#include "WFString.h"

namespace WFAPI {

/**
 * This class contains information update for the distances and times
 * about the next turn. A UpdateNavigationDistanceInfo is an update to an
 * UpdateNavigationInfo.
 */
class UpdateNavigationDistanceInfo
{

public:

   /**
    * Constructor.
    *
    * @param distanceToGoal The distance to left to the destination.
    * @param distanceToGoalInUnit The distance left to the destination
    *                             formated in the distance unit set in settings.
    * @param distanceToNextTurn The distance to the next turn.
    * @param distanceToNextTurnInUnit The distance to the next turn formated in
    *                                 the distance unit set in settings.
    * @param speed The current speed.                               
    * @param timeToGoal Time left to the destination.
    * @param timeToNextTurn Time left to the next turn.
    */
   UpdateNavigationDistanceInfo(wf_uint32 distanceToGoal,
                                const WFString& distanceToGoalInUnit,
                                wf_uint32 distanceToNextTurn,
                                const WFString& distanceToNextTurnInUnit,
                                wf_uint16 speed,
                                wf_uint32 timeToGoal,
                                wf_uint32 timeToNextTurn);

   /**
    * Destructor.
    */
   virtual ~UpdateNavigationDistanceInfo();

   /**
    * The distance left to the destination.
    *
    * @return The distance left in meters.
    */
   wf_uint32 getDistanceToGoal() const;

   /**
    * The distance left to the destination.
    *
    * @return The distance left in the unit set in settings.
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
    * @return The distance to next turn in the unit set in settings.
    */
   const WFString& getDistanceToNextTurnInUnit() const;

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

private:

   /// Distance to goal. In meters.
   wf_uint32 m_distanceToGoal;

   /// Distance to goal in the unit set in settings.
   WFString m_distanceToGoalInUnit;

   /// Distance to next turn. In meters.
   wf_uint32 m_distanceToNextTurn;

   /// Distance to next turn in the unit set in settings.
   WFString m_distanceToNextTurnInUnit;

   /// The road speed. In km/h.
   wf_uint16 m_speed;

   /// The time to goal. In seconds.
   wf_uint32 m_timeToGoal;

   /// The time to next turn. In seconds.
   wf_uint32 m_timeToNextTurn;

};

} // End namespace WFAPI

#endif // End WFAPI_UPDATE_NAVIGATION_DISTANCE_INFO_H

