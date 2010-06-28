/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "UpdateNavigationDistanceInfo.h"


namespace WFAPI {

UpdateNavigationDistanceInfo::UpdateNavigationDistanceInfo(
   wf_uint32 distanceToGoal,
   const WFString& distanceToGoalInUnit,
   wf_uint32 distanceToNextTurn,
   const WFString& distanceToNextTurnInUnit,
   wf_uint16 speed,
   wf_uint32 timeToGoal,
   wf_uint32 timeToNextTurn)
      : m_distanceToGoal(distanceToGoal),
        m_distanceToGoalInUnit(distanceToGoalInUnit),
        m_distanceToNextTurn(distanceToNextTurn),
        m_distanceToNextTurnInUnit(distanceToNextTurnInUnit),
        m_speed(speed),
        m_timeToGoal(timeToGoal),
        m_timeToNextTurn(timeToNextTurn)
{
}

UpdateNavigationDistanceInfo::~UpdateNavigationDistanceInfo()
{
}

wf_uint32
UpdateNavigationDistanceInfo::getDistanceToGoal() const
{
   return m_distanceToGoal;
}

const WFString&
UpdateNavigationDistanceInfo::getDistanceToGoalInUnit() const
{
   return m_distanceToGoalInUnit;
}

wf_uint32
UpdateNavigationDistanceInfo::getDistanceToNextTurn() const
{
   return m_distanceToNextTurn;
}


const WFString&
UpdateNavigationDistanceInfo::getDistanceToNextTurnInUnit() const
{
   return m_distanceToNextTurnInUnit;
}

wf_uint16
UpdateNavigationDistanceInfo::getSpeed() const
{
   return m_speed;
}

wf_uint32
UpdateNavigationDistanceInfo::getTimeToGoal() const
{
   return m_timeToGoal;
}

wf_uint32
UpdateNavigationDistanceInfo::getTimeToNextTurn() const
{
   return m_timeToNextTurn;
}

} // End namespace WFAPI

