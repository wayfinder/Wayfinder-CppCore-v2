/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#define ADDITIONAL_TESTING_APNS "internet.telenor.se;services.telenor.se;internet.tele2.se;online.telia.se;internet.vodafone.ro;"

/* Needed for CAknEnumeratedText. */
#include <aknsettingitemlist.h>

enum iap_searcher_error {
   ErrorNoIAPs          = 0,
   ErrorNoWorkingIAPs   = 1,
   ErrorIAPNotSelected  = 2,
   ErrorIAPNotWorking   = 3,
};

class IAPObserver;

/**
 * IAP Searcher class.
 * Finds and tests all IAP's until a working (perhaps proper)
 * internet access point can be found.
 */
class IAPSearcher : public CBase
{
public:
   /**
    * Constructor
    */
   IAPSearcher(IAPObserver *observer);
   /**
    * Destructor
    */
   ~IAPSearcher();

   /**
    * Should wireless lan's be shown in the enumeration?
    */
   void ShowWlan(TBool status);
   /**
    * Initialize the IAP list
    * @param aSort      If ETrue, list is sorted alphabetically
    */
   void InitIAPData(TBool aSort = ETrue);
   /**
    * Start the search.
    */
   void StartSearch();
   /**
    * Called when a reply was received which indicates that the
    * access point worked.
    */
   void Reply();
   /**
    * Called when a reply was received which indicates that the
    * access point didn't work.
    */
   void ReplyFailed();
   /**
    * Test for active search
    * @return ETrue is search is active.
    */
   TBool Searching();
   /**
    * Show manual IAP selection popup.
    * @return ETrue if user selected an access point, EFalse otherwise.
    */
   TBool SelectManual();
   /**
    * Send a sync message to test current IAP
    */
   void SendSync();
   /**
    * Send current IAP to Nav2
    */
   void SendIAP();
   /**
    * GetIAPName
    * @return current IAP name.
    */
   HBufC* GetIAPName();

   /**
    * iapid2Set
    * Called when IAP id has been set in Nav2.
    */
   void iapid2Set( int32 iapId );

private:

/**
    * Load the CDesCArrayFlat array with the
    * Access point names (APNs)
    */
   void GetPreferredApnsL();


   /**
    * Add the ";" separated list of APNs in aBuf to the aVodaApns 
    * CDesCArrayFlat array
    */
   void AddToListofPreferredApnsL(const TDesC& aBuf);

   TBool CheckValidIap(TDesC& aIapName);
   
   bool getIAPProxy( char*& host, uint32& port, uint32 iap ) const;

private:

   IAPObserver *m_observer;

   bool m_settingIAP;
   bool m_searching;
   bool m_selectManual;
   bool m_showWlan;

   uint32 m_curIAPProxyPort;
   int32 m_iap;
   int32 m_curIAPid;
   int m_repeat;

   char* m_curIAPProxyHost;
   HBufC* m_curIAPName;

   CArrayPtr<CAknEnumeratedText> *m_iapList;

   CDesCArrayFlat* m_vodaApnList;
};


