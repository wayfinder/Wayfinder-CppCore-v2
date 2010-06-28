/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
 
#ifndef WFAPI_ROUTE_STATUS_CODE_H
#define WFAPI_ROUTE_STATUS_CODE_H

#include "WFAPIConfig.h"
#include "StatusCode.h"

namespace WFAPI {

/**
 * Enumeration for route interface specific error codes that can be received.
 */
enum RouteStatusCode
{
   /// General internal problem regarding the route.
   GENERAL_ROUTE_ERROR = START_ROUTE_STATUS_CODE + 1,

   /// There is no way to get from the origin to the destination.
   NO_ROUTE_FOUND,

   /// The route is too far for the selected transportation type.
   TOO_FAR_FOR_VEHICLE,

   /// Problem finding a road near the origin.
   PROBLEM_WITH_ORIGIN,

   /// Problem finding a road near the destination.
   PROBLEM_WITH_DEST,

   /// Customer hasn't the needed right to route.
   NO_ROUTE_RIGHT_ERROR,

   /**
    * We are already waiting to get a reply for an outstanding route request.
    * We currently do not have the support for handling multiple outstanding
    * route requests.
    */
   ALREADY_DOWNLOADING_ROUTE,

   /**
    * Something is wrong with the route that has been sent to us by the server.
    * We can't parse the route information due to data error.
    */
   ROUTE_INVALID,

   /**
    * A warning that a route was made using the gps position as origin
    * and no gps position was available right now, still trying.
    */
   NO_GPS_WARN,

   /**
    * A warning that a route was made using the gps position as origin
    * and was unable to get a gps position.
    */
   NO_GPS_ERROR,

   /// Too far away from the origin of the new route to start navigation.
   TOO_FAR_AWAY

};

} // End namespace WFAPI

#endif // End WFAPI_ROUTE_STATUS_CODE_H
