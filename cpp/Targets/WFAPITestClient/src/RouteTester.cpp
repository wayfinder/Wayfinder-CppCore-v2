/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "RouteTester.h"
#include "RouteInterface.h"
#include "WFString.h"
#include "WFAPITester.h"
#include <iostream>
#include <gtk/gtk.h>

// #define PRINT_VERBOSE

#ifdef HAVE_OGG123
#include <signal.h>
#endif

using namespace std;
using namespace WFAPI;

RouteTester::RouteTester(WFAPITester* parentTester,
                         RouteInterface& routeInterface,
                         NavigationInterface& navigationInterface,
                         const WFString& resourceStoragePath,
                         const WFString& audioTypeDirName,
                         const WFString& audioTypeExt,
                         const WFString& audioLangString) 
   : m_parentTester(parentTester),
     m_routeInterface(routeInterface),
     m_navigationInterface(navigationInterface),
     m_defaultStartCoord(55.5846,13.0076),
     m_defaultDestCoord(55.6087,13.0196),
     m_resourceStoragePath(resourceStoragePath),
     m_audioTypeDirName(audioTypeDirName),
     m_audioTypeExt(audioTypeExt),
     m_audioLangString(audioLangString)
{
   // Add this as listener for route and navigation callbacks.
   m_routeInterface.addRouteListener(this);
   m_navigationInterface.addNavigationInfoUpdateListener(this);

   // Add the commands for route testing.
   addCommandOptions();
}

RouteTester::~RouteTester() 
{
   m_routeInterface.removeRouteListener(this);
   m_navigationInterface.removeNavigationInfoUpdateListener(this);
}

void 
RouteTester::displayRouteBetweenCoordinatesMenu()
{
   // Get the start and destination coordinate from user.
   cout << "Start coord: Enter latitude in degrees (WGS84 decimal format): ";
   double startCoordLat = 0.0;
   cin >> startCoordLat;

   cout << "Start coord: Enter longitude in degrees (WGS84 decimal format): ";
   double startCoordLon = 0.0;
   cin >> startCoordLon;

   cout << "End coord: Enter latitude in degrees (WGS84 decimal format): ";
   double endCoordLat = 0.0;
   cin >> endCoordLat;

   cout << "End coord: Enter longitude in degrees (WGS84 decimal format): ";
   double endCoordLon = 0.0;
   cin >> endCoordLon;

   // Get transportation mode.
   cout << "Time to select transportation mode, type 0 for car and "
        << "1 for pedestrian: ";
   wf_int transportationMode = 0;
   cin >> transportationMode;

   // Data read, create WGS84Coordinates
   WGS84Coordinate startCoord(startCoordLat, startCoordLon);
   WGS84Coordinate endCoord(endCoordLat, endCoordLon);
   // Send the requet for this route.
   routeBetweenCoordinates(startCoord, 
                           endCoord, 
                           TransportationType(transportationMode));
}

void RouteTester::displayRouteToCoordinateMenu()
{
   cout << "Nothing here yet!" << endl;
}

