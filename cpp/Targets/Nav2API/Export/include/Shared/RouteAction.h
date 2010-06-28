/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_ROUTE_ACTION_H
#define WFAPI_ROUTE_ACTION_H

#include "WFAPIConfig.h"

namespace WFAPI {

/**
 * The type of turn to make.
 */
enum RouteAction
{
   /// Invalid
   INVALIDACTION  = 0,

   /// Go straight ahead.
   AHEAD,

   /// Turn left.
   LEFT,

   /// Turn right.
   RIGHT,

   /// Make a U-Turn.
   UTURN,

   /// Start of the route.
   START,

   /// Approaching target.
   FINALLY,

   /// Enter a roundabout.
   ENTER_RDBT,

   /// Exit from Roundabout.
   EXIT_RDBT,

   /// Exit road by ramp.
   EXIT_AT,

   /// Enter road by ramp.
   ON,

   /// Park your car here.
   PARK_CAR,

   /// Keep left when a road separates into two or more.
   KEEP_LEFT,

   /// Keep right when a road separates into two or more.
   KEEP_RIGHT,

   /// Start by makeing a u-turn.
   START_WITH_U_TURN,

   /// Go back in the roundabout.
   U_TURN_RDBT,

   /// Follow the current road.
   FOLLOW_ROAD,

   /// Drive onto a ferry.
   ENTER_FERRY,

   /// Leave a ferry.
   EXIT_FERRY,

   /// Leave one ferry, enter another.
   CHANGE_FERRY,

   /// Turn left at the end of the road.
   END_OF_ROAD_LEFT,

   /// Turn right at the end of the road.
   END_OF_ROAD_RIGHT,

   /// Turn left at the end of the road.
   OFF_RAMP_LEFT,

   /// Turn right at the end of the road.
   OFF_RAMP_RIGHT,

};

} // End namespace WFAPI

#endif // End WFAPI_ROUTE_ACTION_H
