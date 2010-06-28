/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "UIControl.h"
#include <iostream>
#include "GtkDevControl.h"
#include "ScreenPoint.h"

using namespace std;
using namespace WFAPI;


UIControl::UIControl(GtkDevControl* devControl)
{
   m_devControl = devControl;

#if ((defined MAEMO4) || (defined MAEMO5))
   m_ossocontext = NULL;
   m_ossocontext = osso_initialize("WFAPITestClient", "version", 0, NULL);
#endif
}

UIControl::~UIControl()
{
#if ((defined MAEMO4) || (defined MAEMO5))
   osso_deinitialize(m_ossocontext);
   m_ossocontext = NULL;
#endif
}

void 
UIControl::error(const AsynchronousStatus& status)
{

}

void
UIControl::distanceUpdate(const UpdateNavigationDistanceInfo& info)
{
//   // Print header
//   cout << "*******************************************************" << endl;
//   cout << "****************** Distance Update ********************" << endl;
//   cout << "*******************************************************" << endl;
//
//   // Print data
//   cout << "Distance to goal:      " << info.getDistanceToGoal() << endl;
//   cout << "Distance to next turn: " << info.getDistanceToNextTurn() << endl;
//   cout << "Speed:                 " << info.getSpeed() << endl;
//   cout << "Time to goal:          " << info.getTimeToGoal() << endl;
//   cout << "Time to next turn:     " << info.getTimeToNextTurn() << endl;
//
//   // Print footer :)
//   cout << "*******************************************************" << endl;

   // Update distance to next turn.
   char tmp[50];
   sprintf(tmp, "%s", info.getDistanceToNextTurnInUnit().c_str());
   m_devControl->setDistance(tmp);
}

void 
UIControl::infoUpdate(const UpdateNavigationInfo& info)
{
//   // Print header
//   cout << "*******************************************************" << endl;
//   cout << "******************** Info Update **********************" << endl;
//   cout << "*******************************************************" << endl;
//
//   // Print data
//   if (info.isOnTrack()) {
//      cout << "On track!" << endl;
//   } else {
//      cout << "Off track!" << endl;
//   }
//   cout << "Next action: " << info.getNextAction() << endl;
//   cout << "Next crossing: " << info.getNextCrossing() << endl;
//   cout << "Next street name: " << info.getNextStreetName().c_str() << endl;
//   cout << "Current street name: " << info.getCurrentStreetName().c_str()
//        << endl;
//   cout << "Position of next turn: " << info.getPosition() << endl;
//   cout << "Speed: " << info.getSpeed() << endl;
//   cout << "Time to goal: " << info.getTimeToGoal() << endl;
//   cout << "Time to next turn: " << info.getTimeToNextTurn() << endl;
//   cout << "Distance to goal: " << info.getDistanceToGoal() << endl;
//   cout << "Distance to next turn: " << info.getDistanceToNextTurn() << endl;
//   cout << "Action after next crossing: " << info.getActionAfterNextAction()
//        << endl;
//   cout << "Crossing after next corssing: "
//        << info.getCrossingAfterNextCrossing() << endl;

   // Update current street.
   m_devControl->setCurrentStreet(info.getCurrentStreetName().c_str());
}

void 
UIControl::playSound()
{
#if 0
   cout << "*******************************************************" << endl;
   cout << "********************* Play sound **********************" << endl;
   cout << "*******************************************************" << endl;
   for (WFStringArray::const_iterator it = m_currentSounds.begin();
        it != m_currentSounds.end();
        ++it) {
      cout << "Sound file: " << *it << endl;
   }
#endif
}

void 
UIControl::prepareSound(const WFStringArray& soundNames)
{ 

}

void
UIControl::areaUpdate(const AreaUpdateInformation& areaUpdateInformation)
{
//   // Print header
//   cout << "*******************************************************" << endl;
//   cout << "******************** Area Update **********************" << endl;
//   cout << "*******************************************************" << endl;
//
//   // Print data
//   cout << "Position: " << areaUpdateInformation.getPosition() << endl;
//   cout << "InnerRadius: " << areaUpdateInformation.getInnerRadius() << endl;
//   cout << "OuterRadius: " << areaUpdateInformation.getOuterRadius() << endl;
//   cout << "StartAngle: " << areaUpdateInformation.getStartAngle() << endl;
//   cout << "EndAngle: " << areaUpdateInformation.getEndAngle() << endl;
//   cout << "Altitude: " << areaUpdateInformation.getAltitude() << endl;
}

void
UIControl::locationUpdate(const LocationUpdateInformation& locationUpdate)
{
//   // Print header
//   cout << "*******************************************************" << endl;
//   cout << "****************** Location Update ********************" << endl;
//   cout << "*******************************************************" << endl;
//
//   // Print data
//   cout << "Position: " << locationUpdate.getPosition() << endl;
//   cout << "heading: " << locationUpdate.getHeading() << endl;
//   cout << "Speed: " << locationUpdate.getSpeed() << endl;
//   cout << "Altitude: " << locationUpdate.getAltitude() << endl;
//   cout << "PDOP: " << locationUpdate.getPDOP() << endl;


   // Currently some invalid positions can be sent.
   // Filter out these here.
   if (locationUpdate.getGpsQuality() == QUALITY_MISSING) {
      return;
   }
   gint width;
   gint height;
   gtk_window_get_size(m_devControl->getWindow(), &width, &height);

   
   // Update the position of the me-marker
   m_devControl->setMeMarkerPosition(
      locationUpdate.getPosition(),
      WFAPI::ScreenPoint(width/2,height-height/3), // Position on screen.
      locationUpdate.getHeading(), // Heading.
      locationUpdate.getRoutePosition(),
      locationUpdate.getRouteHeading(),
      false,                       // Disable updating zoomlevel.
      10);                          // Zoomlevel value.

#if ((defined MAEMO4) || (defined MAEMO5))
   osso_display_blanking_pause(m_ossocontext);
#endif
}

void
UIControl::startedLbs()
{
//   // Print
//   cout << "******************** LBS Started **********************" << endl;
}

void UIControl::snapToPosition()
{
   m_devControl->centerOnMeMarker();
}
