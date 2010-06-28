/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "LocationInterfaceImpl.h"
#include "LocationInterface.h"
#include "Nav2APIChannel.h"
#include "LocationUpdateInformation.h"
#include "AreaUpdateInformation.h"
#include "LocationListener.h"
#include "MC2Coordinate.h"
#include <math.h>
#include "nav2util.h"
#include "Nav2Logging.h"
#include "Quality.h"
// Nav2
#include "NavRequestType.h"
#include "NParamBlock.h"
#include "NavServerComProtoVer.h"
// GuiProt
#include "GuiProt/GuiProtEnums.h"
#include "GuiProt/GuiProtMess.h"

using namespace std;

namespace WFAPI {

//LocationInterfaceImpl::

LocationInterfaceImpl::CellIDRequestData::CellIDRequestData(
   const WFString& networkType, 
   const WFString& signalStrength)
      : m_networkType(networkType), m_signalStrength(signalStrength)
{
}

LocationInterfaceImpl::TGPPCellIDRequestData::TGPPCellIDRequestData(
   const WFString& networkType, 
   const WFString& signalStrength,
   const WFString& currentMCC,
   const WFString& currentMNC,
   const WFString& currentLAC,
   const WFString& cellID)
      : CellIDRequestData(networkType, signalStrength),
        m_currentMCC(currentMCC), m_currentMNC(currentMNC),
        m_currentLAC(currentLAC), m_cellID(cellID)
{
}

void
LocationInterfaceImpl::TGPPCellIDRequestData::addParamsTo(
   isab::NParamBlock& rd) const
{
   // Network type
   rd.addParam(isab::NParam(6200, m_networkType.c_str()));
   // Current SignalStrength
   rd.addParam(isab::NParam(6201, m_signalStrength.c_str()));
   // Current MCC
   rd.addParam(isab::NParam(6202, m_currentMCC.c_str()));
   // Current MNC
   rd.addParam(isab::NParam(6203, m_currentMNC.c_str()));
   // Current LAC
   rd.addParam(isab::NParam(6204, m_currentLAC.c_str()));
   // Current cellID
   rd.addParam(isab::NParam(6205, m_cellID.c_str()));
}

bool 
LocationInterfaceImpl::TGPPCellIDRequestData::operator == (
   const CellIDRequestData& o) const
{
   const TGPPCellIDRequestData& other = 
      static_cast<const TGPPCellIDRequestData&>(o);
   return m_networkType == other.m_networkType && 
      m_signalStrength == other.m_signalStrength &&
      m_currentMCC == other.m_currentMCC &&
      m_currentMNC == other.m_currentMNC &&
      m_currentLAC == other.m_currentLAC &&
      m_cellID == other.m_cellID;
}

LocationInterfaceImpl::CDMACellIDRequestData::CDMACellIDRequestData(
   const WFString& networkType,
   const WFString& signalStrength,
   const WFString& currentSID,
   const WFString& currentNID,
   const WFString& currentBID)
      : CellIDRequestData(networkType, signalStrength),
        m_currentSID(currentSID), m_currentNID(currentNID), 
        m_currentBID(currentBID)
{
}

void
LocationInterfaceImpl::CDMACellIDRequestData::addParamsTo(
   isab::NParamBlock& rd) const
{
   // Network type
   rd.addParam(isab::NParam(6200, m_networkType.c_str()));
   // Current SignalStrength
   rd.addParam(isab::NParam(6201, m_signalStrength.c_str()));
   // CurrentSID
   rd.addParam(isab::NParam(6206, m_currentSID.c_str()));
   // CurrentNID
   rd.addParam(isab::NParam(6207, m_currentNID.c_str()));
   // CurrentBID
   rd.addParam(isab::NParam(6208, m_currentBID.c_str()));
}

bool 
LocationInterfaceImpl::CDMACellIDRequestData::operator == (
   const CellIDRequestData& o) const
{
   const CDMACellIDRequestData& other = 
      static_cast<const CDMACellIDRequestData&>(o);
   return m_networkType == other.m_networkType && 
      m_signalStrength == other.m_signalStrength &&
      m_currentSID == other.m_currentSID &&
      m_currentNID == other.m_currentNID &&
      m_currentBID == other.m_currentBID;
}

LocationInterfaceImpl::IDENCellIDRequestData::IDENCellIDRequestData(
   const WFString& networkType,
   const WFString& signalStrength,
   const WFString& currentMCC,
   const WFString& currentDNC,
   const WFString& currentSAID,
   const WFString& currentLLAID,
   const WFString& currentCELLID)
      : CellIDRequestData(networkType, signalStrength),
        m_currentMCC(currentMCC), m_currentDNC(currentDNC), 
        m_currentSAID(currentSAID), m_currentLLAID(currentLLAID),
        m_currentCELLID(currentCELLID)
{
}

void
LocationInterfaceImpl::IDENCellIDRequestData::addParamsTo(
   isab::NParamBlock& rd) const
{
   // Network type
   rd.addParam(isab::NParam(6200, m_networkType.c_str()));
   // Current SignalStrength
   rd.addParam(isab::NParam(6201, m_signalStrength.c_str()));
   // CurrentMCC
   rd.addParam(isab::NParam(6209, m_currentMCC.c_str()));
   // CurrentDNC
   rd.addParam(isab::NParam(6210, m_currentDNC.c_str()));
   // CurrentSA_ID
   rd.addParam(isab::NParam(6211, m_currentSAID.c_str()));
   // CurrentLLA_ID
   rd.addParam(isab::NParam(6212, m_currentLLAID.c_str()));
   // CurrentCell_ID
   rd.addParam(isab::NParam(6213, m_currentCELLID.c_str()));
}

bool 
LocationInterfaceImpl::IDENCellIDRequestData::operator == (
   const CellIDRequestData& o) const
{
   const IDENCellIDRequestData& other = 
      static_cast<const IDENCellIDRequestData&>(o);
   return m_networkType == other.m_networkType && 
      m_signalStrength == other.m_signalStrength &&
      m_currentMCC == other.m_currentMCC &&
      m_currentDNC == other.m_currentDNC &&
      m_currentSAID == other.m_currentSAID &&
      m_currentLLAID == other.m_currentLLAID &&
      m_currentCELLID == other.m_currentCELLID;
}



LocationInterfaceImpl::LocationInterfaceImpl(Nav2APIImpl* api) :
   InterfaceImpl(api),
   m_currentPosition(),
   m_currentRoutePosition()
{
   
}

LocationInterfaceImpl::~LocationInterfaceImpl()
{

}

void
LocationInterfaceImpl::addLocationListener(LocationListener* listener)
{
   m_listeners.insert(listener);
}

void
LocationInterfaceImpl::removeLocationListener(LocationListener* listener)
{
   m_listeners.erase(listener);
}

void
LocationInterfaceImpl::receiveAsynchronousReply(
   const isab::GuiProtMess& guiProtMess, RequestID requestID)
{
   // Enough with all the isab prefixing..
   using namespace isab;

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
   case GuiProtEnums::SATELLITE_INFO:
      break;
   case GuiProtEnums::UPDATE_POSITION_INFO:
   {
      //GPS data
      const isab::UpdatePositionMess& dataMessage =
         static_cast<const isab::UpdatePositionMess&>(guiProtMess);

      // Nav2Coordinate currentPosition(dataMessage.getLat(),
      //                                dataMessage.getLon());
      //Save the current position;
      WGS84Coordinate newPos = Nav2Coordinate(dataMessage.getLat(),
                                              dataMessage.getLon());

      if (newPos.isValid()){
         // TODO: Add timestamp to check when needed.
         m_currentPosition = newPos;
      }

      // Nav2Coordinate currentRoutePosition(dataMessage.getRouteLat(),
      //                                     dataMessage.getRouteLon());
      
      // TODO: Should store the latest snapped position 
      m_currentRoutePosition = Nav2Coordinate(dataMessage.getRouteLat(),
                                              dataMessage.getRouteLon());
      
      // NavTask::decodedGpsPositionVelocityFunc add 10x to altitude
      // and 32x to speed.
      LocationUpdateInformation locUpdInfo(
         //Nav2Coordinate(dataMessage.getLat(), dataMessage.getLon()),
         newPos,
         dataMessage.getAlt()/10,
         wf_uint16(double(dataMessage.getHeading())/256.0*360.0),
         convertToGpsQualityEnum(dataMessage.positionQuality()),
         wf_uint16(isab::round_int(dataMessage.getSpeed()*3.6/32)),
         m_currentRoutePosition,
         wf_uint16(double(dataMessage.getRouteHeading())/256.0*360.0));
      nav2log << "locUpdInfo coord " << locUpdInfo.getPosition() << endl
                << " getAltitude " << locUpdInfo.getAltitude() << endl
                << " getHeading " << locUpdInfo.getHeading() << endl
                << " getGpsQuality " << locUpdInfo.getGpsQuality() << endl
                << " getSpeed " << locUpdInfo.getSpeed() << endl;
      
      for (ListenerCont::iterator it = m_listeners.begin(); 
           it != m_listeners.end(); ++it) {
         (*it)->locationUpdate(locUpdInfo);
      }
   }
   break;

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
            case isab::navRequestType::NAV_CELLID_LOOKUP_REPLY:
               // Cell ID lookup reply
               handleCellIDLookupReply(mess);
               break;
            default:
               break;
         }
         
