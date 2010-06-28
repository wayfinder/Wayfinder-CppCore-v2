/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "NokiaMaemoLocationInterfaceImpl.h"
#include "LocationInterface.h"
#include "LocationListener.h"
#include "Nav2APIChannel.h"
#include "LocationStatusCode.h"
#include "GuiProtMess.h"
#include "GuiProtEnums.h"
#include "Nav2Logging.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
#include <location/location-gps-device.h>
#include <location/location-gpsd-control.h>
#ifdef __cplusplus
}
#endif // __cplusplus

#include <iostream>
#include <math.h>
#include <time.h>

using namespace std;
using namespace isab;

namespace WFAPI {

/**
 * Static pointer to this class, will be set by a call to
 * StaticLocationInterfaceImplHolder::setLocationInterfaceImpl.
 * By doing this the static class NokiaMaemoLocationInterfaceImplHolder
 * can access the public functions in NokiaMaemoLocationInterfaceImpl.
 * Needed when getting callbacks from the Maemo gps.
 */
static NokiaMaemoLocationInterfaceImpl* s_locationImpl;

/**
 * Holder class for a pointer to a NokiaMaemoLocationInterfaceImpl.
 */
class StaticLocationInterfaceImplHolder {
public:

   /**
    * Sets the static pointer s_locationImpl to the given value.
    * This way, the static functions in this class will be able
    * to call member functions of NokiaMaemoLocationInterfaceImpl.
    *
    * @param locationImpl A pointer to an instance of
    *                     NokiaMaemoLocationInterfaceImpl.
    */
   static void setLocationInterfaceImpl(
      NokiaMaemoLocationInterfaceImpl* locationImpl) {
      s_locationImpl = locationImpl;
   }