void 
RouteTester::addCommandOptions()
{
   m_commandOptions.
      insert(make_pair("h",
                       CommandHolder("Displays this help menu.", help)));
   m_commandOptions.
      insert(make_pair("d", 
                       CommandHolder("Calculate a default route.", 
                                     cmdDefaultRoute)));
   m_commandOptions.
      insert(make_pair("bc", 
                       CommandHolder("Route between two coordinates, the "
                                     "first coordinate is the start point.",
                                     cmdRouteBetweenCoordinates)));
   m_commandOptions.
      insert(make_pair("tc", 
                       CommandHolder("Route to a specified coordinate.", 
                                     cmdRouteToCoordinate)));
   m_commandOptions.
      insert(make_pair("rr", 
                       CommandHolder("Re route, re calculates the current "
                                     "route from your current position.", 
                                     cmdReroute)));
   m_commandOptions.
      insert(make_pair("l", 
                       CommandHolder("Request route list based on current "
                                     "route.", 
                                     cmdRouteList)));
   m_commandOptions.
      insert(make_pair("s", 
                       CommandHolder("Start route simulation.", 
                                     cmdRouteSimStart)));
   m_commandOptions.
      insert(make_pair("t", 
                       CommandHolder("Stop route simulation.", 
                                     cmdRouteSimStop)));
   m_commandOptions.
      insert(make_pair("pa", 
                       CommandHolder("Pause route simulation.", 
                                     cmdRouteSimPause)));
   m_commandOptions.
      insert(make_pair("re", 
                       CommandHolder("Resume route simulation.", 
                                     cmdRouteSimResume)));
   m_commandOptions.
      insert(make_pair("+", 
                       CommandHolder("Increase speed of route simulation.", 
                                     cmdRouteSimInc)));
   m_commandOptions.
      insert(make_pair("-", 
                       CommandHolder("Decrease speed of route simulation.", 
                                     cmdRouteSimDec)));
   m_commandOptions.
      insert(make_pair("p", 
                       CommandHolder("Snap to the current position.",
                                     cmdRouteSnapToPosition)));

   m_commandOptions.
      insert(make_pair("c", 
                       CommandHolder("Clear current route.", 
                                     cmdRouteClear)));
   m_commandOptions.
      insert(make_pair("b", 
                       CommandHolder("Back to previous menu.", 
                                     back)));
}

void 
RouteTester::routeBetweenCoordinates(const WGS84Coordinate& startCoord,
                                     const WGS84Coordinate& destCoord,
                                     TransportationType transportationType) 
{
   cout << "routeBetweenCoordinates with the following coords:" << endl;

   cout << "startCoord: latitude = " << startCoord.latDeg 
        << ", longitude = " << startCoord.lonDeg << endl;

   cout << "endCoord: latitude = " << destCoord.latDeg 
        << ", longitude = " << destCoord.lonDeg << endl;

   // Send the request for a route to the server.
   AsynchronousStatus status = m_routeInterface.
      routeBetweenCoordinates(startCoord,
                              destCoord,
                              transportationType);
   if (status.getStatusCode() != 0) {
      cout << "Error sending request for the route: "
           << status.getStatusCode() << endl;
      return;
   }
   cout << "Route request sent to engine!" << endl;
   m_requestIDVec.push_back(status.getRequestID());
}

void 
RouteTester::routeToCoordinate(const WGS84Coordinate& destCoord, 
                               TransportationType transportationType) 
{
   AsynchronousStatus status = m_routeInterface.
      routeToCoordinate(destCoord,
                        transportationType);
   if (status.getStatusCode() != 0) {
      cout << "Error routing to coordinate: "
           << status.getStatusCode() << endl;
      return;
   }
   m_requestIDVec.push_back(status.getRequestID());
}

void
RouteTester::routeToFavourite(const Favourite& favourite, 
                              TransportationType transportationType)
{
   AsynchronousStatus status = m_routeInterface.
      routeToFavourite(favourite,
                       transportationType);
   if (status.getStatusCode() != 0) {
      cout << "Error routing to favourite: "
           << status.getStatusCode() << endl;
      return;
   }
   m_requestIDVec.push_back(status.getRequestID());
}

void
RouteTester::routeToSearchItem(const SearchItem& searchItem, 
                               TransportationType transportationType)
{
   AsynchronousStatus status = m_routeInterface.
      routeToSearchItem(searchItem,
                        transportationType);
   if (status.getStatusCode() != 0) {
      cout << "Error routing to searchItem: "
           << status.getStatusCode() << endl;
      return;
   }
   m_requestIDVec.push_back(status.getRequestID());
}
void 
RouteTester::routeReply(RequestID requestID) 
{
   // Find the request id in the vector to see if it belongs to us
   for (vector<RequestID>::iterator it = m_requestIDVec.begin();
        it != m_requestIDVec.end();) {
      if (requestID == (*it)) {
         it = m_requestIDVec.erase(it);
         cout << "RouteTester::routeReply: route has been calculated!" 
              << endl;
      } else {
         ++it;
      }
   }
}

void 
RouteTester::error(const AsynchronousStatus& status)
{
   printErrorStatus(&status, "RouteTester");
}