         break;
      }
   default:
      nav2log << "LocationInterfaceImpl::receiveAsynchronousReply of "
                << "unknown type: " << int(type) << endl;
      break;
   }  
}

void 
LocationInterfaceImpl::setBtChannel(Nav2APIChannel* btChannel)
{
   m_btChannel = btChannel;
}
Nav2APIChannel*
LocationInterfaceImpl::getBtChannel()
{
   return m_btChannel;
}

AsynchronousStatus
LocationInterfaceImpl::sendCellIDRequest(const CellIDRequestData& req)
{
   isab::NParamBlock params;
   req.addParamsTo(params);

   return sendAsynchronousNGPRequestToNav2(
      params, isab::navRequestType::NAV_CELLID_LOOKUP_REQ);
}

void
LocationInterfaceImpl::handleCellIDLookupReply(const isab::DataGuiMess& mess)
{
   isab::NParamBlock params(mess.getAdditionalData(), 
                            mess.getAdditionalSize(),
                            NSC_PROTO_VER);

   // Extract data
   const isab::NParam* coordP  = params.getParam(6300);
   wf_int32 nav2Lat = coordP->getInt32(0);
   wf_int32 nav2Lon = coordP->getInt32(4);
   WGS84Coordinate coord = Nav2Coordinate(nav2Lat, nav2Lon);
   
   if (!m_currentPosition.isValid()){
      // TODO: Add timestamp for checking validity.
      m_currentPosition = coord;
   }
   wf_int32 altitude           = params.getParam(6301)->getInt32();
   wf_uint32 innerRadius       = params.getParam(6302)->getUint32();
   wf_uint32 outerRadius       = params.getParam(6303)->getUint32();
   wf_uint16 startAngle        = params.getParam(6304)->getUint16();
   wf_uint16 endAngle          = params.getParam(6305)->getUint16();

   AreaUpdateInformation info(coord, altitude, innerRadius, outerRadius, 
                              startAngle, endAngle);
   // Send to listeners
   for (ListenerCont::const_iterator it = getListeners().begin();
        it != getListeners().end(); ++it ) {
      (*it)->areaUpdate(info);
   }

   // Request again?! Subclasses may do that.
}

