/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "OverlayViewOperationHandler.h"
#include "MapProjection.h"
#include "OverlayView.h"
#include "MapProjectionAdapter.h"


double OverlayViewOperationHandler::zoom(double factor)
{
   // We only zoom to point if we're zooming in
   if( factor > 1.0 ) {
      m_lastProjectionEvent = MapProjectionListener::UNKNOWN;
      return MapLibOperationHandler::zoom(factor);
   }
   
   if(!m_view->automaticHighlightEnabled() || !m_view->hasValidClosestItem()){
      return MapLibOperationHandler::zoom(factor);
   } else {
      if (!m_lastActionWasZoomRelated)
      {
         m_view->updateClosestItemPoint();
      }
      
      return MapLibOperationHandler::zoom(factor,
                                          m_view->getClosestItemCoord(),
                                          m_view->getClosestItemPoint());
   }
}

double OverlayViewOperationHandler::zoom(
   double factor, 
   const WFAPI::WGS84Coordinate& zoomCoord,
   const WFAPI::ScreenPoint& zoomPoint)
{
   // We only zoom to point w/ overlay if we're zooming in
   if( factor > 1.0 ) {
      m_lastProjectionEvent = MapProjectionListener::UNKNOWN;
      return MapLibOperationHandler::zoom(factor, zoomCoord, zoomPoint);
   }
 
   if(!m_view->automaticHighlightEnabled() || !m_view->hasValidClosestItem()) {
      return MapLibOperationHandler::zoom(factor, zoomCoord, zoomPoint);
   } else {
      if (!m_lastActionWasZoomRelated)
      {   
         m_view->updateClosestItemPoint();
      }

      return MapLibOperationHandler::zoom(factor,
                                          m_view->getClosestItemCoord(),
                                          m_view->getClosestItemPoint());
   }
}

double OverlayViewOperationHandler::setZoomLevel(double zoomLevel)
{

   if (zoomLevel > getZoomLevel()){
      // Currently, we only use magnetic zoom when zooming in.
      m_lastProjectionEvent = MapProjectionListener::UNKNOWN;

      return MapLibOperationHandler::setZoomLevel(zoomLevel);
   }
   
   if(!m_view->automaticHighlightEnabled() || !m_view->hasValidClosestItem()) {
      return MapLibOperationHandler::setZoomLevel(zoomLevel);
   } else {
      if (!m_lastActionWasZoomRelated)
      {   
         m_view->updateClosestItemPoint();
      }

      WFAPI::WGS84Coordinate coord = m_view->getClosestItemCoord();
      WFAPI::ScreenPoint oldPoint = m_view->getClosestItemPoint();

      double ret = MapLibOperationHandler::setZoomLevel(zoomLevel);
      MapLibOperationHandler::setPoint(coord, oldPoint);
      return ret;
   }
}

OverlayViewOperationHandler::
OverlayViewOperationHandler(OverlayView* view,
                            MapProjectionAdapter* projAdapter)
   : MapLibOperationHandler(projAdapter),
     m_lastProjectionEvent(MapProjectionListener::UNKNOWN),
     m_lastActionWasZoomRelated(false),
     m_view(view)
{
   
}

void OverlayViewOperationHandler::projectionUpdated(projectionEvent trigger)
{
   if (m_view->automaticHighlightEnabled()){
      if (trigger == MapProjectionListener::ZOOM){
         // The new event was a regular zoom.
         m_lastActionWasZoomRelated = true;         
      } else if (trigger == MapProjectionListener::ZOOM_POINT){
         // The new event was a zoom to point.
         m_lastActionWasZoomRelated = true;
      } else if (m_lastProjectionEvent == MapProjectionListener::SET_ZOOM &&
                 trigger == MapProjectionListener::SET_POINT){
         // The last event was SET_ZOOM and the new event was SET_POINT,
         // this means that we intercepted the setZoomLevel function.
         m_lastActionWasZoomRelated = true;
      } else {
         // Some other uninteresting action was performed.
         m_lastActionWasZoomRelated = false;         
      }

      m_lastProjectionEvent = trigger;
   } else {
      m_lastProjectionEvent = MapProjectionListener::UNKNOWN;
   }
 
}

