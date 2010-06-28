/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "BuildNmea.h"

#include <math.h>

namespace BUILD_NMEA
{

void correctLat(double& value)
{
   // The latitude of a point on Earth is the angle between the equatorial plane and the line 
   //passing through the point at right angle to the surface of the ellipsoid (see figure above). 
   // The Earth is divided into 90 degrees of latitude going both northwards and southwards from 
   // the equator (0°) to each pole (90° N and 90° S). (Northern positions are considered to be 
   // positive (+) and southern positions to be negative (-).)
   // (see http://home.online.no/~sigurdhu/Deg_formats.htm for more details)
      
   // is this value came in the decimal degrees units correct it.
   if (fabs(value) <= 90) {
      correctLatLon(value);
   }      
}

void correctLon(double& value)
{
   // The longitude of a point on Earth is the angle along the equator between the prime meridian
   // passing through Greenwich and the meridian passing through the point (see figure above). 
   // The Earth is divided into 180° degrees of longitude going both eastwards and westwards from 
   // the prime meridian (0°) until meeting approximately at the international date line (180° E = 180° W).
   // (Eastern positions are considered to be positive (+) and western positions to be negative (-).)
   // (see http://home.online.no/~sigurdhu/Deg_formats.htm for more details)

   // is this value came in the decimal degrees units correct it.
   if (fabs(value) <= 180) {
      correctLatLon(value);
   }
}
   
void correctLatLon(double& value)
{
   int deg = (int)value;
   double mins = (value - deg) * 60;
   value = deg * 100 + mins;				
}

void buildBadGPGSA(char* msgbuf)
{
   char mode = 0x20;
   char modeD = 0x20;
   char gpgsa[256] = {0};
   sprintf(gpgsa, "GPGSA,%c,%c,,,,,,,,,,,,,%f,,", 
           mode,
           modeD,
           50.0);		

   int checkSum = calcChecksum(gpgsa, strlen(gpgsa));
   sprintf(msgbuf, "$%s*%X\n\r", gpgsa, checkSum);	
}

void buildBadGPRMC(char* msgbuf)
{
   char gprmc[256] = {0};

   sprintf(gprmc,
           "GPRMC,%02u%02u%02u,%c,%09.4f,%c,%010.4f,%c,%.2f,%.2f,%02u%02u%02u,,",
           0, 0, 0,
           'V',
           0.0, 'N',
           0.0, 'E',
           0.0, 0.0, 
           0, 0, 0);

   int checkSum = calcChecksum(gprmc, strlen(gprmc));
   sprintf(msgbuf, "$%s*%X\n\r", gprmc, checkSum);
}

void buildGPGSA(GPS_POSITION* pos, char* msgbuf)
{
   char mode = 0x20;
   if (GPS_FIX_SELECTION_AUTO == pos->SelectionType)
      mode = 'A';
   else if (GPS_FIX_SELECTION_MANUAL == pos->SelectionType)
      mode = 'M';

   char modeD = 0x20;
   if (GPS_FIX_2D == pos->FixType)
      modeD = '2';
   else if (GPS_FIX_3D == pos->FixType)
      modeD = '3';

   char gpgsa[256] = {0};
   sprintf(gpgsa, "GPGSA,%c,%c,,,,,,,,,,,,,%.1f,%.1f,%.1f", 
           mode,
           modeD,
           pos->flPositionDilutionOfPrecision,
           pos->flHorizontalDilutionOfPrecision,
           pos->flVerticalDilutionOfPrecision);

   int checkSum = calcChecksum(gpgsa, strlen(gpgsa));
   sprintf(msgbuf, "$%s*%X\n\r", gpgsa, checkSum);
}

void buildGPRMC(GPS_POSITION* pos, char* msgbuf)
{
   // There are devices such are HTC P3300 that deliver latitude and 
   // longitude in decimal degree units, but NMEA protocol uses degree / decimal minutes units
   // thus check and correct if needed.
   // For example: 
   // Mitac Mio A700 & A701 give 5354,3688 value (degree / decimal minutes) that is  equal to 53 degree 54,37 mins
   // HTC P3300 gives 55,71805 value (decimal degree units) that is equal to 55 degree 43,09 mins      
   correctLat(pos->dblLatitude);
   correctLon(pos->dblLongitude);

   BOOL north = TRUE; 
   double lat = pos->dblLatitude;
   if (lat < 0) {
      north = FALSE;
      lat = -lat;
   }

   BOOL east = TRUE;
   double lon = pos->dblLongitude;
   if (lon < 0) {
      east = FALSE;
      lon =-lon;
   }

   float spd = pos->flSpeed;
   float hdg = pos->flHeading;

   SYSTEMTIME time = pos->stUTCTime;
   WORD year = time.wYear - ((time.wYear/100)*100);	
   WORD hour = time.wHour;
   WORD minute = time.wMinute;
   WORD second = time.wSecond;
   WORD day = time.wDay;
   WORD month = time.wMonth;

   char gprmc[256] = {0};
   sprintf(gprmc,
           "GPRMC,%02u%02u%02u,%c,%09.4f,%c,%010.4f,%c,%.2f,%.2f,%02u%02u%02u,,",
           hour, minute, second,
           'A',
           lat, north ? 'N' : 'S', 
           lon, east ? 'E' : 'W', 
           spd, hdg, 
           day, month, year);

   int checkSum = calcChecksum(gprmc, strlen(gprmc));
   sprintf(msgbuf, "$%s*%X\n\r", gprmc, checkSum);
}

void buildGPGGA(GPS_POSITION* pos, char* msgbuf)
{
   // There are devices such are HTC P3300 that deliver latitude and 
   // longitude in decimal degree units, but NMEA protocol uses degree / decimal minutes units
   // thus check and correct if needed.
   // For example: 
   // Mitac Mio A700 & A701 give 5354,3688 value (degree / decimal minutes) that is  equal to 53 degree 54,37 mins
   // HTC P3300 gives 55,71805 value (decimal degree units) that is equal to 55 degree 43,09 mins      
   correctLat(pos->dblLatitude);
   correctLon(pos->dblLongitude);

   BOOL north = TRUE; 
   double lat = pos->dblLatitude;
   if (lat < 0) {
      north = FALSE;
      lat = -lat;
   }

   BOOL east = TRUE;
   double lon = pos->dblLongitude;
   if (lon < 0) {
      east = FALSE;
      lon =-lon;
   }

   SYSTEMTIME time = pos->stUTCTime;
   WORD year = time.wYear - ((time.wYear/100)*100);	
   WORD hour = time.wHour;
   WORD minute = time.wMinute;
   WORD second = time.wSecond;
   WORD day = time.wDay;
   WORD month = time.wMonth;
      
   char gpgga[256] = {0};
   sprintf(gpgga,
           "GPGGA,%02u%02u%02u,%09.4f,%c,%010.4f,%c,1,04,1.0,%.1f,M,,,,0000",
           hour, minute, second,
           lat, north ? 'N' : 'S', 
           lon, east ? 'E' : 'W', 
           pos->flAltitudeWRTSeaLevel
      );

   int checkSum = calcChecksum(gpgga, strlen(gpgga));
   sprintf(msgbuf, "$%s*%X\n\r", gpgga, checkSum);
}

void buildNMEA(GPS_POSITION* pos, char* msgbuf)
{
   if (GPS_VALID_LATITUDE & pos->dwValidFields &&
       GPS_VALID_LONGITUDE & pos->dwValidFields &&
       GPS_VALID_SPEED & pos->dwValidFields &&
       GPS_VALID_HEADING & pos->dwValidFields)
   {
      char* msgGPGSA = msgbuf;	
      buildGPGSA(pos, msgGPGSA);
      char* msgGPRMC = msgbuf + strlen(msgGPGSA);
      buildGPRMC(pos, msgGPRMC);

      // build a GPS Fix message to provide the Nav2 with altitude data.
      if (GPS_VALID_ALTITUDE_WRT_SEA_LEVEL & pos->dwValidFields)
      {
         char* msgGPGGA = msgGPRMC + strlen(msgGPRMC);
         buildGPGGA(pos, msgGPGGA);
      }         
   } 
   else
   {
      char* msgGPGSA = msgbuf;	
      buildBadGPGSA(msgGPGSA);
      char* msgGPRMC = msgbuf + strlen(msgGPGSA);
      buildBadGPRMC(msgGPRMC);
   }	
}

int calcChecksum(char* msgbuf, int len)
{
   int checksum = 0;
   char* pointer = msgbuf;

   for(int i = 0; i < len; ++i) {
      checksum ^= *(pointer + i);
   }	

   return checksum;
}

}
