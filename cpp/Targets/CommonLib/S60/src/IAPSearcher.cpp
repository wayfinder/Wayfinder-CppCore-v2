/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#define CORE_LOGGING_MODULE_NAME "IAPSearcher"

#include "arch.h"
#include "IAPSearcher.h"
#include "IAPObserver.h"
#include "WFTextUtil.h"
#include "CoreDebugPrint.h"
#include <commdb.h>
#include <e32debug.h>
#include <badesca.h>
// #include "PSSettings.h"
// #include "PSTracer.h"

_LIT(KmRouter, "mroute");
_LIT(KInternet, "internet");
_LIT(KOutgoingGPRS, "outgoinggprs");

#define MAX_REPEAT_TEST 1

#undef SPECIAL_IAP_SEARCH_METHOD
#define SPECIAL_IAP_SEARCH_METHOD
   
IAPSearcher::IAPSearcher(IAPObserver *observer)
{
   m_observer = observer;
   m_settingIAP = EFalse;
   m_searching = EFalse;
   m_selectManual = EFalse;
   m_iap = -1;
   m_iapList = NULL;
   m_repeat = 0;
   m_curIAPName = NULL;
   m_showWlan = EFalse;
   m_curIAPProxyHost = NULL;
   m_curIAPProxyPort = 0;
   m_vodaApnList = NULL;
}

IAPSearcher::~IAPSearcher()
{
   delete m_iapList;
   delete[] m_curIAPProxyHost;
   delete m_vodaApnList;
}

void
IAPSearcher::ShowWlan(TBool status)
{
   m_showWlan = status;
}

void
IAPSearcher::InitIAPData(TBool aSort)
{
   GetPreferredApnsL();
   if (m_iapList) {
      delete m_iapList;
   }
   m_iapList = new(ELeave) CArrayPtrFlat<CAknEnumeratedText>(10);
   
   // Open CommsDb without creating a database if it does not exist.
   CCommsDatabase* comdb = NULL;
   TRAPD(err, comdb = CCommsDatabase::NewL(EFalse));
   if (err != KErrNone) {
      // Could not open CommsDB return no iap found.
      return;
   }
   CleanupStack::PushL(comdb);
   CCommsDbTableView * iaptable = comdb->OpenTableLC(TPtrC(IAP));

   TInt curId = 0;
   TInt foo_retval= iaptable->GotoFirstRecord();

   while (foo_retval == KErrNone) {
      HBufC * iap_name = iaptable->ReadLongTextLC(TPtrC(COMMDB_NAME));
      // // RDebug::Print(_L("Current APN name: %S"), iap_name);
      /* Make sure that we don't show APs with mRoute in the name. */
      if (KErrNotFound == iap_name->FindF(KmRouter)) {
         TUint32 iap_id;
         iaptable->ReadUintL(TPtrC(COMMDB_ID), iap_id);
         HBufC* iap_type = iaptable->ReadLongTextLC(TPtrC(IAP_SERVICE_TYPE));
         if (CheckValidIap(*iap_name)) 
         {
            HBufC* temp = HBufC::NewLC(iap_type->Des().Length()+
                                       iap_name->Des().Length()+16);
            temp->Des().Copy(*iap_name);
#ifdef ADD_IAP_TYPE_TO_NAME
            temp->Des().Append(_L(" - "));
            temp->Des().Append(*iap_type);
#endif
            if (!aSort ||
                KErrNotFound == iap_name->FindF(KInternet) ||
               KErrNotFound == iap_type->FindF(KOutgoingGPRS)) {
               /* Not "internet" in the name or not OutgoingGPRS in type. */
               m_iapList->AppendL(new(ELeave) CAknEnumeratedText(iap_id, temp));
            } else {
               m_iapList->InsertL(curId,
                                 new(ELeave)CAknEnumeratedText(iap_id, temp));
               curId++;
            }
            /* CAknEnumeratedText takes ownership of temp string. */
            CleanupStack::Pop(temp); 
         }
         CleanupStack::PopAndDestroy(iap_type);
      }
      CleanupStack::PopAndDestroy(iap_name);
      foo_retval = iaptable->GotoNextRecord();
   }
   // RDebug::Print(_L("Number of valid APNs: %d"), m_iapList->Count());
   CleanupStack::PopAndDestroy(iaptable);
   CleanupStack::PopAndDestroy(comdb);
}

