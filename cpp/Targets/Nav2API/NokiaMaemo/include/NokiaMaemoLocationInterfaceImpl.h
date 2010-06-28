/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_NOKIA_MAEMO_LOCATION_INTERFACE_IMPL_H
#define WFAPI_NOKIA_MAEMO_LOCATION_INTERFACE_IMPL_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
#include <location/location-gps-device.h>
#include <location/location-gpsd-control.h>
#ifdef __cplusplus
}
#endif // __cplusplus

#include "LocationInterfaceImpl.h"
#include "LocationInterface.h"

namespace WFAPI {

/**
 * Maemo specific lbs class. Handles gps communication for
 * Maemo platform.
 */
class NokiaMaemoLocationInterfaceImpl : public LocationInterfaceImpl {
public:

   /**
    * Constructor.
    *
    * @param api A pointer to the Nav2APIImpl.
    */
   NokiaMaemoLocationInterfaceImpl(Nav2APIImpl* api);

   /**
    * Destructor, tries to stop the tracking if ongoing and
    * deregister the client.
    */
   virtual ~NokiaMaemoLocationInterfaceImpl();

   /**
    * Checks if the client is registered and the tracking is on.
    *
    * @return The status LBS_STATUS_STARTED if the lbs has been started,
    *         LBS_STATUS_NOT_STARTED if the lbs has not been started.
    */    
   virtual SynchronousStatus lbsStarted();
   
   /**
    * This asynchronous function starts up the LBS. When operation is done
    * the added LocationListeners will get a callback to startedLbs if
    * successfully started, otherwise error will get called with a error
    * code from StatusCode or LocationStatusCode
    *
    * @return The status and if status is OK.
    */
   virtual AsynchronousStatus startLbs();

   /**
    * Stop LBS updates.
    *
    * @return The status of the operation, OK if operation succeeded and
    *         either a status from StatusCode or LocationStatusCode.
    */
   virtual SynchronousStatus stopLbs();

   /**
    * Called when gps reports new positions.
    *
    * @param event Defines if the call was due to position update or
    *                      if an error has occured.
    * @param gpsData Pointer to struct containing the actual gps data.
    */
   void lbsCallback();

protected:

   /**
    * Internal function, tries to register the the client to the
    * gps server.
    *
    * @return True if successfully registered, false otherwise.
    */
   bool registerLbsClient();

   /**
    * Internal function, tries to deregister the the client to the
    * gps server.
    *
    * @return True if successfully deregistering, false otherwise.
    */
   bool deregisterLbsClient(); 

   /**
    * Internal function, start the gps tracking.
    *
    * @return True if tracking start was successful, false otherwise.
    */
   bool startTracking();

   /**
    * Internal function, stops the gps tracking.
    *
    * @return True if tracking stop was successful, false otherwise.
    */
   bool stopTracking();

   /**
    * Prints the gps data received from the gps server.
    */
   void printGpsData();

   /**
    * Used to convert gps positions to nmea strings and write it to nav2
    */
   void writeNmeaData();

   /**
    * Check if we need to send a new cell id request, it we are in a new
    * cell.
    *
    * @param force If to send request regardless of new cell or not.
    * @return True if cell id request was sent ok, false if not.
    */
   bool checkAndSendCellIDRequest(bool force = false); 
   
private:

   /// Device, the gps device to use
   LocationGPSDevice* m_device;

   /// Control, the gps control pointer to use when manipulating gps settings
   LocationGPSDControl* m_control;

   /// id for gps change callback signal
   guint m_idd_changed;

   /// When true, we are registered to the gps server and tracking is on.
   bool m_registeredAndTracking;

   /**
    * First callback from gps server since started tracking this is true
    * This means that we should call LocationListener::startedLbs.
    */
   bool m_needSendStartedCallback;

   /**
    * True if we received a valid position from the last gps callback.
    */
   bool m_receivedGPSPosition;

   /**
    * The last Cell ID.
    */
   TGPPCellIDRequestData m_lastCellID;
};

} // End namespace WFAPI

#endif // End WFAPI_NOKIA_MAEMO_LOCATION_INTERFACE_IMPL_H
