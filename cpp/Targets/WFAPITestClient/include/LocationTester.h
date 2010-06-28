/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef LOCATION_TESTER_H
#define LOCATION_TESTER_H

#include "LocationListener.h"
#include "GenericTester.h"
#include "GeocodingListener.h"
#include "RequestID.h"

#include <set>

namespace WFAPI {
// Forward declaration
class LocationInterface;
}

using namespace WFAPI;

/**
 * Test class that implements testing for location interface.
 */
class LocationTester : public LocationListener,
                       public GeocodingListener,
                       public GenericTester
{

public:
   
   /**
    * Constructor, adds itself as listener to LocationInterface
    *
    * @param locationInterface The location interface.
    */
   LocationTester(LocationInterface& locationInterface);
                   
   /**
    * Destructor.
    */
   ~LocationTester();

   /**
    * Add an extra location listener.
    *
    * @param listener Listener to add.
    */
   void addLocationListener(LocationListener* listener);

   /**
    * Remove location listener.
    * 
    * @param listener Listener to remove.
    */
   void removeLocationListener(LocationListener* listener);

   /**
    * Test that makes two reverse geocoding requests.
    */
   void requestReverseGeocoding();
   
public: // From base class

   /**
    * Called when an error has occured.
    */
   void error(const AsynchronousStatus& status);

   /**
    * From LocationListener.
    */
   void areaUpdate(const AreaUpdateInformation& areaUpdateInformation);

   /**
    * From LocationListener.
    */
   void locationUpdate(const LocationUpdateInformation& locationUpdate);

   /**
    * Called when the LBS has started.
    */
   void startedLbs();

   /**
    * Handle the input and return true if it was consumed.
    *
    * @param input The input to handle.
    * @param goBack Out parameter. Will be set to true if the
    *               input indicated that the tester should go
    *               out of scope.
    * @return True if the input was handled, false otherwise.
    */
   bool handleInput(const char* input, bool& goBack);

   /**
    * From GeocodingListener.
    */
   virtual void reverseGeocodingReply(RequestID requestID,
                                      GeocodingInformation info);
private:

   /**
    * Prints if the LBS is started.
    */
   void printLbsStarted();

   /// LocationInterface.
   LocationInterface& m_locationInterface;

   /// Typedef for a set that holds RequestID's
   typedef std::set<RequestID> ReverseGeocodingRequests;

   /// Holds all RequestID's for reverse geocodings requests.
   ReverseGeocodingRequests m_revGeoReq;
};

#endif // LOCATION_TESTER_H