void 
RouteTester::getRouteList() 
{
   bool onDetour = false;
   RouteInfoItemArray routeInfoItemArray;
   routeInfoItemArray.reserve(50);
   SynchronousStatus status = m_routeInterface.
      getRouteList(routeInfoItemArray);
   cout << "RT::getRouteList, statusCode = " << status.getStatusCode() << endl;
   cout << "==========================================" << endl;
   for (RouteInfoItemArray::const_iterator it = routeInfoItemArray.begin();
        it != routeInfoItemArray.end();
        ++it) {

      cout << "==========================================" << endl;
      cout << "Street name: " << it->getStreetName().c_str() << endl;
      cout << "Distance to next turn: " << it->getDistanceToNextTurn() 
           << endl;
      cout << "Distance to goal: " << it->getDistanceToGoal() << endl;
      cout << "Time to goal: " << it->getTimeToGoal() << endl;
      cout << "Crossing: " << it->getCrossing() << endl;
      cout << "ExitCount: " << it->getExitCount() << endl;
      cout << "Action: " << it->getAction() << endl;
      cout << "Coordinate of next turn: " << it->getCoordinate() << endl;
      cout << "Has leftSideTraffic: " << it->getIfLeftSideTraffic() << endl;
      cout << "Is high way: " << it->getIfHighWay() << endl;
      cout << "Has speed camera: " << it->getIfHasSpeedCameraLandmark()
           << endl;
      cout << "==========================================" << endl;

      if(it->getIfHasSpeedCameraLandmark()){
         cout << "============== SPEED CAMERA ==============" << endl;
         cout << "Speed camera info: " << it->getSpeedCameraInfo() << endl;
         cout << "============== SPEED CAMERA ==============" << endl;
      }
      
      if(!onDetour && it->getIfHasDetourLandmark())
      {
         cout << "============== DETOUR STARTS =============" << endl;
         cout << "Detour! Information: " << it->getDetourInfo() << endl;
         onDetour = true;
         cout << "============== DETOUR STARTS =============" << endl;
      }
      
      if(onDetour && !it->getIfHasDetourLandmark())
      {
         cout << "=============== DETOUR END  ===============" << endl;
         cout << "Detour Ends! Information: " << it->getDetourInfo() << endl;
         onDetour = false;
         cout << "=============== DETOUR END ===============" << endl;
      }
      
   }
}

void
RouteTester::reRoute() 
{
   AsynchronousStatus status = m_routeInterface.reRoute();
   if (status.getStatusCode() != 0) {
      cout << "Error sending reroute: "
           << status.getStatusCode() << " message " 
           << status.getStatusMessage()<< endl;
      return;
   }
   cout << "re route request sent to engine!" << endl;
   m_requestIDVec.push_back(status.getRequestID());
}

void
RouteTester::routeSimStart()
{
   m_routeInterface.routeSimulationStart();
}

void
RouteTester::routeSimStop()
{
   m_routeInterface.routeSimulationStop();
}

void
RouteTester::routeSimPause()
{
   m_routeInterface.routeSimulationPause();
}

void
RouteTester::routeSimResume()
{
   m_routeInterface.routeSimulationResume();
}

void
RouteTester::routeSimInc()
{
   m_routeInterface.routeSimulationIncreaseSpeed();
}

void
RouteTester::routeSimDec()
{
   m_routeInterface.routeSimulationDecreaseSpeed();
}

void 
RouteTester::clearRoute()
{
   m_routeInterface.removeRoute();
}

