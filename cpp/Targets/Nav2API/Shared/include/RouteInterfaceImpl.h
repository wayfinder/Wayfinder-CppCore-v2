/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_ROUTE_INTERFACE_IMPL_H
#define WFAPI_ROUTE_INTERFACE_IMPL_H

#include "WFAPIConfig.h"
#include "InterfaceImpl.h"
#include "SynchronousStatus.h"
#include "RouteInfoItemArray.h"
#include "TransportationType.h"
#include "RequestID.h"
#include "Favourite.h"
#include "SearchItem.h"

#include "NavServerComEnums.h"
#include "RouteEnums.h"

#include <set>

namespace isab {
// Forward declarations
class GuiProtMess;
}

namespace WFAPI {

// Forward declarations
class RouteListener;

class RouteInterfaceImpl : public InterfaceImpl {
public:

   /**
    * Constructor.
    */ 
   RouteInterfaceImpl(Nav2APIImpl* api);

   /**
    * Destructor.
    */
   virtual ~RouteInterfaceImpl();

   /**
    * Implementation of abstract function from InterfaceImpl.
    */
   //@{
   /**
    * Function called when sending reply from Nav2. This is called when
    * receiving replies from asynchronous requests.
    *
    * @param guiProtMess The returning data of the reply.
    * @param requestID The RequestID for the guiProtMess.
    */
   void receiveAsynchronousReply(const isab::GuiProtMess& guiProtMess,
                                 RequestID requestID);
   //@}

   /**
    * Functions wrapped from RouteInterface.
    */
   //@{

   /**
    * Add a listener for route callbacks.
    * @see RouteListener
    *
    * @param listener The RouteListener to add. 
    *                 It may not be deleted until it is removed by
    *                 calling removeRouteListener.
    */
   void addRouteListener(RouteListener* listener);

   /**
    * Remove a listener.
    *
    * @param listener The RouteListener to remove.
    *                 It should now be deleted by caller, unless caller
    *                 keeps it around for later use.
    */
   void removeRouteListener(RouteListener* listener);

   /**
    * Get the itinerary for the current route. 
    *
    * @param routeInfoList Filled with the route turn for the current route.
    * @return StatusCodes::OK if operation succeeded.
    */
   SynchronousStatus getRouteList(RouteInfoItemArray& routeInfoList);

   /**
    * Stop and remove the current route.
    *
    * @return The status of the operation, StatusCodes::OK if operation
    *         succeeded.
    */
   SynchronousStatus removeRoute();

   /**
    * This asynchronous function initiates a reroute using the current
    * destination, current GPS position and current TransportationType.
    * When reroute is done the added RouteListeners
    * will get a callback with the RequestID returned by this call.
    *
    * @return The status and if status is OK the RequestID that will be 
    *         returned in the RouteListeners routeReply function.
    */
   AsynchronousStatus reRoute();

   /**
    * This asynchronous function initiates a route using the parameters.
    * When route is done the added RouteListeners
    * will get a callback with the RequestID returned by this call.
    *
    * @param startCoord The origin coordinate of the route.
    * @param destCoord The destination coordinate of the route.
    * @param transportationType The selected mode of transportation.
    * @return The status and if status is OK the RequestID that will be
    *         returned in the RouteListeners routeReply function.
    */
   AsynchronousStatus routeBetweenCoordinates(
      const WGS84Coordinate& startCoord,
      const WGS84Coordinate& destCoord,
      TransportationType transportationType);

   /**
    * This asynchronous function initiates a route using the parameters and
    * the current GPS position at start coordinate.
    * When route is done the added RouteListeners
    * will get a callback with the RequestID returned by this call.
    *
    * @param destCoord The destination coordinate of the route.
    * @param transportationType The selected mode of transportation.
    * @return The status and if status is OK the RequestID that will be
    *         returned in the RouteListeners routeReply function.
    */
   AsynchronousStatus routeToCoordinate(const WGS84Coordinate& destCoord, 
                                        TransportationType transportationType);
   /**
    * To route to a favourite.
    * This asynchronous function initiates a route using the parameters and
    * the current GPS position at start coordinate.
    * When route is done the added RouteListeners
    * will get asynchronous callback with the RequestID returned by this call.
    *
    * @param favourite The favourite to route to.
    * @param transportationType The selected mode of transportation.
    * @return The status and if status is OK the RequestID that will be
    *         returned in the RouteListeners routeReply function.
    */
   AsynchronousStatus routeToFavourite(const Favourite& favourite, 
                                       TransportationType transportationType);
   
