/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_LOCATION_INTERFACE_IMPL_H
#define WFAPI_LOCATION_INTERFACE_IMPL_H

#include "InterfaceImpl.h"
#include "LocationInterface.h"
#include "WGS84Coordinate.h"
#include "GpsQualityEnum.h"
#include <set>

#include <vector>
#include <string>

namespace isab {
// Forward declarations
class GuiProtMess;
class NParamBlock;
class DataGuiMess;
}

namespace WFAPI {

// Forward declarations
class Nav2APIChannel;
class LocationListener;

typedef std::vector<std::string> NMEAVec;

class LocationInterfaceImpl : public InterfaceImpl {
public:
   
   /**
    * Constructor.
    *
    * @param api A pointer to the Nav2APIImpl.
    */
   LocationInterfaceImpl(Nav2APIImpl* api);

   /**
    * Destructor
    */
   virtual ~LocationInterfaceImpl();

   /**
    * Add a listener for location updates. 
    * @see LocationListener
    *
    * @param listener The LocationListener to add. 
    *                 It may not be deleted until it is removed by
    *                 calling removeLocationListener.
    */
   void addLocationListener(LocationListener* listener);

   /**
    * Remove a listener.
    *
    * @param listener The LocationListener to remove.
    *                 It should now be deleted by caller, unless caller
    *                 keeps it around for later use.
    */
   void removeLocationListener(LocationListener* listener);

   /**
    * Checks if the lbs engine is started or not.
    *
    * @return The status LBS_STATUS_STARTED if the lbs has been started,
    *         LBS_STATUS_NOT_STARTED if the lbs has not been started.
    */
   virtual SynchronousStatus lbsStarted() = 0;
   
   /**
    * This asynchronous function starts up the LBS. When operation is done
    * the added LocationListeners will get a callback to startedLbs if
    * successfully started, otherwise error will get called with a error
    * code from StatusCode or LocationStatusCode
    *
    * @return The status and if status is OK.
    */
   virtual AsynchronousStatus startLbs() = 0;
   
   /**
    * Stop LBS updates.
    *
    * @return The status of the operation, OK if operation succeeded and
    *         either a status from StatusCode or LocationStatusCode.
    */
   virtual SynchronousStatus stopLbs() = 0;

   /**
    * Set the Location hardware to use right now.
    * Not calling this method will cause the library to use all available
    * hardware.
    *
    * @param hardware The hardware to use.
    *                 WF_MAX_UINT8 = all available hardware,
    *                 0 = no hardware,
    *                 1 = gps only,
    *                 2 = CellID only,
    *                 3 = gps and CellID etc.
    *
    * @return The status of the operation, OK if operation succeeded and
    *         either a status from StatusCode or LocationStatusCode.
    */
   virtual SynchronousStatus setLocationHardware(wf_uint8 hardware);
   
   /**
    * Get the current position.
    *
    * @return The current position as a WGS84Coordinate. 
    */
   WGS84Coordinate getCurrentPosition() const;

   /**
    * Get the current route position, this position should only be used
    * during a route. This position is snapped to the route.
    *
    * @return The snapped position;
    */
   WGS84Coordinate getCurrentRoutePostion() const;
   
   /**
    * Function called when sending reply from Nav2. This is called when
    * receiving replies from asynchronous requests.
    *
    * @param guiProtMess The returning data of the reply.
    * @param requestID The RequestID for the guiProtMess.
    */
   void receiveAsynchronousReply(const isab::GuiProtMess& guiProtMess,
                                 RequestID requestID);

   /**
    * Sets the m_btChannel to a valid pointer, now we can use this
    * channel for writing nmea data to Nav2.
    *
    * @param btChannel An initialized serial channel that will be used
    *                  for writing nmea data to.
    */
   virtual void setBtChannel(Nav2APIChannel* btChannel);

   
   /**
    * Classes can override this function to implement NMEA reply.
    *
    * @param vec A vector of strings that represent NMEA messages
    */ 
   virtual void replayNMEAData(NMEAVec& vec) { }
protected:

