/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "WindowsMobileNav2APIImpl.h"

// Includes needed to obtain IMEI and IMSI.
#include <tapi.h>
#include <extapi.h>

#define CORE_LOGGING_MODULE_NAME "WindowsMobileNav2APIImpl"
#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE

#include "CoreDebugPrint.h"


namespace WFAPI {

WindowsMobileNav2APIImpl::WindowsMobileNav2APIImpl(Nav2APIImpl* nav2APIImpl)
{
    
}

WindowsMobileNav2APIImpl::~WindowsMobileNav2APIImpl()
{
    
}

AsynchronousStatus
WindowsMobileNav2APIImpl::start(Nav2StatusListener* statusListener, 
                                StartupData* startupData,
                                const WFString& clientType,
                                bool httpProto,
                                const WFString& httpRequestString,
                                const ProgramVersion& programVersion)
{
   return AsynchronousStatus(RequestID(0), 0, "", "");
}

SynchronousStatus
WindowsMobileNav2APIImpl::getUserIdentificationData(WFString& imeiStr, WFString& imsiStr)
{
   // First: Initialize the Core's use of TAPI.
   HLINEAPP lphlineApp = NULL;

   DWORD numOfDev = 0;
   DWORD apiVersion = TAPI_API_HIGH_VERSION;

   HINSTANCE hInstance = ::GetModuleHandle(NULL);

   LINEINITIALIZEEXPARAMS liExParams;
   liExParams.dwOptions = LINEINITIALIZEEXOPTION_USEEVENT;
   liExParams.dwTotalSize = sizeof(liExParams);
   
   
   DWORD result = lineInitializeEx(&lphlineApp,
                                   hInstance,
                                   0,
                                   NULL,
                                   &numOfDev,
                                   &apiVersion,
                                   &liExParams);

   if(result) {
      coreprintln("lineInitializeEx failed, reason: %#x", result);

      // Not possible to fetch the user identification data.
      return SynchronousStatus(ERROR_RETREIVING_IMEI_IMSI, "", "");
   }
   
   // Second: Negotiate API version.

   DWORD deviceID = 0;

   DWORD lpNegotiatedAPIVersion = 0;
   
   //Will contain some negotiated stuff.
   LINEEXTENSIONID lineExtensionID;
   
   result = lineNegotiateAPIVersion(lphlineApp,
                                    deviceID,
                                    TAPI_API_LOW_VERSION,
                                    TAPI_API_HIGH_VERSION,
                                    &lpNegotiatedAPIVersion,
                                    &lineExtensionID);

   if(result) {
      coreprintln("lineNegotiateAPIVersion failed, reason: %#x", result);

      // Not possible to fetch the user identification data.
      lineShutdown(lphlineApp);
      return SynchronousStatus(ERROR_RETREIVING_IMEI_IMSI, "", "");
   }
   

   // Third: Open the line. If trying to open the line after negotiating extended
   // API, then it fails.

   HLINE hline = NULL;
   
   result = lineOpen(lphlineApp,
                     deviceID,
                     &hline,
                     lpNegotiatedAPIVersion,
                     0,
                     0,
                     LINECALLPRIVILEGE_NONE,
                     0,
                     0);

   if(result) {
      coreprintln("lineOpen failed, reason: %#x", result);

      // Not possible to fetch the user identification data.
      lineShutdown(lphlineApp);
      return SynchronousStatus(ERROR_RETREIVING_IMEI_IMSI, "", "");
   } 
   
   //Fourth: Negotiate extended version. Not needed but needs to be done!

   DWORD lpNegotiatedExtVersion = 0;

   result = lineNegotiateExtVersion(lphlineApp,
                                    deviceID,
                                    lpNegotiatedAPIVersion,
                                    EXT_API_LOW_VERSION,
                                    EXT_API_HIGH_VERSION,
                                    &lpNegotiatedAPIVersion);

   if(result) {
      coreprintln("lineNegotiateExtVersion failed, reason: %#x", result);
      // Error when calling lineNegotiateExtVersion.
      // Not possible to fetch the user identification data.
      lineClose(hline);
      lineShutdown(lphlineApp);
      return SynchronousStatus(ERROR_RETREIVING_IMEI_IMSI, "", "");
   }
   
   // Fifth: Finally time to get the data we want!

   LPLINEGENERALINFO lpLineInfo = NULL;
   size_t dwNeeded = sizeof ( LINEGENERALINFO );

   // If we only call lineGetGeneralInfo once,
   // lpLineInfo will not contain any data.
   for( int i = 0; i < 2; ++i )
   {
      lpLineInfo = (LPLINEGENERALINFO) calloc( dwNeeded, 1 );
      lpLineInfo->dwTotalSize = dwNeeded;   
   
      // Get general info (such as IMEI, IMSI).
      result = lineGetGeneralInfo(hline, lpLineInfo); 
      if (result) {
         coreprintln("Error when calling lineGetGeneralInfo, reason: %#x",
                     result);
         if ( lpLineInfo ) {
            free( lpLineInfo );
            lpLineInfo = NULL;
         }

         lineClose(hline); 
         lineShutdown(lphlineApp);
         return SynchronousStatus(ERROR_RETREIVING_IMEI_IMSI, "", "");
      }
   
      // Size is right, break the loop.
      if ( lpLineInfo->dwNeededSize <= lpLineInfo->dwTotalSize )
         break;

      // Establish the right size and repeat the attempt.
      dwNeeded = lpLineInfo->dwNeededSize;
      free( lpLineInfo );
      lpLineInfo = NULL;
   }

   if(lpLineInfo && lpLineInfo->dwSubscriberNumberSize > 0 &&
      lpLineInfo->dwSerialNumberSize > 0) {

      // Store imei in imeiStr
      WCHAR imeiInternal[50];
      int i = 0;
      for (i = 0; i < (signed)(lpLineInfo->dwSerialNumberSize/2); i++){
         imeiInternal[i] =
            *((unsigned short *)(lpLineInfo) + i + lpLineInfo->dwSerialNumberOffset/2);
      }
      imeiInternal[i] = NULL;

      char buf[30];
      WideCharToMultiByte(GetACP(), NULL, imeiInternal, -1, buf, 50, NULL,
                          NULL);
      imeiStr = WFString(buf);
            
      // Store Imsi in imsiStr
      WCHAR imsiInternal[50];
      for (i = 0; i < (signed)(lpLineInfo->dwSubscriberNumberSize/2); i++){
         imsiInternal[i] =
            *((unsigned short *)(lpLineInfo) + i + lpLineInfo->dwSubscriberNumberOffset/2);
      }

      imsiInternal[i] = NULL;
      WideCharToMultiByte(GetACP(), NULL, imsiInternal, -1, buf, 50, NULL,
                          NULL);
      imsiStr = WFString(buf);
            
            
   } else {
      // lpLineInfo did not contain any data.
      // Close and shut down the lines.
      lineClose(hline);
      lineShutdown(lphlineApp);
      return SynchronousStatus(ERROR_RETREIVING_IMEI_IMSI, "", "");
   }

   // Close and shut down the lines.
   lineClose(hline);
   lineShutdown(lphlineApp);

   return SynchronousStatus(OK, "", "");

}

} // End of namespace WFAPI
