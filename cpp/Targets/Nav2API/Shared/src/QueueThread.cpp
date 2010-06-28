/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#define CORE_LOGGING_MODULE_NAME "QueueThread"
//#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE
#include "CoreDebugPrint.h"

#include "QueueThread.h"
#include "Nav2APIImpl.h"
#include "Nav2.h"
#include "MapLibAPI.h"
#include "ConfigInterface.h"
#include "DetailedConfigInterface.h"
#include "ScaleConfigInterface.h"
#include "MapLib.h"
#include "RouteID.h"
#include "QueueThread.h"
#include "SettingsInterfaceImpl.h"
#include "WFAPISync.h"
#include <memory>
#include "NavigationInterface.h"
#include "NavigationInterfaceImpl.h"

namespace WFAPI {

QueueThread::QueueThread(isab::Nav2::Channel* guiChannel, Nav2APIImpl* nav2API,
                         std::vector<isab::GuiProtMess*>& initialMsgs,
                         TextLanguage textLanguage)
   
   : Thread("QueueThread"),m_guiChannel(guiChannel), 
     m_nav2API(nav2API), m_inStartup(true), m_initialMsgs(initialMsgs),
     m_shutDown(false), 
     m_mapLibStartupCode(MapLibStartupListener::SUCCESS),
     m_textLanguage(textLanguage)
{
}

QueueThread::~QueueThread()
{
   // No need to tell someone we're done. The deleteNav2 call at the
   // end of run does that. 

   // if mapLibStartupComplete hasn't been called, we delete the remaining
	// parameter messages
	while (m_mapLibParams.size() > 0){
		m_mapLibParams.back()->deleteMembers();
		delete m_mapLibParams.back();
		m_mapLibParams.pop_back();
	}
}

void QueueThread::setMapLibParameters(
   std::vector<isab::GeneralParameterMess*>& mapLibParams)
{
   {
      // Set the MapLib parameters.
      for (size_t i = 0; i < mapLibParams.size(); ++i) {
         isab::GeneralParameterMess* mess = mapLibParams[i];
         if (mess->getParamId() == 
             isab::GuiProtEnums::paramMapCacheSize &&
             mess->getNumEntries() > 0) {
            // paramMapCacheSize
            wf_uint32 nbrBytes = mess->getIntegerData()[0];
            m_nav2API->getConnectedMapLib()->getConfigInterface()->
               getDetailedConfigInterface()->setMaxDiskCacheSize(
                  nbrBytes);
         } else if (mess->getParamId() == 
                    isab::GuiProtEnums::paramDistanceMode &&
                    mess->getNumEntries() > 0) {
            // paramDistanceMode
            wf_uint32 nav2DistanceMode = mess->getIntegerData()[0];
            DistanceUnit distanceUnit =
               SettingsInterfaceImpl::nav2DistanceUnitToWFAPI(nav2DistanceMode);
            m_nav2API->getConnectedMapLib()->getConfigInterface()->
               getScaleConfigInterface()->setDistanceUnit(distanceUnit);
         } else if (mess->getParamId() == 
                    isab::GuiProtEnums::paramPoiCategories) {
            // paramPoiCategories
            POICategories categories = 
               SettingsInterfaceImpl::makePOICategoriesFromNav2(
                  mess);
            m_nav2API->getConnectedMapLib()->getConfigInterface()->
               getDetailedConfigInterface()->
               synchronizePOICategories(categories);
         } else if (mess->getParamId() == 
                    isab::GuiProtEnums::userTrafficUpdatePeriod &&
                    mess->getNumEntries() > 0)
         {
            // userTrafficUpdatePeriod
            bool checkRouteForTrafficChanges = false;
            wf_int32 trafficInformationUpdateTime = 0;
            SettingsInterfaceImpl::nav2UpdateTimeToValues(
               mess->getIntegerData()[0], 
               checkRouteForTrafficChanges,
               trafficInformationUpdateTime);
            m_nav2API->getConnectedMapLib()->getConfigInterface()->
               getDetailedConfigInterface()->showTrafficInfo(
                  checkRouteForTrafficChanges);
            m_nav2API->getConnectedMapLib()->getConfigInterface()->
               getDetailedConfigInterface()->
               setTrafficInfoUpdateInterval(
                  trafficInformationUpdateTime);
         }
         mapLibParams[i]->deleteMembers();
         delete mapLibParams[i];
      }
      mapLibParams.clear();
      // Set language as we have it
      m_nav2API->getConnectedMapLib()->getConfigInterface()->
         getDetailedConfigInterface()->setLanguage(
            getTextLanguage());
 
   }
}


bool QueueThread::doStartup()
{
   coreprintln("Initializing startup");
   
   // In startup
   // Send a msg for a parameter and wait for reply then Nav2 is 
   // considered started
   // Test to wait for msg before sending
   {
      WFAPISync sync(&m_guiMonitor);
      isab::Buffer buf;
      bool readRes = false;
      do {
         readRes = m_guiChannel->readData(&buf, &m_guiMonitor);
      } while (!readRes && !m_shutDown);
               
      if (buf.getLength() > 8) {
         buf.setReadPos(4); // Start after version and length
         std::auto_ptr<isab::GuiProtMess> guiProtMess(
            isab::GuiProtMess::createMsgFromBuf(&buf));
         nav2log << "QueueThread run got first msg from Nav2, id " 
                 << guiProtMess->getMessageID() << endl;
      } else {
         nav2log << "QueueThread run got first msg from Nav2, empty"
                 << endl;
      }
   }
   for (size_t i = 0; i < m_initialMsgs.size(); ++i) {
      m_nav2API->sendGuiMessage(m_initialMsgs[i]);
      m_initialMsgs[i]->deleteMembers();
      delete m_initialMsgs[i];
      m_initialMsgs[i] = NULL;
   }
   // Timeout. If there is a problem this might happen.
   // Send parameter get for wayfinder username.
   isab::GeneralParameterMess gpm(
      (uint16)isab::GuiProtEnums::paramUserAndPassword);
   wf_uint16 reqID = m_nav2API->sendGuiMessage(&gpm);
   // Wait for reply, then Nav2 is up and running!
   StatusCode startupCode = OK;
   while (m_inStartup && !m_shutDown) {
      WFAPISync sync(&m_guiMonitor);
      isab::Buffer buf;
      m_guiChannel->readData(&buf, &m_guiMonitor);
      if (buf.getLength() != 0) {
         // Check if it is the reply we want!
         while (buf.remaining() > 4) {
            // Start after version and length
            buf.setReadPos(buf.getReadPos() + 4); 
            std::auto_ptr<isab::GuiProtMess> guiProtMess(
               isab::GuiProtMess::createMsgFromBuf(&buf));
            if (guiProtMess.get() != NULL && 
                guiProtMess->getMessageID() == reqID) {
               m_inStartup = false;
               // Set startup result
               startupCode = OK;
            } else if (guiProtMess.get() != NULL && 
                       guiProtMess->getMessageType() == 
                       isab::GuiProtEnums::SET_GENERAL_PARAMETER) {
               // Check if to store it for later use in MapLib below
               isab::GeneralParameterMess* mess = 
                  static_cast<isab::GeneralParameterMess*> (
                     guiProtMess.get());
               m_nav2API->handleSetParameter(*mess);
               if (mess->getParamId() == 
                   isab::GuiProtEnums::paramMapCacheSize ||
                   mess->getParamId() == 
                   isab::GuiProtEnums::paramPoiCategories ||
                   mess->getParamId() == 
                   isab::GuiProtEnums::userTrafficUpdatePeriod ||
                   mess->getParamId() ==
                   isab::GuiProtEnums::paramDistanceMode)
               {
                  m_mapLibParams.push_back(mess);
                  guiProtMess.release();
               }
            } else if (guiProtMess.get() != NULL && 
                       guiProtMess->getMessageType() == 
                       isab::GuiProtEnums::SOUND_FILE_LIST) {
               nav2log << "QueueThread run got SOUND_FILE_LIST!"
                       << endl;
               // Send it to NavigationInterfaceImpl
               m_nav2API->getNavigationInterface().getImpl()->
                  receiveAsynchronousReply(
                     *guiProtMess, RequestID( 
                        RequestID::INVALID_REQUEST_ID));
            }
            if (guiProtMess.get() != NULL) {
               guiProtMess->deleteMembers();
            }
         }
      } else {
         // Wait for data
         m_guiMonitor.wait();
      }
   }

   coreprintln("Startup is now complete.");
   
   // Call startupComplete
   m_nav2API->startupComplete(startupCode);

   return true;
}

void QueueThread::run()
{
   coreprintln("@run begin");

   while(!m_shutDown) {
      if (m_inStartup) {
         m_shutDown = !doStartup();
      } else {
         // Else read from m_guiChannel and send to Nav2APIImpl nav2Reply
         //::sleep(4);
         WFAPISync sync(&m_guiMonitor);
         isab::Buffer buf;
         m_guiChannel->readData(&buf, &m_guiMonitor);
         if (buf.getLength() != 0) {
#if 1
            m_nav2API->nav2Reply(buf);
#else
            while (buf.remaining() > 4) {
               nav2log << "QueueThread calling nav2Reply bytes left "
                       << buf.remaining() << endl;
               m_nav2API->nav2Reply(buf);
               buf.alignReadPos32bit();
            }
#endif
         } else {
            // Wait for data
            m_guiMonitor.wait();
         }
      }
   }

   // By deleting Nav2 it starts shutting down and waits until 
   // it is shut down. So this is a blocking call.
   coreprintln("thread loop finished, deleting Nav2..");
   m_nav2API->deleteNav2();
   coreprintln("@run end");
}

void QueueThread::terminate()
{
   coreprintln("@terminate, initiating shutdown");
   
   // Initiate shutdown
   WFAPISync sync(&m_guiMonitor);
   m_shutDown = true;
   m_guiMonitor.notify();
}

void QueueThread::handleMapLibStartupComplete(
   MapLibStartupListener::statusCode code)
{
   m_mapLibStartupCode = code;
   // Set the MapLib parameters.
   setMapLibParameters(m_mapLibParams);
   // Also notify the status listener that MapLib is started.
   m_nav2API->mapLibStartupComplete(OK); 
}

TextLanguage QueueThread::getTextLanguage() const
{
   return m_textLanguage;
}

} //End of namespace WFAPI
