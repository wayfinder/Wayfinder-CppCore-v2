/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _GPS_API_READER_H_
#define _GPS_API_READER_H_

#include <windows.h>
#include <gpsapi.h>
#include <vector>

namespace WFAPI {

class PositionListener;

/**
 * This class handles the gps connection and reading of gps positions
 * for windows mobile. The class handles open and closing of the connection
 * to the gps module and reads positions every second from the gps module.
 * We parse the data to nmea strings and passes it to is PositionListener.
 */
class CGPSAPIReader
{

public:

   /**
    * Constructor.
    *
    * @param listener The listener which should receive callbacks when
    *                 getting new positions from the gps.
    */
   CGPSAPIReader(PositionListener& listener);

   /**
    * Destructor.
    */
   virtual ~CGPSAPIReader();

   /**
    * Opens the gps connection, starts a thread that fetches gps positions
    * every second.
    *
    * @return TRUE if successful otherwise FALSE.
    */
   BOOL open();	

   /**
    * Closes the gps connection, stops the thread that fetches gps positions.
    *
    * @return TRUE if successful otherwise FALSE.
    */
   BOOL close();

private:

   /**
    * Internal function that reads positions from gps every second, gets
    * called by threadProc which is the callback function for the thread.
    *
    * @return Returns 0 when stop has been called.
    */
   DWORD read();

   /**
    * Gets called by read when a new position has been fetched from the gps.
    * This function parses the data to nmea string and calls its listener.
    *
    * @param pGpsLocation The gps location that has been received and that
    *                     should be parsed to nmea data.
    */
   void processPosition(GPS_POSITION* pGpsLocation);

   /**
    * Callback function when starting the thread that fetches gps positions.
    * Calls read, and read will then fetch data every second until stop
    * has been called.
    *
    * @param lpParam Contains an object to this.
    * @return Will return 0 when stopped.
    */
   static DWORD WINAPI threadProc(LPVOID lpParam);

private:

   /// The listener that should receive positios every second.
   PositionListener& m_listener;

   /// Used when calling WaitForSingleObject.
   HANDLE m_hNewLocationData;

   /// Used as parameter when calling GPSOpenDevice.
   HANDLE m_hDeviceStateChange;

   /// In read function we read every second untill this event set set.
   HANDLE m_hStopEvent;

   /// Handle to the gps connection.
   HANDLE m_hDevice;

   /// Critical section that handles reading and parsing of the gps data.
   CRITICAL_SECTION m_cs;

   /// Handle to the thread that reads gps data.
   HANDLE m_hThread;
};

} // End namespace WFAPI

#endif //__GPS_API_READER_H__
