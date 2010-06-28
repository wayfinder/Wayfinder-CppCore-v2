/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_LOCATION_LISTENER_H
#define WFAPI_LOCATION_LISTENER_H

#include "WFAPIConfig.h"
#include "Listener.h"
#include "LocationUpdateInformation.h"
#include "AreaUpdateInformation.h"


namespace WFAPI {

/**
 * Listener for position updates. The abstract functions are the
 * callback functions that is called for the respective events.
 *
 * Create a subclass and implement the callbacks and add a 
 * subclass object to the LocationInterface to start getting
 * position update callbacks.
 *
 * The error function, declared in the super class Listener, will be called
 * for all errors that occur . The status code can be either from the
 * StatusCode enumeration or from LocationStatusCode
 * enumeration.
 */
class LocationListener : public Listener
{

public:

   /**
    * Constructor with no arguments.
    */
   LocationListener();

   /**
    * Destructor.
    */
   virtual ~LocationListener();

   /**
    * Called when a location update with an area location is received.
    *
    * @param areaUpdateInformation The AreaUpdateInformation.
    */
   virtual void areaUpdate(
      const AreaUpdateInformation& areaUpdateInformation) = 0;

   /**
    * Called when a location update with a point location is received.
    *
    * @param locationUpdate The LocationUpdateInformation.
    */
   virtual void locationUpdate(
      const LocationUpdateInformation& locationUpdate) = 0;

   /**
    * Called when the LBS has started.
    */
   virtual void startedLbs() = 0;

private:
};

} // End namespace WFAPI

#endif // End WFAPI_LOCATION_LISTENER_H
