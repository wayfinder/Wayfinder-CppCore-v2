/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef IMEIFETCHER_H
#define IMEIFETCHER_H

#ifndef __WINS__
# if defined NAV2_CLIENT_SERIES60_V3 || defined NAV2_CLIENT_SERIES60_V5
#  define GETPHONEID
# else
#  undef GETPHONEID
# endif
#endif

# include <e32base.h>
# include <stdlib.h>
# include <f32file.h>

# ifdef GETPHONEID
#  include <etel3rdparty.h>
# else
   /**
    * This is when NAV2_CLIENT_SERIES60_V3 or NAV2_CLIENT_SERIES60_V5 not is
    * defined and a faked IMEI should be used.
    */ 
# endif

/// Faked IMEI is read from this file.
_LIT(KImeiFile, "c:\\imei.txt");

/**
 * This class is used for fetching the IMEI from the Symbian S60 platform.
 * If not runned on the S60 plattform it reads IMEI from a txt file, this can
 * be used for simulating an IMEI. This class is using a callback function so
 * user of this class must also declare the ImeiNumberSet function in it's own
 * class. Upon completion of the asynchronous call, a call back method to the
 * ImeiRequester will be called.
 */

/* Declare the client class */
template <class ImeiRequester>
class CImeiFetcher : public CActive
{
   /**
    * Constructor.
    * 
    * @param aWayfinderAppUi pointer to WayfinderAppUi. Used for
    *                        callbacks.
    * @param aPriority       the priority of the active object
    *                       request issued through GetIMEI. Defaults
    *                       to EPriorityStandard.
    */                       
   CImeiFetcher(ImeiRequester& aImeiRequester,
                enum TPriority aPriority = EPriorityStandard)
      : CActive(aPriority), iImeiRequester(aImeiRequester)
   {
   }

   /**
    * Second phase constructor.
    * Must be called before GetIMEI();
    */
   void ConstructL()
   {
#ifdef GETPHONEID
      iPkg = new (ELeave) CTelephony::TPhoneIdV1Pckg(iV1);
      iTelephony = CTelephony::NewL();
#endif
      CActiveScheduler::Add(this);
   }
public:

   /**
    * Static constructor.
    *
    * Completes both stages of construction so that the object is
    * ready for a call to GetIMEI. the aActivate argument gioves the
    * user an option to call GetIMEI as a part of the construction.
    * @param aWayfinderAppUi pointer to WayfinderAppUi. Used for
    *                        callbacks.
    * @param aActivate       if ETrue, GetIMEI is called on the object 
    *                        before it is returned. Defaults to EFalse.
    * @param aPriority       the priority of the active object
    *                        request issued through GetIMEI. Defaults
    *                        to EPriorityStandard.
    * @return a new, completely constructed CImeiFetcher object.
    */
   static 
      CImeiFetcher<ImeiRequester>* NewL(ImeiRequester& aImeiRequester,
            TBool aActivate = EFalse,
            enum TPriority aPriority = EPriorityStandard)
   {
      CImeiFetcher<ImeiRequester>* self =
         new (ELeave) CImeiFetcher(aImeiRequester, aPriority);
      CleanupStack::PushL(self);
      self->ConstructL();
      if(aActivate){
         self->GetIMEI();
      }
      CleanupStack::Pop(self);
      return self;
   }

   /**
    * Destructor
    */
   ~CImeiFetcher()
   {
      Cancel();   // if any request outstanding, calls DoCancel() to cleanup
#ifdef GETPHONEID
      delete iTelephony;
      delete iPkg;
#endif
   }