   /**
    * Abstract super class for all cellID lookup request data.
    */
   class CellIDRequestData {
   public:
      CellIDRequestData(const WFString& networkType, 
                        const WFString& signalStrength);

      virtual void addParamsTo(isab::NParamBlock& rd) const = 0;

      virtual bool operator == (const CellIDRequestData& o) const = 0;

   protected:

      WFString m_networkType;
      WFString m_signalStrength;
   };

   /**
    * CellID lookup request data for 3GPP, gsm, networks.
    */
   class TGPPCellIDRequestData : public CellIDRequestData {
   public:
      TGPPCellIDRequestData(const WFString& networkType, 
                            const WFString& signalStrength,
                            const WFString& currentMCC,
                            const WFString& currentMNC,
                            const WFString& currentLAC,
                            const WFString& cellID);

      void addParamsTo(isab::NParamBlock& rd) const;

      bool operator == (const CellIDRequestData& o) const;

   private:

      WFString m_currentMCC;
      WFString m_currentMNC;
      WFString m_currentLAC;
      WFString m_cellID;
   };

   /**
    * CellID lookup request data for CDMA networks.
    */
   class CDMACellIDRequestData : public CellIDRequestData {
   public:
      CDMACellIDRequestData(const WFString& networkType,
                            const WFString& signalStrength,
                            const WFString& currentSID,
                            const WFString& currentNID,
                            const WFString& currentBID);

      void addParamsTo(isab::NParamBlock& rd) const;

      bool operator == (const CellIDRequestData& o) const;

   private:

      WFString m_currentSID;
      WFString m_currentNID;
      WFString m_currentBID;
   };

   /**
    * CellID lookup request data for iDEN networks.
    */
   class IDENCellIDRequestData : public CellIDRequestData {
   public:
      IDENCellIDRequestData(const WFString& networkType,
                            const WFString& signalStrength,
                            const WFString& currentMCC,
                            const WFString& currentDNC,
                            const WFString& currentSAID,
                            const WFString& currentLLAID,
                            const WFString& currentCELLID);

      void addParamsTo(isab::NParamBlock& rd) const;

      bool operator == (const CellIDRequestData& o) const;

   private:

      WFString m_currentMCC;
      WFString m_currentDNC;
      WFString m_currentSAID;
      WFString m_currentLLAID;
      WFString m_currentCELLID;
   };

   /**
    * Getter for the channel to write nmea data to, ends up in Nav2.
    *
    * @return The channel to Nav2 to send nmea data via.
    */
   virtual Nav2APIChannel* getBtChannel();
   
   /**
    * Send a cell id request.
    *
    * @param req The cell id data to send.
    * @return The status of the operation.
    */
   AsynchronousStatus sendCellIDRequest(const CellIDRequestData& req);

   /**
    * Handle a cell id lookup reply.
    */
   virtual void handleCellIDLookupReply(const isab::DataGuiMess& mess);

   typedef std::set<LocationListener*> ListenerCont;

   /**
    * @see InterfaceImpl.
    */
   virtual const ListenerCont& getListeners();

   /**
    * Internal convert function that converts from isab::Quality
    * to GpsQualityEnum value.
    *
    * @param nav2Quality The quality to convert.
    * @return The converted value.
    */
   GpsQualityEnum convertToGpsQualityEnum(wf_uint8 nav2Quality);
   
private:

   /// Channel to write nmea data to nav2 in.
   Nav2APIChannel* m_btChannel;
   
   /**
    * Set of listeners.
    */
   ListenerCont m_listeners;

   /// Holds the current position
   WGS84Coordinate m_currentPosition;

   /// Holds the current snapped position
   WGS84Coordinate m_currentRoutePosition;

};
} // End namespace WFAPI

#endif // End WFAPI_LOCATION_INTERFACE_IMPL_H
