/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef _S60LOCATIONINTERFACEIMPL_H_
#define _S60LOCATIONINTERFACEIMPL_H_

#include "LocationInterfaceImpl.h"
#include "LocationInterface.h"
#include "LbsPositionListener.h"
#include "NetworkInfoHandler.h"
#include "NetworkInfoObserver.h"

class CLbsPositionRequestor;
class CNetworkInfoHandler;

namespace WFAPI {

/**
 * Symbian series60 specific lbs class. Passes on position messages
 * and errors to Nav2 and LocationInterface users.
 */
class S60LocationInterfaceImpl : public LocationInterfaceImpl,
                                 public MLbsPositionListener,
                                 public MNetworkInfoObserver {
public:
   
   /**
    * Constructor.
    *
    * @param api A pointer to the Nav2APIImpl.
    */
   S60LocationInterfaceImpl(Nav2APIImpl* api);

   /**
    * Destructor
    */
   virtual ~S60LocationInterfaceImpl();

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
    * @see LbsPositionListener:PositionInfoUpdated
    */
   virtual void PositionInfoUpdated(HPositionGenericInfo& aPosInfo,
                                    const TDesC& aModulename );

   /**
    * @see LbsPositionListener:showError
    */
   virtual void ShowError(PositionEvent aEvent);

   /**
    * Enum to control what we want to use the result of 
    * the call to FetchNetworkInfoL for.
    */
   enum TNetworkInfoState {
      ENetworkInfoIdle,
      EUseForGettingPosition,
      EUseForGettingPositionAndRoamingState,
      EUseForDebugDialog,
      EUseForDebugSms
   };

    /**
    * Initializes a asynchronous sequence to get all network information that 
    * we are interested in.
    */
   void FetchNetworkInfoL(enum TNetworkInfoState aState);
   
   /**
    * From MNetworkInfoObserver, called when all network info is available.
    */
   void NetworkInfoDone();

   /**
    * From MNetworkInfoObserver, called when there already is an 
    * outstanding request to get network info.
    */
   void NetworkInfoInUse();

   /**
    * From MNetworkInfoObserver, called when we failed to fetch the network 
    * info from the phone.
    */
   void NetworkInfoFailed(TInt aError);

   /**
    * Wrapper that sends the CellID request
    */
   bool checkAndSendCellIDRequest(bool force);

protected:

private:

   //! Buffer for NMEA data.
   HBufC8 *iNMEA8Bit;

   /// Used to request positions from the symbian LBS framework.
   CLbsPositionRequestor* m_positionRequestor;

   /// Used to request information derived from the network, such as cellid data
   CNetworkInfoHandler* iNetworkInfoHandler;

   /// State to control what the result of FetchNetworkInfoL should be used for.
   TNetworkInfoState iNetworkInfoState;

   TInt32 iNGPCellIdSeqId;

   /// Current Cell position information if known
   class CCellPosInformation* iCellPosition;

   /// Force a CellID lookup
   TBool iForceCellIDRequest;

   /// Indicates whether to tell listeners that the LBS is started.
   /// This will be done when receiving the first position.
   TBool m_needSendStartedCallback;
   
   /// Indicates whether the LBS has been started or not.
   TBool m_lbsStarted;

   /// The last CellID we checked with the server
   TGPPCellIDRequestData m_lastTGPPCellID;
   CDMACellIDRequestData m_lastCDMACellID;
   
   /// Indicates whether we have received gps positions from the system in the current callback
   TBool m_receivedGPSPosition;
};

} // End namespace WFAPI

#endif /* _S60LOCATIONINTERFACEIMPL_H_ */
