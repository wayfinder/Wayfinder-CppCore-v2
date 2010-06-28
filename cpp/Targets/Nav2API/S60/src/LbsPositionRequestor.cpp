
#include <lbs.h>
#include "LbsPositionRequestor.h"
#include "LbsPositionListener.h"
#include "LocationStatusCode.h"

// One second
const TInt KSecond = 1000000;

// The update interval
const TInt KUpdateInterval = KSecond;

// Update time out
const TInt KUpdateTimeOut = 15*KSecond;

// Maximum age
const TInt KMaxAge = 500000;

// Unknown string used when module name is not known
_LIT(KUnknown,"Unknown");

//The name of the requestor
_LIT(KRequestor,"LbsPositionRequestor");


CLbsPositionRequestor::CLbsPositionRequestor(
   MLbsPositionListener& aPositionListener )
   : CActive(CActive::EPriorityStandard),
     iGenInfo(NULL),
     iPositionListener( aPositionListener )
{
   // Set update interval to one second to receive one position
   // data per second
   iUpdateops.SetUpdateInterval(TTimeIntervalMicroSeconds(KUpdateInterval));

   // If position server could not get position
   // In two minutes it will terminate the position request
   iUpdateops.SetUpdateTimeOut(TTimeIntervalMicroSeconds(KUpdateTimeOut));

   // Positions which have time stamp below KMaxAge can be reused
   iUpdateops.SetMaxUpdateAge(TTimeIntervalMicroSeconds(KMaxAge));

   // Enables location framework to send partial position data
   iUpdateops.SetAcceptPartialUpdates(ETrue);

   // Add this position requestor to the active scheduler
   CActiveScheduler::Add( this );
}

void CLbsPositionRequestor::ConstructL()
{
   // Initialise the position request sequence
   DoInitialiseL();
}

CLbsPositionRequestor*
CLbsPositionRequestor::NewL( MLbsPositionListener& aPositionListener )
{
   //Create the object
   CLbsPositionRequestor* self = new( ELeave ) CLbsPositionRequestor(
      aPositionListener);

   //Push to the cleanup stack
   CleanupStack::PushL( self );

   //Construct the object
   self->ConstructL();

   //Remove from cleanup stack
   CleanupStack::Pop( self );

   //Return pointer to the created object
   return self;
}

CLbsPositionRequestor::~CLbsPositionRequestor()
{
   // Cancel active object
   Cancel();

   // Close the positioner
   iPositioner.Close();

   // Close the session to the position server
   iPosServer.Close();

   delete iGenInfo;
}

void CLbsPositionRequestor::DoCancel()
{
   // When a client application wishes to close one of its connections
   // to Location Server, there can be no outstanding requests on that
   // particular connection. If a client application attempts to close
   // a connection before outstanding requests have been cancelled or
   // completed, it is panicked.

   //If we are getting the last known position
   iPositioner.CancelRequest(EPositionerNotifyPositionUpdate);

}

void CLbsPositionRequestor::RunL()
{
   switch ( iStatus.Int() )
   {
      // The fix is valid
   case KErrNone:
      // The fix has only partially valid information.
      // It is guaranteed to only have a valid timestamp
   case KPositionPartialUpdate:
   {

      // Pre process the position information
      PositionUpdatedL();

      break;
   }
   // The data class used was not supported
   case KErrArgument:
   {

      // Request next position
      iPositioner.NotifyPositionUpdate( *iGenInfo, iStatus );

      // Set this object active
      SetActive();
      break;
   }
   // The position data could not be delivered
   case KPositionQualityLoss:
   {
      // Send event to position listener
      iPositionListener.ShowError( ErrQualityLoss );

      // Request position
      iPositioner.NotifyPositionUpdate( *iGenInfo, iStatus );

      // Set this object active
      SetActive();
      break;
   }
   // Access is denied
   case KErrAccessDenied:
   {
      // Send error to position listener
      iPositionListener.ShowError(ErrAccessDenied);
      break;
   }
   // Request timed out
   case KErrTimedOut:
   {
      // Send error to position listener
      iPositionListener.ShowError(ErrTimeOut);

      // Request position
      iPositioner.NotifyPositionUpdate( *iGenInfo, iStatus );

      // Set this object active
      SetActive();
      break;
   }
   // The request was canceled
   case KErrCancel:
   {
      // Send error to position listener
      iPositionListener.ShowError(ErrCanceled);
      break;
   }
   // There is no last known position
   case KErrUnknown:
   {
      // Send event to position listener
      iPositionListener.ShowError(LastPosUnknown);

      // Request next position
      iPositioner.NotifyPositionUpdate( *iGenInfo, iStatus );

      // Set this object active
      SetActive();
      break;
   }
   // Unrecoverable errors.
   default:
   {
      // Send error to position listener
      iPositionListener.ShowError(ErrLocRequest);
      break;
   }
   }

}

void CLbsPositionRequestor::DoInitialiseL()
{
   // Alloc & initialize the generic info buffer.
   // 10k should be enough.
   iGenInfo = HPositionGenericInfo::NewL( 10 * 1024 );
   iGenInfo->SetRequestedField( EPositionFieldNMEASentences );
   iGenInfo->SetRequestedField( EPositionFieldNMEASentencesStart );

   // Connect to the position server
   TInt error = iPosServer.Connect( );
    
   // The connection failed
   if ( KErrNone != error )
   {
      // Show error to the user and leave
      iPositionListener.ShowError(ErrPosServConn);
      return;
   }

   // Open subsession to the position server
   error = iPositioner.Open(iPosServer);

   // The opening of a subsession failed
   if ( KErrNone != error )
   {
      // Show error to the user and leave
      iPositionListener.ShowError(ErrOpenPos);
      iPosServer.Close();
      return;
   }

   // Set position requestor
   error = iPositioner.SetRequestor( CRequestor::ERequestorService ,
                                     CRequestor::EFormatApplication ,
                                     KRequestor );

   // The requestor could not be set
   if ( KErrNone != error )
   {
      // Show error to the user and leave

      iPositionListener.ShowError(ErrSetRequestor);
      iPositioner.Close();
      iPosServer.Close();
      return;
   }

   // Set update options
   error =  iPositioner.SetUpdateOptions( iUpdateops );

   // The options could not be updated
   if ( KErrNone != error  )
   {
      // Show error to the user and leave
      iPositionListener.ShowError(ErrSetUpOpt);
      iPositioner.Close();
      iPosServer.Close();
      return;
   }

   iPositioner.NotifyPositionUpdate( *iGenInfo, iStatus );
   
   // Set this active object active
   SetActive();
}

void
CLbsPositionRequestor::GetModuleName(const TPositionModuleId& aModuleId)
{
   // Module information of the position module
   TPositionModuleInfo modInfo;

   // Get module info by module id
   TInt error = iPosServer.GetModuleInfoById( *&aModuleId, modInfo );

   // If error occured
   if ( KErrNone != error )
   {
      // Set the name of the module to 'Unknown'
      iModuleName = KUnknown;
   }
   else
   {
      // Get the name of the position module
      modInfo.GetModuleName(iModuleName);
   }
}

void CLbsPositionRequestor::PositionUpdatedL()
{

   // Send position information to registered listener
   iPositionListener.PositionInfoUpdated( *iGenInfo, iModuleName );

   //! Set requested data field.
   iGenInfo->SetRequestedField( EPositionFieldNMEASentences );
   iGenInfo->SetRequestedField( EPositionFieldNMEASentencesStart );

   // Request next position
   iPositioner.NotifyPositionUpdate( *iGenInfo, iStatus );

   // Set this object active
   SetActive();

}