void IAPSearcher::StartSearch()
{
   if (!m_searching && !m_settingIAP) {
      /* Initialize search variables. */
      m_iap = 0;
      m_settingIAP = ETrue;
      InitIAPData();
      m_repeat = 0;

      if (m_iapList->Count() == 0) {
         /* No access points found! */
         m_settingIAP = EFalse;
         m_observer->ReportFailureIAP(ErrorNoIAPs);
      } else {
         /* Kickstart the process. */
         SendIAP();
      }
   }
}

void
IAPSearcher::SendIAP()
{
   // List all IAPs in the list.
   /* Get current IAP id. */   
   m_curIAPid = ((*m_iapList)[m_iap])->EnumerationValue();
   HBufC *tmp = ((*m_iapList)[m_iap])->Value();
   delete m_curIAPName;
   m_curIAPName = HBufC::NewL(tmp->Des().Length());
   m_curIAPName->Des().Copy(*tmp);
   // RDebug::Print(_L("Testing: %S"), m_curIAPName);
   char* name = WFTextUtil::newTDesDupL(*m_curIAPName);
   delete name;
   
   // Check for proxy.
   delete[] m_curIAPProxyHost;
   m_curIAPProxyHost = NULL;
   m_curIAPProxyPort = 0;
   bool hasProxy = false;
#ifndef __WINS__   
   hasProxy = getIAPProxy(m_curIAPProxyHost, m_curIAPProxyPort, m_curIAPid);
#endif
   
   if (hasProxy) {
      HBufC* tmpHost = WFTextUtil::AllocL(m_curIAPProxyHost);
      delete tmpHost;
   }
   
   m_observer->SendIAP(m_curIAPid, m_curIAPProxyHost, m_curIAPProxyPort);
}

