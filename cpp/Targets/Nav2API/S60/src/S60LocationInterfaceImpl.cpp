/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "S60LocationInterfaceImpl.h"
#include "LocationInterface.h"
#include "LbsPositionRequestor.h"
#include "LocationListener.h"
#include "Nav2.h"
#include "Nav2APIChannel.h"
#include "GuiProtMess.h"
#include "NetworkInfoHandler.h"
#include "WFTextUtil.h"
#include <nav2util.h>
#include <e32debug.h>

//#define DUMP_NMEA
#ifdef DUMP_NMEA
#include "FileSystem.h"
static const char* s_fileName = "c:\\shared\\wf_nmea_dump.txt";
static FILE* s_file;
#endif

using namespace std;

namespace WFAPI {

S60LocationInterfaceImpl::
S60LocationInterfaceImpl(Nav2APIImpl* api) :
   LocationInterfaceImpl(api),
   m_needSendStartedCallback(false),
   m_lbsStarted(false),
   m_lastTGPPCellID(TGPPCellIDRequestData("","","","","","")),
   m_lastCDMACellID(CDMACellIDRequestData("","","","",""))
{
   m_positionRequestor = NULL;
   iNetworkInfoHandler = CNetworkInfoHandler::NewL(this);
   //! Alloc NMEA buffers.
   iNMEA8Bit = HBufC8::New( 10 * 1024 );
#ifdef DUMP_NMEA
   s_file = FileSystem::openFile(s_fileName);
#endif
}

S60LocationInterfaceImpl::~S60LocationInterfaceImpl()
{
   delete m_positionRequestor;
   delete iNetworkInfoHandler;
   delete iNMEA8Bit;
#ifdef DUMP_NMEA
   FileSystem::closeFile(s_file);
#endif
}

SynchronousStatus
S60LocationInterfaceImpl::lbsStarted()
{
   wf_int statusCode = LBS_STATUS_STARTED;
   if (!m_lbsStarted) {
      statusCode = LBS_STATUS_NOT_STARTED;
   }
   return SynchronousStatus(statusCode, "", "");
}


AsynchronousStatus
S60LocationInterfaceImpl::startLbs()
{
   if (m_lbsStarted) {
      // We are already started.
      return AsynchronousStatus(RequestID(RequestID::INVALID_REQUEST_ID),
                                OK, "", "");
   }

   // Send CellID request
   if (!checkAndSendCellIDRequest(true)) {
      return AsynchronousStatus(RequestID(RequestID::INVALID_REQUEST_ID),
                                LBS_STARTUP_FAILED, "", "");
   }

#if 0
   // Test code
   // Network UMTS signalStrength 6 mcc 240 mnc 8 lac 330 cellID 50141
   TGPPCellIDRequestData defaultCellIDRequest(
      "UMTS", "6"/*What units?*/, "0xF0"/*MCC 240 se*/, 
      "0x8"/*MNC*/, "0x14a"/*Phoney LAC*/, "0xC3DD"/*cellID*/);
   sendCellIDRequest(defaultCellIDRequest);
   
#endif

   // We have to trap any leaves from CLbsPositionRequestor.
   TInt result;
   TRAP(result,
        m_positionRequestor = CLbsPositionRequestor::NewL(*this));
   if (result != KErrNone){
      // Failed to start the position requestor.
      return AsynchronousStatus(RequestID(RequestID::INVALID_REQUEST_ID),
                                LBS_STARTUP_FAILED, "", "");   
   }

   // Notify Nav2 that the GPS is connected.
   isab::GenericGuiMess message(isab::GuiProtEnums::CONNECT_GPS);
   sendAsynchronousRequestToNav2(&message);

   m_lbsStarted = true;
   m_needSendStartedCallback = true;

   return AsynchronousStatus(RequestID(RequestID::INVALID_REQUEST_ID),
                             OK, "", "");
}

SynchronousStatus
S60LocationInterfaceImpl::stopLbs()
{
   if (m_lbsStarted) {
      delete m_positionRequestor;
      m_positionRequestor = NULL;
      m_lbsStarted = false;
   }
   
   // Notify Nav2 that the GPS is disconnected.
   isab::GenericGuiMess message(isab::GuiProtEnums::DISCONNECT_GPS);
   sendAsynchronousRequestToNav2(&message);
   
   return SynchronousStatus(OK, "", "");
}


void
S60LocationInterfaceImpl::ShowError(PositionEvent aEvent)
{
   RDebug::Print(_L("S60LocIntImp:ShowError: %d\n"), aEvent);
   wf_int statusCode = OK;

   switch(aEvent){
      case LastPosUnknown:
         // This is not a problem, it only indicates that
         // there was no last known position.
         statusCode = OK;
         break;
         
      case ErrLocRequest:
         // Unspecified error from the position requestor.
         statusCode = GENERAL_ERROR;
         break;
         
      case ErrAccessDenied:
         // The position requestor got an access denied message.
         statusCode = GENERAL_ERROR;
         break;
         
      case ErrOpenPos:
         // The positionRequestor failed to open a session
         // to the position service.
         statusCode = LBS_STARTUP_FAILED;
         break;
         
      case ErrPosServConn:
         // The positionRequestor failed to connect to the
         // to the position service.
         statusCode = LBS_STARTUP_FAILED;
         break;
         
      case ErrSetRequestor:
         // The positionRequestor failed to register itself
         // to the position service.
         statusCode = LBS_STARTUP_FAILED;
         break;
         
      case ErrSetUpOpt:
         // The positionRequestor failed to setup the options
         // for the position service. I.e. update interval, time out,
         // maximum age, partial updates etc.
         statusCode = LBS_STARTUP_FAILED;
         break;
         
      case ErrCanceled:
         // The positionRequestors position requesting was canceled.
         statusCode = GENERAL_ERROR;
         break;
         
      case ErrQualityLoss:
         // A position with insufficient quality was discarded.
         statusCode = OK;
         break;
      case ErrTimeOut:
         // A position request timed out.
         statusCode = LBS_ERROR_TIME_OUT;
         break;
      default:
         break;
   }

   if (statusCode != OK) {
      const ListenerCont& listeners = getListeners();
      for (ListenerCont::iterator it = listeners.begin(); 
           it != listeners.end(); ++it) {
         // Report the error to the listeners.
         (*it)->error(AsynchronousStatus(RequestID::INVALID_REQUEST_ID,
                                         statusCode,
                                         "", ""));
      }
   }
}

void
S60LocationInterfaceImpl::FetchNetworkInfoL(
   enum TNetworkInfoState aState)
{
   iNetworkInfoState = aState;
   iNetworkInfoHandler->FetchAllNetworkInfoL();
}

void
S60LocationInterfaceImpl::PositionInfoUpdated(
   HPositionGenericInfo& aPosInfo, const TDesC& aModulename)
{
   m_receivedGPSPosition = false;
   //! Get count of sentences.
   TUint8 nrOfSentences = 0;
   if (aPosInfo.GetValue(EPositionFieldNMEASentences, nrOfSentences)
        == KErrNotFound)
   {
      RDebug::Print(_L("S60LocIntImp:NMEA sentence count ZERO."));
      
   } else {

      if (m_needSendStartedCallback) {
         // This is the first callback since the lbs was started,
         // we need to call startedLbs for the listeners.
         const ListenerCont& listeners = getListeners();
         for (ListenerCont::iterator it = listeners.begin(); 
              it != listeners.end(); ++it) {
            (*it)->startedLbs();
         }
         m_needSendStartedCallback = false;
      }

      //! Zero the NMEA sentence storage.
      TPtr8 nmeaStore = iNMEA8Bit->Des();
      nmeaStore.Zero();

      //! Get sentences.
      TPtrC8 nmeaBuf;
      for (TInt sentenceNr = EPositionFieldNMEASentencesStart;
           sentenceNr < (EPositionFieldNMEASentencesStart + nrOfSentences);
           sentenceNr++)
      {
         if (aPosInfo.GetValue((TUint16) sentenceNr, nmeaBuf)
             == KErrNotFound)
         {
            RDebug::Print(_L("S60LocIntImp:NMEA missing, number:"), sentenceNr);
         }
         else if (nmeaBuf.Length() == 0)
         {
            RDebug::Print(_L("S60LocIntImp:NMEA length ZERO."));
         }
         else
         {
            //! Append it to dump store.
            nmeaStore.Append(nmeaBuf);
            
            
            //RDebug::Print(_L("NMEA Sentences:\n%s"),nmeaBuf.Ptr());
            
            char last = nmeaBuf.Ptr()[nmeaBuf.Length() - 1 ];
            
			if (last != '\n' && last != '\r'){
			   // No newline or carriage return found, 
			   // so we need to add it for nmea parsing to work
			   nmeaStore.Append(_L("\n"));
			}
               
            m_receivedGPSPosition = true;
         }
      }

      getBtChannel()->getChannel()->writeData(iNMEA8Bit->Des().Ptr(),
                                              iNMEA8Bit->Des().Size());
#ifdef DUMP_NMEA
      char* nmeaString = WFTextUtil::newTDesDupL(*iNMEA8Bit);
      FileSystem::corefwrite(s_file, nmeaString);
      delete nmeaString;
#endif
         
   }

   // Slightly hacky but I'm testing if we have changed cell id
   if (!m_receivedGPSPosition) {
      checkAndSendCellIDRequest(false);
   }

}

void S60LocationInterfaceImpl::NetworkInfoDone()
{
   // Got all the network info we're interested in.

   if (iNetworkInfoState == EUseForGettingPosition || iNetworkInfoState == EUseForGettingPositionAndRoamingState) {
      // Network info should be used to quickly retrieve an
      // estimated position from the server.
      CTelephony::TNetworkMode networkMode = iNetworkInfoHandler->iNetworkMode;
      TBuf<16> sigStrengthTxt;
      sigStrengthTxt.Num(iNetworkInfoHandler->iSigStrength);
      char* signalStrength = WFTextUtil::newTDesDupL(sigStrengthTxt);
      char* mcc = WFTextUtil::newTDesDupL(iNetworkInfoHandler->iMcc);
      char* mnc = WFTextUtil::newTDesDupL(iNetworkInfoHandler->iMnc);
      TBuf<16> lacTxt;
      lacTxt.Num(iNetworkInfoHandler->iLac);
      char* lac = WFTextUtil::newTDesDupL(lacTxt);
      TBuf<16> cellIdTxt;
      cellIdTxt.Num(iNetworkInfoHandler->iCellId);
      char* cellId = WFTextUtil::newTDesDupL(cellIdTxt);
      if (networkMode == CTelephony::ENetworkModeUnknown) {
         // We don't know what network mode we're using, do nothing.
      } else if (networkMode == CTelephony::ENetworkModeAmps ||
                 networkMode == CTelephony::ENetworkModeCdma95 ||
                 networkMode == CTelephony::ENetworkModeCdma2000) {
         // CDMA Network.
         char* cdmaType = NULL;
         if (networkMode == CTelephony::ENetworkModeCdma2000) {
            cdmaType = isab::strdup_new("EVDO");
         } else {
            cdmaType = isab::strdup_new("CDMA");
         }
         char* cdmaSID = WFTextUtil::newTDesDupL(iNetworkInfoHandler->iCdmaSID);
         TBuf<16> bidTxt;
         bidTxt.Num(iNetworkInfoHandler->iCdmaBID);
         char* cdmaBID = WFTextUtil::newTDesDupL(bidTxt);
         CDMACellIDRequestData cellIDReq(cdmaType, signalStrength,
                                         cdmaSID, mnc, cdmaBID);
         //iNGPCellIdSeqId = sendCellIDRequest(cellIDReq);
         if (!iForceCellIDRequest && cellIDReq == m_lastCDMACellID) {
            // Same cell no need to send again
            RDebug::Print(_L("S60LocIntImp::NetworkInfoDone Unchanged cellid not sending new request\n"));
         } else {
            sendCellIDRequest(cellIDReq);
            m_lastCDMACellID = cellIDReq;
            iForceCellIDRequest = false;
         }
         delete[] cdmaBID;
         delete[] cdmaSID;
         delete[] cdmaType;
      } else {
         // GSM/UMTS Network.
         char* tgppType = NULL;
         if (networkMode == CTelephony::ENetworkModeGsm) {
            tgppType = isab::strdup_new("GPRS");
         } else {
            tgppType = isab::strdup_new("UMTS");
         }
         TGPPCellIDRequestData cellIDReq(tgppType, signalStrength,
                                         mcc, mnc, lac, cellId);
         // Use request number in HandleRequestFailedL to check for correct error.
         //iNGPCellIdSeqId = sendCellIDRequest(cellIDReq);
         if (!iForceCellIDRequest && cellIDReq == m_lastTGPPCellID) {
            // Same cell no need to send again
            RDebug::Print(_L("S60LocIntImp::NetworkInfoDone Unchanged cellid not sending new request\n"));
         } else {
            sendCellIDRequest(cellIDReq);
            m_lastTGPPCellID = cellIDReq;
            iForceCellIDRequest = false;
         }
         delete[] tgppType;
      }
      delete[] cellId;
      delete[] lac;
      delete[] mnc;
      delete[] mcc;
      delete[] signalStrength;

      if (iNetworkInfoState == EUseForGettingPositionAndRoamingState) {
         // Register for changes of the network registration status so 
         // we can show a dialog when it has changed.
      }
   } 
   iNetworkInfoState = ENetworkInfoIdle;
}

void S60LocationInterfaceImpl::NetworkInfoInUse()
{
   // Could not get the network info since the AO was busy.
   iNetworkInfoState = ENetworkInfoIdle;
}

void S60LocationInterfaceImpl::NetworkInfoFailed(TInt aError)
{
   // Could not get the network info since the fetching returned an error.

#if defined __WINS__
   // For testing the procedure in emulator we fake that it worked fine
   // to get the network info from the phone.
   NetworkInfoDone();
#endif

   iNetworkInfoState = ENetworkInfoIdle;
}


bool
S60LocationInterfaceImpl::checkAndSendCellIDRequest(bool force)
{
   iForceCellIDRequest = force;
   FetchNetworkInfoL(EUseForGettingPositionAndRoamingState);

   //always return true for now, the call to get cell data is async on symbian
   return true;
}

} // End namespace WFAPI


