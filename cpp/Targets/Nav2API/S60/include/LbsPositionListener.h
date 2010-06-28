/*
 * ==============================================================================
 *  Name        : LbsPositionListener.h
 *  Part of     : LBSReference example application
 *  Interface   : 
 *  Description : Position listener interface
 *  Version     : 1.0
 *
 *  Copyright (c) 2005-2006 Nokia Corporation.
 *  This material, including documentation and any related 
 *  computer programs, is protected by copyright controlled by 
 *  Nokia Corporation.
 * ==============================================================================
 */

#ifndef LBSPOSITIONLISTENER_H
#define LBSPOSITIONLISTENER_H

#include <lbssatellite.h>
#include <lbspositioninfo.h>
#include "LocationStatusCode.h"

class TPositionInfo;
class TPositionSatelliteInfo;

enum PositionEvent
{
   LastPosUnknown = 0, // The last position is unknown.
   ErrLocRequest, // Request error.
   ErrAccessDenied, // Access denied.
   ErrOpenPos, // Error while opening a positioner.
   ErrPosServConn, // Error when connecting to server.
   ErrSetRequestor, // Error when setting the requestor.
   ErrSetUpOpt, // Error when setting update options.
   ErrCanceled, // Location request canceled.
   ErrQualityLoss, // Unable to return position information.
   ErrTimeOut, // The request timed out.
};

/**
 *   Position data listener interface listens position information.
 *   It can also be used to transfer error messages and codes.
 */
class MLbsPositionListener
{
public:

   /**
    * PositionInfoUpdatedL informs the listener about new location information
    * @param aModulename The name of the module which provided this information
    * @param aPosInfo Position information class
    */
   virtual void PositionInfoUpdated(HPositionGenericInfo& aPosInfo,
                                    const TDesC& aModulename ) = 0;

   /**
   * ShowErrorL informs the listener about occurred error during position 
   * request process
   * @param aEvent The type of error event.
   */
   virtual void ShowError(PositionEvent aEvent) = 0;
        
};

#endif //LBSPOSITIONLISTENER_H


// End of File
