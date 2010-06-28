/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _WMLOCATIONINTERFACEIMPL_H_
#define _WMLOCATIONINTERFACEIMPL_H_

#include "LocationInterfaceImpl.h"
#include "LocationInterface.h"
#include "PositionListener.h"
#include "CellIDListener.h"

namespace WFAPI {

class CGPSAPIReader;
class CellIDReader;

/**
 * Windows mobile specific lbs class. Passes on position messages
 * and errors to Nav2 and LocationInterface users.
 */
class WMLocationInterfaceImpl : public LocationInterfaceImpl,
                                public PositionListener,
                                public CellIDListener
{
public:
   /**
    * Constructor.
    *
    * @param api A pointer to the Nav2APIImpl.
    */
   WMLocationInterfaceImpl(Nav2APIImpl* api);

   /**
    * Destructor
    */
   virtual ~WMLocationInterfaceImpl();

   /**
    * Checks if the client is registered.
    *
    * @return The status LBS_STATUS_STARTED if the lbs has been started,
    *         LBS_STATUS_NOT_STARTED if the lbs has not been started.
    */    
   virtual SynchronousStatus lbsStarted();
   
   /**
    * This asynchronous function starts up the LBS. When operation is done
    * the added LocationListeners will get a callback to startedLbs if
    * successfully started, otherwise error will get called with a error
    * code from StatusCode or LocationStatusCode
    *
    * @return The status and if status is OK.
    */
   virtual AsynchronousStatus startLbs();
   
   /**
    * Stop LBS updates.
    *
    * @return The status of the operation, OK if operation succeeded and
    *         either a status from StatusCode or LocationStatusCode.
    */
   virtual SynchronousStatus stopLbs();

   /**
    * From PositionListener, gets called every second when a new position
    * has been fetched from the gps module.
    *
    * @param nmeaString A string containing position values in nmea format.
    */
   virtual void positionInfoUpdated(const char* nmeaString);

   /**
    * @see CellIDListener::cellIDInfoUpdated
    */
   virtual void cellIDInfoUpdated(const WFString& networkType, 
                                  const WFString& signalStrength,
                                  const WFString& currentMCC,
                                  const WFString& currentMNC,
                                  const WFString& currentLAC,
                                  const WFString& cellID);

   /**
    * A helper function that notifies the location listeners
    * that the location based services system has been started.
    */
   void WMLocationInterfaceImpl::notifyListenersStarted();

   /**
    * A helper function that tells nav2 that the gps is connected or
    * disconnected.
    * 
    * @param connect True if the gps should be connected, false if not.
    */
   void connectGpsNav2(bool connect);
   
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
   SynchronousStatus setLocationHardware(wf_uint8 hardware);

private:

   /**
    * This function notifies the listeners that the lbs has been started.
    */
   void sendStartedCallback();
   
   /// The engine that handles connection and reading data from the gps module.
   CGPSAPIReader* m_gpsApiReader;
   CellIDReader* m_cellIDReader;

   /// This is set to true when the lbs system has been started.
   bool m_lbsStarted;

   /// A member holding the hardware we're currently using internally to get positions
   wf_uint8 m_currentHardware;
};


} // End of namespace WFAPI


#endif /* _WMLOCATIONINTERFACEIMPL_H_ */
