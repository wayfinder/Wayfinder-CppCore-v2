/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef ROUTE_TESTER_H
#define ROUTE_TESTER_H


#include "RouteListener.h"
#include "NavigationInfoUpdateListener.h"
#include "RequestID.h"
#include "NavigationInterface.h"
#include "WGS84Coordinate.h"
#include "TransportationType.h"
#include "Nav2API.h"
#include "WFStringArray.h"
#include "GenericTester.h"
#include "Favourite.h"
#include "SearchItem.h"
#include <vector>

/// Forward declarations
class WFAPITester;

using namespace WFAPI;

/**
 * Test class that implements route and navigation testing.
 */
class RouteTester : public RouteListener,
                    public NavigationInfoUpdateListener,
                    public GenericTester

{

public:

   /**
   * Destructor.
   */
   ~RouteTester();

   /**
    * Constructor, adds itself as listener to RouteInterface and
    * NavigationInterface.
    *
    * @param parentTester The WFAPITester that owns the instance of this
    *                     class. Needed for some interaction with the
    *                     MapLibAPI handling classes.
    * @param routeInterface The route interface which to add ourself as
    *                       listener and to request routes from.
    * @param navigationinterface The navigation interface which to add
    *                            ourself to for navigation updates.
    * @param resourceStoragePath The path to resource storage.
    * @param audioTypeDirName The name of the sound directory.
    * @param audioTypeExt The file extension for the sounds used.
    * @param audioLangString The string for the language.
    */
   RouteTester(WFAPITester* parentTester,
               RouteInterface& routeInterface,
               NavigationInterface& navigationInterface,
               const WFString& resourceStoragePath,
               const WFString& audioTypeDirName,
               const WFString& audioTypeExt,
               const WFString& audioLangString);

   /**
    * Requests a route by calling m_routeInterface with the given
    * parameters. The returned request id is stored in a vector.
    * When we get routeReply we can check if it is a request for us or not.
    *
    * @param startCoord The coordinate of the origin point of the route.
    * @param destCoord The coordinate of the destination point of the route.
    * @param transportationtype The transportation mode for which the route
    *                           should be optimized.
    */
   void routeBetweenCoordinates(const WGS84Coordinate& startCoord,
                           const WGS84Coordinate& destCoord,
                           TransportationType transportationType);

   /**
    * Requests a route from the current postion to the given destination.
    *
    * @param destCoord The coordinate of the destination point of the route.
    * @param transportationType The transportation mode for which the route
    *                           should be optimized.
    */
   void routeToCoordinate(const WGS84Coordinate& destCoord, 
                     TransportationType transportationType);
   /**
    * Requests a route from the current postion to the given favourite.
    *
    * @param favourite The favourite to route to.
    * @param transportationType The transportation mode for which the route
    *                           should be optimized.
    */
   void routeToFavourite(const Favourite& favourite, 
                         TransportationType transportationType);
   /**
    * Requests a route from the current postion to the given searchItem.
    *
    * @param searchItem The searchItem to route to.
    * @param transportationType The transportation mode for which the route
    *                           should be optimized.
    */
   void routeToSearchItem(const SearchItem& searchItem, 
                          TransportationType transportationType);

   /**
    * Requests the complete route list of the current route.
    */
   void getRouteList();

   /**
    * Requests a re route of the current route.
    */
   void reRoute();

   /**
    * Starts simulation of the current route.
    * NOTE: This should not be called unless a route has been calculated.
    */
   void routeSimStart();

   /**
    * Stops simulation of the current route.
    * NOTE: This should not be called unless a route has been calculated.
    */
   void routeSimStop();

   /**
    * Pauses the current route simulation.
    * NOTE: This should not be called unless a route has been calculated.
    */
   void routeSimPause();

   /**
    * Resumes the current route simulation.
    * NOTE: This should not be called unless a route has been calculated.
    */
   void routeSimResume();

   /**
    * Increases the speed of the current route simulation.
    * NOTE: This should not be called unless a route has been calculated.
    */
   void routeSimInc();

   /**
    * Decreases the speed of the current route simulation.
    * NOTE: This should not be called unless a route has been calculated.
    */
   void routeSimDec();

   /**
    * Clear the current route.
    * NOTE: This should not be called unless a route has been calculated.
    */
   void clearRoute();
   
   /**
    * Handle the input and return true if it was consumed.
    *
    * @param input The input to handle.
    * @param goBack Out parameter. Will be set to true if the
    *               input indicated that the tester should go
    *               out of scope.
    * @return True if the input was handled, false otherwise.
    */
   bool handleInput(const char* input, bool& goBack);

   /**
    * Add external navigation update info listener.
    *
    * @param listener The listener to add.
    */
   void addNavigationInfoUpateListener(NavigationInfoUpdateListener* listener);

   /**
    * Remove external navigation update info listener.
    *
    * @param listener The listener to remove.
    */
   void removeNavigationInfoUpateListener(NavigationInfoUpdateListener* listener);
   
public: // From base classes

   /**
    * From RouteListener, called when a route has been calculated.
    *
    * @param requestID The request id for the route request, will be matched
    *                  against the request ids we have stored in our vector.
    */
   void routeReply(RequestID requestID);

   /**
    * From RouteListener, called when the user has reached the end of a route.
    *
    * @param requestID The request id for the route request.
    */
   void reachedEndOfRouteReply(RequestID requestID);

   /**
    * Called when an error has occured.
    */
   virtual void error(const AsynchronousStatus& status);

   /**
    * From NavigationListener, update function called during a route.
    *
    * @param info Info containing information about the route.
    */
   virtual void distanceUpdate(const UpdateNavigationDistanceInfo& info);

   /**
    * From NavigationListener, update function called during a route.
    *
    * @param info Info containing information about the route.
    */
   virtual void infoUpdate(const UpdateNavigationInfo& info);

   /**
    * From NavigationListener, called when a sound should be played.
    *
    * @param info Info containing information about the route.
    */
   virtual void playSound();

   /**
    * From NavigationListener, called before playSound so that we can
    * load sounds etc.
    *
    * @param info Info containing information about the route.
    */
   virtual void prepareSound(const WFStringArray& soundNames);

   /**
    * Play the current sounds.
    */
   void playCurrentSounds();

private:

   /**
    * Adds the command options valid for this class. The storage is a map
    * where the key is a String and the value is a CommandHolder.
    * The CommandHolder holds the actual command to perform so by finding
    * in the map after user input we can map this against a real command.
    */
   void addCommandOptions();

   /**
    * Display sub menu for route between two coordinates.
    */
   void displayRouteBetweenCoordinatesMenu();

   /**
    * Display sub menu for route to a coordinate from the current position.
    */
   void displayRouteToCoordinateMenu();

   /// A pointer to the parent tester class.
   WFAPITester* m_parentTester;
   
   /// Reference to the RouteInterface
   RouteInterface& m_routeInterface;

   /// Reference to the NavigationInterface
   NavigationInterface& m_navigationInterface;

   /// Vector containing the request ids for our requests.
   std::vector<RequestID> m_requestIDVec;

   /// Default start coordinate used when calculating the default route.
   WGS84Coordinate m_defaultStartCoord;

   /// Default dest coordinate used when calculating the default route.
   WGS84Coordinate m_defaultDestCoord;

   /// The sounds from the last prepare sounds call.
   WFStringArray m_currentSounds;

   /// The path to the resources.
   WFString m_resourceStoragePath;

   /// The name of the sound directory.
   WFString m_audioTypeDirName;

   /// The file extension for the sounds used.
   WFString m_audioTypeExt;

   /// The language as two letter string.
   WFString m_audioLangString;
};

#endif
