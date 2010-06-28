/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef THREADOWNER_H
#define THREADOWNER_H

#include <e32base.h>
#include <badesca.h>

//#include "NewAudioServerTypes.h"

//struct NewAudioThreadData;

#if 0
class MNewAudioObserver 
{
   public:
      // TInt aErr          one of the system errors
      // TInt aSoundLength  time from the start of the clips 
      //                    until the timing marker expressed 
      //                    in ms
      virtual void PrepareSoundCompleted(TInt ms, TInt aErr) = 0;

      // TInt aErr          one of the system errors
      virtual void PlaySoundCompleted(TInt aErr) = 0;
};

class CNewAudioOwner;
#endif
#if 0
// Us this class to connect to a preexisting server.
// This is probably not intended, use CNewAudio instead.
class CNewAudioNoServer : protected RSessionBase, protected CActive
{
   public:

      /// Returns the special string for the timing marker sound
      static const TDesC& getTimingMarker();

      /// Returns the special string for the end marker.
      static const TDesC& getEndMarker();

      static CNewAudioNoServer* NewLC(MNewAudioObserver & aObs);
      virtual ~CNewAudioNoServer();
      void ConstructL();

      /// Loads the sounds from disk. Calls PrepareSoundCompleted when done
      void PrepareSoundL( CDesCArray& clips );
      /// Plays the previously loaded sounds. Calls PlaySoundCompleted after.
      void PlaySound();
      /// Stops the current sound
      void StopSound() { Cancel(); }
      /// Sets volume in percent of max volume.
      void SetVolume( int volPercent );

   protected:
      CNewAudioNoServer(MNewAudioObserver & aObs);

   protected:   // From CActive
      void DoCancel();
      void RunL();

   protected:
      MNewAudioObserver & iObs;
      NewAudioServerTypes::TRequestTypes iOutstandingRequest;
      TInt iSoundLength;              // Filled in by the server on return
      /// Passed to the server. Points to iSoundLength
      TPtr8 iSoundLengthPtr;          
      HBufC * iMsgBuffer;
      /// Passed to the server, Points to contents of iMsgBuffer
      TPtr iMsgBufferPtr;
      /// Passed to the server. Contains the length of iMsgBuffer
      TInt iMsgBufferLength;
};
#endif
#if 0
class CNewAudio : public CNewAudioNoServer
{
   public:
      static CNewAudio* NewLC(MNewAudioObserver & aObs);
      static CNewAudio* NewL(MNewAudioObserver & aObs);
      virtual ~CNewAudio();
      void ConstructL();

   protected:
      CNewAudio(MNewAudioObserver & aObs);

   protected:
      // Owns the server thread
      CNewAudioOwner * iServerOwner;
};
#endif

class CThreadAdmin : public CBase
{
public:
   virtual ~CThreadAdmin();
   virtual void StartL() = 0;
   virtual void StopL() = 0;
};

class CThreadAdminFactory : public CBase
{
protected:
   HBufC* iName;
   enum TThreadPriority iPriority;
public:
   HBufC* AllocNameL()
   {
      return iName ? iName->AllocL() : KNullDesC().AllocL();
   }
   HBufC* SetNameL(const TDesC& aName)
   {
      return static_cast<HBufC*>(User::LeaveIfNull(SetName(aName)));
   }
   HBufC* SetName(const TDesC& aName)
   {
      HBufC* tmp = aName.Alloc();
      if(tmp){
         delete iName;
         iName = tmp;
      }
      return tmp;
   }
   enum TThreadPriority ThreadPriority() const
   {
      return iPriority;
   }

   virtual class CThreadAdmin* CreateAdminL() = 0;
   class CThreadAdmin* CreateAdminLC()
   {
      class CThreadAdmin* obj = CreateAdminL();
      CleanupStack::PushL(obj);
      return obj;
   }
   CThreadAdminFactory(enum TThreadPriority aPriority = EPriorityNormal) :
      iPriority(aPriority)
   {}
   virtual ~CThreadAdminFactory();
};


/**************************************
 * CNewAudioOwner
 **************************************/
class CThreadOwner : public CBase
{
public:
   static class CThreadOwner* NewLC(class CThreadAdminFactory* aFactory);
   static class CThreadOwner* NewL(class CThreadAdminFactory* aFactory);
   static class CThreadOwner* New(class CThreadAdminFactory* aFactory);
   virtual ~CThreadOwner();
   void ConstructL(class CThreadAdminFactory* aFactory);
protected:

   struct ThreadData {
      HBufC* serverName;
      enum TThreadPriority priority;
      
      // Used by the server thread to signal the 
      // original thread that it is ready to receive
      // requests.
      TRequestStatus *startupStatus;
      TThreadId startupStatusThread;

      // Used by the original thread to signal the 
      // server thread to stop
      TRequestStatus *stopNowStatus;
      TThreadId stopNowStatusThread;

      class CThreadAdminFactory* iFactory;
      ThreadData() : 
         serverName(NULL), priority(EPriorityNormal), startupStatus(NULL), 
         stopNowStatus(NULL), iFactory(NULL)
      {}
      ~ThreadData()
      {
         delete serverName;
      }
   };

   class CStopWaiter : public CActive
   {
   public:
      static class CStopWaiter* NewLC(CThreadAdmin& aAdmin,
                                      ThreadData & threadData);
      virtual ~CStopWaiter();
      TRequestStatus * getStatusVar();
   protected:
      CStopWaiter(CThreadAdmin& aAdmin, ThreadData & threadData);
      void DoCancel();
      void RunL();
   public:
      CThreadAdmin & iAdmin;
   };

protected:
   CThreadOwner();
   static TInt StartServerThreadFunc(TAny *aPtr);
   static void StartServerThreadFuncStage2L(struct ThreadData& threadData, 
                                            class CThreadAdmin** aAdmin,
                                            class CStopWaiter **stopWaiter);
   static void SignalStartupOkFailed(TInt result, 
                                     struct ThreadData& threadData);

protected:
   // Communication area to the new thread.
   // May be NULL if this object does not own the server.
   // Currently never NULL since that functionallity is not implemented.
   struct ThreadData *iThreadData;

   class RThread iServerThread;
   class TRequestStatus iServerExitStatus;
};


#endif /* NEWAUDIO_H */

