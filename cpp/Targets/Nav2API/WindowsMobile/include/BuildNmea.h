/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef NMEA_PARSER_H
#define NMEA_PARSER_H

#include <windows.h>
#include <gpsapi.h>

namespace BUILD_NMEA
{

/**
 * Calculates a checksum of the msgbuf.
 *
 * @param msgbuf The buffer to calculate the checksum for.
 * @param len The length of the buffer.
 */
int calcChecksum(char* msgbuf, int len);

/**
 * Puts together a GPGSA string of the msgbuf.
 *
 * @param msgbuf The buffer containing the data.
 */
void buildBadGPGSA(char* msgbuf);

/**
 * Puts together a GPRMC string of the msgbuf.
 *
 * @param msgbuf The buffer containing the data.
 */
void buildBadGPRMC(char* msgbuf);

/**
 * Puts together a GPGSA string of the msgbuf.
 *
 * @param pos The struct containing the gps data. 
 * @param msgbuf The buffer containing the data.
 */
void buildGPGSA(GPS_POSITION* pos, char* msgbuf);

/**
 * Puts together a GPRMC string of the msgbuf.
 *
 * @param pos The struct containing the gps data. 
 * @param msgbuf The buffer containing the data.
 */
void buildGPRMC(GPS_POSITION* pos, char* msgbuf);

/**
 * Puts together a GPGGA string of the msgbuf.
 *
 * @param pos The struct containing the gps data. 
 * @param msgbuf The buffer containing the data.
 */
void buildGPGGA(GPS_POSITION* pos, char* msgbuf);

/**
 * Corrects the lat.
 *
 * @param value The lat to be corrected.
 */
void correctLat(double& value);

/**
 * Corrects the lon
 *
 * @param value The lon to be corrected.
 */
void correctLon(double& value);

/**
 * The entry point when parsing a GPS_POSITION to a nmea string.
 *
 * @param pos The structure containing the gps data retreived from the gps
 *        module.
 * @param msgbuf This will containg the parsed nmea string when done.
 */
void buildNMEA(GPS_POSITION* pos, char* msgbuf);

/**
 * Corrects lat and lon.
 *
 * @param value The value to correct.
 */
void correctLatLon(double& value);
} // End namespace NMEA_PARSER

#endif // End define NMEA_PARSER_H
