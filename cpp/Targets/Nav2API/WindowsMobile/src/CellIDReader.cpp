/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#define CORE_LOGGING_MODULE_NAME "CELLIDREADER"
// #define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE

#include "CellIDReader.h"
#include "CoreDebugPrint.h"
#error "You need to download a Windows Mobile Platform Builder ril.h and put it in cpp/Targets/Nav2API/WindowsMobile/include/"
#include "ril.h"
#include "StatusCode.h"

using namespace WFAPI;

/**
 * The following is a typedef for the function that is used to initialize
 * the Radio Interface Layer. We need this to fetch a function pointer
 * to the real initialization function at runtime.
 * 
 * @param dwIndex The index of the RIL port to use (e.g., 1 for RIL1:).
 * @param pfnResult Function result callback.
 * @param pfnNotify notification callback.
 * @param dwNotificationClasses Classes of notifications to be enabled for
 *                              this client.
 * @param dwParam Custom parameter passed to result and notification
 *                callbacks
 * @param lphRil The return value, a handle to the RIL instance.
 */
typedef HRESULT (*RILInitialize) (DWORD dwIndex, 
                                  RILRESULTCALLBACK pfnResult, 
                                  RILNOTIFYCALLBACK pfnNotify, 
                                  DWORD dwNotificationClasses, 
                                  DWORD dwParam, 
                                  HRIL* lphRil );
typedef HRESULT (*RILDeinitialize) (HRIL hRil);

/**
 * The following is a typedef for the function that is used to
 * get cell tower information from the Radio Interface Layer.
 * We need this to create a function pointer to call.
 * 
 * @param hRil A handle to the RIL instance returned by RILInitialize
 */
typedef HRESULT (*RILGetCellTowerInfo) (HRIL hRil);




/**
 * This is the callback function that will be called with the cell id
 * information.
 */
void CALLBACK ResultCallback(DWORD dwCode,
                             HRESULT hrCmdID,
                             const void *pData,
                             DWORD cbData,
                             DWORD dwParam)
{
   switch (dwCode) {
   case RIL_RESULT_OK:
   {
      coreprintln("Result ok!");
      WFAPI::CellIDReader* reader =
         reinterpret_cast<WFAPI::CellIDReader*>(dwParam);

      const RILCELLTOWERINFO* cellInfo =
         static_cast<const RILCELLTOWERINFO*>(pData);
      char signalStrength[20];
      sprintf(signalStrength, "%d", cellInfo->dwRxQuality);

      char countryCode[10];
      sprintf(countryCode, "%d", cellInfo->dwMobileCountryCode);

      char networkCode[10];
      sprintf(networkCode, "%d", cellInfo->dwMobileNetworkCode);

      char locationAreaCode[10];
      sprintf(locationAreaCode, "%d", cellInfo->dwLocationAreaCode);

      char cellID[10];
      sprintf(cellID, "%d", cellInfo->dwCellID);
      
      reader->getCellIDListener().cellIDInfoUpdated("GSM",
                                                    signalStrength,
                                                    countryCode,
                                                    networkCode,
                                                    locationAreaCode,
                                                    cellID);
   }
   break;

   case RIL_RESULT_NOCARRIER:
      coreprintln("No carried detected.");
      break;
   case RIL_RESULT_ERROR:
      coreprintln("Error when fetching network information.");
      break;
   case RIL_RESULT_NODIALTONE:
      coreprintln("No dial tone was detected.");
      break;
   case RIL_RESULT_BUSY:
      coreprintln("The line was busy.");
      break;
   case RIL_RESULT_NOANSWER:
      coreprintln("Got no answer.");
      break;
   case RIL_RESULT_CALLABORTED:
      coreprintln("Call was aborted");
      break;
   default:
      coreprintln("Undefined error from RIL!");
   }
   
   
}

CellIDReader::CellIDReader(CellIDListener& listener)
   : m_hRil(NULL),
     m_rilDll(NULL),
     m_listener(listener)
{
   initializeRIL();
}


CellIDListener&
CellIDReader::getCellIDListener()
{
   return m_listener;
}

bool CellIDReader::requestCellID()
{
   RILGetCellTowerInfo hrilTowerInfo = NULL;

   hrilTowerInfo = (RILGetCellTowerInfo)GetProcAddress(
      m_rilDll,TEXT("RIL_GetCellTowerInfo"));

   if (hrilTowerInfo == NULL)  {
      // Failed to get the function pointer to GetCellTowerInfo
      return false;
   }

   (*hrilTowerInfo)(m_hRil);
   
   return true;
}

CellIDReader::~CellIDReader()
{
   if (m_hRil != NULL) {
      RILDeinitialize deInit = NULL;
      deInit =
         (RILDeinitialize)GetProcAddress(m_rilDll, TEXT("RIL_Deinitialize"));
      
      if (NULL != deInit) {
         (*deInit)(m_hRil);
      }    
   }
   
   if (m_rilDll != NULL)  {
      FreeLibrary(m_rilDll);
   }

}

bool CellIDReader::initializeRIL()
{
   if (m_hRil != NULL){
      // Already initialized.
      return true;
   }
   
   m_rilDll = NULL;
   m_rilDll = LoadLibrary(TEXT("ril.dll"));

   if(m_rilDll == NULL) {
      // Failed to load the dynamic library.
      return false;
   }
   
   RILInitialize hrilInit = NULL;
   
   hrilInit = 
      (RILInitialize)GetProcAddress(m_rilDll,TEXT("RIL_Initialize"));
   
   if (hrilInit == NULL){
      // Failed to get the adress for the initialization function.
      return false;
   }
   
   (*hrilInit)(1, ResultCallback , NULL, NULL,
               reinterpret_cast<DWORD>(this), &m_hRil);
   
   
   return true;
}


