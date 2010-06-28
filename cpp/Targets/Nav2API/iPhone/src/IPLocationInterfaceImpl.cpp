/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#define CORE_LOGGING_MODULE_NAME "IPhoneLocationInterfaceImpl"
//#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE

#include "CoreDebugPrint.h"
#include "IPLocationInterfaceImpl.h"
#include "LocationStatusCode.h"
#include "Nav2APIChannel.h"
#include "GuiProtMess.h"
#include "LocationListener.h"
#include "WGS84Coordinate.h"

#include <math.h>
#include <time.h>

namespace WFAPI {

IPLocationInterfaceImpl::IPLocationInterfaceImpl(Nav2APIImpl* api)
  : LocationInterfaceImpl(api),
    m_lbsStarted(false),
    m_needSendStartedCallback(false),
    m_gpsPosReceived(false)
{
   m_locationUtil = new IPhoneLocationUtil();
   m_locationUtil->addListener(this);
   m_currentHardware = WF_MAX_UINT8; //use every hardware possible by default
   m_requestedHardware = WF_MAX_UINT8;
}

IPLocationInterfaceImpl::~IPLocationInterfaceImpl()
{
   delete m_locationUtil;
   m_locationUtil = NULL; 
}

SynchronousStatus IPLocationInterfaceImpl::lbsStarted()
{
   wf_int statusCode = LBS_STATUS_STARTED;
   if (!m_lbsStarted) {
      statusCode = LBS_STATUS_NOT_STARTED;
   }
   return SynchronousStatus(statusCode, "", "");
}

AsynchronousStatus IPLocationInterfaceImpl::startLbs()
{
   bool alsoUseGps=false;
   //if there's a 1 on bit number 1, we want to use gps. this could be
   // the case with WF_MAX_UINT8, 0x01, 0x03, etc:
   if(m_requestedHardware & 0x01) {
      alsoUseGps = true;
    
      // Notify Nav2 that the GPS is connected.
      isab::GenericGuiMess message(isab::GuiProtEnums::CONNECT_GPS);
      sendAsynchronousRequestToNav2(&message);
   }
   m_needSendStartedCallback = true;
   if(!m_lbsStarted) {
      m_gpsPosReceived = false;
   }
   
   if(m_requestedHardware == 0) {//user doesn't want any lbs hardware active
      
      //we should callback on lbsStarted() here
      //note that the callback will arrive before this
      //function returns. potentially dangerous, so the callback function
      //should not run lots of code before returning.
      const ListenerCont& listeners = getListeners();
      for (ListenerCont::iterator it = listeners.begin(); 
           it != listeners.end(); ++it) {
         (*it)->startedLbs();
      }
      m_needSendStartedCallback = false;
      m_lbsStarted = true;
   }
   else {
      m_locationUtil->startRequesting(alsoUseGps);
   }
   //we're now considering ourselves to be using the hardware the user requested.
   // one could do some error handling here, if the platform supported reporting it.
   m_currentHardware = m_requestedHardware;
   
   //return with OK to say we are trying to start the lbs engine. callback with
   //result later, when we know more.
   return AsynchronousStatus(RequestID(RequestID::INVALID_REQUEST_ID),
                             OK, "", "");      
}

SynchronousStatus IPLocationInterfaceImpl::stopLbs()
{
   if (m_lbsStarted == true) {
      m_locationUtil->stopRequesting();
      m_lbsStarted = false;
      m_gpsPosReceived = false;
   } else {
      return SynchronousStatus(LBS_STOP_FAILED, "", "");
   }
   isab::GenericGuiMess message(isab::GuiProtEnums::DISCONNECT_GPS);
   sendAsynchronousRequestToNav2(&message);
   return SynchronousStatus(OK, "", "");
}

void IPLocationInterfaceImpl::lbsFailed() {
   // this is called from IPLocationUtil if there's a problem with
   // CoreLocation Framework
   coreprintln("lbsFailed()!");
   m_lbsStarted = false;
   stopLbs();
   //notify listeners that lbs startup failed
   const ListenerCont& listeners = getListeners();
   for (ListenerCont::iterator it = listeners.begin(); 
        it != listeners.end(); ++it) {
      (*it)->error(
         AsynchronousStatus(RequestID(RequestID::INVALID_REQUEST_ID),
                                      LBS_STARTUP_FAILED, "", "")
         );
   }
}
void IPLocationInterfaceImpl::lbsCallback(double latitude, double longitude,
                                          double altitude, double speed,
                                          double track,
                                          double horizontalAccuracy,
                                          double verticalAccuracy, double timestamp)
{
   coreprintln("lbsCallback, argument latitude=%f, longitude=%f, altitude=%f, speed=%f, track=%f, horizontalAccuracy=%f, verticalAccuracy=%f, timestamp=%f",
               latitude, longitude, altitude, speed,
               track, horizontalAccuracy, verticalAccuracy, timestamp);

   bool firstCallback = false;
   if (m_needSendStartedCallback) {
      // This is the first callback since the lbs was started,
      // we need to call startedLbs for the listeners.
      const ListenerCont& listeners = getListeners();
      for (ListenerCont::iterator it = listeners.begin(); 
           it != listeners.end(); ++it) {
         (*it)->startedLbs();
      }
      m_needSendStartedCallback = false;
      m_lbsStarted = true;
      firstCallback = true;
   }

   double pdop = 50;
   
   if ((firstCallback) || (m_currentHardware == 2) || (!m_gpsPosReceived && horizontalAccuracy > 100 && verticalAccuracy < 0)) {
      // Let's assume that this is a cellID callback if the
      // verticalAccuracy < 0 (ie altitude value is invalid) and
      // horizontalAccuracy > 100, and no better positions have previously
      // been received.
      // Also, if this is the very first callback, always treat it as a cellId
      
      //Instead of reporting lac, mmc, cellid etc to nav2, we just notify the
      //area update listeners of a coordinate that the apple framework gave us.
      //This is kind of like a hack though.
      
      const WGS84Coordinate position(latitude, longitude);
      AreaUpdateInformation info(position, altitude, 0, horizontalAccuracy, 
                                 0, 360);
      // Send to listeners (if we should use cellID)
      if(m_currentHardware & 0x02) {
         for (ListenerCont::const_iterator it = getListeners().begin();
              it != getListeners().end(); ++it ) {
            (*it)->areaUpdate(info);
         }
      }
      return;
   }
   else {
      if(!(m_currentHardware & 0x01)) { //user doesn't want gps
         //we shouldn't ever arrive here, but just in case
         return;
      }
      //this is a GPS update, since horizontalAccuracy < 100 meters or there
      // are altitude information available. This means there will not be any
      // more cellid positions reported to us, and therefor all consecutive
      // updates will be treated as gps positions.
      m_gpsPosReceived = true;
      
      // for iphone we need to translate a metric calculated error to
      // a pdop value. The below translation table is not the right way
      // to do it, but it might serve a good enough solution.
      //
      //nav2 seems to use these values for classification:
      // pdop <= 2 => QualityExcellent
      // pdop >  2 && gps has 3d => QualityDecent
      // pdop >  2 && gps has 2d => QualityPoor
      // pdop >  2 && gps has not 2d => QualityUseless
      // pdop > 49 => pdop too high for navigation
      // gps reports fatal error => QualityMissing (seems unlikely to happen,
      // ignore case)
      if      (horizontalAccuracy >= 0 && horizontalAccuracy <= 10) pdop = 1;
      else if (horizontalAccuracy > 10 && horizontalAccuracy <= 20) pdop = 2;
      else if (horizontalAccuracy > 20 && horizontalAccuracy <= 30) pdop = 3;
      else if (horizontalAccuracy > 30 && horizontalAccuracy <= 40) pdop = 4;
      else if (horizontalAccuracy > 40 && horizontalAccuracy <= 50) pdop = 5;
      else if (horizontalAccuracy > 50 && horizontalAccuracy <= 60) pdop = 6;
      else if (horizontalAccuracy > 60 && horizontalAccuracy <= 77) pdop = 7;
      else if (horizontalAccuracy > 77) {
         //if the accuracy is really bad, pick a pdop too high for navigation
         //note that this is the only check on iphone that throws a bad gps
         //position away.
         pdop = 50;
      }
   }
   
   if (horizontalAccuracy < 0) {
      //the lat and lon are invalid, so construct a pdop too large for nav2 to
      // consider. One might also return here.
      pdop=50;
   }
   
   if (isnan(latitude) ||
       isnan(longitude) ||
       isnan(speed)  ||
       isnan(track) ) {
      //Partial update - we should send dummy data
      coreprintln("Bad gps status received");
      //SendGpsBadStatusToUI();
      return;
   }

   const time_t time = (time_t)timestamp;
   tm* tm=gmtime(&time);
   //GPS timestamp functionality should be rewritten
   coreprintln("Time from gps: ", asctime(tm));
   double param, fractpart, intpart;
   bool has3D = 0;
   if(verticalAccuracy >= 0) 
      has3D = 1;
   bool north = true, east = true;
   double lat = latitude;
   if(lat < 0){
      north = false;
      lat = -lat;
   }
   double lon = longitude;
   if(lon < 0){
      east = false;
      lon = -lon;
   }

   double spd = speed;
   double hdg = track;
   //Vertical position uncertainty
   double pdopval = 10.0;
   if(!isnan(pdop))
      pdopval = pdop;

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

   snprintf(msgGPGSA, 128, "$GPGSA;M;%c;;;;;;;;;;;;;%f;;*\n\r", has3D ? '3' : '2', pdopval);
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
            altitude);