bool IAPSearcher::getIAPProxy(char*& host, uint32& port, uint32 iap) const
{
   host = NULL;
   port = 0;
   bool found = false;

   int32 currIAPId = iap;
   if (currIAPId >= 0) { 

      // Set to a real value
      // Open database and don't create it if does not exist.
      CCommsDatabase* comdb = NULL;
      TRAPD(err, comdb = CCommsDatabase::NewL(EFalse));
      if (err != KErrNone) {
         // Could not open CommsDB return no proxy found.
         return EFalse;
      }
      CleanupStack::PushL(comdb);
      //DBG("getIAPProxy comdb created");
      // First get the IAP
      CCommsDbTableView* iaptable = comdb->OpenViewMatchingUintLC(
         TPtrC(IAP), TPtrC(COMMDB_ID), currIAPId);
      //DBG("getIAPProxy iaptable created");
      TInt iapres = iaptable->GotoFirstRecord();
      //DBG("getIAPProxy iaptable goto res %d", iapres);
      if (iapres == KErrNone) {
         //DBG("getIAPProxy iaptable have record");
         HBufC* iap_name = iaptable->ReadLongTextLC(
            TPtrC(COMMDB_NAME));
         uint32 iap_service = 0;
         iaptable->ReadUintL(TPtrC(IAP_SERVICE), iap_service);
         //DBG("iap_service %"PRIu32, iap_service);
         // The current IAP exists!
         HBufC* iap_type = iaptable->ReadLongTextLC(
            TPtrC(IAP_SERVICE_TYPE));
         // Find Proxy for ISP (and same service type)
         CCommsDbTableView* proxytable = NULL;
         TRAPD(err, 
               proxytable = comdb->OpenViewMatchingUintLC(TPtrC(PROXIES), 
                                                          TPtrC(PROXY_ISP), 
                                                          iap_service);
               CleanupStack::Pop(proxytable));
         if (err != KErrNone) {
            // Cleanup and return no proxy found.
            delete proxytable;
            CleanupStack::PopAndDestroy(iap_type);
            CleanupStack::PopAndDestroy(iap_name);
            CleanupStack::PopAndDestroy(iaptable);
            CleanupStack::PopAndDestroy(comdb);
            return EFalse;
         }
         CleanupStack::PushL(proxytable);
         //DBG("getIAPProxy created proxytable");
         TInt dretval= proxytable->GotoFirstRecord();
         while (dretval == KErrNone && !found) {
            //DBG("getIAPProxy proxy loop");
            // Check if matching proxy service type
            HBufC* proxy_service_type = proxytable->ReadLongTextLC(
               TPtrC(PROXY_SERVICE_TYPE));
            if (proxy_service_type != NULL &&
                 proxy_service_type->CompareC(*iap_type) == 0) 
            {
               //DBG("getIAPProxy proxy loop match");
               // Match!
               // PROXY_USE_PROXY_SERVER 
               TBool proxy_use_proxy_server = 0;
               proxytable->ReadBoolL(TPtrC(PROXY_USE_PROXY_SERVER), 
                                      proxy_use_proxy_server);
                  //DBG("getIAPProxy proxy loop use proxy");
                  // PROXY_SERVER_NAME - Name of the proxy server
                  HBufC* proxy_server_name = proxytable->ReadLongTextLC(
                     TPtrC(PROXY_SERVER_NAME));
                  if (proxy_server_name) {
                     //DBG("getIAPProxy proxy loop have proxy server");
                     found = true;
                     // Convert to something we can use.
                     host = WFTextUtil::TDesCToUtf8L(proxy_server_name->Des());
                     //DBG("getIAPProxy proxy loop have proxy server name "
                     //     "%s", host);
                     proxytable->ReadUintL(TPtrC(PROXY_PORT_NUMBER), 
                                            port);
                     // Sanity on port
                     if (port == 9201) {
                        // We don't talk wap
                        // Or no proxy at all?
                        port = 8080;
                     } else if (port == 0) {
                        // Not valid => no proxy
                        found = false;
                        delete [] host;
                        host = NULL;
                     }
                  } // End if have proxy_server_name
                  CleanupStack::PopAndDestroy(proxy_server_name);
            } // End if service type matches
            CleanupStack::PopAndDestroy(proxy_service_type);

            // Next proxy
            dretval = proxytable->GotoNextRecord();
         } // End while all proxies

         // Perhaps "IAP_SERVICE_TYPE" table -> [GPRS|ISP]_IP_GATEWAY 
         // especially in s60v1
         CleanupStack::PopAndDestroy(proxytable);
         CleanupStack::PopAndDestroy(iap_type);
         CleanupStack::PopAndDestroy(iap_name);
      } // End if the current IAP is found (iapres == KErrNone)
      //DBG("getIAPProxy get IAP proxy after end if current IAP is found");
      CleanupStack::PopAndDestroy(iaptable);
      CleanupStack::PopAndDestroy(comdb);
      //DBG("getIAPProxy get IAP proxy ends");
      
   } // End if selected IAP is a valid number and we want proxy 

   return found;
}

void
IAPSearcher::SendSync()
{
   m_observer->SendNop();
}

void IAPSearcher::Reply()
{
   // RDebug::Print(_L("IAP worked!")); 
   /* Sync worked! */
   m_searching = EFalse;
   m_selectManual = EFalse;

   /* Set as default access point. */
   // When special style searching don't show any dialogs.
   m_observer->SendIAP(m_curIAPid, m_curIAPProxyHost,
                       m_curIAPProxyPort, ETrue, EFalse);
}
void IAPSearcher::ReplyFailed()
{
   // RDebug::Print(_L("IAP didn't work!"));
   if (m_selectManual) {
      /* We're testing a manually selected entry. */
      /* This means that the manually selected entry */
      /* does not work. */
      m_searching = EFalse;
      m_selectManual = EFalse;
      m_observer->ReportFailureIAP(ErrorIAPNotWorking, ETrue);
      /* Send the user back to the select menu. */
      SelectManual();
      return;
   }
   if (m_repeat >= MAX_REPEAT_TEST-1) {
      /* Failed more than X times */
      m_repeat = 0;
      m_iap++;
      if (m_iap >= m_iapList->Count()) {
         /* All APs tested, failed. */
         m_observer->ReportFailureIAP(ErrorNoWorkingIAPs);
         return;
      }
      /* Fallthrough */
   } else {
      m_repeat++;
   }
   m_settingIAP = ETrue;
   m_searching  = EFalse;
   SendIAP();
}

