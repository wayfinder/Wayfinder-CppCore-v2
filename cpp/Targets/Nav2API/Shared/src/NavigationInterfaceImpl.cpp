/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "NavigationInterfaceImpl.h"
#include "NavigationInfoUpdateListener.h"
#include "Nav2APIImpl.h"
#include "RouteInterfaceImpl.h"

#include "GuiProtMess.h"
#include "GuiProtEnums.h"
#include "GuiParameterEnums.h"
#include "Nav2Logging.h"

#include "MC2Coordinate.h"
#include "WGS84Coordinate.h"

#include "DistancePrintingPolicy.h"
#include "nav2util.h"
#include <math.h>

using namespace isab;
using namespace std;

// Define that prints human readable strings for booleans.
#define bp(a) ((a) ? "true" : "false" )

namespace WFAPI {

NavigationInterfaceImpl::NavigationInterfaceImpl(Nav2APIImpl* api) : 
      InterfaceImpl(api),
      m_distanceUnit(KM)
{
}

NavigationInterfaceImpl::~NavigationInterfaceImpl()
{
}

void 
NavigationInterfaceImpl::addNavigationInfoUpateListener(
   NavigationInfoUpdateListener* listener)
{
   m_listeners.insert(listener);
}

void 
NavigationInterfaceImpl::removeNavigationInfoUpateListener(
   NavigationInfoUpdateListener* listener)
{
   m_listeners.erase(listener);
}

SynchronousStatus
NavigationInterfaceImpl::soundPrepared(wf_uint32 duration)
{
   isab::GenericGuiMess replyMess(isab::GuiProtEnums::PREPARE_SOUNDS_REPLY,
                                  uint32(duration/100)/*In tenth*/);
   return sendAsynchronousRequestToNav2(&replyMess);
}

SynchronousStatus
NavigationInterfaceImpl::soundPlayed()
{
   isab::GenericGuiMess replyMess(isab::GuiProtEnums::PLAY_SOUNDS_REPLY);
   return sendAsynchronousRequestToNav2(&replyMess);
}


void 
NavigationInterfaceImpl::receiveAsynchronousReply(
   const isab::GuiProtMess& guiProtMess,
   RequestID requestID)
{
   nav2log << "NavIntImpl::receiveAsynchronousReply starts" << endl;
   // Enough with all the isab prefixing..
   using namespace isab;

   // Get the status.
   AsynchronousStatus status = createAsynchStatus(&guiProtMess, requestID);
   
   // Check status.
   if (status.getStatusCode() != OK) {
      // Bad status. Report error function for all listeners.
      nav2log << "NavIntImpl::receiveAsynchronousReply bad status "
                << status.getStatusCode() 
                << " message " << status.getStatusMessage().c_str() << " URL "
                << status.getStatusURL().c_str() << endl;
      reportError(status, m_listeners.begin(), m_listeners.end());
      return;
   }
   
   isab::GuiProtEnums::MessageType type = guiProtMess.getMessageType();
   switch (type) {
      case isab::GuiProtEnums::PREPARE_SOUNDS: 
      {
         nav2log << "NavIntImpl::receiveAsynchronousReply PREPARE_SOUNDS";
         WFStringArray soundNames;
         const isab::PrepareSoundsMess& reply = 
            static_cast<const isab::PrepareSoundsMess&>(guiProtMess);
         const int* soundIndexes = reply.getSounds();
         // For all the sounds
         for ( wf_int32 i = 0; i < reply.getNumSounds(); ++i) {
            // Find sound names in array and add to soundNames
            // Array index check!
            if (soundIndexes[i] >= 0 &&
                soundIndexes[i] < wf_int32(m_soundFileNames.size())) {
               soundNames.push_back(m_soundFileNames[soundIndexes[i]]);
               nav2log << " " << m_soundFileNames[soundIndexes[i]].c_str();
            }
         }
         nav2log << " Total " << reply.getNumSounds() << " sounds"
                   << endl;

         // call all listeners prepareSound
         for (ListenerCont::iterator it = m_listeners.begin(); 
              it != m_listeners.end(); ++it) {
            nav2log << "NavIntImpl::receiveAsynchronousReply calling "
                      << "listener" << endl;
            (*it)->prepareSound(soundNames);
         }
         break;
      }
      case isab::GuiProtEnums::PLAY_SOUNDS:
      {
         nav2log << "NavIntImpl::receiveAsynchronousReply PLAY_SOUNDS" 
                   << endl;
         // call all listeners playSound
         for (ListenerCont::iterator it = m_listeners.begin(); 
              it != m_listeners.end(); ++it) {
            (*it)->playSound();
         }
         break;
      }
      case isab::GuiProtEnums::SOUND_FILE_LIST:
      {
         nav2log << "NavIntImpl::receiveAsynchronousReply SOUND_FILE_LIST"
                   << endl;
         // Fill the member list of soundindex to filename
         m_soundFileNames.clear();
         const isab::SoundFileListMess& reply = 
            static_cast<const isab::SoundFileListMess&>(guiProtMess);
         char** strings = reply.getStringData();
         for ( wf_int32 i = 0; i < reply.getNumEntries(); ++i) {
            m_soundFileNames.push_back(strings[i]);
         }
         break;
      }
      case isab::GuiProtEnums::UPDATE_ROUTE_INFO:
      {
         nav2log << "NavIntImpl::receiveAsynchronousReply UPDATE_ROUTE_INFO"
                   << endl;
         // Get the data
         const isab::UpdateRouteInfoMess& reply = 
            static_cast<const isab::UpdateRouteInfoMess&>(guiProtMess);
         const isab::RouteInfo& ri = *reply.getRouteInfo();
         bool nextHighway = false;
         const char * detour = NULL;
         const char * speedcam = NULL;
         wf_uint32 nextDist = WF_MAX_UINT32;
         wf_uint32 nextExit = 0;
         isab::RouteAction nextAction = InvalidAction;
         isab::RouteCrossing nextCrossing = NoCrossing;
         isab::RouteAction actionAfterNextAction = InvalidAction;
         isab::RouteCrossing crossingAfterNextCrossing = NoCrossing;
         WFString nextStreetName;
         WFString currentStreetName;
         WFString nextStreetNameAfterNextCrossing;
         WGS84Coordinate position;

         if ( ri.currCrossing.valid ) {
            nextDist = ri.currCrossing.distToNextCrossing;
            // nextExit = ri.nextCrossing1.exitCount;
            nextAction = isab::RouteAction(ri.currCrossing.action);
            nextCrossing = isab::RouteCrossing(ri.currCrossing.crossingType);
            actionAfterNextAction = isab::RouteAction(ri.nextCrossing1.action);
            crossingAfterNextCrossing = 
               isab::RouteCrossing(ri.nextCrossing1.crossingType);
            nextHighway = ri.nextSeg1.isHighway();
            detour = ri.hasDetourLandmark();
            speedcam = ri.hasSpeedCameraLandmark();
            currentStreetName = ri.currSeg.streetName;
            nextStreetName = ri.nextSeg1.streetName;
            nextStreetNameAfterNextCrossing = ri.nextSeg2.streetName;
            position = WGS84Coordinate(
               Nav2Coordinate(ri.currCrossing.lat, ri.currCrossing.lon));
         }
         
         if ((ri.onTrackStatus != OnTrack) || (ri.currCrossing.changed) ||
             (ri.currSeg.changed)) {

            // call infoUpdate or distanceUpdate
            nav2log << "NavIntImpl::receiveAsynchronousReply fullUpdate" 
                      << endl;

            nextExit = ri.currCrossing.exitCount;

            // infoUpdate
            // routeActionFromNav2(),
            UpdateNavigationInfo info(
               RouteInterfaceImpl::nav2ActionToRouteAction(
                  actionAfterNextAction),
               RouteInterfaceImpl::nav2CrossingToRouteCrossing(
                  crossingAfterNextCrossing),
               currentStreetName,
               ri.distToGoal/*distanceToGoal*/,
               convertDistance(ri.distToGoal),
               ri.distToWpt/*distanceToNextTurn*/,
               convertDistance(ri.distToWpt),
               ri.currCrossing.distToNextCrossing,/*DistToNextNextTurn*/
               convertDistance(ri.currCrossing.distToNextCrossing),
               RouteInterfaceImpl::nav2ActionToRouteAction(nextAction),
               RouteInterfaceImpl::nav2CrossingToRouteCrossing(nextCrossing),
               nextStreetName,
               nextStreetNameAfterNextCrossing,
               ri.onTrackStatus == OnTrack,
               position,
               wf_uint16(isab::round_int(ri.speed*3.6)), // km/h
               ri.timeToGoal,
               ri.timeToWpt/10/*timeToNextTurn, in 1/10 seconds*/,
               nextExit,
               ri.nextCrossing1.exitCount,
               (ri.currSeg.isHighway() || nextHighway),
               detour?1:0, speedcam?1:0,
               ri.currSeg.isLeftTraffic(),
               ri.currCrossing.changed,
               ri.crossingNo);
            nav2log << "NavIntImpl::receiveAsynchronousReply nbr listeners "
                      << m_listeners.size() << endl;
            for (ListenerCont::iterator it = m_listeners.begin();
                 it != m_listeners.end(); ++it) {
               nav2log << "NavIntImpl::receiveAsynchronousReply listener "
                         << (*it) << endl;
               (*it)->infoUpdate(info);
               nav2log << "NavIntImpl::receiveAsynchronousReply listener "
                         << (*it) << " done" << endl;
            }
         } else {
            // distanceUpdate
            nav2log << "NavIntImpl::receiveAsynchronousReply distanceUpdate"
                      << endl;
            UpdateNavigationDistanceInfo info(
               ri.distToGoal/*distanceToGoal*/,
               convertDistance(ri.distToGoal),
               ri.distToWpt/*distanceToNextTurn*/,
               convertDistance(ri.distToWpt),
               wf_uint16(isab::round_int(ri.speed*3.6)), // km/h
               ri.timeToGoal,
               ri.timeToWpt/10/*timeToNextTurn, in 1/10 seconds*/);
            nav2log << "NavIntImpl::receiveAsynchronousReply nbr listeners "
                      << m_listeners.size() << endl;
            for (ListenerCont::iterator it = m_listeners.begin();
                 it != m_listeners.end(); ++it) {
               nav2log << "NavIntImpl::receiveAsynchronousReply Sending to "
                         << "a listener" << endl;
               (*it)->distanceUpdate(info);
            }
         }
         break;
      }

      default:
         nav2log << "NavigationInterfaceImpl::receiveAsynchronousReply of "
                   << "unknown type: " << int(type) << endl;
         break;
   }

   nav2log << "NavIntImpl::receiveAsynchronousReply Ends" << endl;
}

void
NavigationInterfaceImpl::setDistanceUnit(DistanceUnit distanceUnit)
{
   m_distanceUnit = distanceUnit;
}

WFString
NavigationInterfaceImpl::convertDistance(wf_uint32 distance)
{
   char* res = NULL;
   switch(m_distanceUnit) {
      case KM: {
         res = DistancePrintingPolicy
            ::convertDistanceMetric(distance,
                                    DistancePrintingPolicy::Round,
                                    true);
         break;
      }
      case MILES_FEET: {
         res = DistancePrintingPolicy::
            convertDistanceImperialFeet(distance,
                                        DistancePrintingPolicy::Round,
                                        true);
         break;
      }
      case MILES_YARD: {
         res = DistancePrintingPolicy::
            convertDistanceImperialYards(distance,
                                         DistancePrintingPolicy::Round,
                                         true);
         break;
      }
   }

   WFString str(res);
   delete[] res;

   return str;
}


} // End namespace WFAPI