   for(i = 0; i < strlen(from); ++i){
      char* pos = strchr(msgGPGGA, int(from[i]));
      while(pos != NULL){
         *pos = to[i];
         pos = strchr(pos, from[i]);
      }
   }

   coreprintln("Writing %d bytes to btCh.", strlen(msgbuf));
   getBtChannel()->getChannel()->writeData((uint8*)(msgbuf), strlen(msgbuf));
   coreprintln("data written");

   coreprintln("--- Printing gps data ---");
   coreprintln("  * Raw nmea *");
   coreprintln("%s", msgbuf);
}

SynchronousStatus IPLocationInterfaceImpl::setLocationHardware(wf_uint8 hardware) {
   if(m_requestedHardware != hardware) { //the wanted hardware differs from what we're using
      m_requestedHardware = hardware;
      if(m_lbsStarted) {

         if( (m_currentHardware & 0x01) && !(m_requestedHardware & 0x01) ) { //gps is active right now
            //since we're shutting down gps at least, tell nav2 so
            isab::GenericGuiMess message(isab::GuiProtEnums::DISCONNECT_GPS);
            sendAsynchronousRequestToNav2(&message);
            m_gpsPosReceived = false;
         }
         
         //only do startLbs() once more if we've already started it, ie. dont
         // let setLocationHardware(...) start the LBS system by itself.
         startLbs();
      }
   }
   return SynchronousStatus(OK, "", "");
}

} //end of namespace WFAPI
