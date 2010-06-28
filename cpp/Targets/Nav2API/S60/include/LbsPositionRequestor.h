#ifndef LBSPOSITIONREQUESTOR_H
#define LBSPOSITIONREQUESTOR_H

#include <lbs.h>
#include <lbssatellite.h>

class MLbsPositionListener;

/**
 *  Position requestor.
 *  Request the position from the Location FW and
 *  handles the position requesting process
 */
class CLbsPositionRequestor : public CActive
{
public: 

   /**
    * Creates a new instance of a CLbsPositionRequestor
    * 
    * @param aPositionListener Position listener to be registered
    * @return The newly created CLbsPositionRequestor object
    */
   static CLbsPositionRequestor* NewL(
      MLbsPositionListener& aPositionListener) ;

   /**
    * Destructor.
    */
   virtual ~CLbsPositionRequestor();

protected:

   /**
    * @see CActive::DoCancel
    */
   void DoCancel();

   /**
    * @see CActive::RunL
    */
   void RunL();

private:

   /**
    * By default Symbian 2nd phase constructor is private.
    */
   void ConstructL();

   /**
    * C++ default constructor. Declared private to enforce usage of NewL.
    * 
    * @param aPositionListener Position listener to be registered
    */
   CLbsPositionRequestor(MLbsPositionListener& aPositionListener);

   /**
    * Starts a position requesting sequence
    */
   void DoInitialiseL();
        
   /**
    * Gets the name of the positioning module. If the name is not
    * available the name will be an empty string
    * 
    * @param aModuleId The id of the module
    */
   void GetModuleName(const TPositionModuleId& aModuleId);  
        
   /**
    * Preprocess the position information
    */
   void PositionUpdatedL();

private:

   /// The id of the currently used PSY
   TPositionModuleId iUsedPsy; 

   /// Position server
   RPositionServer iPosServer;

   /// Positioner
   RPositioner iPositioner;

   /// Basic location info
   TPositionInfo iPositionInfo;  

   // Generic position info, used to fetch nmea sentences.
   HPositionGenericInfo *iGenInfo;

   /// Position listener
   MLbsPositionListener& iPositionListener;
    
   // Module name
   TBuf<KPositionMaxModuleName> iModuleName;
        
   /// The id of the used psy
   TPositionUpdateOptions iUpdateops;
};

#endif // LBSPOSITIONREQUESTOR_H

