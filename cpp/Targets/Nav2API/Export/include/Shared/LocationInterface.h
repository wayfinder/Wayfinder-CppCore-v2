/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_LOCATION_INTERFACE_H
#define WFAPI_LOCATION_INTERFACE_H

#include "WFAPIConfig.h"
#include "Interface.h"
#include "SynchronousStatus.h"
#include "AsynchronousStatus.h"

namespace WFAPI {

// Forward declarations
class LocationListener;
class Nav2APIImpl;
class LocationInterfaceImpl;
class GeocodingListener;
class AreaUpdateInformation;
class LocationUpdateInformation;

/**
 * Interface class for position updates.
 */
class LocationInterface : public Interface
{

public:
   /**
    * Destructor.
    */
   virtual ~LocationInterface();

   /**
    * Add a listener for location updates. 
    * @see LocationListener
    *
    * @param listener The LocationListener to add. 
    *                 It may not be deleted until it is removed by
    *                 calling removeLocationListener.
    */
   void addLocationListener(LocationListener* listener);
   
   /**
    * Remove a listener.
    *
    * @param listener The LocationListener to remove.
    *                 It should now be deleted by caller, unless caller
    *                 keeps it around for later use.
    */
   void removeLocationListener(LocationListener* listener);


   /**
    * Add a listener for geocoding updates. 
    * @see GeocodingListener
    *
    * @param listener The GeocodingListener to add. 
    *                 It may not be deleted until it is removed by
    *                 calling removeGeocodingListener.
    */
   void addGeocodingListener(GeocodingListener* listener);

   /**
    * Remove a listener.
    *
    * @param listener The GeocodingListener to remove.
    *                 It should now be deleted by caller, unless caller
    *                 keeps it around for later use.
    */
   void removeGeocodingListener(GeocodingListener* listener);
   
   /**
    * Checks if the lbs engine is started or not.
    *
    * @return The status LBS_STATUS_STARTED if the lbs has been started,
    *         LBS_STATUS_NOT_STARTED if the lbs has not been started.
    */
   SynchronousStatus lbsStarted();
   
   /**
    * This asynchronous function starts up the LBS. When operation is done
    * the added LocationListeners will get a callback to startedLbs if
    * successfully started, otherwise error will get called with a error
    * code from StatusCode or LocationStatusCode
    *
    * @return The status and if status is OK.
    */
   AsynchronousStatus startLbs();

   /**
    * Stop LBS updates.
    *
    * @return The status of the operation, OK if operation succeeded and
    *         either a status from StatusCode or LocationStatusCode.
    */
   SynchronousStatus stopLbs();

   /**
    * Set the Location hardware to use right now.
    * Not calling this method will cause the library to use all available
    * hardware.
    * If startLbs() has been called before this function is called, the api
    * will respond with a callback on the users startedLbs(), after the
    * hardware change is complete. If not, the callback to lbsStarted()
    * will arrive after the startLbs() is called (if successful).
    *
    * @param hardware The hardware to use.
    *                 WF_MAX_UINT8 = all available hardware,
    *                 0 = no hardware,
    *                 1 = gps only,
    *                 2 = CellID only,
    *                 3 = gps and CellID,
    *
    * @return The status of the operation, OK if operation succeeded and
    *         either a status from StatusCode or LocationStatusCode.
    */
   SynchronousStatus setLocationHardware(wf_uint8 hardware);

   /**
    * Request a reverse geocoding from a specified area.
    * The reply will asynchronously be sent to registered geocoding listeners.
    *
    * @param locationArea The area that is to be looked up.
    * @return A status indicating whether the request was successful or not.
    */
   AsynchronousStatus
   requestReverseGeocoding(const AreaUpdateInformation& locationArea);

   /**
    * Request a reverse geocoding from a specified location.
    * The reply will asynchronously be sent to registered geocoding listeners.
    *
    * @param location The location that is to be looked up.
    * @return A status indicating whether the request was successful or not.
    */
   AsynchronousStatus
   requestReverseGeocoding(const LocationUpdateInformation& location);

   /**
    * Parses a file of properly formatted NMEA data and replays it with
    * 1 second intervals.
    *
    * @param fileName The name of the file to parse.
    * @param begin The first message to use, messages before this was discarded
    * @param end The last message to use, messages after this will be discarded
    */ 
   void replayNMEAData(const char* fileName, int begin = 0, int end = -1);
   
   /**
    * Internal function.
    */
   LocationInterfaceImpl* getImpl();

private:
   /**
    * Do not create LocationInterface get it from the Nav2API class.
    */
   LocationInterface(Nav2APIImpl* nav2APIimpl);

   /**
    * We do not allow copies of this interface.
    */
   LocationInterface(const LocationInterface&);
   const LocationInterface& operator=(const LocationInterface&);

   friend class Nav2APIImpl;

   struct impl;

   impl* m_impl;
};

} // End namespace WFAPI

#endif // End WFAPI_LOCATION_INTERFACE_H