HBufC*
IAPSearcher::GetIAPName()
{
   return m_curIAPName;
}

TBool IAPSearcher::Searching()
{
   return (m_searching || m_settingIAP);
}

TBool
IAPSearcher::SelectManual()
{
   return false;
}

TBool
IAPSearcher::CheckValidIap(TDesC& aIapName)
{
   // search OutGoingGPRS for matching name 
   // get the list of  IAPs
   TBool apnFound = EFalse;
   TInt indexOfMatchingIap =0;

   // Get the phoness IAPs from CCommDb
   CCommsDatabase* commDb = NULL;
   TRAPD(err, commDb = CCommsDatabase::NewL(EFalse));
   if (err != KErrNone) {
      // Could not open CommsDB return no iap found.
      return EFalse;
   }
   CleanupStack::PushL(commDb);
   CCommsDbTableView * iaptable = commDb->OpenTableLC(TPtrC(OUTGOING_GPRS));

   // for each Access Point Name (APN) in the Comms database
   if (iaptable->GotoFirstRecord() == KErrNone) { 
      do {
         // first find the correct entry in OUTGOING_GPRS by comparing
         // the IAP name 
         HBufC* iap_name = iaptable->ReadLongTextLC(TPtrC(COMMDB_NAME));
         if ((0 == aIapName.CompareF(*iap_name)) || 
              (aIapName.Find(*iap_name) != KErrNotFound) ||
              (iap_name->Des().Find(aIapName) != KErrNotFound)) {
            // Correct entry found now check if its a valid APN
            HBufC* apn = iaptable->ReadLongTextLC(TPtrC(GPRS_APN));
            if (0 == m_vodaApnList->Find(apn->Des(), indexOfMatchingIap)) {
               // found a suitable APN
               apnFound = ETrue;
               // RDebug::Print(_L("Found potentially valid APN:"));
               // RDebug::Print(_L("Apn desc: %S"), apn);
               // RDebug::Print(_L("aIapName: %S"), &aIapName);
            }
            CleanupStack::PopAndDestroy(apn);
         }
         CleanupStack::PopAndDestroy(iap_name);
         if (apnFound) {
            break;
         }
      } while((!apnFound) && iaptable->GotoNextRecord() == KErrNone);
   }
   CleanupStack::PopAndDestroy(iaptable);   
   CleanupStack::PopAndDestroy(commDb);
   return(apnFound);
}


void 
IAPSearcher::AddToListofPreferredApnsL(const TDesC& aBuf)
{
   TInt pos = 0;
   TBool found = ETrue;
   TBuf<300> newIaps;
   newIaps.Copy(aBuf);

   while (found) {
      pos = newIaps.Find(_L(";"));
      if (pos > 0) {
         // found next entry
         m_vodaApnList->AppendL(newIaps.Mid(0, pos));
         newIaps.Delete(0,pos+1);
      } else {
         // all done 
         found = EFalse;
      }
   }   
}


void 
IAPSearcher::GetPreferredApnsL()
{
   if (m_vodaApnList != NULL) {
      delete m_vodaApnList;
   }
   m_vodaApnList = new (ELeave) CDesCArrayFlat(16);

#ifdef __WINS__
   // add some extra test APNs for use on the emulator
   _LIT16(KWinsIaps, "winsock;internet;iNet;");
   AddToListofPreferredApnsL(KWinsIaps);
#endif

#ifdef ADDITIONAL_TESTING_APNS
   // add some extra test APNs otherwise nobody is going to be able to test !
   MLIT(KTestingApns, ADDITIONAL_TESTING_APNS);
   AddToListofPreferredApnsL(KTestingApns);
#endif

}

void
IAPSearcher::iapid2Set(int32 iapId)
{
   if (m_settingIAP && iapId == m_curIAPid) {
      m_settingIAP = EFalse;
      m_searching  = ETrue;
      // Send sync to test the now set IAP
      SendSync();
   }
}
