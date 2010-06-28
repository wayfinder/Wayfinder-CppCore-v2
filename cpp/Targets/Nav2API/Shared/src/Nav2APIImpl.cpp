/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#define CORE_LOGGING_MODULE_NAME "Nav2APIImpl"
//#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE
#include "CoreDebugPrint.h"

#include "FileSystem.h"
#include "LogFiles.h"
#include "Nav2APIImpl.h"
#include "Nav2.h"
#include "LanguageHelper.h"
#include "MC2SimpleString.h"
#include "WFAPISync.h"
#include "Interface.h"
#include "InterfaceImpl.h"
#include "StartupData.h"
#include "Nav2StatusListener.h"
#include "AsynchronousStatus.h"
#include "Nav2APIChannel.h"
#include "TextLanguage.h"
#include "ReadyToRunListener.h"
#include "DistanceUnit.h"
#include "ProgramVersion.h"
// Nav2
#include "Thread.h"
#include "Monitor.h"
#include "Mutex.h"
#include "GuiProtMess.h"
#include "GuiProtEnums.h"
#include "GuiParameterEnums.h"
#include "GuiProtRouteMess.h"
#include "Nav2Logging.h"
// Status codes
#include "RouteStatusCode.h"
#include "NetworkStatusCode.h"
#include "FavouriteStatusCode.h"
#include "LocationStatusCode.h"
// Interfaces
#include "RouteInterface.h"
#include "NavigationInterface.h"
#include "NetworkInterface.h"
#include "LocationInterface.h"
#include "FavouriteInterface.h"
#include "SettingsInterface.h"
#include "SearchInterface.h"
#include "ImageInterface.h"
#include "BillingInterface.h"
#include "TunnelInterface.h"
// Impls
#include "RouteInterfaceImpl.h"
#include "NavigationInterfaceImpl.h"
#include "NetworkInterfaceImpl.h"
#include "LocationInterfaceImpl.h"
#include "FavouriteInterfaceImpl.h"
#include "SettingsInterfaceImpl.h"
#include "MapRequesterInterfaceImpl.h"
#include "SearchInterfaceImpl.h"
#include "CategoryTreeInterfaceImpl.h"
#include "OneSearchInterfaceImpl.h"
#include "ImageInterfaceImpl.h"
#include "BillingInterfaceImpl.h"
#include "TunnelInterfaceImpl.h"

#include <memory>
//#include <iostream>
#include <assert.h>
#include <ctype.h>
#include <string>

#include "MapLibAPI.h"
#include "MapLibStartupListener.h"
#include "ConfigInterface.h"
#include "DetailedConfigInterface.h"
#include "MapLib.h"
#include "RouteID.h"
#include "QueueThread.h"
#include "CoreFunctionPrologue.h"
#include "NotificationCenter.h"

using namespace std;

