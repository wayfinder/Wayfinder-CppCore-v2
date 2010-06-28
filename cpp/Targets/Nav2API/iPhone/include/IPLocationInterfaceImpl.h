/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _IPLOCATIONINTERFACEIMPL_H_
#define _IPLOCATIONINTERFACEIMPL_H_

#include "LocationInterfaceImpl.h"
#include "LocationInterface.h"
#include "IPhoneLocationUtil.h"
#include "IPLBSListener.h"

class IPhoneLocationUtil;

namespace WFAPI {

/**
 * iPhone specific lbs class. Passes on position messages
 * and errors to Nav2 and LocationInterface users.
 */
class IPLocationInterfaceImpl : public LocationInterfaceImpl,
                                public IPLBSListener
{
public:
   /**
    * Constructor.
    *
    * @param api A pointer to the Nav2APIImpl.
    */
   IPLocationInterfaceImpl(Nav2APIImpl* api);

   /**
    * Destructor
    */
   virtual ~IPLocationInterfaceImpl();

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
    * This callback function is called when ever the system has got new
    * positions from the lbsManager. It then reports them as NMEA sentences
    * to NAV2.
    *
    *  @param latitude The latitude
    *  @param longitude The longitude
    *  @param altitude The Altitude
    *  @param speed The speed
    *  @param track The Track angle
    *  @param horizontalAccuracy The accuracy of the horizontal (2D) positions
    *  @param verticalAccuracy The accuracy of the vertical (altitude) positions
    *  @param timestamp The timestamp of the position, seconds since unix epoch
    */
   void lbsCallback(double latitude, double longitude, double altitude,
                    double speed, double track, double horizontalAccuracy,
                    double verticalAccuracy, double timestamp);

   /**
    * This callback function is called when ever the system is having a hard
    * time starting the lbs engine, typically if the user doesn't accept our apps''
    * request to use the CoreLocation Framework
    */
   void lbsFailed();

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
    * A member holding our iPhone helper class, which takes care of the
    * objective C implementation of lbs calls to the iPhone location manager
   */
   IPhoneLocationUtil::IPhoneLocationUtil* m_locationUtil;

   /// This is set to true when the gps has been started.
   bool m_lbsStarted;

   /// This is set to true when the first lbsCallback is called, hence we have
   /// gotten position updates from the lbs framework and we consider the lbs
   /// engine to be started.
   bool m_needSendStartedCallback;

   /// This is set to true once we have received some positions that we guess
   /// is actual gps positions and not something else, ie not cellID or wifi pos.
   bool m_gpsPosReceived;

   /// A member holding the hardware we're currently using internally to get positions
   wf_uint8 m_currentHardware;

   /// A member holding the hardware the users is requesting to use for getting positions
   wf_uint8 m_requestedHardware;
   
};


} // End of namespace WFAPI


#endif /* _IPLOCATIONINTERFACEIMPL_H_ */

