/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "LocationInterface.h"
#include "LocationInterfaceImpl.h"
#include "AreaUpdateInformation.h"
#include "LocationUpdateInformation.h"
#include "GeocodingInterfaceImpl.h"
#include "CoreFunctionPrologue.h"
#include "NMEALocationInterfaceImpl.h"
#include <string>
#include <vector>

#if defined NAV2_CLIENT_NOKIAMAEMO
# include <NokiaMaemoLocationInterfaceImpl.h>
#elif defined NAV2_CLIENT_I386
# include <LinuxLocationInterfaceImpl.h>
#elif defined NAV2_CLIENT_SERIES60_V3 || NAV2_CLIENT_SERIES60_V5
# include <S60LocationInterfaceImpl.h>
#elif defined _WIN32_WCE
# include <WMLocationInterfaceImpl.h>
#elif defined IPHONE
# include <IPLocationInterfaceImpl.h>
#else
   // Include location interface for symbian
#endif

#define CORE_LOGGING_MODULE_NAME "LocationInterface"
//#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE

#include "CoreDebugPrint.h"

namespace WFAPI {

struct LocationInterface::impl {
   impl(Nav2APIImpl* nav2APIImpl) : m_locImpl(nav2APIImpl),
                                    m_geoImpl(nav2APIImpl){}


#if defined NAV2_CLIENT_NOKIAMAEMO
   NokiaMaemoLocationInterfaceImpl m_locImpl;
#elif defined NAV2_CLIENT_I386 
   LinuxLocationInterfaceImpl m_locImpl;
   // NMEALocationInterfaceImpl m_locImpl;
#elif defined NAV2_CLIENT_SERIES60_V3 || defined NAV2_CLIENT_SERIES60_V5
   S60LocationInterfaceImpl m_locImpl;
#elif defined _WIN32_WCE
   WMLocationInterfaceImpl m_locImpl;
#elif defined IPHONE
   IPLocationInterfaceImpl m_locImpl;
#else
   // Implement location interface for symbian and declare here
#endif

   GeocodingInterfaceImpl m_geoImpl;
};

LocationInterface::LocationInterface(Nav2APIImpl* nav2APIImpl)
{
   m_impl = new impl(nav2APIImpl);
}

LocationInterface::~LocationInterface()
{
   delete m_impl;
}

void
LocationInterface::addLocationListener(LocationListener* listener)
{
   CORE_FUNCTION_PROLOGUE();

   m_impl->m_locImpl.addLocationListener(listener);
}

void
LocationInterface::removeLocationListener(LocationListener* listener)
{
   CORE_FUNCTION_PROLOGUE();

   m_impl->m_locImpl.removeLocationListener(listener);
}

SynchronousStatus
LocationInterface::lbsStarted()
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_locImpl.lbsStarted();
}


AsynchronousStatus
LocationInterface::startLbs()
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_locImpl.startLbs();
}

SynchronousStatus
LocationInterface::stopLbs()
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_locImpl.stopLbs();
}

SynchronousStatus
LocationInterface::setLocationHardware(wf_uint8 hardware) {
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_locImpl.setLocationHardware(hardware);
}

LocationInterfaceImpl*
LocationInterface::getImpl()
{
   return &m_impl->m_locImpl;
}

AsynchronousStatus
LocationInterface::requestReverseGeocoding(
   const AreaUpdateInformation& locationArea)
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_geoImpl.requestReverseGeocoding(
      locationArea.getPosition(),
      GeocodingInformation::CITY);
}

AsynchronousStatus
LocationInterface::requestReverseGeocoding(
   const LocationUpdateInformation& location)
{
   CORE_FUNCTION_PROLOGUE();

   return m_impl->m_geoImpl.requestReverseGeocoding(
      location.getPosition(),
      GeocodingInformation::ADDRESS);
}

void
LocationInterface::addGeocodingListener(GeocodingListener* listener)
{
   CORE_FUNCTION_PROLOGUE();

   m_impl->m_geoImpl.addGeocodingListener(listener);
}

void
LocationInterface::removeGeocodingListener(GeocodingListener* listener)
{
   CORE_FUNCTION_PROLOGUE();

   m_impl->m_geoImpl.removeGeocodingListener(listener);
}

#define MAX_LINE_SIZE 512

void
LocationInterface::replayNMEAData(const char* fileName, int begin, int end)
{
   CORE_FUNCTION_PROLOGUE();

   coreprintln("replayNMEAData(%s)", fileName);
   
   FILE * pFile;
   char tmpBuf[MAX_LINE_SIZE];

   pFile = fopen (fileName, "r");
   if (pFile == NULL) {
      coreprintln_error ("Error opening file");
      return;
   } else {
      std::string nmeaStr;
      
      // Read first $GPGSV line

      std::vector<std::string> messages;
      
      std::string curMessage;

      int i = 0;
      
      while(fgets (tmpBuf , MAX_LINE_SIZE , pFile)) {
      
         curMessage += tmpBuf;

         i = (i + 1) % 7;
         
         if( i == 0) {
            // coreprintln("Read NMEA sentence:\n%s", curMessage.c_str());
            messages.push_back(curMessage);
            curMessage = "";
         }
      }

      if(end == -1) {
         end = messages.size();
      }

      messages = std::vector<std::string>(
         messages.begin() + begin,
         messages.begin() + end);
      
      m_impl->m_locImpl.replayNMEAData(messages);
      fclose (pFile);
   }

   
   // std::vector<std::string> messages;

   // std::string bigData(data);
   
}

} // End namespace WFAPI