bool 
RouteTester::handleInput(const char* input, bool& goBack)
{
   // Find the command and handle it.
   StringMap::const_iterator it = m_commandOptions.find(input);

   if (it == m_commandOptions.end()) {
      // Not a command, return false to indicate that the command wasn't 
      // handled.
      return false;
   }

   // Handle the command.
   switch((*it).second.m_commandType) {
      case help:  
         printHelp();
         break;
      case cmdDefaultRoute:
         routeBetweenCoordinates(m_defaultStartCoord,
                                 m_defaultDestCoord,
                                 CAR);
         break;
      case cmdRouteBetweenCoordinates:
         displayRouteBetweenCoordinatesMenu();
         break;
      case cmdRouteToCoordinate:
         displayRouteToCoordinateMenu();
         break;
      case cmdReroute:
         reRoute();
         break;
      case cmdRouteSimStart:
         m_parentTester->snapToPosition();
         routeSimStart();
         break;
      case cmdRouteSimStop:
         routeSimStop();
         break;
      case cmdRouteSimPause:
         routeSimPause();
         break;
      case cmdRouteSimResume:
         routeSimResume();
         break;
      case cmdRouteSimInc:
         routeSimInc();
         break;
      case cmdRouteSimDec:
         routeSimDec();
         break;
      case cmdRouteSnapToPosition:
         m_parentTester->snapToPosition();
         break;
      case cmdRouteClear:
         clearRoute();
         break;
      case cmdRouteList:
         getRouteList();
         break;
      case back:
         goBack = true;
         break;
      default:
         cout << "Command not handled here, please try again or select "
              << "B to go back." << endl;
         return false;
         break;
   }
   // If we get here, the key has been consumed.
   return true;
}

void 
RouteTester::addNavigationInfoUpateListener(NavigationInfoUpdateListener* listener)
{
   m_navigationInterface.addNavigationInfoUpdateListener(listener);
}

void 
RouteTester::removeNavigationInfoUpateListener(NavigationInfoUpdateListener* listener)
{
   m_navigationInterface.removeNavigationInfoUpdateListener(listener);
}


void
RouteTester::distanceUpdate(const UpdateNavigationDistanceInfo& info)
{
#ifdef PRINT_VERBOSE
   // Print header
   cout << "*******************************************************" << endl;
   cout << "****************** Distance Update ********************" << endl;
   cout << "*******************************************************" << endl;

   // Print data
   cout << "Distance to goal:              " << info.getDistanceToGoal() << endl;
   cout << "Distance to goal in unit:      " << info.getDistanceToGoalInUnit()
        << endl;
   cout << "Distance to next turn:         " << info.getDistanceToNextTurn() << endl;
   cout << "Distance to next turn in unit: " <<
      info.getDistanceToNextTurnInUnit() << endl;
   cout << "Speed:                         " << info.getSpeed() << endl;
   cout << "Time to goal:                  " << info.getTimeToGoal() << endl;
   cout << "Time to next turn:             " << info.getTimeToNextTurn() << endl;

   // Print footer :)
   cout << "*******************************************************" << endl;

#else
   cout << "Distance update: " << info.getDistanceToGoal() << "m to goal, "
        << info.getDistanceToNextTurn() << "m to next turn."
        << endl;
#endif
}

void 
RouteTester::infoUpdate(const UpdateNavigationInfo& info)
{
#ifdef PRINT_VERBOSE
   // Print header
   cout << "*******************************************************" << endl;
   cout << "******************** Info Update **********************" << endl;
   cout << "*******************************************************" << endl;

   // Print data
   if (info.isOnTrack()) {
      cout << "On track!" << endl;
   } else {
      cout << "Off track!" << endl;
   }
   cout << "Next action: " << info.getNextAction() << endl;
   cout << "Next crossing: " << info.getNextCrossing() << endl;
   cout << "Exit count: " << info.getExitCount() << endl;
   cout << "Next street name: " << info.getNextStreetName().c_str() << endl;
   cout << "Current street name: " << info.getCurrentStreetName().c_str()
        << endl;
   cout << "Next street after next turn: " <<
      info.getNextStreetNameAfterNextTurn().c_str() << endl;
   cout << "Position of next turn: " << info.getPosition() << endl;
   cout << "Speed: " << info.getSpeed() << endl;
   cout << "Time to goal: " << info.getTimeToGoal() << endl;
   cout << "Time to next turn: " << info.getTimeToNextTurn() << endl;
   cout << "Distance to goal: " << info.getDistanceToGoal() << endl;
   cout << "Distance to goal: " << info.getDistanceToGoalInUnit() << endl;
   cout << "Distance to next turn: " << info.getDistanceToNextTurn() << endl;
   cout << "Distance to next turn: " << info.getDistanceToNextTurnInUnit() << endl;
   cout << "Distance to turn after next turn: "
        << info.getDistanceToTurnAfterNextTurn() << endl;
   cout << "Distance to turn after next turn: "
        << info.getDistanceToTurnAfterNextTurnInUnit() << endl;
   cout << "Action after next crossing: " << info.getActionAfterNextAction()
        << endl;
   cout << "Crossing after next crossing: "
        << info.getCrossingAfterNextCrossing() << endl;
   cout << "Exit count for action after next action: "
        << info.getExitCountForActionAfterNextAction() << endl;
   cout << "NextIsHighway: " << info.getNextHighway() << endl;
   cout << "Detour: " << info.getDetour() << endl;
   cout << "Speedcamera: " << info.getSpeedcam() << endl;
   cout << "IsLeftSideTraffic: " << info.getIfLeftSideTraffic() << endl;
#else
   cout << "Info upd: Next street: " << info.getNextStreetName() << ", "
        << "action: " << info.getNextAction() << std::endl;
#endif
}

