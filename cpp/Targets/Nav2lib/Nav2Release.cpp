/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "Nav2.h"

#define NAV2_CLASS Nav2Release
#define __NAV2_OBJ_RELEASE__

#ifdef __NAV2_OBJ_RELEASE__
# define RELEASE_AS_BOOL true
#else
# define RELEASE_AS_BOOL false
#endif

#include "Nav2.h"
#include "nav2util.h"

#include <algorithm> 
#include <stdio.h>

#include "MsgBufferEnums.h"
#include "Module.h"

#include "CtrlHubInternal.h"
#include "ConnectionManager.h"

#include "ParameterEnums.h"
#include "ParameterInternal.h"

#include "Serial.h"
#include "TCPSocket.h"
#include "TcpSerial.h"
#include "TcpSymbianSerial.h"

#include "MapEnums.h"
#include "NavServerComEnums.h"
#include "NavPacket.h"
#include "NavServerCom.h"

#include "isabTime.h"
#include "Quality.h"
#include "PositionState.h"
#include "RouteEnums.h"
#include "NavTask.h"

#include "RequestList.h"
#include "NavServerComInternal.h"

#include "Gps.h"
#include "GpsCommon.h"
#include "GpsSirf.h"

#include "../NavTask/Point.h"
#include "NavTaskInternal.h"

#include "Destinations.h"
#include "ErrorModule.h"
#include "UiCtrl.h"

#include "PacketEnums.h"
#include "DestinationsInternal.h"

#include "Mutex.h"
#include "QueueSerial.h"

#ifndef __NAV2_OBJ_RELEASE__
# include "FakeNavServerCom.h"
# include "TestShell.h"
#endif

#if !defined __NAV2_OBJ_RELEASE__ || defined __linux
# include "Simulation.h"
# include "GpsTrimble.h"
#endif


#ifdef __SYMBIAN32__
# include "TimerThread.h"
#endif

#include "GlobalData.h"

#undef LOGPOINTER
#define LOGPOINTER mainlog
#include "LogMacros.h"

#undef NEW_KERN_SVR_6_AVOIDANCE