   /**
    * Static callback function. Used as callback for the Maemo
    * gps. This function will be called periodically when subscribing
    * for gps positions. Will call NokiaMaemoLocationInterfaceImpl::
    * lbsCallback when position is received.
    *
    */
   static void lbsCallback() {
      s_locationImpl->lbsCallback();
   }
};

NokiaMaemoLocationInterfaceImpl::
NokiaMaemoLocationInterfaceImpl(Nav2APIImpl* api) :
   LocationInterfaceImpl(api),
   m_registeredAndTracking(false),
   m_needSendStartedCallback(false),
   m_receivedGPSPosition(false),
   m_lastCellID(TGPPCellIDRequestData("","","","","",""))
{
   // Set the static s_locationImpl to this
   StaticLocationInterfaceImplHolder::setLocationInterfaceImpl(this);

   // Initialize the gps control and device with NULL
   m_control = NULL;
   m_device = NULL;
}

NokiaMaemoLocationInterfaceImpl::~NokiaMaemoLocationInterfaceImpl()
{
   if (m_registeredAndTracking) {
      // Try to stop tracking and deregister client.
      stopTracking();
      deregisterLbsClient();
   }
}

SynchronousStatus
NokiaMaemoLocationInterfaceImpl::lbsStarted()
{
   wf_int statusCode = LBS_STATUS_STARTED;
   if (!m_registeredAndTracking) {
      statusCode = LBS_STATUS_NOT_STARTED;
   }
   return SynchronousStatus(statusCode, "", "");
}

AsynchronousStatus
NokiaMaemoLocationInterfaceImpl::startLbs()
{
   if (m_registeredAndTracking) {
      // We are already started and tracking are on.
      nav2log << "[Debug] NokiaMaemoLocationInterfaceImpl::startLbs : "
              << "We are already started and are tracking are on."
              << endl;
      return AsynchronousStatus(RequestID(RequestID::INVALID_REQUEST_ID),
                                OK, "", "");
   }

   // Send CellID request
   if (!checkAndSendCellIDRequest(true)) {
      nav2log << "[Debug] NokiaMaemoLocationInterfaceImpl::startLbs : "
              << "Send CellID request failed" << endl;
      return AsynchronousStatus(RequestID(RequestID::INVALID_REQUEST_ID),
                                LBS_STARTUP_FAILED, "", "");
   }

   bool started = registerLbsClient();
   if (!started) {
      // Not good, could not start the gps.
      nav2log << "[Debug] NokiaMaemoLocationInterfaceImpl::startLbs : " 
              << "registerLbsClient() failed." << endl;
      return AsynchronousStatus(RequestID(RequestID::INVALID_REQUEST_ID),
                                LBS_STARTUP_FAILED, "", "");
   } else {
      if (!startTracking()) {
         // Failed to start the tracking, fatal error. Deregister
         // client
         nav2log << "[Debug] NokiaMaemoLocationInterfaceImpl::startLbs : "
                 << "startTracking() failed." << endl;
         deregisterLbsClient();
         return AsynchronousStatus(RequestID(RequestID::INVALID_REQUEST_ID),
                                   LBS_START_TRACKING_FAILED, "", "");
      }
      m_registeredAndTracking = true;
      m_needSendStartedCallback = true;
      isab::GenericGuiMess message(isab::GuiProtEnums::CONNECT_GPS);
      sendAsynchronousRequestToNav2(&message);
      return AsynchronousStatus(RequestID(RequestID::INVALID_REQUEST_ID),
                                OK, "", "");
   }   
}

SynchronousStatus
NokiaMaemoLocationInterfaceImpl::stopLbs()
{
   if (m_registeredAndTracking) {
      // Try to stop the tracking.
      bool stopTrackingStatus = stopTracking();
      // Try to deregister the client. We try to do this no matter
      // the status of stopTracking.
      bool closeGpsConStatus = deregisterLbsClient();
      if (!stopTrackingStatus || !closeGpsConStatus) {
         return SynchronousStatus(LBS_STOP_FAILED, "", "");
      }
   }
   m_registeredAndTracking = false;
   isab::GenericGuiMess message(isab::GuiProtEnums::DISCONNECT_GPS);
   sendAsynchronousRequestToNav2(&message);

   return SynchronousStatus(OK, "", "");
}

bool
NokiaMaemoLocationInterfaceImpl::registerLbsClient()
{
   //Init glib. Should probably only be done if not already initialized
   g_type_init();
   // Check if g_thread already has been initialized. note that
   // all parts doing this init must check if its already initialized
   // i.e. WFAPITestClient does this init aswell.
   if (!g_thread_supported ()) g_thread_init(NULL);

   // Location Control setup
   m_control = location_gpsd_control_get_default();
//   if(m_control->can_control)
      location_gpsd_control_start (m_control);

   // Location Device setup
   m_device = (LocationGPSDevice*)g_object_new(LOCATION_TYPE_GPS_DEVICE, NULL);
   location_gps_device_reset_last_known (m_device);
   nav2log << "[Debug] GPS Device pointer adress: " << (unsigned int)m_device << endl;
   nav2log << "[Info] GPS Device object obtained\n" << endl;

   if (m_device == NULL) {
      // Could not register client 
      nav2log << "NokiaMaemoLocationInterfaceImpl::registerLbsClient failed, invalid device id." << endl;
      return false;
   }
   return true;
}

bool 
NokiaMaemoLocationInterfaceImpl::deregisterLbsClient() 
{

//   if(m_control->can_control)
   location_gpsd_control_stop(m_control);
   g_object_unref(m_control);
   g_object_unref(m_device);

   // Add verification the control stop and unref went alright.
   return true;
}   

bool 
NokiaMaemoLocationInterfaceImpl::startTracking()
{
   nav2log << "startTracking : connecting to signal 'changed'" << endl;
   m_idd_changed = g_signal_connect (m_device, "changed",
                                        G_CALLBACK (StaticLocationInterfaceImplHolder::lbsCallback),
                                        NULL);
   nav2log << "startTracking : signal 'changed' got idd=" << m_idd_changed << endl;

   if (! m_idd_changed) {
      nav2log << "NokiaMaemoLocationInterfaceImpl::startTracking failed."
              << endl;
      return false;
   }
   return true;
}   

bool
NokiaMaemoLocationInterfaceImpl::stopTracking()
{
   nav2log << "stopTracking : disconnecting from signal 'changed' with idd="
           << m_idd_changed << endl;
   if (g_signal_handler_is_connected (m_device, m_idd_changed)) {
      g_signal_handler_disconnect(m_device, m_idd_changed);
      if (g_signal_handler_is_connected (m_device, m_idd_changed)) {
         nav2log << "stopTracking : disconnect from signal 'changed' with idd="
                 << m_idd_changed << " failed" << endl;
         return false; //could not disconnect from signal
      }
      // set the changed signal connect int back to initial value disconnected
      m_idd_changed = 0;
   }
   else {
      nav2log << "stopTracking : skipping signal 'changed' disconnect "
              << "since it's already disconnected" << endl;
      return true;
   }
   return true;
}

void
NokiaMaemoLocationInterfaceImpl::lbsCallback()
{
   m_receivedGPSPosition = false;
   wf_int statusCode = OK;
   nav2log << "Entered lbsCallback" << endl;
   if (!m_device) {
      nav2log << "GPSDevice is null" << endl;
      return;
   }
   nav2log << "lbsCallback: GPSMode [ "
        << m_device->fix->mode << " ]" << endl;
   switch (m_device->fix->mode) {
   case 2: //LOCATION_GPS_DEVICE_MODE_2D
      nav2log << "mode = LOCATION_GPS_DEVICE_MODE_2D" << endl;
   case 3: //LOCATION_GPS_DEVICE_MODE_3D
      nav2log << "mode = LOCATION_GPS_DEVICE_MODE_3D" << endl;
      if (m_needSendStartedCallback) {
         nav2log << "first callback since lbs started" << endl;
         // This is the first callback since the lbs was started,
         // we need to call startedLbs for the listeners.
         const ListenerCont& listeners = getListeners();
         for (ListenerCont::iterator it = listeners.begin(); 
            it != listeners.end(); ++it) {
            (*it)->startedLbs();
         }
         m_needSendStartedCallback = false;
      }
      if ( (m_device->fix->fields & LOCATION_GPS_DEVICE_LATLONG_SET) || 
           (m_device->fix->fields & LOCATION_GPS_DEVICE_ALTITUDE_SET) ) {
         nav2log << "LATLONG or ALTITUDE is set, time="
                 << (double)m_device->fix->time << endl;
         m_receivedGPSPosition = true;
         // Print the gps data to nav2log, for debug purposes
         printGpsData();

         // Write the nmea data to the bt channel
         writeNmeaData();
      } else {
         nav2log << "lbsCallback: Gps state not available (no positions)."
              << endl;
      } 
      break;
   default:
      break;
   }

   // Slightly hacky but I'm testing if we have changed cell id
   if (!m_receivedGPSPosition) {
      checkAndSendCellIDRequest();
   }

   if (statusCode != OK) {
      const ListenerCont& listeners = getListeners();
      for (ListenerCont::iterator it = listeners.begin(); 
         it != listeners.end(); ++it) {
         // Report the error to the listeners.
         (*it)->error(AsynchronousStatus(RequestID::INVALID_REQUEST_ID,
                                         statusCode,
                                         "", ""));
      }
   }
}

void
NokiaMaemoLocationInterfaceImpl::writeNmeaData()
{
   if (isnan(m_device->fix->latitude) ||
       isnan(m_device->fix->longitude) ||
       isnan(m_device->fix->speed)  ||
       isnan(m_device->fix->track) ) {
      //Partial update - we should send dummy data
      nav2log << "Bad gps status received";
      //SendGpsBadStatusToUI();
      return;
   }

   nav2log << "from gps: lat=" << m_device->fix->latitude
           << " lon=" << m_device->fix->longitude
           << " speed=" << m_device->fix->speed
           << " track=" << m_device->fix->track
           << " pdop=" << m_device->fix->eph
           << endl;

   const time_t time = (time_t)m_device->fix->time;
   tm* tm=gmtime(&time);
   nav2log << "Time from gps: " << asctime(tm) << endl;
   double param, fractpart, intpart;
   bool has3D = 0;
   if(m_device->fix->mode == LOCATION_GPS_DEVICE_MODE_3D) 
      has3D = 1;
   bool north = true, east = true;
   double lat = m_device->fix->latitude;
   if(lat < 0){
      north = false;
      lat = -lat;
   }
   double lon = m_device->fix->longitude;
   if(lon < 0){
      east = false;
      lon = -lon;
   }

   double spd = m_device->fix->speed;
   double hdg = m_device->fix->track;
   //Horizontal position uncertainty
   double pdop = 10.0;
   if(!isnan(m_device->fix->eph))
      pdop = m_device->fix->eph;

   int year = tm->tm_year - ((tm->tm_year/100)*100);
   int deglat, deglon;
   fractpart = modf (lat, &intpart);
   deglat=(int)intpart;
   fractpart = modf (lon, &intpart);
   deglon=(int)intpart;

   char msgbuf[384];
   char* msgGPGSA = msgbuf;

   double minlatfloat, minlonfloat;
   minlatfloat = (lat-deglat)*60;
   minlonfloat = (lon-deglon)*60;

   wf_int32 minlat, minlon;
   wf_int32 minlatfrac, minlonfrac;
   fractpart = modf (minlatfloat, &intpart);
   minlat=(int)intpart;
   minlatfloat=fractpart;
   fractpart = modf (minlonfloat, &intpart);
   minlon=(int)intpart;
   minlonfloat=fractpart;

   fractpart = modf (minlatfloat*10000, &intpart);
   minlatfrac=(int)intpart;
   fractpart = modf (minlonfloat*10000, &intpart);
   minlonfrac=(int)intpart;

   snprintf(msgGPGSA, 128, "$GPGSA;M;%c;;;;;;;;;;;;;%f;;*\n\r", has3D ? '3' : '2', pdop);
   char from[] = ",;";
   char to[] = ".,";
   size_t i = 0;
   for(i = 0; i < strlen(from); ++i){
      char* pos = strchr(msgGPGSA, int(from[i]));
      while(pos != NULL){
         *pos = to[i];
         pos = strchr(pos, from[i]);
      }
   }

   char* msgGPRMC = msgbuf + strlen(msgGPGSA);

   snprintf(msgGPRMC,128,
            "$GPRMC;%02u%02u%02u.000;%c;%02u%02u.%04u;%c;%03u%02u.%04u;%c;%f;%f;%02u%02u%02u;;*\n\r",
            //"$GPRMC;%02u%02u%02u;%c;%02u%09.6f;%c;%03u%09.6f;%c;%f;%f;%02u%02u%02u;;*\n\r",
            //"$GPRMC,%u%u%u,A,%02u%09.6f,%c,%03u%09.6f,%c,%f,%u%u%u,,*\n\r",
            tm->tm_hour, tm->tm_min, tm->tm_sec,
            'A',
            deglat, minlat, minlatfrac, north ? 'N' : 'S',
            deglon, minlon, minlonfrac, east ? 'E' : 'W',
            1.9438445*spd, hdg,
            tm->tm_mday+1, tm->tm_mon+1, year);
   for(i = 0; i < strlen(from); ++i){
      char* pos = strchr(msgGPRMC, int(from[i]));
      while(pos != NULL){
         *pos = to[i];
         pos = strchr(pos, from[i]);
      }
   }

   char* msgGPGGA = msgbuf + strlen(msgbuf);

   snprintf(msgGPGGA,128,
            "$GPGGA;%02u%02u%02u.000;%02u%02u.%04u;%c;%03u%02u.%04u;%c;1;04;1.0;%f;M;;;;0000*\n\r",
            tm->tm_hour, tm->tm_min, tm->tm_sec,
            deglat, minlat, minlatfrac, north ? 'N' : 'S',
            deglon, minlon, minlonfrac, east ? 'E' : 'W',
            m_device->fix->altitude);

   for(i = 0; i < strlen(from); ++i){
      char* pos = strchr(msgGPGGA, int(from[i]));
      while(pos != NULL){
         *pos = to[i];
         pos = strchr(pos, from[i]);
      }
   }

   nav2log << "Writing " << strlen(msgbuf) << " bytes to btCh" << endl;
   getBtChannel()->getChannel()->writeData((uint8*)(msgbuf), strlen(msgbuf));
   nav2log << "data written" << endl;

   nav2log << "--- Printing gps data ---" << endl;
   nav2log << "  * Raw nmea *" << endl;
   nav2log << msgbuf << endl;

}

void
NokiaMaemoLocationInterfaceImpl::printGpsData()
{
   /*
    * NOTE: Before calling this method we should have checked that there are
    * positions to read. At the moment this is for debug purposes only.
    */
   nav2log << "--- Printing gps data ---" << endl;
   nav2log << "  * Parsed gps data *" << endl;

   nav2log << "   Nbr satellites in view: " << m_device->satellites_in_view << endl;
   nav2log << "   utctime = " << m_device->fix->time << ", "
        << "lon = " << m_device->fix->longitude << ", "  
        << "lat = " << m_device->fix->latitude << ", "  
        << "alt = " << m_device->fix->altitude << ", "  
        << "speed = " << m_device->fix->speed << ", "  
        << "course = " << m_device->fix->track << ", "  
        << "nbr sats used = " << m_device->satellites_in_use << ", "  
        << "eph (HDOP) = " << m_device->fix->eph << ", "  
        << "epv (VDOP) = " << m_device->fix->epv << endl;

}


bool
NokiaMaemoLocationInterfaceImpl::checkAndSendCellIDRequest(bool force)
{
   //this will never work for MAEMO4, always return true
   bool ok = true;

   return ok;
}


} // End namespace WFAPI
