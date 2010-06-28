/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_ROUTE_INTERFACE_H
#define WFAPI_ROUTE_INTERFACE_H

#include "WFAPIConfig.h"
#include "Interface.h"
#include "TransportationType.h"
#include "RouteInfoItemArray.h"
#include "SynchronousStatus.h"
#include "AsynchronousStatus.h"
#include "RequestID.h"
#include "Favourite.h"
#include "SearchItem.h"

namespace WFAPI {

// Forward declarations
class RouteListener;
class Nav2APIImpl;
class RouteInterfaceImpl;

/**
 * Interface class for routes.
 */
class RouteInterface : public Interface
{

public:

   /**
    * Destructor.
    */
   virtual ~RouteInterface();

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
    * will get a callback with the RequestID returned by this call.
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

   /**
    * Internal function.
    */
   RouteInterfaceImpl* getImpl();

private:
   /**
    * Do not create RouteInterface get it from the Nav2API class.
    */
   RouteInterface(Nav2APIImpl* nav2APIimpl);

   /**
    * We do not allow copies of this interface.
    */
   RouteInterface(const RouteInterface&);
   const RouteInterface& operator=(const RouteInterface&);

   friend class Nav2APIImpl;

   struct impl;

   impl* m_impl;
};

} // End namespace WFAPI

#endif // End WFAPI_ROUTE_INTERFACE_H
