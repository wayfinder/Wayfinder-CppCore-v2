/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#define CORE_LOGGING_MODULE_NAME "WMLocIntImpl"
// #define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE

#include "WMLocationInterfaceImpl.h"
#include "LocationStatusCode.h"
#include "LocationListener.h"

#include "Nav2APIChannel.h"
#include "GPSAPIReader.h"
#include "CellIDReader.h"
#include "GuiProtMess.h"
#include "CoreDebugPrint.h"

namespace WFAPI {

WMLocationInterfaceImpl::WMLocationInterfaceImpl(Nav2APIImpl* api)
   : LocationInterfaceImpl(api),
     m_currentHardware(WF_MAX_UINT8),
     m_lbsStarted(false)
{
   m_gpsApiReader = new CGPSAPIReader(*this);
   m_cellIDReader = new CellIDReader(*this);
}

WMLocationInterfaceImpl::~WMLocationInterfaceImpl()
{
   if (m_lbsStarted) {
      stopLbs();
   }
   delete m_gpsApiReader;
   delete m_cellIDReader;
}

SynchronousStatus
WMLocationInterfaceImpl::lbsStarted()
{
   wf_int statusCode = LBS_STATUS_STARTED;
   if (!m_lbsStarted) {
      statusCode = LBS_STATUS_NOT_STARTED;
   }
   return SynchronousStatus(statusCode, "", "");
}

AsynchronousStatus
WMLocationInterfaceImpl::startLbs()
{
   if(m_currentHardware & 0x02) { //user wants cellID
      m_cellIDReader->requestCellID();
      coreprintln("requestCellID() in WMLocationInterfaceImpl::startLbs()");
   }
   if(m_currentHardware & 0x01) { //user wants gps
      if (m_gpsApiReader->open() == TRUE) {
         coreprintln("gps connected in startLbs()");
         // Notify Nav2 that the GPS is connected.
         connectGpsNav2(true);
      }
      else {
         return AsynchronousStatus(RequestID(RequestID::INVALID_REQUEST_ID),
                                   LBS_STARTUP_FAILED, "", "");
      }
   }
   m_lbsStarted = true;
   // This call will actually indicate that the lbs system has been started,
   // the cell id response might arrive without the listeners being
   // notified in the startedLbs callback.
   // We do this callback inside this function until proper thread
   // synchronization has been implemented in this locationInterfaceImpl.
   notifyListenersStarted();

   return AsynchronousStatus(RequestID(RequestID::INVALID_REQUEST_ID),
                             OK, "", "");
}

SynchronousStatus
WMLocationInterfaceImpl::stopLbs()
{
   if (m_lbsStarted == true) {
      if( m_currentHardware & 0x01 ) { //gps is presumably started
         if(m_gpsApiReader->close() == TRUE) {
            connectGpsNav2(false);
         }
         else {
            return SynchronousStatus(LBS_STOP_FAILED, "", "");
         }
      }
      //one might stop some CellID stuff here if needed.
      
      m_lbsStarted = false;
      
      return SynchronousStatus(OK, "", "");
   } else {
      return SynchronousStatus(LBS_STOP_FAILED, "", "");
   }
}

void
WMLocationInterfaceImpl::positionInfoUpdated(const char* nmeaString)
{
   getBtChannel()->getChannel()->writeData(
      reinterpret_cast<const uint8*>(nmeaString), strlen(nmeaString));
}

void
WMLocationInterfaceImpl::cellIDInfoUpdated(
   const WFString& networkType, 
   const WFString& signalStrength,
   const WFString& currentMCC,
   const WFString& currentMNC,
   const WFString& currentLAC,
   const WFString& cellID)
{
   
   sendCellIDRequest(
      LocationInterfaceImpl::TGPPCellIDRequestData(networkType,
                                                   signalStrength,
                                                   currentMCC,
                                                   currentMNC,
                                                   currentLAC,
                                                   cellID));
   // coreprintln("Sending CELLID REQUEST!-------------------");
   // coreprintln(networkType.c_str());
   // coreprintln(signalStrength.c_str());
   // coreprintln(currentMCC.c_str());
   // coreprintln(currentMNC.c_str());
   // coreprintln(currentLAC.c_str());
   // coreprintln(cellID.c_str());
}

void WMLocationInterfaceImpl::notifyListenersStarted() {
   const ListenerCont& listeners = getListeners();
   for (ListenerCont::const_iterator it = listeners.begin(); 
        it != listeners.end(); ++it) {
      (*it)->startedLbs();
   }   
}

void WMLocationInterfaceImpl::connectGpsNav2(bool connect) {
   if(connect == true) {
      isab::GenericGuiMess message(isab::GuiProtEnums::CONNECT_GPS);
      sendAsynchronousRequestToNav2(&message);
   }
   else {
      isab::GenericGuiMess message(isab::GuiProtEnums::DISCONNECT_GPS);
      sendAsynchronousRequestToNav2(&message);
   }
}

SynchronousStatus
WMLocationInterfaceImpl::setLocationHardware(wf_uint8 hardware) {
   if(m_currentHardware != hardware) { //the wanted hardware differs from what we're using
      if(m_lbsStarted) {
         if( (m_currentHardware & 0x01) && !(hardware & 0x01) ) {
            //gps is active right now, but user wants to shut it down
            //tell nav2 so:
            connectGpsNav2(false);
            if (m_gpsApiReader->close() != TRUE) {
               coreprintln("gps failed to close in setLocationHardware()");
               return SynchronousStatus(GENERAL_ERROR, "", "");               
            }
            coreprintln("gps closed in setLocationHardware()");
         }
         if( !(m_currentHardware & 0x01) && (hardware & 0x01) ) {
            //gps is not active right now, but user wants to start it
            if (m_gpsApiReader->open() != TRUE) {
               coreprintln("gps failed to open in setLocationHardware()");
               return SynchronousStatus(GENERAL_ERROR, "", "");               
            }
            //tell nav2 so:
            connectGpsNav2(true);
            coreprintln("gps open in setLocationHardware()");
         }
         if( !(m_currentHardware & 0x02) && (hardware & 0x02) ) {
            //this is not ideal, since cellID should rather be checked
            //periodically instead, and reported to user when location change
            //has been detected.
            m_cellIDReader->requestCellID();
            coreprintln("requestCellID() in setLocationHardware()");            
         }
      }
      m_currentHardware = hardware;
      notifyListenersStarted();
   }
   return SynchronousStatus(OK, "", "");
}

} //end of namespace WFAPI