   /**
    * Function to retrive IMEI.
    */
   void GetIMEI()
   {
#ifdef GETPHONEID
      iTelephony->GetPhoneId(iStatus, *iPkg );
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
    * Cancel request.
    */
   void DoCancel()
   {
      if(IsActive()){
#ifdef GETPHONEID
         iTelephony->CancelAsync(CTelephony::EGetPhoneIdCancel);
#else
         RThread thisThread;
         TRequestStatus * tmpPtr = &iStatus;
         thisThread.RequestComplete(tmpPtr, KErrCancel);
#endif
      }
   } 

   /**
    * To be run when the request of IMEI is completed.
    */
   void RunL()
   {
      if(iStatus == KErrNone){ 
#ifdef GETPHONEID
         iImeiRequester.ImeiNumberSet((*iPkg)().iSerialNumber);
#else
         class TTime now;
         now.HomeTime();
         // Use all the bits in the time value to srand
         srand( HIGH( now.Int64() ) ^ LOW ( now.Int64() ) );
         class RFs fs;
         User::LeaveIfError(fs.Connect());
         CleanupClosePushL(fs);
         class RFile imeiFile;
         TFileName filename;
         filename.Copy(KImeiFile);
/*          iImeiRequester.GetFullAppPath(filename, KImeiFile); */
         TInt res = imeiFile.Open(fs,
               filename,
               EFileShareReadersOnly|EFileRead);
         TBuf8<16> imei;
         if(res == KErrNotFound){
            User::LeaveIfError(imeiFile.Create(fs, filename, 
                     EFileShareExclusive | EFileWrite));
            CleanupClosePushL(imeiFile);
            imei.SetLength(15);
            for(TInt i = 0; i < 15; ++i){
               imei[i] = TText8(int(25.0*rand()/(RAND_MAX+1.0)) + 'a');
            }
            User::LeaveIfError(imeiFile.Write(imei));
         } else if(res == KErrNone){
            CleanupClosePushL(imeiFile);
            User::LeaveIfError(imeiFile.Read(imei,15));
         } else {
            User::LeaveIfError(res);
         }
         TBuf<16> iImei(imei.Length());
         for(TInt i = 0; i < imei.Length(); ++i){
            iImei[i] = imei[i];
         }
         iImeiRequester.ImeiNumberSet(iImei);
         CleanupStack::PopAndDestroy(2); //imeiFile and fs
#endif
      } else {
         iImeiRequester.ErrorReceivingImei();
      }
   }

   TInt RunError(TInt aError)
   {
      // Got a leave in RunL
      iImeiRequester.ErrorReceivingImei();

      // Need to return KErrNone, if not the CActiveScheduler::Error
      // will get called.
      return KErrNone;      
   }
   
   /*
    * Returns the found IMEI number. Note that if the asynchronous
    * request has not yet completed the content of the descriptor is
    * most likely KNullDesC.
    * @return a descriptor holding the IMEI number.
    */
   const TDesC& IMEI() const
   {
#ifdef GETPHONEID
      return iV1.iSerialNumber;
#else
      return iImei;
#endif
   }

   /*
    * Returns the found Manufacturer name. Note that if the asynchronous
    * request has not yet completed the content of the descriptor is
    * most likely KNullDesC.
    * @retern a descriptor holding the manufacturer name.
    */
   const TDesC& Manufacturer() const
   {
#ifdef GETPHONEID
      return iV1.iManufacturer;
#else
      return KNullDesC;
#endif
   }

   /*
    * Returns the found device model name. Note that if the asynchronous
    * request has not yet completed the content of the descriptor is
    * most likely KNullDesC.
    * @return a descriptor holding the device model name.
    */ 
   const TDesC& Model() const
   {
#ifdef GETPHONEID
      return iV1.iModel;
#else
      return KNullDesC;
#endif
   }

private:
#ifdef GETPHONEID

   ///Telephony object. Provides APIs for fetching IMEI number.
   class CTelephony* iTelephony;
   
   ///Data object that holds imei number, phone model, and phone manufacturer.
   CTelephony::TPhoneIdV1 iV1;
   
   ///Descriptor adaptor for iV1;
   CTelephony::TPhoneIdV1Pckg* iPkg;
   
#else
   TBuf<16> iImei;
#endif

   ///Requester to return IMEI number to.
   ImeiRequester& iImeiRequester;
};
#endif