#ifdef HAVE_OGG123
void* playSoundRun(void* attr)
{
   RouteTester* r = static_cast<RouteTester*>(attr);
   r->playCurrentSounds();

   return NULL;
}
#endif

/**
 * Function used for thread syncronization so soundPlayed in navigation
 * interface can be called from another thread.
 *
 * @param callbackdata Contains the navigations interface
 * @return returns false so the callback is not scheduled again.
 */
gboolean soundplayedFunction(void* callbackData) {
   static_cast<NavigationInterface*>(callbackData)->soundPlayed();
   return false;
}

void
RouteTester::playCurrentSounds()
{
#ifdef HAVE_OGG123
   std::string command( "ogg123" );
   for (WFStringArray::const_iterator it = m_currentSounds.begin(); 
        it != m_currentSounds.end(); ++it) {
      command += " ";
      command += (*it).c_str();
   }
   int res = system(command.c_str());

   cerr << "playCurrentSounds about to call soundPlayed, res " << res << endl;
   // Make the callback saying that the sound has been played.

   //Calls soundPlayed in navigationInterface. Done this way to syncronize
   //threads.
   g_idle_add_full(G_PRIORITY_DEFAULT, soundplayedFunction,
                   &m_navigationInterface, NULL);


   cerr << "playCurrentSounds ends" << endl;
#endif
}

void 
RouteTester::playSound()
{
   cout << "*******************************************************" << endl;
   cout << "********************* Play sound **********************" << endl;
   cout << "*******************************************************" << endl;
   for (WFStringArray::const_iterator it = m_currentSounds.begin();
        it != m_currentSounds.end();
        ++it) {
      cout << "Sound file: " << *it << endl;
   }

#ifdef HAVE_OGG123
   pthread_t threadID;
   pthread_create(&threadID, NULL, &playSoundRun, this);
#else
   // Make the callback saying that the sound has been played.
   m_navigationInterface.soundPlayed();
#endif
}

void 
RouteTester::prepareSound(const WFStringArray& soundNames)
{ 
   cout << "*******************************************************" << endl;
   cout << "******************** Prepare sound ********************" << endl;
   cout << "*******************************************************" << endl;
   m_currentSounds.clear();
   for (WFStringArray::const_iterator it = soundNames.begin();
        it != soundNames.end();
        ++it) {
      cout << "Sound file: " << *it << endl;
      m_currentSounds.push_back((std::string(m_resourceStoragePath.c_str()) + 
                                 "/" + m_audioTypeDirName.c_str() + "/"+
                                 m_audioLangString.c_str() +"/" + (*it).c_str()
                                 + "." + m_audioTypeExt.c_str()).c_str());
   }
   // Make the callback saying that the sound has been prepared and how long
   // it will take in milli seconds.
   m_navigationInterface.soundPrepared(/*duration*/1000);
}

void RouteTester::reachedEndOfRouteReply(RequestID requestID)
{
   cout << "*******************************************************" << endl;
   cout << "**************** REACHED END OF ROUTE! ****************" << endl;
   cout << "*******************************************************" << endl;
}