namespace isab{
class Module;
class Log;

Nav2Release::Nav2Release(Nav2StartupData &sdata, ConnectionManager* connMgr,
                         NotificationCenter* notificationCenter) :
   Nav2(sdata)
{
   //locks
   if (initialized) {
      //return ALREDY_STARTED;
      return;
   }
   initialized=1;

#ifdef NEW_KERN_SVR_6_AVOIDANCE
#define KERN_SVR_6_AMOUNT (1024*50)
   uint32 *kern_svr_6 = NULL;
   kern_svr_6 = new uint32[KERN_SVR_6_AMOUNT];
   delete[] kern_svr_6;
#endif

   m_globalData->m_logmaster = new LogMaster(sdata.writableFilesPath);
   m_globalData->m_logmaster->setSuperLevels(sdata.superLogLevels);
#ifdef __SYMBIAN32__
   m_archGlobalData->m_timerThread = new TimerThread();
   // Set up an RSocketServ - this will hopefully keep the
   // GPRS connection alive for now...
   m_archGlobalData->m_socketServ = new RSocketServ();
# if !(defined NAV2_CLIENT_UIQ3 || defined NAV2_CLIENT_SERIES60_V3)
   m_archGlobalData->m_socketServ->Connect();
   m_archGlobalData->m_socketServ->Share(RSessionBase::EAutoAttach);
# else
   TSessionPref sessionPrefs;
   sessionPrefs.iAddrFamily = KAfInet;
   sessionPrefs.iProtocol = KProtocolInetTcp;
   m_archGlobalData->m_socketServ->Connect( sessionPrefs );
   TInt err = m_archGlobalData->m_socketServ->ShareAuto();
   if( err != KErrNone ){
   }
# endif
   m_archGlobalData->m_use_exception_handler = sdata.useExceptionHandler;
#endif

   if (sdata.debugOutput != NULL) {
      m_globalData->m_logmaster->setDefaultOutput(sdata.debugOutput);
   }

   // Log object for Navigate2lib's admin parts.
   mainlog = new Log("Navigate2");
   DBG("Creating modules.");

#if !(defined __SYMBIAN32__ || defined(_WIN32_WCE))
   #define SUPPORTS_JTC
#endif

   
   // The CtrlHub is created first, so all other modules have
   // something to connect to.
#ifdef SUPPORTS_JTC 
   try {
#endif
      hub = new CtrlHub();
      DBG("Created CtrlHub.");
#ifdef SUPPORTS_JTC
   }
   catch ( JTCException& e ) {
      DBG("Err %d, msg %s", e.getError(), e.getMessage() );
   }
#endif


   // The parameter module has no sub-modules.
   Parameter *p;
   allModules.push_back(p   = new Parameter(RELEASE_AS_BOOL));
   if (sdata.wfType != GuiProtEnums::InvalidWayfinderType) {
      // Force feed the WayfinderType parameter to the parameter module.
      // Not the prettiest of hacks, but it does work.
      int32 tmp = int32(sdata.wfType);
      p->decodedSetParam(uint32(ParameterEnums::WayfinderType), &tmp, 1, 0);
   }
   hub->addModule(p->newPublicCtrlHub(), Module::ParameterModuleId);


#ifndef __NAV2_OBJ_RELEASE__
   // The NavServerCom module uses a TcpSerial module to connect to
   // the NavigatorServer. The servers name and port is decided on a
   // per-session-basis and controlled through parameters.
   if(! sdata.fakeNavServerCom)
#endif
   {
      class SerialProviderPublic* tcp_serial = NULL;

#if defined __SYMBIAN32__
      {
         class TcpSymbianSerial* tcp;
         allModules.push_back(tcp = new TcpSymbianSerial("SNSCTcp", 
                                                         sdata.allowWLAN));
         tcp_serial = tcp->newPublicSerial();
      }
#else
      {
         class TcpSerial* tcp;
         allModules.push_back(tcp = new TcpSerial("NSCTcp", notificationCenter));
         if (connMgr != NULL){
            connMgr->addListener(tcp);            
         }
         tcp_serial = tcp->newPublicSerial();
      } 
#endif
      class NavServerCom *nsc = 
         new NavServerCom(tcp_serial, 
                          sdata.serialNumber,
                          sdata.extraServerHost, sdata.extraServerPort, 
                          sdata.majorVer, sdata.minorVer, sdata.buildVer,
                          sdata.majorRes, sdata.minorRes, sdata.buildRes,
                          sdata.clientType, sdata.clientTypeOptions,
                          sdata.httpProto, sdata.httpUserAgent,
                          sdata.httpRequest, sdata.httpHostOverride,
                          sdata.useTracking, sdata.maxnbroutstanding,
                          sdata.licenceKeyType,
                          sdata.wayfinderIDStartUP,
                          &sdata.hardwareIDs );
      allModules.push_back( nsc );
      hub->addModule(nsc->newPublicCtrlHub(), 
                     Module::NavServerComModuleId);
   }
#ifndef __NAV2_OBJ_RELEASE__
   else 
   {
      FakeNavServerCom* fake;
      allModules.push_back( fake = new FakeNavServerCom() );
      hub->addModule(fake->newPublicCtrlHub(),
                     Module::NavServerComModuleId);
   }
#endif

#ifndef __NAV2_OBJ_RELEASE__
   if(sdata.testshellPort != 0){
      // The TestShell should probably not be included in any release
      // product.  It uses a TcpSerial module to either connect to or be
      // connected to by a host.
      TcpSerial* shellTcp;
      if(sdata.testshellHost != NULL){
         shellTcp = new TcpSerial(sdata.testshellHost, sdata.testshellPort, "ShellTcp");
      } else {
         shellTcp = new TcpSerial(sdata.testshellPort, "ShellTcp");
      }
      allModules.push_back(shellTcp);
      TestShell *shell;
      shell = new TestShell(shellTcp->newPublicSerial(), sdata.errorTable, sdata.testshellRegTest);
      allModules.push_back(shell);
      hub->addModule(shell->newPublicCtrlHub());
   }
#endif


   NavTask *nt = NULL;
// These defines did not work very well, if you want to use simulatorPort 
// and simulateFromFile you have to set it up below.
// #ifndef __linux
// #ifndef __NAV2_OBJ_RELEASE__
   if (sdata.simulatorPort == 0 && sdata.simulateFromFile == NULL) 
// #endif
   {
      m_bt = new QueueSerial("BtQueue", new KeepLatestBuffer, 1000*1000);
      allModules.push_back( m_bt );
      //we could check for GPS protocol here. Not done now since
      //we never use any other kind of GPS unit than NMEA.
      GpsSirf *sirf  = new GpsSirf(m_bt->newPublicSerial());
      allModules.push_back( sirf );
      nt = new NavTask(sirf->newPublicGps());
      allModules.push_back( nt );
   }
// #else
//       if(0){} //to match the else below. 
// #endif //__linux 
// #if !defined __NAV2_OBJ_RELEASE__ || defined __linux
//       else
//       {
//          //options:
//          //* from file: Sim->Trimble->NavTask
//          //* NMEA from Tcp: tcp->sirf->NavTask
//          //* Trimble from Tcp: tcp->sim->trimble->Navtask
//          SerialProviderPublic* spp = NULL;
//          GpsProviderPublic* gpp = NULL;
//          if(sdata.simulateFromFile == NULL){ //tcp module
//             if(sdata.simulatorHost != NULL){
//                allModules.push_back(new TcpSerial(sdata.simulatorHost, 
//                                                   sdata.simulatorPort,
//                                                   "NTTcp"));
//             } else {
//                allModules.push_back(new TcpSerial(sdata.simulatorPort, 
//                                                   "NTTcp"));
//             }
//             spp =static_cast<TcpSerial*>(allModules.back())->newPublicSerial();
//          }
//          if((sdata.simulateFromFile != NULL)){
//             //files are always Trimble?
//             allModules.push_back(new Simulation(NULL, sdata.simulateFromFile));
//             spp=static_cast<Simulation*>(allModules.back())->newPublicSerial();
//          } else if(sdata.gpsProtocol == Nav2StartupData::Trimble){
//             allModules.push_back(new Simulation(spp));
//             spp=static_cast<Simulation*>(allModules.back())->newPublicSerial();
//          } 
//          if((sdata.simulateFromFile != NULL) || 
//             (sdata.gpsProtocol == Nav2StartupData::Trimble)){
//             allModules.push_back(new GpsTrimble(spp));
//             gpp = static_cast<GpsTrimble*>(allModules.back())->newPublicGps();
//          } else {
//             allModules.push_back(new GpsSirf(spp));
//             gpp = static_cast<GpsSirf*>(allModules.back())->newPublicGps();
//          }
//          allModules.push_back(nt = new NavTask(gpp));
//       }
// #endif
   // connect the navtask thread to the data->hub
   hub->addModule(nt->newPublicCtrlHub(), Module::NavTaskModuleId);



   // UiCtrl connects to the Wayfinder Gui application by Tcp or a QueueSerial.
   UiCtrl* uic;
//#ifndef __linux
//#ifndef __NAV2_OBJ_RELEASE__
   if(sdata.guiPort == 0)
//#endif
   {
      m_gui = new QueueSerial("GuiQueue", new KeepLatestGuiMessage);
      allModules.push_back( m_gui );
      uic = new UiCtrl(m_gui->newPublicSerial(),
                       sdata.errorTable,
                       sdata.audioSyntax,
                       sdata.serialNumber,
                       sdata.httpProto,
                       sdata.useIAPProxy,
                       sdata.hardIapProxyHost,
                       sdata.hardIapProxyPort,
                       sdata.userClipString);
   } 
//#else
//      if(0){} //to match the else below
//#endif
//#if !defined __NAV2_OBJ_RELEASE__ || defined __linux
#ifndef __SYMBIAN32__
   else 
   {
      TcpSerial *uiTcp;
      if(sdata.guiHost){
         uiTcp = new TcpSerial(sdata.guiHost, sdata.guiPort, "GuiTcp");
      } else {
         uiTcp = new TcpSerial(sdata.guiPort, "GuiTcp");
      }
      allModules.push_back( uiTcp );
      uic = new UiCtrl(uiTcp->newPublicSerial(),
                       sdata.errorTable,
                       sdata.audioSyntax,
                       sdata.serialNumber,
                       sdata.httpProto,
                       sdata.useIAPProxy,
                       sdata.hardIapProxyHost,
                       sdata.hardIapProxyPort,
                       sdata.userClipString);
   }
//#endif
#endif
   allModules.push_back( uic );
   hub->addModule(uic->newPublicCtrlHub(), Module::DisplaySerialModuleId);

   std::vector<isab::HWID*>::iterator imei = 
      std::find_if(sdata.hardwareIDs.begin(), 
                   sdata.hardwareIDs.end(), 
                   std::bind2nd(HWType(), "imei"));

   char* imeiKey = NULL;
   if (imei != sdata.hardwareIDs.end()) {
      imeiKey = (*imei)->m_key;
   }
   Destinations* dst = new Destinations( sdata.using_fav_ver_2, imeiKey );
   allModules.push_back( dst );
   hub->addModule(dst->newPublicCtrlHub(), 
                  Module::DestinationsModuleId);


   for (ModuleIterator i = allModules.begin(); i != allModules.end(); ++i) {
      int status = (*i)->start();
      if (status != Thread::GOOD_START) {
         DBG("start for module %s returned %i", (*i)->getName(), status);
         while (1) {
            joinModule(*i, (*i)->getName(), 1000, 0);
            if (i == allModules.begin()) { break; }
            --i;
         }
         return /* THREAD_START_FAILED */;
      } else {
         THREADINFO(**i);
      }
   }

   hub->start(); // FIXME: Check return value.
   THREADINFO(*hub);
   hub->startupComplete();

}

}