   /**
    * To route to a search item.
    * This asynchronous function initiates a route using the parameters and
    * the current GPS position at start coordinate.
    * When route is done the added RouteListeners
    * will get a callback with the RequestID returned by this call.
    *
    * @param searchItem The searchItem to route to.
    * @param transportationType The selected mode of transportation.
    * @return The status and if status is OK the RequestID that will be
    *         returned in the RouteListeners routeReply function.
    */
   AsynchronousStatus routeToSearchItem(const SearchItem& searchItem, 
                                        TransportationType transportationType);
    /**
    * Start simulation of a route. Note that a route needs to be
    * calculated before this function is called. If no route has
    * been calculated this function does nothing.
    *
    * @return The status of simulation start command, always OK.
    */
   SynchronousStatus routeSimulationStart();

   /**
    * Pauses simulation of a route. A call to routeSimulationResume
    * will resume the paused simulation. Note that a route needs to be
    * calculated before this function is called. If no route has
    * been calculated this function does nothing.
    *
    * @return The status of simulation pause command, always OK.
    */
   SynchronousStatus routeSimulationPause();

   /**
    * Resumes a paused simulation. Note that a route needs to be
    * calculated before this function is called. If no route has
    * been calculated this function does nothing.
    *
    * @return The status of simulation resume command, always OK.
    */
   SynchronousStatus routeSimulationResume();

   /**
    * Stop simulation of a route. Note that a route needs to be
    * calculated before this function is called. If no route has
    * been calculated this function does nothing.
    *
    * @return The status of simulation stop command, always OK.
    */
   SynchronousStatus routeSimulationStop();

   /**
    * Sets the simulation of a route to repeat the route simulation.
    * When the route is finished the simulation starts over from
    * the beginning.
    */
   SynchronousStatus routeSimulationRepeatOn();

   /**
    * Stops repeat of route simulation.
    */
   SynchronousStatus routeSimulationRepeatOff();

   /**
    * Increase the speed of the simulation of the route. 
    * Note that a route needs to be calculated before this function
    * is called. If no route has been calculated this function
    * does nothing.
    *
    * @return The status of increase the speed of simulation.
    */
   SynchronousStatus routeSimulationIncreaseSpeed();

   /**
    * Decrease the speed of the simulation of the route. 
    * Note that a route needs to be calculated before this function
    * is called. If no route has been calculated this function
    * does nothing.
    *
    * @return The status of decrease the speed of simulation.
    */
   SynchronousStatus routeSimulationDecreaseSpeed();

   //@}

   /**
    * Convert between Nav2 RouteAction to WFAPI::RouteAction
    *
    * @param action The Nav2 RouteAction.
    * @return The corresponding WFAPI::RouteAction.
    */
   static RouteAction nav2ActionToRouteAction(
         isab::RouteEnums::RouteAction action);
        
   /**
    * Convert between Nav2 RouteCrossing and WFAPI::RouteCrossing.
    *
    * @param crossing The Nav2 RouteCrossing.
    * @return The corresponding WFAPI::RouteCrossing.
    */
   static RouteCrossing nav2CrossingToRouteCrossing(
         isab::RouteEnums::RouteCrossing crossing);

   /**
    * Convert NavServerComEnums::VehicleType to WFAPI::TransportationType.
    *
    * @param transportationType The NavServerComEnums::VehicleType.
    * @return The corresponding WFAPI::TransportationType.
    */
   static TransportationType convertVehicleType(
      isab::NavServerComEnums::VehicleType vehicleType);

protected:

private:

   /**
    * Convert WFAPI::TransportationType to NavServerComEnums::VehicleType.
    *
    * @param transportationType The WFAPI::TransportationType.
    * @return The corresponding NavServerComEnums::VehicleType.
    */
   isab::NavServerComEnums::VehicleType convertTranspType(
                                    TransportationType transportationType);

   /**
    * Update the transportation type if necessary.
    *
    * @param transportationType The transportation type to update to.
    * @return An AsynchronousStatus with statusCode OK if OK, otherwise
    *         with appropriate failure codes set. Will contain
    *         INVALID_REQUEST_ID as RequestID.
    */
   AsynchronousStatus updateTransportationType(
         TransportationType transportationType);

   /**
    * Send a isab::GuiFileOperationCommand message to Nav2 and 
    * return the status of the operation.
    *
    * @param command The command to send.
    * @return The status of the operation.
    */
   SynchronousStatus sendGuiFileOperationCommand(wf_uint16 command);

   /**
    * Set of listeners.
    */
   std::set<RouteListener*> m_listeners;

   /**
    * The current vehicle type.
    * If not set at all, then NavServerComEnums::invalidVehicleType.
    */
   isab::NavServerComEnums::VehicleType m_curVehicleType;

   /// The route id, negative if not set.
   wf_int64 m_routeID;

   /**
    * The request id for a specific route, used for sending
    * reachedEndOfRouteReply to RouteListener.
    */
   RequestID m_requestID;

   /**
    * If a request to remove a route has been made. If this is set to true,
    * then no reachedEndOfRouteReply is sent to the route listener.
    */
   bool m_requestedRemoveRoute;
   
};

} // End namespace WFAPI

#endif // End WFAPI_ROUTE_INTERFACE_IMPL_H

