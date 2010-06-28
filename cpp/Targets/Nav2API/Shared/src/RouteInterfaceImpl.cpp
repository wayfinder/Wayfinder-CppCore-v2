/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#define CORE_LOGGING_MODULE_NAME "RouteInterfaceImpl"
//#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE

#include "CoreDebugPrint.h"

#include "RouteInterfaceImpl.h"
#include "RouteListener.h"
#include "Nav2APIImpl.h"

#include "Nav2Logging.h"

#include "GuiProtMess.h"
#include "GuiProtEnums.h"
#include "GuiParameterEnums.h"
#include "GuiProtRouteMess.h"
#include "GuiProtFileMess.h"
#include "GuiFileOperation.h"
#include "RouteInfo.h"

#include <algorithm>

#include "MC2Coordinate.h"

#include "MapLibAPI.h"
#include "MapLib.h"
#include "RouteID.h"
#include "MapOperationInterface.h"

#include "RequestID.h"

#include <string>

using namespace std;

namespace WFAPI {

RouteInterfaceImpl::RouteInterfaceImpl(Nav2APIImpl* api) : 
   InterfaceImpl(api),
   m_requestID(RequestID::INVALID_REQUEST_ID),
   m_requestedRemoveRoute(false)
{
   // Initially set to invalid vehicle type.
   m_curVehicleType = isab::NavServerComEnums::invalidVehicleType;

}

RouteInterfaceImpl::~RouteInterfaceImpl()
{

}

void 
RouteInterfaceImpl::addRouteListener(RouteListener* listener)
{
   m_listeners.insert(listener);
}

void 
RouteInterfaceImpl::removeRouteListener(RouteListener* listener)
{
   m_listeners.erase(listener);
}

SynchronousStatus 
RouteInterfaceImpl::getRouteList(RouteInfoItemArray& routeInfoList)
{
   // Enough with all the isab prefixing..
   using namespace isab;
   using namespace isab::RouteInfoParts;

   // We should check if it's necessary to request route list.

   // Request route list (all of it, from start).
   GetFilteredRouteListMess getRouteListMess(0, -1);
   GuiProtMess* reply = 
      sendSynchronousRequestToNav2(&getRouteListMess);
   
   // Delete members.
   getRouteListMess.deleteMembers();

   // Check status of reply.
   AsynchronousStatus status = createAsynchStatus(
      reply, RequestID(RequestID::INVALID_REQUEST_ID));
   
   if (status.getStatusCode() != OK) {
      // Bad status.
      reply->deleteMembers();
      delete reply;
      
      return SynchronousStatus(status.getStatusCode(),
                               status.getStatusMessage(),
                               status.getStatusURL());
   }
 
   isab::GuiProtEnums::MessageType type = reply->getMessageType();
   
   // Also check type, just to make sure.
   if (type != isab::GuiProtEnums::ROUTE_LIST) {
      // Very strange. Received wrong message type.
      reply->deleteMembers();
      delete reply;
      return SynchronousStatus(GENERAL_ERROR, "", "");
   }
   // Everything should be OK now.
   
   // Cast to correct object.
   RouteListMess* routeListMess = static_cast<RouteListMess*>(reply);
   
   // Get the route list.
   const RouteList* list = routeListMess->getRouteList();

   const std::vector<RouteListCrossing*>& crossings = list->crossings;
   
   routeInfoList.clear();
   routeInfoList.reserve(crossings.size());

   // If current road is a detour.
   bool isDetourLandmark = false;   
   
   // Go through the entire list.
   for (wf_uint32 i = 0; i < crossings.size(); ++i) {
      
      // The combined RouteListCrossing
      const RouteListCrossing* rlCrossing = crossings[i];

      // The crossing part.
      const Crossing& crossing = rlCrossing->crossing;

      int exitCount = rlCrossing->crossing.exitCount;

      // Segments between this crossing and next.
      const std::vector<isab::RouteInfoParts::Segment*>& segments = 
         rlCrossing->segments;
      
      WFString streetName;
      bool isLeftSide = false;
      bool isHighWay = false;
      
      if (segments.size() > 0) {
         const char* name = segments.front()->streetName;
         if (name != NULL) {
            streetName = name;
         }
         isLeftSide = segments.front()->isLeftTraffic();
         isHighWay = segments.front()->isHighway();
      }

      bool isSpeedCameraLandmark = false;
      WFString speedCameraInfo = "";
      WFString detourInfo = "";
      const char* landmarkInfo;

      landmarkInfo = rlCrossing->hasDetourLandmark(Landmark::Begins);
      if (landmarkInfo != NULL) { 
         // Detour starts.
         isDetourLandmark = true;
         detourInfo = landmarkInfo;
      }
      
      landmarkInfo = rlCrossing->hasDetourLandmark(Landmark::Ends);
      if (landmarkInfo != NULL) {
         // Detour ends
         isDetourLandmark = false;
         detourInfo = landmarkInfo;
      }

      landmarkInfo = rlCrossing->hasSpeedCameraLandmark(Landmark::Begins);
      if(landmarkInfo != NULL) {
         // SpeedCamera Found
         isSpeedCameraLandmark = true;
         speedCameraInfo = landmarkInfo;
      }

      landmarkInfo = rlCrossing->hasSpeedCameraLandmark(Landmark::Ends);
      if(landmarkInfo != NULL) {
         // End of SpeedCamera Found 
         isSpeedCameraLandmark = true;
         speedCameraInfo = landmarkInfo;
      }
      
      RouteInfoItem item(nav2ActionToRouteAction(
                            isab::RouteEnums::RouteAction(crossing.action)),
                         MC2Coordinate(Nav2Coordinate(crossing.lat, crossing.lon)),
                         nav2CrossingToRouteCrossing(
                            isab::RouteEnums::RouteCrossing(crossing.crossingType)),
                         rlCrossing->distToGoal,
                         crossing.distToNextCrossing,
                         rlCrossing->timeToGoal,
                         streetName,
                         isLeftSide,
                         isHighWay,
                         isSpeedCameraLandmark,
                         speedCameraInfo,
                         isDetourLandmark,
                         detourInfo,
                         exitCount);
      
      // Add item to resulting list.
      routeInfoList.push_back(item);   
   }

   
   // Clean up.
   reply->deleteMembers();
   delete reply;

   // All is OK if we get down here.
   return SynchronousStatus(OK, "", "");
}

SynchronousStatus 
RouteInterfaceImpl::removeRoute()
{
   // Set so no reachedEndOfRouteReply will be sent to listeners.
   m_requestedRemoveRoute = true;
   
   // Send invalidate route with route id zero.
   // Future improvement: double check if this is correct.
   isab::GenericGuiMess stop(isab::GuiProtEnums::INVALIDATE_ROUTE,
                             (uint32)0, (uint32)0);

   AsynchronousStatus status = sendAsynchronousRequestToNav2(&stop);
      
   // Clean up request.
   stop.deleteMembers();
   
   if (status.getStatusCode() == OK &&
       getApi()->getConnectedMapLib() != NULL) {
      // Unset route in MapLib too
      getApi()->getConnectedMapLib()->getMapLib()->clearRouteID();
      // Also set heading to north
      getApi()->getConnectedMapLib()->getMapLib()->getMapOperationInterface()
         ->setAngle(0);
   }

   return SynchronousStatus(status.getStatusCode(),
                            status.getStatusMessage(),
                            status.getStatusURL());
}

AsynchronousStatus
RouteInterfaceImpl::reRoute()
{
   isab::GenericGuiMess reroute(isab::GuiProtEnums::REROUTE);
   AsynchronousStatus status = sendAsynchronousRequestToNav2(&reroute);

   // Clean up.
   reroute.deleteMembers();
   return status;
}

isab::NavServerComEnums::VehicleType
RouteInterfaceImpl::convertTranspType(TransportationType transportationType)
{
   switch (transportationType) {
      case CAR:
         return isab::NavServerComEnums::passengerCar;
      case PEDESTRIAN:
         return isab::NavServerComEnums::pedestrian;
      // Intentionally no default. Must handle all cases.
   }
   return isab::NavServerComEnums::passengerCar;
}

const char* transportationTypeToString(TransportationType type) {
   switch(type) {
   case CAR:
      return "car";
   case PEDESTRIAN:
      return "pedestrian";
   default:
      return "invalid";
   }
}

AsynchronousStatus
RouteInterfaceImpl::updateTransportationType(
      TransportationType transportationType)
{
   coreprintln_info("updateTransportationType(%s)",
                    transportationTypeToString(transportationType));
                    
   // Convert to Nav2 vehicle type.
   isab::NavServerComEnums::VehicleType vehicleType = 
         convertTranspType(transportationType); 

   // Check if Nav2 needs to be updated with new vehicle type.
   if (m_curVehicleType == vehicleType) {
      // Nothing to do. Transportation type already set.
      coreprintln_info("No need to update transportation type");
      return createOKAsynchStatus();
   }
   // else   

   // Send parameter to Nav2.
   isab::GeneralParameterMess setgpm(
         isab::GuiProtEnums::paramTransportationType,
         int32(vehicleType));

   isab::GuiProtMess* res = sendSynchronousRequestToNav2(&setgpm);

   // Delete members from setgpm.
   setgpm.deleteMembers();

   // Create the AsynchronousStatus based on the message content.
   AsynchronousStatus status = createAsynchStatus(
      res, RequestID(RequestID::INVALID_REQUEST_ID));

   if (status.getStatusCode() == OK) {
      m_curVehicleType = vehicleType;
   }
   
   // Delete message including members.
   res->deleteMembers();
   delete res;

   return status;
}

AsynchronousStatus
RouteInterfaceImpl::routeBetweenCoordinates(const WGS84Coordinate& startCoord,
      const WGS84Coordinate& destCoord,
      TransportationType transportationType)
{
   // Update the transportation type if necessary.
   AsynchronousStatus status = updateTransportationType(transportationType);

   if (status.getStatusCode() != OK) {
      return status;
   }
   // else

   Nav2Coordinate start(startCoord); 
   Nav2Coordinate dest(destCoord); 
   
   // Please verify that it is possible to leave description empty,
   // i.e. the last parameter.
   isab::RouteMess message(isab::GuiProtEnums::PositionTypePosition, "",
                           start.nav2lat, start.nav2lon,
                           isab::GuiProtEnums::PositionTypePosition, "",
                           dest.nav2lat, dest.nav2lon, "");
   
   AsynchronousStatus routeStatus = sendAsynchronousRequestToNav2(&message);
   
   // Delete members.
   message.deleteMembers();
   
   return routeStatus;
}

AsynchronousStatus
RouteInterfaceImpl::routeToCoordinate(const WGS84Coordinate& destCoord, 
      TransportationType transportationType)
{
   // Update the transportation type if necessary.
   AsynchronousStatus status = updateTransportationType(transportationType);

   if (status.getStatusCode() != OK) {
      return status;
   }
   // else
   
   Nav2Coordinate nav2Coord(destCoord); 
   // Please verify that it is possible to leave description empty,
   // i.e. the last parameter.
   isab::RouteMess message(isab::GuiProtEnums::PositionTypePosition,
                           "", nav2Coord.nav2lat, nav2Coord.nav2lon, 
                           "" );

   AsynchronousStatus routeStatus = sendAsynchronousRequestToNav2(&message);

   // Delete members.
   message.deleteMembers();
   
   return routeStatus;
}

AsynchronousStatus
RouteInterfaceImpl::routeToFavourite(const Favourite& favourite, 
                                     TransportationType transportationType)
{
   return routeToCoordinate(favourite.getPosition(), transportationType);
}

AsynchronousStatus
RouteInterfaceImpl::routeToSearchItem(const SearchItem& searchItem, 
                                      TransportationType transportationType)
{
   return routeToCoordinate(searchItem.getPosition(), transportationType);
}

SynchronousStatus
RouteInterfaceImpl::sendGuiFileOperationCommand(wf_uint16 command)
{
   isab::GuiFileOperationCommand *op =
      new isab::GuiFileOperationCommand(0, command);

   isab::GuiProtFileMess message(op);

   isab::GuiProtMess* reply = sendSynchronousRequestToNav2(&message);

   SynchronousStatus status = createAsynchStatus(reply);

   message.deleteMembers();

   reply->deleteMembers();
   delete reply;

   return status;
}

SynchronousStatus 
RouteInterfaceImpl::routeSimulationStart()
{
   isab::GenericGuiMess gpsMess(isab::GuiProtEnums::CONNECT_GPS);
   sendAsynchronousRequestToNav2(&gpsMess);

   SynchronousStatus simulationStatus =  
      sendGuiFileOperationCommand(isab::command_simulate_start);

   nav2log << "RouteInterfaceImpl::routeSimulationStart, status code: "
           << simulationStatus.getStatusCode() << endl;

   // Return as sync status.
   return simulationStatus;
}

SynchronousStatus
RouteInterfaceImpl::routeSimulationPause()
{
   return sendGuiFileOperationCommand(isab::command_simulate_pause);
}

SynchronousStatus 
RouteInterfaceImpl::routeSimulationResume()
{
   return sendGuiFileOperationCommand(isab::command_simulate_resume);
}

SynchronousStatus
RouteInterfaceImpl::routeSimulationStop()
{
   SynchronousStatus status =  
      sendGuiFileOperationCommand(isab::command_simulate_stop);

   if (status.getStatusCode() == OK &&
       getApi()->getConnectedMapLib() != NULL) {
      // Set heading to north
      getApi()->getConnectedMapLib()->getMapLib()->getMapOperationInterface()
         ->setAngle(0);
   }

   return status;
}

SynchronousStatus
RouteInterfaceImpl::routeSimulationRepeatOn()
{
   return sendGuiFileOperationCommand(isab::command_simulate_rep_on);
}

SynchronousStatus
RouteInterfaceImpl::routeSimulationRepeatOff()
{
   return sendGuiFileOperationCommand(isab::command_simulate_rep_off);
}

SynchronousStatus
RouteInterfaceImpl::routeSimulationIncreaseSpeed()
{
   return sendGuiFileOperationCommand(isab::command_simulate_inc_speed);
}

SynchronousStatus
RouteInterfaceImpl::routeSimulationDecreaseSpeed()
{
   return sendGuiFileOperationCommand(isab::command_simulate_dec_speed);
}

void 
RouteInterfaceImpl::receiveAsynchronousReply(
      const isab::GuiProtMess& guiProtMess,
      RequestID requestID)
{
   // Enough with all the isab prefixing..
   using namespace isab;

   // Get the status.
   AsynchronousStatus status = createAsynchStatus(&guiProtMess, requestID);
   
   // Check status.
   if (status.getStatusCode() != OK) {
      // Bad status. Report error function for all listeners.
      nav2log << "RouteInterfaceImpl::receiveAsynchronousReply, error code:"
              << status.getStatusCode() << endl;
      reportError(status, m_listeners.begin(), m_listeners.end());
      return;
   }
   
   isab::GuiProtEnums::MessageType type = guiProtMess.getMessageType();
   switch (type) {
      case isab::GuiProtEnums::STARTED_NEW_ROUTE:
      {
         const StartedNewRouteMess* dataMessage =
            static_cast<const StartedNewRouteMess *>(&guiProtMess);
         m_routeID = dataMessage->getRouteId();

         /*
          * If routeID == 0, then the map should be cleaned. This happends in
          * two cases, when the route is first calculated and when the end of
          * the route is reached. A reachedEndOfRouteReply should only be sent
          * in the second case. This is the same to when the routeID is zero
          * and when requestID is the same as the requestID for the calculated
          * route.
          */ 
         if(m_routeID == 0){
            if(m_requestID == requestID) {
               /*
                * If this is m_requestedRemoveRoute is not set, then a
                * reachedEndOfRouteReply should be sent to the listener.
                * If set, then the user of the API's have requested to remove
                * route and no reachedEndOfRouteReply should be sent.
                */
               if(!m_requestedRemoveRoute) {
                  // Notify listeners that a new route has been started.
                  for (std::set<RouteListener*>::iterator it = m_listeners.begin();
                       it != m_listeners.end(); ++it) {
                     (*it)->reachedEndOfRouteReply(requestID);
                  }
               } else {
                  m_requestedRemoveRoute = false;
               }
            } 
         }
         // Notify listeners that a new route has been started.
         for (std::set<RouteListener*>::iterator it = m_listeners.begin();
              it != m_listeners.end(); ++it) {
            (*it)->routeReply(requestID);
         }
         m_requestID=requestID;
         
      } break;
      default:
         nav2log << "RIImpl::receiveAsynchronousReply of unknown type: "
              << (int) type << endl;
         break;
   }
}

RouteAction
RouteInterfaceImpl::nav2ActionToRouteAction(
      isab::RouteEnums::RouteAction action)
{
   switch (action) {
      case isab::RouteEnums::InvalidAction:
         return INVALIDACTION;
      case isab::RouteEnums::Ahead:
         return AHEAD;
      case isab::RouteEnums::Left:
         return LEFT;
      case isab::RouteEnums::Right:
         return RIGHT;
      case isab::RouteEnums::UTurn:
         return UTURN;
      case isab::RouteEnums::Start:
         return START;
      case isab::RouteEnums::Finally:
         return FINALLY;
      case isab::RouteEnums::EnterRdbt:
         return ENTER_RDBT;
      case isab::RouteEnums::ExitRdbt:
         return EXIT_RDBT;
      case isab::RouteEnums::ExitAt:
         return EXIT_AT;
      case isab::RouteEnums::On:
         return ON;
      case isab::RouteEnums::ParkCar:
         return PARK_CAR;
      case isab::RouteEnums::KeepLeft:
         return KEEP_LEFT;
      case isab::RouteEnums::KeepRight:
         return KEEP_RIGHT;
      case isab::RouteEnums::StartWithUTurn:
         return START_WITH_U_TURN;
      case isab::RouteEnums::UTurnRdbt:
         return U_TURN_RDBT;
      case isab::RouteEnums::FollowRoad:
         return FOLLOW_ROAD;
      case isab::RouteEnums::EnterFerry:
         return ENTER_FERRY;
      case isab::RouteEnums::ExitFerry:
         return EXIT_FERRY;
      case isab::RouteEnums::ChangeFerry:
         return CHANGE_FERRY;
      case isab::RouteEnums::EndOfRoadLeft:
         return END_OF_ROAD_LEFT;
      case isab::RouteEnums::EndOfRoadRight:
         return END_OF_ROAD_RIGHT;
      case isab::RouteEnums::OffRampLeft:
         return OFF_RAMP_LEFT;
      case isab::RouteEnums::OffRampRight:
         return OFF_RAMP_RIGHT;
      default:
         return INVALIDACTION;
   }
}

RouteCrossing
RouteInterfaceImpl::nav2CrossingToRouteCrossing(
      isab::RouteEnums::RouteCrossing crossing)
{
   switch (crossing) {
      case isab::RouteEnums::NoCrossing:
         return NOCROSSING;
      case isab::RouteEnums::Crossing3Ways:
         return CROSSING_3_WAYS;
      case isab::RouteEnums::Crossing4Ways:
         return CROSSING_4_WAYS;
      case isab::RouteEnums::CrossingMultiway:
         return CROSSING_MULTIWAY;
      // Intentially left out default handling.
   }
   return NOCROSSING;
}

TransportationType
RouteInterfaceImpl::convertVehicleType(
   isab::NavServerComEnums::VehicleType vehicleType)
{
   switch(vehicleType) {
      case isab::NavServerComEnums::passengerCar:
         return CAR;
      case isab::NavServerComEnums::pedestrian:
         return PEDESTRIAN;
      case isab::NavServerComEnums::emergencyVehicle:
      case isab::NavServerComEnums::taxi:
      case isab::NavServerComEnums::publicBus:
      case isab::NavServerComEnums::deliveryTruck:
      case isab::NavServerComEnums::transportTruck:
      case isab::NavServerComEnums::highOccupancyVehicle:
      case isab::NavServerComEnums::bicycle:
      case isab::NavServerComEnums::publicTransportation:
      case isab::NavServerComEnums::invalidVehicleType:
         return CAR;
   }

   return CAR;
}

} // End namespace WFAPI