namespace WFAPI {


Nav2APIImpl::Nav2APIImpl(Nav2API* api)
      : m_nextRequestID(1), m_nextMessageID(1),m_nav2(NULL), m_mapLibAPI(NULL),
        m_guiChannel(NULL), // Set this when Nav2 exists...
        m_bluetoothChannel(NULL),
        m_nbrSynchonous(0), 
        m_favouriteInterface(new FavouriteInterface(this)),
        m_imageInterface(new ImageInterface(this)),
        m_locationInterface(new LocationInterface(this)),
        m_navigationInterface(new NavigationInterface(this)),
        m_networkInterface(new NetworkInterface(this)),
        m_routeInterface(new RouteInterface(this)),
        m_searchInterface(new SearchInterface(this)),
        m_settingsInterface(new SettingsInterface(this)),
        m_billingInterface(new BillingInterface(this)),
        m_tunnelInterface(new TunnelInterface(this)),
        m_mapRequesterInterfaceImpl(new MapRequesterInterfaceImpl(this)),
        m_sessionData(NULL),
        m_readyToRunListener(NULL),
        m_queueThread(NULL),
        m_shutDown(false),
        m_mutex(new isab::Mutex()),
        m_api(api),
        m_statusNotifier(NULL)
{
   coreprintln_init();
   m_notificationCenter = new NotificationCenter;
   m_mapRequesterInterfaceImpl->setNetworkInterfaceImpl(m_networkInterface->getImpl());
   initNoReplyMessageSet();
   resetCategoryTags();
}

Nav2APIImpl::~Nav2APIImpl()
{
   // coretracefunc();
   coreprintln_deinit();
   
   // (Use auto_ptr?)
   delete m_readyToRunListener;
   delete m_guiChannel;
   delete m_bluetoothChannel;
   delete m_favouriteInterface;
   delete m_imageInterface;
   delete m_locationInterface;
   delete m_navigationInterface;
   delete m_networkInterface;
   delete m_routeInterface;
   delete m_searchInterface;
   delete m_settingsInterface;
   delete m_billingInterface;
   delete m_tunnelInterface;
   delete m_sessionData;
   delete m_mapRequesterInterfaceImpl;
   delete m_mutex;
   delete m_queueThread;
   delete m_notificationCenter;
   delete m_statusNotifier;
}

void
Nav2APIImpl::initNoReplyMessageSet() 
{
   // Insert the messages that should be fire and forget, we should not
   // wait for a reply for these messages.  
   m_noReplyMessages.insert(isab::GuiProtEnums::PREPARE_SOUNDS_REPLY);
   m_noReplyMessages.insert(isab::GuiProtEnums::PLAY_SOUNDS_REPLY);
   m_noReplyMessages.insert(isab::GuiProtEnums::CONNECT_GPS);
   m_noReplyMessages.insert(isab::GuiProtEnums::DISCONNECT_GPS);
   m_noReplyMessages.insert(isab::GuiProtEnums::INVALIDATE_ROUTE);
}

void 
Nav2APIImpl::nav2Reply(isab::Buffer& buf)
{   
   //nav2log << "Nav2APIImpl::nav2Reply Starts. readPos " << buf.getReadPos() << endl;
   buf.setReadPos(buf.getReadPos() + 4); // Start after version and length   
   //nav2log << "Nav2APIImpl::nav2Reply After setReadPos. readPos " << buf.getReadPos() << endl;
   auto_ptr<isab::GuiProtMess> guiProtMess( 
      isab::GuiProtMess::createMsgFromBuf(&buf));

   if (guiProtMess.get() == NULL) {
      nav2log << "Nav2APIImpl::nav2Reply NULL GuiProtMess." << endl;
      // Safety for unlikely cases
      return;
   }
   
   WFAPISync sync(m_mutex);

   if (guiProtMess->getMessageType() == 
       isab::GuiProtEnums::GET_MULTI_VECTOR_MAP_REPLY) {
      // MapLib first!
      m_mapLibMsgs.push_back(guiProtMess.release());
      if (m_readyToRunListener != NULL) {
         // Possible callbacks from ready call leads to mutex unlocking.
         m_mutex->unlock();
         m_readyToRunListener->ready();
         m_mutex->lock();
      } else {
         nav2log << "Got premature maplib message. Storing it for later." 
                 << endl;
      }

      // Else, wait for connectMapLib to be called and call ready() from there.
      return;
   }

   wf_uint16 guiProtID = guiProtMess->getMessageID();
   RequestInformationMap::iterator it = m_requestInformation.find(
         guiProtID);
   //nav2log << "Nav2APIImpl::nav2Reply guiID " << guiProtID 
   //         << " found? " << (it != m_requestInformation.end()) << endl;

   if (it != m_requestInformation.end()) {
      nav2log << "Nav2APIImpl::nav2Reply reqID " 
                << it->second.getRequestID().getID() << endl;
      if (it->second.isSynchronous()) {
         nav2log << "Nav2APIImpl::nav2Reply isSynchronous" << endl;
         // Synchronous
         m_nbrSynchonous--;
         it->second.getInterface()->receiveSynchronousReply(
            guiProtMess.release(), it->second.getRequestID());
         m_requestInformation.erase(it);
         
         if (m_nbrSynchonous == 0) {
            // Reply all the stored asynchronous replies

            if (m_readyToRunListener != NULL) {
               if (!m_msgQueue.empty()) {
                  // Request callback
                  while (!m_msgQueue.empty()) {
                     m_msgs.push_back(m_msgQueue.front());
                     m_msgQueue.pop_front();
                  }
                  m_mutex->unlock();
                  m_readyToRunListener->ready();
                  m_mutex->lock();
               } // Else nothing to do.
            } else {
               while (!m_msgQueue.empty()) {
                  nav2log << "Nav2APIImpl::nav2Reply msgQueue " 
                            << m_msgQueue.size() << endl;
                  auto_ptr<isab::GuiProtMess> asynMess(m_msgQueue.front());
                  m_msgQueue.pop_front();
                  wf_uint16 asynID = asynMess->getMessageID();
                  it = m_requestInformation.find(asynID);
               
                  if (it != m_requestInformation.end()) {
                     RequestInformation reqInfo(it->second);
                     m_requestInformation.erase(it);
                     m_mutex->unlock();
                     reqInfo.getInterface()->receiveAsynchronousReply(
                        *asynMess, reqInfo.getRequestID());
                     m_mutex->lock();
                  } else {
                     m_mutex->unlock();
                     unsolicitedReply(*asynMess);
                     m_mutex->lock();
                  }
                  asynMess->deleteMembers();
               }
            } // End else no callback required
         }
      } else {
         if (m_readyToRunListener != NULL) {
            // Store message for later synchronised callback.
            m_msgs.push_back(guiProtMess.release());
            m_mutex->unlock();
            m_readyToRunListener->ready();
            m_mutex->lock();
         } else {
            // Asynchronous
            nav2log << "Nav2APIImpl::nav2Reply Asynchronous" << endl;
            if (m_nbrSynchonous > 0) {
               // Store the asynchronous reply until the synchronous reply
               // comes.
               nav2log << "Nav2APIImpl::nav2Reply storing it, type "
                         << int(guiProtMess->getMessageType()) << endl;
               m_msgQueue.push_back(guiProtMess.release());
            } else {
               RequestInformation reqInfo(it->second);
               m_requestInformation.erase(it);
               m_mutex->unlock();
               nav2log << "Nav2APIImpl::nav2Reply replying" << endl;
               reqInfo.getInterface()->receiveAsynchronousReply(
                  *guiProtMess, reqInfo.getRequestID());
               m_mutex->lock();
               
            }
         } // End else no callback required
      }
   } else { // End if we have a receiver for the guiProtMess
      //nav2log << "Nav2APIImpl::nav2Reply unsolicited" << endl;
      if (m_nbrSynchonous > 0) {
         // Store the asynchronous reply until the synchronous reply comes.
         nav2log << "Nav2APIImpl::nav2Reply unsolicited storing it, type "
                   << int(guiProtMess->getMessageType()) << endl;
         m_msgQueue.push_back(guiProtMess.release());
      } else {
         if (m_readyToRunListener != NULL) {
            // Store message for later synchronised callback.
            m_msgs.push_back(guiProtMess.release());
            m_mutex->unlock();
            m_readyToRunListener->ready();
            m_mutex->lock();
         } else {
            m_mutex->unlock();
            unsolicitedReply(*guiProtMess);
            m_mutex->lock();
         }
      }
   }

   if (guiProtMess.get() != NULL) {
      guiProtMess->deleteMembers();
   }
}


wf_uint16
Nav2APIImpl::sendGuiMessage(isab::GuiProtMess* guiProtMess) 
{
   wf_uint16 msgID = m_nextMessageID;
   m_nextMessageID += 2; // Nav2 uses it this way, Gui msgs are odd.

   isab::Buffer buf;
   guiProtMess->setMessageId(msgID);
   guiProtMess->serialize(&buf);
   m_guiChannel->getChannel()->writeData(buf.accessRawData(), buf.getLength());
 
   return msgID;
}

RequestID
Nav2APIImpl::nav2Request(isab::GuiProtMess* guiProtMess,
                         InterfaceImpl* interface,
                         bool synchronous,
                         RequestID* requestID)
{
   if (m_shutDown) {
      // Is in progress of shutting down, no more messages can
      // be sent.
      return RequestID(RequestID::INVALID_REQUEST_ID);
   }
   WFAPISync sync(m_mutex);
   if (synchronous) {
      // Check current threadid against QueueThread's!
      // If QueueThread return error!
      if (isab::Thread::currentThread() == m_queueThread) {
         // This is bad!
         nav2log << "Nav2APIImpl::nav2Request queueThread called me!"
                   << endl;
         assert(isab::Thread::currentThread() != m_queueThread);
         return RequestID(RequestID::INVALID_REQUEST_ID);
      }
      m_nbrSynchonous++;
   }

   nav2log << "Nav2APIImpl::nav2Request, pre sendGuiMessage" << endl;
   wf_uint16 msgID = sendGuiMessage(guiProtMess);
   nav2log << "Nav2APIImpl::nav2Request, post sendGuiMessage" << endl;

   RequestID usedRequestID(0); // Value set in if-statement
   if (requestID == NULL) {
      usedRequestID = makeRequestID();
   } else {
      usedRequestID = *requestID;
   }

   // Check if the message type of the message to send is located in
   // the set of message types that should be fire and forget. 
   if (m_noReplyMessages.find(guiProtMess->getMessageType()) == 
       m_noReplyMessages.end()) {
      // The message type was not found in the m_noReplyMessages, this
      // means that we should add information to the m_requestInformation.
      nav2log << "IntImpl::nav2Request synchronous " << synchronous
                << " guiID " << msgID << " reqID " << usedRequestID.getID()
                << endl;
      m_requestInformation.insert(
         make_pair(msgID,
                   RequestInformation(interface, usedRequestID, 
                                      synchronous)));
   }

   nav2log << "Nav2APIImpl::nav2Request nbr requests " 
             << m_requestInformation.size() << " IDs";
   for (RequestInformationMap::const_iterator it = 
           m_requestInformation.begin(); it != m_requestInformation.end();
        ++it) {
      nav2log << " " << it->second.getRequestID().getID();
   }
   nav2log << endl;
   nav2log << "Nav2APIImpl::nav2Request, pre return" << endl;
   return usedRequestID;
}

RequestID 
Nav2APIImpl::makeRequestID()
{
   return RequestID(m_nextRequestID++);
   
}

// Helper function
void cleanMsgQueueFromID(Nav2APIImpl::MsgQueue& msgs,
                         wf_uint16 guiID) {
   for (Nav2APIImpl::MsgQueue::iterator it = msgs.begin(); 
        it != msgs.end(); /*advances in code*/) {
      if ((*it)->getMessageID() == guiID) {
         it = msgs.erase(it);
      } else {
         ++it;
      }
   }
}

void
Nav2APIImpl::cancelRequest(RequestID requestID)
{
   WFAPISync sync(m_mutex);
   for (RequestInformationMap::iterator guiProtMessRequestIDIterator = 
           m_requestInformation.begin() ;
        guiProtMessRequestIDIterator != m_requestInformation.end() ; ) {

      if (guiProtMessRequestIDIterator->second.getRequestID() == requestID) {
         // Also check m_msgs and m_msgQueue
         wf_uint16 guiID = guiProtMessRequestIDIterator->first;
         cleanMsgQueueFromID(m_msgs, guiID);
         cleanMsgQueueFromID(m_msgQueue, guiID);

         if (guiProtMessRequestIDIterator->second.isSynchronous()) {
            // This should not happen! We never return requestID for
            // synchronous calls!
            m_nbrSynchonous--;
            // if zero then send stored!
            
         }
         m_requestInformation.erase(guiProtMessRequestIDIterator++);
         // No break here as there can be more than one request with
         // the same requestID
      } else {
         ++guiProtMessRequestIDIterator;
      }
   }
}


Nav2APIImpl::RequestInformation::RequestInformation(InterfaceImpl* interface,
                                                    RequestID requestID,
                                                    bool synchronous)
      : m_interface(interface), m_requestID(requestID),
        m_synchronous(synchronous)
{
}

Nav2APIImpl::RequestInformation::~RequestInformation()
{
}

InterfaceImpl* 
Nav2APIImpl::RequestInformation::getInterface()
{
   return m_interface;
}

RequestID 
Nav2APIImpl::RequestInformation::getRequestID() const
{
   return m_requestID;
}

bool
Nav2APIImpl::RequestInformation::isSynchronous() const
{
   return m_synchronous;
}

void
Nav2APIImpl::connectMapLib(MapLibAPI* mapLibAPI) {
   WFAPISync sync(m_mutex);

   nav2log << "Nav2APIImpl::connectMapLib" << endl;
   m_mapLibAPI = mapLibAPI;

   m_mapLibAPI->setStartupListener(m_queueThread);
   // Check if any pending messages exists for MapLib to poll.
   if (! m_mapLibMsgs.empty()) {
      nav2log << "Nav2APIImpl::connectMapLib - sending premature maplib messages." 
           << endl;
      m_mutex->unlock();
      m_readyToRunListener->ready();
      m_mutex->lock();
   }
}

FavouriteInterface&
Nav2APIImpl::getFavouriteInterface() {
   return *m_favouriteInterface;
}

ImageInterface&
Nav2APIImpl::getImageInterface() {
   return *m_imageInterface;
}

LocationInterface&
Nav2APIImpl::getLocationInterface() {
   return *m_locationInterface;
}

NavigationInterface&
Nav2APIImpl::getNavigationInterface() {
   return *m_navigationInterface;
}

NetworkInterface&
Nav2APIImpl::getNetworkInterface() {
   return *m_networkInterface;
}

RouteInterface&
Nav2APIImpl::getRouteInterface() {
   return *m_routeInterface;
}

SearchInterface&
Nav2APIImpl::getSearchInterface() {
   return *m_searchInterface;
}

SettingsInterface&
Nav2APIImpl::getSettingsInterface() {
   return *m_settingsInterface;
}

BillingInterface&
Nav2APIImpl::getBillingInterface() {
   return *m_billingInterface;
}

TunnelInterface&
Nav2APIImpl::getTunnelInterface() {
   return *m_tunnelInterface;
}
   
MapRequesterInterfaceImpl& 
Nav2APIImpl::getMapRequesterInterfaceImpl()
{
   return *m_mapRequesterInterfaceImpl;
}

// This should be moved to shared code 
// Right now this is a copy of MapPlotter::getBitMapExtension.
const char*
Nav2APIImpl::getImageExtension(ImageExtension imageExtension) const
{
   switch (imageExtension) {
   case MIF:
      return "mif";
   case SVG:
      return "svg";
   case PNG:
      return "png";
   }
   
   return "png";
}

CombinedSearch::CSImageDimension
Nav2APIImpl::getImageDimension(ImageDimension imageDimension) const
{
   CombinedSearch::CSImageDimension dimension;
   switch(imageDimension) {
   case DIMENSION_20X20:
      dimension.width = 20;
      dimension.height = 20;
      break;
   case DIMENSION_40X40:
      dimension.width = 40;
      dimension.height = 40;
      break;
   case DIMENSION_50X50:
      dimension.width = 50;
      dimension.height = 50;
      break;
   case DIMENSION_75X75:
      dimension.width = 75;
      dimension.height = 75;
      break;
   default:
      dimension.width = 0;
      dimension.height = 0;
      break;
   }
   
   return dimension;
}

void
Nav2APIImpl::start(StartupData* startupData,
                   const WFString& audioTypeDirName,
                   const ProgramVersion& programVersion, 
                   const WFString& clientType, 
                   const WFString& clientTypeOptions,
                   bool httpProto,
                   const WFString& httpUserAgent,
                   const WFString& httpRequestString,
                   const WFString& httpFixedHost,
                   bool useTracking,
                   bool wayfinderIDStartUp,
                   const HardwareIDArray& hwids,
                   ReadyToRunListener* readyListener,
                   Nav2StatusNotifier* statusNotifier)
{
   CORE_FUNCTION_PROLOGUE_SET_CONTEXT();
   
   if(startupData != NULL) {
      m_sessionData = new InternalSessionData(startupData);
   }
   m_statusNotifier = statusNotifier;
   m_networkInterface->getImpl()->setConnectionManager(
      startupData->getConnectionManager());
   
   // Nav2 startup stuff
   bool startedOk = true;

   // Create 
   isab::Nav2Error::Nav2ErrorTable* errorTable = NULL;
   isab::AudioCtrlLanguage* audioSyntax = NULL;
   LanguageHelper::createErrorTableAndSyntax( 
      LanguageHelper::textLanguageToNav2(m_sessionData->getTextLanguage()),
      errorTable, audioSyntax);

   isab::Nav2StartupData startData(
      isab::GuiProtEnums::Gold,
      m_sessionData->getCommonDataStoragePath().c_str(),
      m_sessionData->getParameterStoragePath().c_str(),
      errorTable, audioSyntax);

   // Set the data path to the search interface
   WFString imageStoragePath = m_sessionData->getImageStoragePath();
   m_searchInterface->getImpl()->setImagePath(imageStoragePath.c_str());

   const char* imageExtension = getImageExtension(startupData->getImageExtension());
   WFString categoryImagePath = m_sessionData->getCategoryImageStoragePath();
   m_searchInterface->getImpl()->
      setCategoryImagePath(categoryImagePath.c_str());   
   m_searchInterface->getImpl()->
      setImageExt(getImageExtension(startupData->getImageExtension()));
   m_searchInterface->getImpl()->
      setImageDimension(getImageDimension(startupData->getImageDimension()));

   WFString categoryTreeImagePath =
      m_sessionData->getCategoryTreeImageStoragePath();
   m_searchInterface->getCategoryTreeImpl()->
      setCategoryImagePath(categoryTreeImagePath.c_str());
   m_searchInterface->getCategoryTreeImpl()->setImageExt(imageExtension);
   m_searchInterface->getCategoryTreeImpl()->
      setImageDimension(getImageDimension(startupData->getImageDimension()));

   m_searchInterface->getOneSearchImpl()->
      setImagePath(categoryTreeImagePath.c_str());
   m_searchInterface->getOneSearchImpl()->setImageExt(imageExtension);
   
   // Program version
   startData.setProgramVersion(programVersion.majorV,
                               programVersion.minorV,
                               programVersion.buildV);

   // Client type
   startData.clientType          = clientType.c_str();
   startData.clientTypeOptions   = clientTypeOptions.c_str();

   // HW IDs
   for (wf_uint32 i = 0; i < hwids.size(); ++i) {
      startData.hardwareIDs.push_back(new isab::HWID(hwids[i].key.c_str(), 
                                                     hwids[i].type.c_str()));
   }

   // HTTP settings
   startData.setHttpProto(httpProto);
   if (!httpUserAgent.empty()) {
      startData.setHttpUserAgent(httpUserAgent.c_str());
   }
   if (!httpRequestString.empty()) {
      startData.httpRequest = httpRequestString.c_str();
   }
   if (!httpFixedHost.empty()) {
      startData.httpHostOverride = httpFixedHost.c_str();
   }
   
   startData.setUseTracking(useTracking);
   startData.wayfinderIDStartUP = wayfinderIDStartUp;

   // Creates and starts Nav2
   m_nav2 =
      new isab::Nav2Release(startData,
                            m_networkInterface->getImpl()->getConnectionManager(),
                            m_notificationCenter);

   // Set this for later use
   m_guiChannel = new Nav2APIChannel(m_nav2->getGuiChannel());

   // Set this for later use, this should be sent to the gps interface
   // for writing nmea data to nav2
   m_bluetoothChannel = new Nav2APIChannel(m_nav2->getBluetoothChannel());
   
   m_locationInterface->getImpl()->setBtChannel(m_bluetoothChannel);
   
   // Set this for later use.
   m_readyToRunListener = readyListener;

   // GuiProtEnums::LOAD_AUDIO_SYNTAX with _XX language path
   // Make audio path from getResourceStoragePath and VoiceLanguage
   MC2SimpleString audioPath(
      m_sessionData->getResourceStoragePath().c_str());
   // Add the directory name of the type of sounds used.
   audioPath.append("/");
   audioPath.append(audioTypeDirName.c_str());
   audioPath.append("/");
   audioPath.append(getVoiceLanguageAsString(m_sessionData->getVoiceLanguage(),
                                             true).c_str());
   audioPath.append("/");
   // Add the syntax file
   audioPath.append(getVoiceLanguageAsString(m_sessionData->getVoiceLanguage(),
                                             false).c_str());
   audioPath.append(".syn");
   isab::GenericGuiMess* loadAudio = new isab::GenericGuiMess(
      isab::GuiProtEnums::LOAD_AUDIO_SYNTAX, audioPath.c_str());
   vector<isab::GuiProtMess*> initialMsgs;
   initialMsgs.push_back(loadAudio);
   // Set language in Nav2 too
   isab::GeneralParameterMess* setLang = new isab::GeneralParameterMess(
      isab::GuiProtEnums::paramLanguage, 
      wf_int32(LanguageHelper::textLanguageToNav2(
                  m_sessionData->getTextLanguage())));
   initialMsgs.push_back(setLang);

   // Let QueueThread wait until startup is done and call startupcomplete
   
   // Start QueueThread if all is ok to far
   if (startedOk) {
      m_queueThread = new QueueThread(
         m_guiChannel->getChannel(), this, initialMsgs, 
         m_sessionData->getTextLanguage());
      if (m_queueThread->start() != isab::Thread::GOOD_START) {
         startedOk = false;
         // Remove the remains
         delete m_queueThread;
         m_queueThread = NULL;
      }
   }

   if (!startedOk) {
      m_statusNotifier->addError(AsynchronousStatus(
                                    RequestID::INVALID_REQUEST_ID,
                                    GENERAL_ERROR, "", ""));
   } // Else wait for QueueThread to call startupComplete in this class.
}

void Nav2APIImpl::addStatusListener(Nav2StatusListener* statusListener)
{
   if(m_statusNotifier) {
      m_statusNotifier->addListener(statusListener);      
   }
}

void Nav2APIImpl::removeStatusListener(Nav2StatusListener* statusListener)
{
   if(m_statusNotifier) {
      m_statusNotifier->removeListener(statusListener);      
   }
}

void
Nav2APIImpl::stop()
{
   coretracefunc();
   
   m_shutDown = true;
   m_queueThread->terminate();
}

void
Nav2APIImpl::deleteNav2()
{
   coretracefunc();
   
   delete m_nav2;
   m_nav2 = NULL;

   coreprintln("Issuing STOP_COMPLETE!");
   
   m_statusNotifier->addMessage(Nav2StatusNotifier::STOP_COMPLETE, OK);
}

void
Nav2APIImpl::startupComplete(StatusCode status) 
{
  nav2log << "Nav2APIMPL::startupcomplete" << endl;
   if (status == OK) {
      coreprintln("Nav2APIImpl::startupcomplete, status OK");
      
      m_statusNotifier->addMessage(Nav2StatusNotifier::STARTUP_COMPLETE,
                                   status);
   } else {
      coreprintln("Nav2APIImpl::startupcomplete, status NOT OK");
      
      m_statusNotifier->addError(AsynchronousStatus(
                                    RequestID::INVALID_REQUEST_ID,
                                    status, "", ""));
   }
}

void
Nav2APIImpl::mapLibStartupComplete(StatusCode status) 
{
   if (status == OK) {
      m_statusNotifier->addMessage(Nav2StatusNotifier::MAPLIB_STARTUP_COMPLETE,
                                   status);
   } else {
      m_statusNotifier->addError(AsynchronousStatus(
                                    RequestID::INVALID_REQUEST_ID,
                                    status, "", ""));
   }
}



void
Nav2APIImpl::unsolicitedReply(const isab::GuiProtMess& guiProtMess)
{
   // Else check for errors or unsolicited updates
   switch (guiProtMess.getMessageType()) {
      case isab::GuiProtEnums::MESSAGETYPE_ERROR: 
      {
         const isab::ErrorMess* errorMsg = static_cast<const isab::ErrorMess*>(
            &guiProtMess);
         // Call errorListener
         nav2log << "Nav2APIImpl::nav2Reply unsolicited error" << endl;

         wf_uint32 statusCode = nav2ErrorNumberToStatusCode(
            errorMsg->getErrorNumber());
         // if(statusCode >= NET_ERR_UNKNOWN &&
         //    statusCode <= NET_EVENT_CLOSE_IND) {
         //    m_networkInterface->getImpl()->
         //       reportNetworkError(AsynchronousStatus(
         //                             RequestID::INVALID_REQUEST_ID,
         //                             statusCode, "", ""));
         // } else {
            const char* errStr = errorMsg->getErrorString();
            const char* errUrl = errorMsg->getErrorURL();
            const char* errData = errorMsg->getErrorData();
            m_statusNotifier->addError(AsynchronousStatus(
                                          RequestID::INVALID_REQUEST_ID,
                                          statusCode,
                                          errStr != NULL ? errStr : "",
                                          errUrl != NULL ? errUrl : "",
                                          errData != NULL ? errData : ""));
            //}
         break;
      }

      // Route updates...
      case isab::GuiProtEnums::STARTED_NEW_ROUTE: 
      case isab::GuiProtEnums::REROUTE:
      {
         nav2log << "Nav2APIImpl::nav2Reply unsolicited route" << endl;
         // Set routeID in MapLib too
         if (getConnectedMapLib() != NULL && guiProtMess.getMessageType() == 
             isab::GuiProtEnums::STARTED_NEW_ROUTE) {
            const isab::StartedNewRouteMess* dataMessage =
               static_cast<const isab::StartedNewRouteMess*>(&guiProtMess);
            getConnectedMapLib()->getMapLib()->setRouteID(
               RouteID(dataMessage->getRouteId()));
         }
         // Send message to 
         m_routeInterface->getImpl()->receiveAsynchronousReply(
            guiProtMess, RequestID(RequestID::INVALID_REQUEST_ID));
         break;
      }

      // Navigation updates...
      case isab::GuiProtEnums::PREPARE_SOUNDS: 
      case isab::GuiProtEnums::PLAY_SOUNDS:
      case isab::GuiProtEnums::SOUND_FILE_LIST:
      case isab::GuiProtEnums::UPDATE_ROUTE_INFO:
      {
         nav2log << "Nav2APIImpl::nav2Reply unsolicited navigation" 
                   << endl;
         m_navigationInterface->getImpl()->receiveAsynchronousReply(
            guiProtMess, RequestID(RequestID::INVALID_REQUEST_ID));
         break;
      }
         
      // Favourite updates...
      case isab::GuiProtEnums::FAVORITES_CHANGED: 
      {
         nav2log << "Nav2APIImpl::nav2Reply unsolicited favourites" 
                   << endl;
         m_favouriteInterface->getImpl()->receiveAsynchronousReply(
            guiProtMess, RequestID(RequestID::INVALID_REQUEST_ID));
         break;
      }
         
      // GPS updates...
      case isab::GuiProtEnums::UPDATE_POSITION_INFO:
      case isab::GuiProtEnums::SATELLITE_INFO:
      {
         nav2log << "Nav2APIImpl::nav2Reply unsolicited location " 
                  << int(guiProtMess.getMessageType()) << endl;
         m_locationInterface->getImpl()->receiveAsynchronousReply(
           guiProtMess, RequestID(RequestID::INVALID_REQUEST_ID));
         break;
      }

      // Search
      case isab::GuiProtEnums::SEARCH_RESULT_CHANGED:
      {
         nav2log << "Nav2APIImpl::nav2Reply unsolicited search" 
                   << endl;
         // Uncomment when Impl is done!
         //m_searchInterface->getImpl()->receiveAsynchronousReply(
         //   guiProtMess, RequestID(RequestID::INVALID_REQUEST_ID));
         break;
      }

      case isab::GuiProtEnums::PROGRESS_INDICATOR:
         // These are quite annoying to look at.
         // Ignore for now.
         break;

      // Settings updates...(We ignore these? Get them when needed.)
      // PARAMETER_CHANGED

      case isab::GuiProtEnums::SET_GENERAL_PARAMETER: {
         const isab::GeneralParameterMess* mess = 
            static_cast<const isab::GeneralParameterMess*> (
               &guiProtMess);
         handleSetParameter(*mess);
         break;
      }
      case isab::GuiProtEnums::NEW_VERSION_MSG: {
         m_settingsInterface->getImpl()->receiveAsynchronousReply(
            guiProtMess, RequestID(RequestID::INVALID_REQUEST_ID));         
      }

      default:
         nav2log << "Nav2APIImpl::nav2Reply unsolicited unknown" 
                 << " message type " << int(guiProtMess.getMessageType())
                 << endl;
         break;
   }
}

wf_uint32
Nav2APIImpl::nav2ErrorNumberToStatusCode(wf_uint32 errorNumber) const 
{
   wf_uint32 res = GENERAL_ERROR;
   switch (isab::Nav2Error::ErrorNbr(errorNumber)) {
      // General
      case isab::Nav2Error::NSC_EXPIRED_USER:
         res = EXPIRED_ERROR;
         break;
      case isab::Nav2Error::NSC_AUTHORIZATION_FAILED:
         res = UNAUTHORIZED_ERROR;
         break;
      case isab::Nav2Error::NSC_SERVER_NOT_OK:
         res = GENERAL_SERVER_ERROR;
         break;
      case isab::Nav2Error::NSC_SERVER_OUTSIDE_MAP:
         res = OUTSIDE_MAP_ERROR;
         break;
      case isab::Nav2Error::NSC_SERVER_UNAUTHORIZED_MAP:
         res = OUTSIDE_ALLOWED_MAP_ERROR;
         break;

      // Route errors
      case isab::Nav2Error::NSC_NO_ROUTE_RIGHTS:
         res = NO_ROUTE_RIGHT_ERROR;
         break;
      case isab::Nav2Error::NSC_NO_GPS_WARN:
         res = NO_GPS_WARN;
         break;
      case isab::Nav2Error::NSC_NO_GPS_ERR:
         res = NO_GPS_ERROR;
         break;
      case isab::Nav2Error::NSC_SERVER_ROUTE_TOO_LONG:
         res = TOO_FAR_FOR_VEHICLE;
         break;
      case isab::Nav2Error::NSC_SERVER_NO_ROUTE_FOUND:
         res = NO_ROUTE_FOUND;
         break;
      case isab::Nav2Error::NSC_SERVER_BAD_ORIGIN:
         res = PROBLEM_WITH_ORIGIN;
         break;
      case isab::Nav2Error::NSC_SERVER_BAD_DESTINATION:
         res = PROBLEM_WITH_DEST;
         break;
      case isab::Nav2Error::NAVTASK_ALREADY_DOWNLOADING_ROUTE:
         res = ALREADY_DOWNLOADING_ROUTE;
         break;
      case isab::Nav2Error::NAVTASK_ROUTE_INVALID:
         res = ROUTE_INVALID;
         break;
      case isab::Nav2Error::NAVTASK_NSC_OUT_OF_SYNC:
      case isab::Nav2Error::NAVTASK_INTERNAL_ERROR:
      case isab::Nav2Error::NAVTASK_CONFUSED:
      case isab::Nav2Error::NAVTASK_NO_ROUTE:
         res = GENERAL_ROUTE_ERROR;
         break;
      case isab::Nav2Error::NAVTASK_FAR_AWAY:
         res = TOO_FAR_AWAY;
         break;

      // Network transport failed
      case isab::Nav2Error::NSC_TRANSPORT_FAILED:
         res = NETWORK_TRANSPORT_FAILED;
         break;
      case isab::Nav2Error::DEST_SYNC_ALREADY_IN_PROGRESS:
         res = FAVOURITE_SYNC_ALREADY_IN_PROGRESS;
         break;
      // Network
      case isab::Nav2Error::NSC_TCP_INTERNAL_ERROR:
      case isab::Nav2Error::NSC_TCP_INTERNAL_ERROR2:
      case isab::Nav2Error::NSC_SERVER_COMM_TIMEOUT_CONNECTED:
      case isab::Nav2Error::NSC_SERVER_COMM_TIMEOUT_CONNECTING:
      case isab::Nav2Error::NSC_SERVER_COMM_TIMEOUT_DISCONNECTING:
      case isab::Nav2Error::NSC_SERVER_COMM_TIMEOUT_CLEAR:
      case isab::Nav2Error::NSC_SERVER_COMM_TIMEOUT_WAITING_FOR_USER:
      case isab::Nav2Error::NSC_FAKE_CONNECT_TIMEOUT:
      case isab::Nav2Error::NSC_SERVER_REQUEST_TIMEOUT:
         res = NETWORK_TIMEOUT_ERROR;
         break;
      case isab::Nav2Error::NSC_NO_NETWORK_AVAILABLE:
      case isab::Nav2Error::NSC_SERVER_NOT_FOUND:
      case isab::Nav2Error::NSC_SERVER_UNREACHABLE:
      case isab::Nav2Error::NSC_SERVER_NOT_RESPONDING:
      case isab::Nav2Error::NSC_SERVER_CONNECTION_BROKEN:
      case isab::Nav2Error::NSC_FLIGHT_MODE:
         res = GENERAL_NETWORK_ERROR;
         break;

      

      case isab::Nav2Error::GUIPROT_FAILED_GET_TOP_REGION_LIST:
         res = GENERAL_ERROR;
         break;

      case isab::Nav2Error::GUIPROT_FAILED_GET_SIMPLE_PARAMETER:
         res = GENERAL_ERROR;
         break;
      case isab::Nav2Error::GUIPROT_FAILED_SET_SIMPLE_PARAMETER:
         res = GENERAL_ERROR;
         break;

      case isab::Nav2Error::GUIPROT_FAILED_GET_FAVORITES:
      case isab::Nav2Error::GUIPROT_FAILED_GET_FAVORITES_ALL_DATA:
      case isab::Nav2Error::GUIPROT_FAILED_SORT_FAVORITES:
      case isab::Nav2Error::GUIPROT_FAILED_SYNC_FAVORITES:
      case isab::Nav2Error::GUIPROT_FAILED_GET_FAVORITE_INFO:
         res = GENERAL_FAVOURITE_ERROR;
         break;

      case isab::Nav2Error::GUIPROT_FAILED_ADD_FAVORITE:
      case isab::Nav2Error::GUIPROT_FAILED_ADD_FAVORITE_FROM_SEARCH:
         res = FAILED_ADD_FAVOURITE;
         break;
      case isab::Nav2Error::GUIPROT_FAILED_REMOVE_FAVORITE:
         res = FAILED_REMOVE_FAVOURITE;
         break;
      case isab::Nav2Error::GUIPROT_FAILED_CHANGE_FAVORITE:
         res = FAILED_CHANGE_FAVOURITE;
         break;
      case isab::Nav2Error::GUIPROT_FAILED_ROUTE_TO_FAVORITE:
         // Not used feature
         res = GENERAL_ERROR;
         break;
      case isab::Nav2Error::GUIPROT_FAILED_DISCONNECT_GPS:
         res = LBS_STOP_FAILED;
         break;
      case isab::Nav2Error::GUIPROT_FAILED_CONNECT_GPS:
         res = LBS_STARTUP_FAILED;
         break;

      default:
         coreprintln("Serious problem, hit default case: isab::Nav2Error::ErrorNbr(errorNumber)=%d", isab::Nav2Error::ErrorNbr(errorNumber));
         break;
   }

   return res;
}

MapLibAPI*
Nav2APIImpl::getConnectedMapLib()
{
   return m_mapLibAPI;
}

Nav2API*
Nav2APIImpl::getNav2API()
{
   return m_api;
}


void
Nav2APIImpl::doCallbacks()
{
   internalDoCallbacks(m_msgs);
   internalDoCallbacks(m_mapLibMsgs);
}

WFString
Nav2APIImpl::getVoiceLanguageAsString(VoiceLanguage::VoiceLanguage lang, 
                                      bool uppercase)
{
   WFString res = LanguageHelper::getLanguageCodeForVoice(lang);   
   if (!uppercase) {
      // Lower case it
      std::string tmp;
      for (unsigned int l = 0; l < res.length(); ++l) {
         tmp.append(1, tolower(res[l]));
      }
      res = tmp.c_str();
   }

   return res;
}


void
Nav2APIImpl::internalDoCallbacks(MsgQueue& msgs)
{
   WFAPISync sync(m_mutex);

   while (!msgs.empty()) {
//      nav2log << "Nav2APIImpl::doCallbacks msgs " 
//                << msgs.size() << endl;
      auto_ptr<isab::GuiProtMess> mess(msgs.front());
      msgs.pop_front();
      wf_uint16 guiID = mess->getMessageID();
      RequestInformationMap::iterator it = m_requestInformation.find(guiID);
      
      if (it != m_requestInformation.end()) {
         RequestInformation reqInfo(it->second);
         m_requestInformation.erase(it);
         m_mutex->unlock();
//         nav2log << "Nav2APIImpl::doCallbacks replying" << endl;
         reqInfo.getInterface()->receiveAsynchronousReply(
            *mess, reqInfo.getRequestID());
         m_mutex->lock();
      } else {
         m_mutex->unlock();
         unsolicitedReply(*mess);
         m_mutex->lock();
      }
      mess->deleteMembers();
   }
}

void 
Nav2APIImpl::handleSetParameter(const isab::GeneralParameterMess& mess)
{
   switch(mess.getParamId()) {
      case isab::GuiProtEnums::paramDistanceMode:
      {
         // Send to NavigationInterface
         DistanceUnit distanceUnit =
            SettingsInterfaceImpl::nav2DistanceUnitToWFAPI(
               mess.getIntegerData()[0]);
         getNavigationInterface().getImpl()->setDistanceUnit(distanceUnit);
         break;
      }
      case isab::GuiProtEnums::paramTopRegionList:
         getSearchInterface().getImpl()->handleSetTopRegionList();
         break;
      case isab::GuiProtEnums::paramCategoryChecksum:
         m_categoryState[CHECKSUM_TAG] = true;
         getSearchInterface().getImpl()->handleSetCategoryChecksum(
            *mess.getIntegerData());
         break;
      case isab::GuiProtEnums::paramCategoryIds:
         m_categoryState[ID_TAG] = true;
         break;
      case isab::GuiProtEnums::paramCategoryNames:
         m_categoryState[NAMES_TAG] = true;
         break;
      case isab::GuiProtEnums::paramCategoryIcons:
         m_categoryState[ICONS_TAG] = true;
         break;
      case isab::GuiProtEnums::paramCategoryIntIds:
         m_categoryState[INTS_TAG] = true;
         break;
      default:
         nav2log << "Nav2APIImpl::handleSetParameter unsolicited "
                 << "parameter ID 0x" << hex << mess.getParamId() << dec 
                 << endl;
   }

   if(allCategoriesUpdated()) {
      m_searchInterface->getImpl()->notifyCategoriesUpdated();
      
      resetCategoryTags();
   }
}

void Nav2APIImpl::resetCategoryTags()
{
   for(int i = 0; i < NUM_TAGS; i++) {
      m_categoryState[i] = false;
   }
}

bool Nav2APIImpl::allCategoriesUpdated() const
{
   for(int i = 0; i < NUM_TAGS; i++) {
      if(!m_categoryState[i]) {
         return false;
      }
   }

   return true;
}

WGS84Coordinate
Nav2APIImpl::getCurrentPosition()
{
   return m_locationInterface->getImpl()->getCurrentPosition();
}

const InternalSessionData*
Nav2APIImpl::getSessionData() const
{
   return m_sessionData;
}

NotificationCenter* WFAPI::Nav2APIImpl::getNotificationCenter()
{
   return m_notificationCenter;
}

} // End namespace WFAPI

