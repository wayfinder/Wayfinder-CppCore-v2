/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
 
#ifndef WFAPI_LOCATION_STATUS_CODE_H
#define WFAPI_LOCATION_STATUS_CODE_H

#include "WFAPIConfig.h"
#include "StatusCode.h"

namespace WFAPI {

/**
 * Enumeration for location interface specific error codes that can be
 * received.
 */
enum LocationStatusCode
{
   /// The startup of LBS failed.
   LBS_STARTUP_FAILED = START_LOCATION_STATUS_CODE + 1,

   /// The lbs is started.
   LBS_STATUS_STARTED,

   /// The lbs is not started.
   LBS_STATUS_NOT_STARTED,

   /// The stop of LBS failed.
   LBS_STOP_FAILED,

   /// Could not start tracking.
   LBS_START_TRACKING_FAILED,

   /// Time out error from the gps.
   LBS_ERROR_TIME_OUT,

   /// Out of service error from the gps.
   LBS_OUT_OF_SERVICE,
   
};

} // End namespace WFAPI

#endif // End WFAPI_LOCATION_STATUS_CODE_H
