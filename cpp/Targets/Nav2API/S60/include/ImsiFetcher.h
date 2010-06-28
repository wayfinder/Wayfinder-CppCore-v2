/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef IMSIFETCHER_H
#define IMSIFETCHER_H

#ifndef __WINS__
#if defined NAV2_CLIENT_SERIES60_V3 || defined NAV2_CLIENT_SERIES60_V5
#define GETSUBSCRIBERID
#else
#undef GETSUBSCRIBERID
#endif
#endif

# include <e32base.h>
# include <stdlib.h>
# include <f32file.h>

#ifdef GETSUBSCRIBERID
#include <etel3rdparty.h>
#else
/**
 * This is when NAV2_CLIENT_SERIES60_V3 or NAV2_CLIENT_SERIES60_V5 not is
 * defined and a faked IMEI should be used.
 */
#endif

/// Faked IMSI is read from this file.
_LIT(KImsiFile, "c:\\imsi.txt");

/**
 * This class is used for fetching the IMSI from the Symbian S60 platform.
 * If not runned on the S60 plattform it reads IMSI from a txt file, this can
 * be used for simulating an IMSI. This class is using a callback function so
 * user of this class must also declare the ImsiNumberSet function in it's own
 * class. Upon completion of the asynchronous call, a call back method to the
 * ImsiRequester will be called.
 */
template <class ImsiRequester>
class CImsiFetcher : public CActive
{
   /**
    * Constructor.
    * @param aWayfinderAppUi Pointer to WayfinderAppUi. Used for
    *                        callbacks.
    * @param aPriority       The priority of the active object
    *                        request issued through GetIMSI. Defaults
    *                        to EPriorityStandard.
    */                       
   CImsiFetcher(ImsiRequester& aImsiRequester,
                enum TPriority aPriority = EPriorityStandard)
      : CActive(aPriority), iImsiRequester(aImsiRequester)
   {
   }

   /**
    * Second phase constructor. Must be called before GetIMSI();
    */
   void ConstructL()
   {
#ifdef GETSUBSCRIBERID
      iTelephony = CTelephony::NewL();
#endif
      CActiveScheduler::Add(this);
   }
public:

   /*
    * Static constructor.
    * Completes both stages of construction so that the object is
    * ready for a call to GetIMSI. the aActivate argument gioves the
    * user an option to call GetIMSI as a part of the construction.
    * @param aWayfinderAppUi pointer to WayfinderAppUi. Used for
    *                        callbacks.
    * @parameter aActivate  if ETrue, GetIMSI is called on the object 
    *                       before it is returned. Defaults to EFalse.
    * @param aPriority      the priority of the active object
    *                       request issued through GetIMSI. Defaults
    *                       to EPriorityStandard.
    * @return a new, completely constructed CImsiFetcher object.
    */
   static 
      CImsiFetcher<ImsiRequester>* NewL(ImsiRequester& aImsiRequester,
            TBool aActivate = EFalse,
            enum TPriority aPriority = EPriorityStandard)
   {
      CImsiFetcher<ImsiRequester>* self =
         new (ELeave) CImsiFetcher(aImsiRequester, aPriority);
      CleanupStack::PushL(self);
      self->ConstructL();
      if(aActivate){
         self->GetIMSI();
      }
      CleanupStack::Pop(self);
      return self;
   }

   /*
    * Destructor
    */
   ~CImsiFetcher()
   {
      Cancel();   // if any request outstanding, calls DoCancel() to cleanup
#ifdef GETSUBSCRIBERID
      delete iTelephony;
#endif
   }

   /**
    * Function to retrieve the IMSI.
   */
   void GetIMSI()
   {
#ifdef GETSUBSCRIBERID
      CTelephony::TSubscriberIdV1Pckg subscriberIdPckg( iPkg );
      iTelephony->GetSubscriberId(iStatus, subscriberIdPckg);

      SetActive();
#else
      iStatus=KRequestPending;
      SetActive();
      class RThread thisThread;
      class TRequestStatus* tmpPtr = &iStatus;
      thisThread.RequestComplete(tmpPtr, KErrNone);
#endif
   }

   /**
    * To cancel request.
    */
   void DoCancel()
   {
      if(IsActive()){
#ifdef GETSUBSCRIBERID      
         iTelephony->CancelAsync(CTelephony::EGetSubscriberIdCancel);
#else
         RThread thisThread;
         TRequestStatus * tmpPtr = &iStatus;
         thisThread.RequestComplete(tmpPtr, KErrCancel);
#endif
      }
   } 

   /**
    * To be run when the request of the IMSI is completed.
    */
   void RunL()
   {
      if(iStatus == KErrNone){
#ifdef GETSUBSCRIBERID
         iImsiRequester.ImsiNumberSet(iPkg.iSubscriberId);
#else
         class TTime now;
         now.HomeTime();
         // Use all the bits in the time value to srand
         srand( HIGH( now.Int64() ) ^ LOW ( now.Int64() ) );
         class RFs fs;
         User::LeaveIfError(fs.Connect());
         CleanupClosePushL(fs);
         class RFile imsiFile;
         TFileName filename;
         filename.Copy(KImsiFile);
/*          iImeiRequester.GetFullAppPath(filename, KImeiFile); */
         TInt res = imsiFile.Open(fs,
                                  filename,
                                  EFileShareReadersOnly|EFileRead);
         TBuf8<16> imsi;
         if(res == KErrNotFound){
            User::LeaveIfError(imsiFile.Create(fs, filename, 
                                               EFileShareExclusive | EFileWrite));
            CleanupClosePushL(imsiFile);
            imsi.SetLength(15);
            for(TInt i = 0; i < 15; ++i){
               imsi[i] = TText8(int(25.0*rand()/(RAND_MAX+1.0)) + 'a');
            }
            User::LeaveIfError(imsiFile.Write(imsi));
         } else if(res == KErrNone){
            CleanupClosePushL(imsiFile);
            User::LeaveIfError(imsiFile.Read(imsi,15));
         } else {
            User::LeaveIfError(res);
         }
         TBuf<16> iImsi(imsi.Length());
         for(TInt i = 0; i < imsi.Length(); ++i){
            iImsi[i] = imsi[i];
         }
         iImsiRequester.ImsiNumberSet(iImsi);
         CleanupStack::PopAndDestroy(2);//imsiFile and fs
#endif
      } else {
         iImsiRequester.ErrorReceivingImsi();
      }
   }

   TInt RunError(TInt aError)
   {
      // Got a leave in RunL
      iImsiRequester.ErrorReceivingImsi();

      // Need to return KErrNone, if not the CActiveScheduler::Error
      // will get called.
      return KErrNone;
   }

   /*
    * Returns the found IMSI number. Note that if the asynchronous
    * request has not yet completed the content of the descriptor is
    * most likely KNullDesC.
    * @return a descriptor holding the IMSI number.
    */
   const TDesC& IMSI() const
   {
#ifdef GETSUBSCRIBERID
      return iV1.iSubscriberId;
#else
      return iImsi;
#endif
   }


private:
#ifdef GETSUBSCRIBERID
   ///Telephony object. Provides APIs for fetching IMSI number.
   class CTelephony* iTelephony; 

   ///Data object that holds imsi number, phone model, and phone manufacturer.
   CTelephony::TSubscriberIdV1 iV1;

   ///Descriptor adaptor for iV1;
   CTelephony::TSubscriberIdV1 iPkg;
   
#else
   TBuf<16> iImsi;
#endif

///Requester to return IMSI number to.
   ImsiRequester& iImsiRequester;
};
#endif
