/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _GEOCODINGLISTENER_H_
#define _GEOCODINGLISTENER_H_

#include "WFAPIConfig.h"
#include "Listener.h"

namespace WFAPI {

/**
 * The purpose of this class is to hold information supplied from
 * a reverse geocoding request.
 */
class GeocodingInformation {
public:
   /**
    * This enum indicates the precision of the reverse Geocoding lookup.
    * The precision is ordered in ascending order, i.e. COUNTRY is the lowest
    * available precision and ADDRESS is the highest available precision.
    *
    * The member highestPrecision indicates which of the fields that contain
    * information. Fields with higher precision than indicated by this
    * value will be empty.
    */
   enum Precision{
      /// The country
      COUNTRY = 0,
      
      /// The municipal
      MUNICIPAL,

      /// The city
      CITY,

      /// The district
      DISTRICT,

      /// The adress
      ADDRESS

   } 
   /// precision 
   highestPrecision;

   /// The field containing the name of the country.
   WFString countryName;
   
   /// The field containing the municipal name.
   WFString municipalName;

   /// The field containing the name of the city.
   WFString cityName;

   /// The field containing the name of the district.
   WFString districtName;

   /// The field containing the address.
   WFString addressName;

   /// The ID of the top region.
   wf_uint32 topRegionID;
};


/**
 * Listener for Geocoding and reverse Geocoding lookups. The abstract
 * functions are callback functions that are called as a response to
 * the respective asynchronous call in LocationInterface.
 *
 * Create a subclass and implement the callbacks. Then pass a pointer to
 * this class to the appropriate Geocoding request.
 *
 * The error function, declared in the super class Listener, will be called
 * for all requests that returns with an error. The status code can be
 * either from the StatusCode enumeration or from RouteStatusCode enumeration.
 */
class GeocodingListener : public Listener
{
public:

   /**
    * Constructor with no arguments.
    */
   GeocodingListener();

   /**
    * Destructor
    */
   virtual ~GeocodingListener();

   /**
    * This is the callback function that will be called when the reply for
    * a reverse geocoding request is available.
    *
    * @param requestID The identification of the reverse geocoding request.
    * @param info Contains all available 
    */
   virtual void reverseGeocodingReply(RequestID requestID,
                                      GeocodingInformation info) = 0;

};

} // End of namespace WFAPI
#endif /* _GEOCODINGLISTENER_H_ */
