/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#define CORE_LOGGING_MODULE_NAME "SettingsInterfaceImpl"
#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE
#include "CoreDebugPrint.h"

#include "SettingsInterfaceImpl.h"
#include "Nav2APIImpl.h"
#include "RouteInterfaceImpl.h"
#include "SettingsListener.h"

#include <string>

// Nav2
#include "Nav2Logging.h"
#include "GuiProtMess.h"
#include "GuiProtEnums.h"
#include "GuiParameterEnums.h"
#include "Buffer.h"
#include "DistancePrintingPolicy.h"
#include "NavServerComEnums.h"
#include "NParamBlock.h"
#include "NavRequestType.h"
#include "LangConverter.h"
#include "NavServerComProtoVer.h"
#include "crc32.h"

// MapLib
#include "MapLibAPI.h"
#include "ConfigInterface.h"
#include "DetailedConfigInterface.h"
#include "ScaleConfigInterface.h"

// Same as in C++/Targets/WayFinder/symbian-r6/SettingsData.h
#define VECTOR_MAP_POI_CATEGORY_VERSION 5

using namespace isab;
using namespace std;

namespace WFAPI {

SettingsInterfaceImpl::SettingsInterfaceImpl(Nav2APIImpl* api)
      : InterfaceImpl(api)
{
   m_crcData = "eulacrc.dat";
}

SettingsInterfaceImpl::~SettingsInterfaceImpl()
{
}

void
SettingsInterfaceImpl::receiveAsynchronousReply(
   const isab::GuiProtMess& guiProtMess, RequestID requestID)
{
   // Get the status.
   AsynchronousStatus status = createAsynchStatus(&guiProtMess, requestID);
   
   // Check status.
   if (status.getStatusCode() != OK) {
      
      // Bad status. Report error function for all listeners.
      reportError(status, m_listeners.begin(), m_listeners.end());
      return;
   }

   isab::GuiProtEnums::MessageType type = guiProtMess.getMessageType();
   switch (type) {

   case isab::GuiProtEnums::GUI_TO_NGP_DATA_REPLY: {
      const isab::DataGuiMess& mess =
         static_cast<const isab::DataGuiMess&>(guiProtMess);
      
      int32 type = -1;
      NonOwningBuffer dataMessageType(
         const_cast<uint8*>(mess.getData()), mess.getSize());
         
      if (dataMessageType.remaining() >= 2) {
         type = dataMessageType.readNextUnaligned16bit();
      }
      switch (type) {
      case isab::navRequestType::NAV_GET_KEYED_DATA_REPLY:
         // handle eula reply
         handleEULAReply(mess);
         break;
      default:
         break;
      }
         
      break;
   }
   case isab::GuiProtEnums::NEW_VERSION_MSG:
   {
      const isab::NewVersionMess& mess =
         static_cast<const isab::NewVersionMess&> (guiProtMess);
      handleNewVersionMsg(mess);
      break;
   }
   default:
      nav2log << "SettingsInterfaceImpl::receiveAsynchronousReply of "
              << "unknown type: " << int(type) << endl;
      break;
   } 
   
}

SynchronousStatus
SettingsInterfaceImpl::getAutoReroute(bool& autoReroute)
{
   return getParam(isab::GuiProtEnums::paramAutoReroute, autoReroute);
}

SynchronousStatus
SettingsInterfaceImpl::getAvoidHighway(bool& avoidHighway)
{
   return getParam(isab::GuiProtEnums::paramHighways, avoidHighway);
}

SynchronousStatus
SettingsInterfaceImpl::getAvoidTollRoad(bool& avoidTollRoad)
{
   return getParam(isab::GuiProtEnums::paramTollRoads, avoidTollRoad);
}

SynchronousStatus
SettingsInterfaceImpl::getCheckRouteForTrafficChanges(
   bool& checkRouteForTrafficChanges)
{
   // Let nav2 know that user has switched this option off.
   // Set the second highest bit, which is used as a flag.
   // updTime |= 0x40000000;
   int32 val = 0;
   SynchronousStatus res = getParam(
      isab::GuiProtEnums::userTrafficUpdatePeriod, val);
   if (res()) {
      checkRouteForTrafficChanges = (wf_uint32(val) & 0x40000000) != 0;
   }
   return res;
}

SynchronousStatus
SettingsInterfaceImpl::getDistanceUnit(DistanceUnit& distanceUnit)
{
   int32 val = 0;
   SynchronousStatus res = getParam(
      isab::GuiProtEnums::paramDistanceMode, val);
   if (res()) {
      distanceUnit = nav2DistanceUnitToWFAPI(val);
   }
   return res;
}

SynchronousStatus
SettingsInterfaceImpl::getRouteCost(RouteCost& routeCost)
{
   int32 val = 0;
   SynchronousStatus res = getParam(
      isab::GuiProtEnums::paramTimeDist, val);
   if (res()) {
      routeCost = nav2RouteCostToWFAPI(val);
   }
   return res;  
}

SynchronousStatus
SettingsInterfaceImpl::getTrafficInformationUpdateTime(
   wf_uint32& trafficInformationUpdateTime)
{
   // See getCheckRouteForTrafficChanges
   int32 val = 0;
   SynchronousStatus res = getParam(
      isab::GuiProtEnums::userTrafficUpdatePeriod, val);
   if (res()) {
      trafficInformationUpdateTime = (0x3fffffff & wf_uint32(val));
   }
   return res;
}

SynchronousStatus
SettingsInterfaceImpl::getVoiceVerbosity(VoiceVerbosity& voiceVerbosity)
{
   int32 val = 0;
   SynchronousStatus res = getParam(
      isab::GuiProtEnums::paramTurnSoundsLevel, val);
   if (res()) {
      voiceVerbosity = nav2VoiceVerbosityToWFAPI(val);
   }
   return res;
}

SynchronousStatus
SettingsInterfaceImpl::getCurrentTransportationType(
   TransportationType& transportationType)
{
   int32 val = 0;
   SynchronousStatus res = getParam(
      isab::GuiProtEnums::paramTransportationType, val);
   if (res()) {
      transportationType = RouteInterfaceImpl::convertVehicleType(
         isab::NavServerComEnums::VehicleType(val));
   }
   return res;
}


SynchronousStatus
SettingsInterfaceImpl::setAutoReroute(const bool& newVal)
{
   return setParam(isab::GuiProtEnums::paramAutoReroute, int32(newVal));
}

SynchronousStatus
SettingsInterfaceImpl::setAvoidHighway(const bool& newVal)
{
   return setParam(isab::GuiProtEnums::paramHighways, int32(newVal));
}

SynchronousStatus
SettingsInterfaceImpl::setAvoidTollRoad(const bool& newVal)
{
   return setParam(isab::GuiProtEnums::paramTollRoads, int32(newVal));
}

SynchronousStatus
SettingsInterfaceImpl::setCheckRouteForTrafficChanges(const bool& newVal)
{
   // Let nav2 know that user has switched this option off.
   // Set the second highest bit, which is used as a flag.
   // updTime |= 0x40000000;
   int32 val = 0;
   SynchronousStatus res = getParam(
      isab::GuiProtEnums::userTrafficUpdatePeriod, val);
   if (res()) {
      if (newVal) {
         val = (wf_uint32(val) | 0x40000000);
      } else {
         val = (wf_uint32(val) & 0xbfffffff);
      }
      res = setParam(isab::GuiProtEnums::userTrafficUpdatePeriod, val);
      if (res()) {
         // Also set in MapLib.
         if ( getApi()->getConnectedMapLib() != NULL ) {
            getApi()->getConnectedMapLib()->getConfigInterface()->
               getDetailedConfigInterface()->showTrafficInfo(newVal);
         }
      }
   }

   return res;
}

SynchronousStatus
SettingsInterfaceImpl::setDistanceUnit(DistanceUnit newVal)
{
   SynchronousStatus res =  setParam(isab::GuiProtEnums::paramDistanceMode,
                                     WFAPIDistanceUnitToNav2(newVal));
   if (res()) {
      // Also set in MapLib.
      if ( getApi()->getConnectedMapLib() != NULL ) {
        getApi()->getConnectedMapLib()->getConfigInterface()->
           getScaleConfigInterface()->setDistanceUnit(newVal);
      }
   }
   return res;
}

SynchronousStatus
SettingsInterfaceImpl::setRouteCost(RouteCost newVal)
{
   return setParam(
      isab::GuiProtEnums::paramTimeDist, WFAPIRouteCostToNav2(newVal));
}

SynchronousStatus
SettingsInterfaceImpl::setTrafficInformationUpdateTime(wf_uint32 newVal)
{
   // See setCheckRouteForTrafficChanges
   int32 val = 0;
   SynchronousStatus res = getParam(
      isab::GuiProtEnums::userTrafficUpdatePeriod, val);
   if (res()) {
      val = ((wf_uint32(val) & 0x40000000) | (newVal & 0x3fffffff));
      res = setParam(isab::GuiProtEnums::userTrafficUpdatePeriod, val);
      if (res()) {
         // Also set in MapLib.
         if ( getApi()->getConnectedMapLib() != NULL ) {
            getApi()->getConnectedMapLib()->getConfigInterface()->
               getDetailedConfigInterface()->setTrafficInfoUpdateInterval(
                  (newVal & 0x3fffffff));
         }
      }
   }

   return res;
}

void
SettingsInterfaceImpl::setMaxDiskCacheSize(wf_uint32 nbrBytes)
{
   if (getApi()->getConnectedMapLib() != NULL) {
      setParam(isab::GuiProtEnums::paramMapCacheSize, int32(nbrBytes));
      getApi()->getConnectedMapLib()->getConfigInterface()->
         getDetailedConfigInterface()->setMaxDiskCacheSize(nbrBytes);
   }
}

bool
SettingsInterfaceImpl::setCategoryVisible(wf_uint32 id, bool visible)
{
   if (getApi()->getConnectedMapLib() != NULL) {
      // Set it
      bool res = getApi()->getConnectedMapLib()->getConfigInterface()->
         getDetailedConfigInterface()->setCategoryVisible(id, visible);
      if (res) {
         // Get new
         POICategories categories = getApi()->getConnectedMapLib()->
            getConfigInterface()->getDetailedConfigInterface()->
            getPOICategoriesSnapshot();
         // Save
         setPOICategoriesinNav2(categories);
      }

      return res;
   } else {
      return false;
   }
}

bool
SettingsInterfaceImpl::synchronizePOICategories(POICategories& localCategories)
{
   if (getApi()->getConnectedMapLib() != NULL) {
      // Synchronise
      bool res = getApi()->getConnectedMapLib()->getConfigInterface()->
         getDetailedConfigInterface()->synchronizePOICategories(
            localCategories);
      if (res) {
         // Save
         setPOICategoriesinNav2(localCategories);
      }

      return res;
   } else {
      return false;
   }
}


SynchronousStatus
SettingsInterfaceImpl::setVoiceVerbosity(VoiceVerbosity newVal)
{
   return setParam(isab::GuiProtEnums::paramTurnSoundsLevel, 
                   WFAPIVoiceVerbosityTonav2(newVal));
}

SynchronousStatus
SettingsInterfaceImpl::setUsePositionForSearchCategoriesRequests(bool newVal)
{
   return setParam(isab::GuiProtEnums::paramPositionBasedSearchCategories,
                   int32(newVal));
}

SynchronousStatus
SettingsInterfaceImpl::getUsePositionForSearchCategoriesRequests(bool &usePos)
{
   return getParam(
      isab::GuiProtEnums::paramPositionBasedSearchCategories, usePos);
}

DistanceUnit
SettingsInterfaceImpl::nav2DistanceUnitToWFAPI(wf_int32 dist)
{
   switch (isab::DistancePrintingPolicy::DistanceMode(dist)) {
      case isab::DistancePrintingPolicy::ModeMetric:
      case isab::DistancePrintingPolicy::ModeMetricSpace:
         return KM;
      case isab::DistancePrintingPolicy::ModeImperialFeet:
      case isab::DistancePrintingPolicy::ModeImperialFeetSpace:
         return MILES_FEET;
      case isab::DistancePrintingPolicy::ModeImperialYards:
      case isab::DistancePrintingPolicy::ModeImperialYardsSpace:
         return MILES_YARD;

      case isab::DistancePrintingPolicy::ModeInvalid:
         return KM;
   }

   return KM;
}

wf_int32
SettingsInterfaceImpl::WFAPIDistanceUnitToNav2(DistanceUnit dist)
{
   switch (dist) {
      case KM:
         return isab::DistancePrintingPolicy::ModeMetric;
      case MILES_FEET:
         return isab::DistancePrintingPolicy::ModeImperialFeet;
      case MILES_YARD:
         return isab::DistancePrintingPolicy::ModeImperialYards;
   }

   return isab::DistancePrintingPolicy::ModeMetric;
}

RouteCost
SettingsInterfaceImpl::nav2RouteCostToWFAPI(wf_int32 cost)
{
   switch (isab::NavServerComEnums::RouteCostType(cost)) {
      case isab::NavServerComEnums::DISTANCE:
         return DISTANCE;
      case isab::NavServerComEnums::TIME:
         return TIME;
      case isab::NavServerComEnums::TIME_WITH_DISTURBANCES:
      case isab::NavServerComEnums::INAVLID:
         return TIME_WITH_TRAFFIC_DISTURBANCES;
   }

   return TIME_WITH_TRAFFIC_DISTURBANCES;
}

wf_int32
SettingsInterfaceImpl::WFAPIRouteCostToNav2(RouteCost cost)
{
   switch (cost) {
      case DISTANCE:
         return isab::NavServerComEnums::DISTANCE;
      case TIME:
         return isab::NavServerComEnums::TIME;
      case TIME_WITH_TRAFFIC_DISTURBANCES:
         return isab::NavServerComEnums::TIME_WITH_DISTURBANCES;
   }

   return isab::NavServerComEnums::TIME_WITH_DISTURBANCES;
}

VoiceVerbosity
SettingsInterfaceImpl::nav2VoiceVerbosityToWFAPI(wf_int32 verb)
{
   switch(isab::GuiProtEnums::TurnSoundsLevel(verb)) {
      case isab::GuiProtEnums::turnsound_mute:
         return NONE;
      case isab::GuiProtEnums::turnsound_min:
      case isab::GuiProtEnums::turnsound_less:
         return FEW;
      case isab::GuiProtEnums::turnsound_normal:
      case isab::GuiProtEnums::turnsound_more:
      case isab::GuiProtEnums::turnsound_max:
      case isab::GuiProtEnums::turnsound_invalid:
         return FULL;
   }

   return FULL;
}

wf_int32
SettingsInterfaceImpl::WFAPIVoiceVerbosityTonav2(VoiceVerbosity verb)
{
   switch(verb) {
      case NONE:
         return isab::GuiProtEnums::turnsound_mute;
      case FEW:
         return isab::GuiProtEnums::turnsound_less;
      case FULL:
         return isab::GuiProtEnums::turnsound_normal;
   }

   return isab::GuiProtEnums::turnsound_normal;
}

void
SettingsInterfaceImpl::nav2UpdateTimeToValues( 
   wf_int32 nav2Val, 
   bool& checkRouteForTrafficChanges,
   wf_int32& trafficInformationUpdateTime)
{
   checkRouteForTrafficChanges = (wf_uint32(nav2Val) & 0x40000000) != 0;
   trafficInformationUpdateTime = (wf_uint32(nav2Val) & 0x3fffffff);
}


SynchronousStatus
SettingsInterfaceImpl::getParam(wf_uint16 paramID, wf_int32& value)
{
   isab::GeneralParameterMess mess(paramID);

   isab::GuiProtMess* reply = sendSynchronousRequestToNav2(&mess);
   
   // Check status of reply.
   AsynchronousStatus status = createAsynchStatus(
      reply, RequestID(RequestID::INVALID_REQUEST_ID));

   if (status.getStatusCode() != OK) {
      // Bad status.
      mess.deleteMembers();
      reply->deleteMembers();
      delete reply;
      return status;
   }

   // Done get value
   isab::GeneralParameterMess* gpm = static_cast<isab::GeneralParameterMess*>
      (reply);
   if (gpm->getNumEntries() > 0) {
      value = gpm->getIntegerData()[0];
   }

   // Delete members.
   mess.deleteMembers();
   reply->deleteMembers();
   delete reply;
   
   return createOKAsynchStatus();
}

SynchronousStatus
SettingsInterfaceImpl::getParam(wf_uint16 paramID, bool& value)
{
   int32 val = 0;
   SynchronousStatus res = getParam(paramID, val);
   if (res()) {
      value = val != 0;
   }
   return res;
}

SynchronousStatus
SettingsInterfaceImpl::setParam(wf_uint16 paramID, wf_int32 value)
{
   //nav2log << "setParam " << hex << paramID << dec << " value " 
   //          << value << endl;
   isab::GeneralParameterMess mess(paramID, value);

   isab::GuiProtMess* reply = sendSynchronousRequestToNav2(&mess);
   
   // Check status of reply.
   AsynchronousStatus status = createAsynchStatus(
      reply, RequestID(RequestID::INVALID_REQUEST_ID));

   if (status.getStatusCode() != OK) {
      // Bad status.
      nav2log << "setParam " << hex << paramID << dec << " Bad status"
                << endl;
      mess.deleteMembers();
      reply->deleteMembers();
      delete reply;
      return status;
   }

   // Done!

   // Delete members.
   mess.deleteMembers();
   reply->deleteMembers();
   delete reply;
   
   return createOKAsynchStatus();
}

SynchronousStatus
SettingsInterfaceImpl::setPOICategoriesinNav2(POICategories& categories)
{
   // Buffer will resize itself if larger than this.
   isab::Buffer buf(256);

   /* Version number. */
   buf.writeNextUnaligned16bit(VECTOR_MAP_POI_CATEGORY_VERSION);

   for (size_t i = 0; i < categories.size(); ++i) {
      buf.writeNextCharString(categories[i].getName().c_str());
      buf.writeNextUnaligned32bit(categories[i].getID());
      buf.writeNext8bit(categories[i].isVisible());
   }

   /* Cast necessary to lose warning about const. */
   uint8* rawData = const_cast<uint8*>(buf.accessRawData());
   /* Save the buffer in Nav2. */
   isab::GeneralParameterMess gen_mess(
      isab::GuiProtEnums::paramPoiCategories,
      rawData,
      buf.getLength());
      
   // Send it
   isab::GuiProtMess* reply = sendSynchronousRequestToNav2(&gen_mess);
   
   // Check status of reply.
   AsynchronousStatus status = createAsynchStatus(
      reply, RequestID(RequestID::INVALID_REQUEST_ID));

   // Delete members.
   gen_mess.deleteMembers();
   reply->deleteMembers();
   delete reply;

   return status;
}

POICategories
SettingsInterfaceImpl::makePOICategoriesFromNav2(
   isab::GeneralParameterMess* mess)
{
   POICategories categories;

   if (mess->getParamType() == isab::GuiProtEnums::paramPoiCategories &&
       mess->getBinaryData() != NULL) {
      // Create from the buffer
      isab::Buffer buf(const_cast<uint8*>(mess->getBinaryData()),
                       mess->getNumEntries(), mess->getNumEntries());
      while (buf.remaining() >= 4) {
         const char* name = buf.getNextCharString();

         wf_uint32 id = buf.readNextUnaligned32bit();
         bool visible = buf.readNext8bit() != 0;

         categories.push_back(POICategory(name, id, visible));
      }
   }

   return categories;
}

AsynchronousStatus
SettingsInterfaceImpl::isEULAUpdated(const WFString& filename)
{
   m_eulaDataFileName = filename;
   using namespace std;
   isab::NParamBlock eulaRequestParams;
   string crcfileName(getApi()->getSessionData()->getCommonDataStoragePath().c_str());
   crcfileName.append(m_crcData.c_str());

   FILE* crcFile;
   crcFile = fopen(crcfileName.c_str(), "r");

   // Check if there exists a file containing a crc for the eula.
   if(crcFile != NULL) {
      char crcData[100];
      fgets(crcData, 100, crcFile);
      uint32 crc = atol(crcData);
      eulaRequestParams.addParam(isab::NParam(6101, (uint32)crc));   
      fclose(crcFile);
   }
   eulaRequestParams.addParam(isab::NParam(6100, filename.c_str()));

   uint16 nav2Lang = LangConverter::textLanguageToNav2Lang(
      getApi()->getSessionData()->getTextLanguage());
   eulaRequestParams.addParam(isab::NParam(6, nav2Lang));
   return sendAsynchronousNGPRequestToNav2(
      eulaRequestParams, isab::navRequestType::NAV_GET_KEYED_DATA_REQ);
}


void
SettingsInterfaceImpl::handleEULAReply(const isab::DataGuiMess& mess)
{
   isab::NParamBlock messageParams(mess.getAdditionalData(),
                                   mess.getAdditionalSize(),
                                   NSC_PROTO_VER);

   const isab::NParam* crcParam = messageParams.getParam(6101);
   // Check if message contains a CRC, if not there are also no data.

   if(crcParam != NULL) {
      // The message contains data and therefor also contains a new eula.
      wf_uint32 crcFromMessage = crcParam->getUint32();
      std::vector<const NParam*> dataParams;
      messageParams.getAllParams(6102, dataParams);

      // Collect the eula from the message.
      std::vector<const NParam*>::const_iterator paramIt = dataParams.begin();
      uint32 eulaTotalLength = 0;
      for(; paramIt != dataParams.end(); paramIt++) {
         eulaTotalLength += (*paramIt)->getLength();
      }
      // Allocate space for the buffer + newline
      uint8* eulaData = new uint8[eulaTotalLength + 1];
      paramIt = dataParams.begin();
      uint32 eulaDataIndex = 0;
      uint32 index = 0;
      for(; paramIt != dataParams.end(); paramIt++) {
         const uint8* paramBuf = (*paramIt)->getByteArray();
         for(index = 0; index < (*paramIt)->getLength() ; index++) {
            eulaData[eulaDataIndex] = paramBuf[index];
            eulaDataIndex++;
         }
      }
      eulaData[eulaDataIndex] = '\0';

      // Saves the new CRC in the crcFile.
      string crcfileName(getApi()->getSessionData()->
                         getCommonDataStoragePath().c_str());
      crcfileName.append(m_crcData.c_str());
      
      FILE* crcFile;
      crcFile = fopen(crcfileName.c_str(), "w");
      if(crcFile != NULL) {
         fprintf(crcFile, "%d", crcFromMessage);
         fclose(crcFile);
      }
      
      // Saves the new eula to file, the old one is erased.
      string eulaDataFileName(getApi()->getSessionData()->
                              getCommonDataStoragePath().c_str());
      eulaDataFileName.append(m_eulaDataFileName.c_str());
      FILE* eulaDataFile;
      eulaDataFile = fopen(eulaDataFileName.c_str(), "w");
      if(eulaDataFile != NULL) {
         fprintf(eulaDataFile, "%s", eulaData);
         fclose(eulaDataFile);
      }
      for(ListenerCont::iterator it = m_listeners.begin();
          it != m_listeners.end(); ++it) {
         (*it)->eulaAvailable(true, m_eulaDataFileName);
      }
      
      delete[] eulaData; //Delete the eula after it has been written to file.
   } else {
      // There are no new eula at the server.
      for(ListenerCont::iterator it = m_listeners.begin();
          it != m_listeners.end(); ++it) {
         (*it)->eulaAvailable(false, m_eulaDataFileName);
      }
   }    
}

SynchronousStatus
SettingsInterfaceImpl::getUsername(WFString& username)
{
   isab::GeneralParameterMess mess(isab::GuiProtEnums::paramWebUsername);
   
   isab::GuiProtMess* reply = sendSynchronousRequestToNav2(&mess);

   // Check status of reply.
   AsynchronousStatus status = createAsynchStatus(
      reply, RequestID(RequestID::INVALID_REQUEST_ID));

   if (status.getStatusCode() != OK) {
      // Bad status.
      mess.deleteMembers();
      reply->deleteMembers();
      delete reply;
      return status;
   }

   isab::GeneralParameterMess* gpm =
      static_cast<isab::GeneralParameterMess*>(reply);

   if (gpm->getNumEntries() > 0) {
      username = (gpm->getStringData())[0];
   }

   // Delete members.
   mess.deleteMembers();
   reply->deleteMembers();
   delete reply;

   return createOKAsynchStatus();
   
}

SynchronousStatus
SettingsInterfaceImpl::setPassword(const WFString& password)
{

   isab::GeneralParameterMess mess(isab::GuiProtEnums::paramWebPassword,
                                   const_cast<char *> (password.c_str()));

   isab::GuiProtMess* reply = sendSynchronousRequestToNav2(&mess);

   // Check status of reply.
   AsynchronousStatus status = createAsynchStatus(
      reply, RequestID(RequestID::INVALID_REQUEST_ID));

   if (status.getStatusCode() != OK) {
      // Bad status.
      mess.deleteMembers();
      reply->deleteMembers();
      delete reply;
      return status;
   }

   // Delete members.

   reply->deleteMembers();
   delete reply;

   // Done
   return createOKAsynchStatus();
}

SynchronousStatus
SettingsInterfaceImpl::setUIN(const WFString& UIN)
{
   char* uinstr[2];
   uinstr[0] = const_cast<char*>(UIN.c_str());
   uinstr[1] = "ChangeMe";

   isab::GeneralParameterMess
      mess(static_cast<uint16>(isab::GuiProtEnums::paramUserAndPassword),
           static_cast<char**>(uinstr), static_cast<int32>(2));

   isab::GuiProtMess* reply = sendSynchronousRequestToNav2(&mess);

   // Check status of reply.
   AsynchronousStatus status = createAsynchStatus(
      reply, RequestID(RequestID::INVALID_REQUEST_ID));

   if (status.getStatusCode() != OK) {
      // Bad status.
      mess.deleteMembers();
      reply->deleteMembers();
      delete reply;
      return status;
   }

   // Delete members.

   reply->deleteMembers();
   delete reply;

   /*
    * We need to also get the new uin to make sure that the new uin is known to
    * all modules and parts of nav2. We also test this way that the uin that
    * has been set is the same as we wanted to set.
    */ 
   
   isab::GeneralParameterMess checkMess(isab::GuiProtEnums::paramUserAndPassword);
   
   reply = sendSynchronousRequestToNav2(&mess);

   // Check status of reply.
   status = createAsynchStatus(reply, RequestID(RequestID::INVALID_REQUEST_ID));

   if (status.getStatusCode() != OK) {
      // Bad status.
      checkMess.deleteMembers();
      reply->deleteMembers();
      delete reply;
      return status;
   }

   isab::GeneralParameterMess* gpm =
      static_cast<isab::GeneralParameterMess*>(reply);
   WFString retrievedUIN;
   bool success = false;
   if (gpm->getNumEntries() > 0) {
      retrievedUIN = (gpm->getStringData())[0];
      // Check the retrieved UIN the same as the one we set.
      if(retrievedUIN == UIN) {
         coreprintln("Successfull change of UIN");
         success = true;
      } else {
         // This should never happen.
         coreprintln("Failed to change UIN");
      }
   }

   // Delete members.
   checkMess.deleteMembers();
   reply->deleteMembers();
   delete reply;

   // check if the set of UIN was successfull or not.
   if(success) {
      return createOKAsynchStatus();
   }
   return SynchronousStatus(GENERAL_ERROR,"","");
}

void
SettingsInterfaceImpl::handleNewVersionMsg(const isab::NewVersionMess& mess)
{
   m_versionInfo.version = mess.getVersion();
   m_versionInfo.url = mess.getUrl();
   m_versionInfo.versionString = mess.getVersionString();
   m_versionInfo.force = mess.isForce();
   for(ListenerCont::const_iterator it = m_listeners.begin();
       it != m_listeners.end(); ++it) {
      (*it)->newVersionAvailable(m_versionInfo.version, m_versionInfo.url,
                                 m_versionInfo.versionString, m_versionInfo.force);
   }   
}

void
SettingsInterfaceImpl::addSettingsListener(SettingsListener* listener)
{
   m_listeners.insert(listener);
}

void
SettingsInterfaceImpl::removeSettingsListener(SettingsListener* listener)
{
   m_listeners.erase(listener);
}

SynchronousStatus
SettingsInterfaceImpl::getNewVersionInformation(WFString& version,
                                                WFString& url,
                                                WFString& versionString,
                                                bool& force)
{
   version = m_versionInfo.version;
   url = m_versionInfo.url;
   versionString = m_versionInfo.versionString;
   force = m_versionInfo.force;
   return createOKAsynchStatus();
}

} // End namespace WFAPI.




