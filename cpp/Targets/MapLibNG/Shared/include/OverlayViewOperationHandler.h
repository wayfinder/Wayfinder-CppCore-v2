/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef OVERLAYVIEWOPERATIONHANDLER_H
#define OVERLAYVIEWOPERATIONHANDLER_H

#include "MapProjection/MapProjection.h"
#include "StackedSet.h"
#include "StackedDialog.h"
#include "MapLibOperationHandler.h"

class OverlayView;
class MapProjectionAdapter;
        
/**
 * This class is used to overload certain map operations to
 * behave in a way specific to the OverlayView.
 */
class OverlayViewOperationHandler : public MapProjectionListener,
                                    public MapLibOperationHandler {
public:
   /**
    * Constructor.
    * @param view The OverlayView that is tied to the operations.
    * @param projAdapter The projection adapter that performs
    *                    the map operations.
    */
   OverlayViewOperationHandler(OverlayView* view,
                               MapProjectionAdapter* projAdapter);

   /**
    * This function is overloaded to enable magnetic zoom
    * @see MapLibOperationHandler::zoom
    */
   virtual double zoom(double factor);

   /**
    * This function is overloaded to enable magnetic zoom
    * @see MapLibOperationHandler::zoom
    */
   virtual double zoom(double factor, 
                       const WFAPI::WGS84Coordinate& zoomCoord,
                       const WFAPI::ScreenPoint& zoomPoint);
   /**
    * This function is overloaded to enable magnetic zoom
    * @see MapLibOperationHandler::setZoomLevel
    */
   virtual double setZoomLevel(double zoomLevel);

   /**
    * We need to keep track of the type of projection updates
    * in order to update the magnetic zoom parameters correctly.
    */ 
   virtual void projectionUpdated(projectionEvent trigger);
   
private:
   MapProjectionListener::projectionEvent m_lastProjectionEvent;
   bool m_lastActionWasZoomRelated;
   OverlayView* m_view;
};

#endif // OVERLAYVIEWOPERATIONHANDLER_H
