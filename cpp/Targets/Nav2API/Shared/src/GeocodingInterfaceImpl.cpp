/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "GeocodingInterfaceImpl.h"
#include "NParamBlock.h"
#include "LanguageHelper.h"
#include "Nav2APIImpl.h"
#include "MC2Coordinate.h"
#include "NavRequestType.h"
#include "GuiProt/GuiProtEnums.h"
#include "GuiProt/GuiProtMess.h"
#include "NavServerComProtoVer.h"
#include "Utility.h"
#include "Nav2Logging.h"

#include <utility>

namespace WFAPI{

GeocodingInterfaceImpl::GeocodingInterfaceImpl(Nav2APIImpl* api)
   : InterfaceImpl(api)
{
   
}

GeocodingInterfaceImpl::~GeocodingInterfaceImpl()
{
    
}

void
GeocodingInterfaceImpl::addGeocodingListener(GeocodingListener* listener)
{
   m_geoListeners.insert(listener);
}

void
GeocodingInterfaceImpl::removeGeocodingListener(GeocodingListener* listener)
{
   m_geoListeners.erase(listener);
}


AsynchronousStatus GeocodingInterfaceImpl::requestReverseGeocoding(
   const WGS84Coordinate& coord,
   GeocodingInformation::Precision desiredPrecision)
{
   isab::NParamBlock params;
   
   TextLanguage lang =
      getApi()->getSessionData()->getTextLanguage();
   isab::NParam langParam(6);
   langParam.addUint16(uint16(LanguageHelper::textLanguageToNav2(lang)));
   params.addParam(langParam);

   isab::NParam coordParam(1400);
   Nav2Coordinate nav2coord(coord);
   
   coordParam.addInt32(nav2coord.nav2lat);
   coordParam.addInt32(nav2coord.nav2lon);

   params.addParam(coordParam);
   AsynchronousStatus ret = sendAsynchronousNGPRequestToNav2(
      params, isab::navRequestType::NAV_REV_GEOCODING_REQ);

   m_requestHolder.insert(std::make_pair(ret.getRequestID(), desiredPrecision));

   return ret;
}

void
GeocodingInterfaceImpl::handleRevGeocodingReply(const isab::DataGuiMess& mess,
                                                const RequestID& id)
{

   RequestHolder::iterator requestInfo = m_requestHolder.find(id);
   if(requestInfo == m_requestHolder.end()) {
      //Request id not found in m_requestHolder, so we return.
      return;
   }
   
   isab::NParamBlock params(mess.getAdditionalData(), 
                            mess.getAdditionalSize(),
                            NSC_PROTO_VER);

   GeocodingInformation info;

   // We only return the information specified by the precision
   // of the last request. Due to fall-through, every field 'below'
   // the specified precision will be filled.
   switch (requestInfo->second) // i.e precision
   {
      case GeocodingInformation::ADDRESS:
         if (params.getParam(1504) != NULL) {
            info.addressName = params.getParam(1504)->getString();
         }
      case GeocodingInformation::DISTRICT:
         if (params.getParam(1503) != NULL) {
            info.districtName = params.getParam(1503)->getString();
         }
      case GeocodingInformation::CITY:
         if (params.getParam(1502) != NULL) {
            info.cityName = params.getParam(1502)->getString();
         }
      case GeocodingInformation::MUNICIPAL:
         if (params.getParam(1501) != NULL) {
            info.municipalName = params.getParam(1501)->getString();
         }
      case GeocodingInformation::COUNTRY:
         if (params.getParam(1500) != NULL) {
            info.countryName = params.getParam(1500)->getString();
         }
      default:
         break;
   }
   info.topRegionID = MAX_UINT32;
   if(params.getParam(1505) != NULL) {
      info.topRegionID = params.getParam(1505)->getUint32();
   }

   info.highestPrecision = requestInfo->second;
   // PRINT_VAL(countryName);
   // PRINT_VAL(municipalName);
   // PRINT_VAL(cityName);
   // PRINT_VAL(districtName);
   // PRINT_VAL(addressName);

   // WFAPI::WGS84Coordinate w = Nav2Coordinate(97035245,22704988);

   // PRINT_VAL(w);

   for (ListenerCont::const_iterator it = m_geoListeners.begin();
        it != m_geoListeners.end(); ++it ) {
      (*it)->reverseGeocodingReply(id, info);
   }
   m_requestHolder.erase(requestInfo);

}
void
GeocodingInterfaceImpl::receiveAsynchronousReply(
   const isab::GuiProtMess& guiProtMess,
   RequestID requestID)
{
   // Enough with all the isab prefixing..
   using namespace isab;

   // Get the status.
   AsynchronousStatus status = createAsynchStatus(&guiProtMess, requestID);
   
   // Check status.
   if (status.getStatusCode() != OK) {
      // Bad status. Report error function for all listeners.
      reportError(status, m_geoListeners.begin(), m_geoListeners.end());
      return;
   }
   
   isab::GuiProtEnums::MessageType type = guiProtMess.getMessageType();
   switch (type) {
      // This is the case we are interested in, a NGP data reply.
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
            case isab::navRequestType::NAV_REV_GEOCODING_REPLY:
               // Reply from reverse geocoding request.
               handleRevGeocodingReply(mess, requestID);
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



} // End of namespace WFAPI.