const LocationInterfaceImpl::ListenerCont&
LocationInterfaceImpl::getListeners()
{
   return m_listeners;
}

SynchronousStatus LocationInterfaceImpl::setLocationHardware(wf_uint8 hardware) {
   //this implementation should not do anything, ie not change the default
   // behaviour of using all hardware available.
   return SynchronousStatus(GENERAL_ERROR, "", "");
}

WGS84Coordinate
LocationInterfaceImpl::getCurrentPosition() const
{
   return m_currentPosition;
}

WGS84Coordinate
LocationInterfaceImpl::getCurrentRoutePostion() const
{
   return m_currentRoutePosition;
}

GpsQualityEnum
LocationInterfaceImpl::convertToGpsQualityEnum(wf_uint8 nav2Quality)
{
   switch(isab::Quality(nav2Quality)) {
   case isab::QualityMissing:
      return QUALITY_MISSING;
   case isab::QualitySearching:
      return QUALITY_SEARCHING;
   case isab::QualityUseless:
      return QUALITY_USELESS;
   case isab::QualityPoor:
      return QUALITY_POOR;
   case isab::QualityDecent:
      return QUALITY_DESCENT;
   case isab::QualityExcellent:
      return QUALITY_EXCELLENT;
   case isab::QualityDemohx:
   case isab::QualityDemo1x: 
   case isab::QualityDemo2x: 
   case isab::QualityDemo4x:
      return QUALITY_USELESS;
   }
   return QUALITY_USELESS;
}

} // End namespace WFAPI


