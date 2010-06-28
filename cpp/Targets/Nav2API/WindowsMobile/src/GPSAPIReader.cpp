/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "GPSAPIReader.h"

#include "PositionListener.h"
#include "BuildNmea.h"
#include "CoreDebugPrint.h"
#include <math.h>

const DWORD READ_TIMEOUT = 10000;
#ifdef GPSID_FIX
#  pragma message("GPSID_FIX Enabled")
#endif

namespace WFAPI
{

CGPSAPIReader::CGPSAPIReader(PositionListener& listener) :
   m_listener(listener),
   m_hNewLocationData(NULL),
   m_hDeviceStateChange(NULL),
   m_hStopEvent(NULL),
   m_hDevice(NULL),
   m_hThread(NULL)
{
   InitializeCriticalSection(&m_cs);

   m_hNewLocationData = CreateEvent(NULL, false, false, NULL);
   ASSERT(NULL != m_hNewLocationData);
   m_hDeviceStateChange = CreateEvent(NULL, false, false, NULL);
   ASSERT(NULL != m_hDeviceStateChange);
   m_hStopEvent = CreateEvent(NULL, true, false, NULL);
   ASSERT(NULL != m_hStopEvent);
}

CGPSAPIReader::~CGPSAPIReader()
{
   if (m_hNewLocationData) {
      CloseHandle(m_hNewLocationData);
      m_hNewLocationData = NULL;
   }

   if (m_hDeviceStateChange) {
      CloseHandle(m_hDeviceStateChange);
      m_hDeviceStateChange = NULL;
   }

   if (m_hDevice) {
      close();
   }

   if (m_hStopEvent) {
      CloseHandle(m_hStopEvent);
      m_hStopEvent = NULL;
   }

   DeleteCriticalSection(&m_cs);
}

BOOL CGPSAPIReader::open()
{
   // open gps device.
   m_hDevice = GPSOpenDevice(m_hNewLocationData, m_hDeviceStateChange, NULL, 0);
   ASSERT(NULL != m_hDevice);

   // create reading thread.
   if (m_hDevice) {
      DWORD dwThread = 0;
      // m_hThread = CreateThread(
      //    NULL, 0, SAVE_THREAD_START_ROUTINE_T<threadProc>,
      //    (LPVOID)this, 0, &dwThread);

      ResetEvent(m_hStopEvent);
      m_hThread = CreateThread(
         NULL, 0, threadProc,
         (LPVOID)this, 0, &dwThread);

      ASSERT(m_hThread != NULL);
      
      coreprintln("threadID 0x%08x, threadProc 0x%08x\n",
                  dwThread, threadProc);
   }

   return (NULL != m_hDevice && NULL != m_hThread);
}

BOOL CGPSAPIReader::close()
{
   // clear previous stored data.
   
   if (m_hDevice) {
      // terminate reading thread.
      SetEvent(m_hStopEvent);

      if (m_hThread) {
         int wait_for = 3 * READ_TIMEOUT;
         if (WaitForSingleObject(m_hThread, wait_for) == WAIT_TIMEOUT) {
            coreprintln("forced to kill thread after %s ms of waiting it for termination", wait_for);
            TerminateThread(m_hThread, 0);
         }
         CloseHandle(m_hThread);
         m_hThread = NULL;
      }
                
      // close the device.
      DWORD dwResult = GPSCloseDevice(m_hDevice);
      if (ERROR_SUCCESS == dwResult) {
         m_hDevice = NULL;
         return true;
      }
   }
	
   return false;
}

DWORD CGPSAPIReader::read()
{
   const DWORD DRIVER_READ_TIMEOUT = 3000;
   const DWORD MAXIMUM_AGE = 5000; 

   GPS_POSITION gpsPosition = {0};
   gpsPosition.dwVersion = GPS_VERSION_1;
   gpsPosition.dwSize = sizeof(gpsPosition);
#ifdef GPSID_FIX   
   BYTE gpsPositionRaw[376] = {0};
   GPS_POSITION* pGpsLocation = (GPS_POSITION*)gpsPositionRaw;
      pGpsLocation->dwVersion = GPS_VERSION_1;
         pGpsLocation->dwSize = sizeof(gpsPositionRaw);
#endif   

   DWORD dwRet = 0;
   while (WAIT_TIMEOUT == WaitForSingleObject(m_hStopEvent, 0)) {
      
      if (WAIT_OBJECT_0 == (dwRet = WaitForSingleObject(
                               m_hNewLocationData, DRIVER_READ_TIMEOUT))) {
         
         if (ERROR_SUCCESS == (dwRet = GPSGetPosition(
                                  m_hDevice, &gpsPosition, MAXIMUM_AGE, 0))) {
            processPosition(&gpsPosition);
         }
#ifdef GPSID_FIX         
         else if (ERROR_INVALID_PARAMETER == dwRet)
         {
            coreprintln("Look like a GPSID_FIX client");
            if (ERROR_SUCCESS == (dwRet = m_gps.GPSGetPosition(
                                     m_hDevice, pGpsLocation, MAXIMUM_AGE, 0)))
            {
               processPosition(pGpsLocation);
            }
            else
            {
               coreprintln("GPSGetPosition ret: 0x%lx", dwRet);
            }            
         }
#endif
         else {
            coreprintln("GPSGetPosition ret: 0x%lx", dwRet);
         }
      } else if(WAIT_TIMEOUT == dwRet) {
         coreprintln("NewLocationData wait TIMEOUT");
         
         // still no data. process the bad gps output.
         gpsPosition.dwValidFields = 0;
         processPosition(&gpsPosition);
      } else {
         coreprintln("NewLocationData wait FAILED: 0x%lx\n", dwRet);
      }
   }	

   return 0;
}

DWORD WINAPI CGPSAPIReader::threadProc(LPVOID lpParam)
{	
   CGPSAPIReader* reader = static_cast< CGPSAPIReader* >(lpParam);
   return reader->read();   
}

void CGPSAPIReader::processPosition(GPS_POSITION* pGpsLocation)
{
   EnterCriticalSection(&m_cs);

   char gpsData[500] = {0};
   BUILD_NMEA::buildNMEA(pGpsLocation, gpsData);

   coreprintln("%d bytes readed: %s\n", strlen(gpsData), gpsData);

   // m_btChannel.getChannel()->writeData(
   //    reinterpret_cast<const uint8*>(gpsData), strlen(gpsData));
   m_listener.positionInfoUpdated(gpsData);
   
   // // store received data to inner buffer.
   // elem_t* buffer = (elem_t*)gpsData;
   // m_buffer.insert(m_buffer.end(), buffer, buffer + strlen(gpsData));

   LeaveCriticalSection(&m_cs);

   // data exist.
}

}
