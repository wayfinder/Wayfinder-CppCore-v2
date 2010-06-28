/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "SettingsTester.h"
#include "SettingsInterface.h"
#include "WFString.h"
#include <iostream>
#include <stdlib.h>

#ifndef DATAPATH
# define DATAPATH "./nav2data/"
#endif

using namespace std;
using namespace WFAPI;

SettingsTester::SettingsTester(SettingsInterface& settingsInterface) 
      : m_settingsInterface(settingsInterface)
{
   m_settingsInterface.addSettingsListener(this);
   // Add the commands for setting tests
   addCommandOptions();
}

SettingsTester::~SettingsTester() 
{
}

bool
SettingsTester::handleInput(const char* input, bool& goBack)
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
         
   case cmdSettingsA:
      setSettingsA();
      break;
   case cmdSettingsB:
      setSettingsB();
      break;
   case cmdSettingsShow:
      settingsShow();
      break;
   case cmdCheckEula:
      checkEula();
      break;
   case cmdCheckUsername:
      checkUsername();
      break;
   case cmdSetPassword:
      setPassword();
      break;
   case cmdSetUIN:
      setUIN();
      break;
   case cmdGetVersionInfo:
      getVersionInfo();
      break;
   case back:
      goBack = true;
      break;      
   default:
      cout << "Command not handled here, "
           << "please try again or select B to go back." << endl;
      return false;
      break;
   }
   // If we get here, the key has been consumed.
   return true;
}

void 
SettingsTester::addCommandOptions()
{
   m_commandOptions.
      insert(make_pair("h",
                       CommandHolder("Displays this help menu.", help)));

   m_commandOptions.
      insert(make_pair("s",
                       CommandHolder("Displays the current settings values.", 
                                     cmdSettingsShow)));
   m_commandOptions.
      insert(make_pair("1",
                       CommandHolder("Set the 1st set of values for settings.", 
                                     cmdSettingsA)));
   m_commandOptions.
      insert(make_pair("2",
                       CommandHolder("Set the 2nd set of values for settings.", 
                                     cmdSettingsB)));
   m_commandOptions.
      insert(make_pair("b", 
                       CommandHolder("Back to previous menu.", 
                                     back)));
   m_commandOptions.
      insert(make_pair("e", 
                       CommandHolder("Check Eula.", 
                                     cmdCheckEula)));
   m_commandOptions.
      insert(make_pair("u", 
                       CommandHolder("Check username.", 
                                     cmdCheckUsername)));
   m_commandOptions.
      insert(make_pair("p", 
                       CommandHolder("Set password.", 
                                     cmdSetPassword)));
   m_commandOptions.
      insert(make_pair("a", 
                       CommandHolder("Set UIN.", 
                                     cmdSetUIN)));

   m_commandOptions.
      insert(make_pair("v", 
                       CommandHolder("Get version information.", 
                                     cmdGetVersionInfo)));

}

// Define that prints human readable strings for booleans.
#define bp(a) ((a) ? "true" : "false" )

void
SettingsTester::settingsShow() 
{
   // NOTE: To be sure, check res for errors before making the next call,
   // This to make sure everything went alright.

   // Get all the settings and print
   bool autoReroute = false;
   SynchronousStatus res = m_settingsInterface.getAutoReroute(autoReroute);
   bool avoidHighway = false;
   res = m_settingsInterface.getAvoidHighway(avoidHighway);
   bool avoidTollRoad = false;
   res = m_settingsInterface.getAvoidTollRoad(avoidTollRoad);
   bool checkRouteForTrafficChanges = false;
   res = m_settingsInterface.getCheckRouteForTrafficChanges(
      checkRouteForTrafficChanges);
   DistanceUnit distanceUnit;
   res = m_settingsInterface.getDistanceUnit(distanceUnit);
   RouteCost routeCost;
   res = m_settingsInterface.getRouteCost(routeCost);
   VoiceVerbosity voiceVerbosity;
   res = m_settingsInterface.getVoiceVerbosity(voiceVerbosity);
   wf_uint32 trafficInformationUpdateTime = 0;
   res = m_settingsInterface.getTrafficInformationUpdateTime(
      trafficInformationUpdateTime);
   TransportationType transportationType;
   res = m_settingsInterface.getCurrentTransportationType(transportationType);
   bool usePos;
   res = m_settingsInterface.getUsePositionForSearchCategoriesRequests(usePos);
   

   cout << "   AutoReroute " << bp(autoReroute) << endl;
   cout << "   AvoidHighway " << bp(avoidHighway) << endl;
   cout << "   AvoidTollRoad " << bp(avoidTollRoad) << endl;
   cout << "   CheckRouteForTrafficChanges " 
        << bp(checkRouteForTrafficChanges) << endl;
   cout << "   DistanceUnit " 
        << DistanceUnitPrinter::distanceUnitToString(distanceUnit).c_str()
        << endl;
   cout << "   RouteCost " 
        << RouteCostPrinter::routeCostToString(routeCost).c_str() << endl;
   cout << "   VoiceVerbosity " 
        << VoiceVerbosityPrinter::voiceVerbosityToString(
           voiceVerbosity).c_str() << endl;
   cout << "   TrafficInformationUpdateTime " << trafficInformationUpdateTime
        << endl;
   cout << "   TransportationType " 
        << TransportationTypePrinter::transportationTypeToString(
              transportationType) << endl;
   cout << "   Use position for search categories request " << bp(usePos)
        << endl;
}

void
SettingsTester::checkEula()
{
   cout << "Check for updated EULA" << endl;
   WFString filename = "eula.txt";
   AsynchronousStatus res = m_settingsInterface.isEULAUpdated(filename);
}

void
SettingsTester::setSettingsA()
{
/*
   // The A set of settings.
   AutoReroute true
   AvoidHighway false
   AvoidTollRoad false
   CheckRouteForTrafficChanges false
   DistanceUnit km
   RouteCost time with traffic disturbances
   VoiceVerbosity full
   TrafficInformationUpdateTime 30
*/
   setSettings(true, false, false, false, KM, TIME_WITH_TRAFFIC_DISTURBANCES,
               FULL, 30, false);
}

void
SettingsTester::setSettingsB()
{
/*
   // The B set of settings.
   AutoReroute false
   AvoidHighway false
   AvoidTollRoad false
   CheckRouteForTrafficChanges true
   DistanceUnit miles yard
   RouteCost distance
   VoiceVerbosity few
   TrafficInformationUpdateTime 10
*/
   setSettings(false, false, false, true, MILES_YARD, DISTANCE,
               FEW, 10, true);
}

void
SettingsTester::setSettings(bool autoReroute,
                            bool avoidHighway,
                            bool avoidTollRoad,
                            bool checkRouteForTrafficChanges,
                            DistanceUnit distanceUnit,
                            RouteCost routeCost,
                            VoiceVerbosity voiceVerbosity,
                            wf_uint32 trafficInformationUpdateTime,
                            bool usePosForSearchCatRequest)
{
   // NOTE: To be sure, check res for errors before making the next call,
   // This to make sure everything went alright.

   // Set all the settings
   SynchronousStatus res = m_settingsInterface.setAutoReroute(autoReroute);
   res = m_settingsInterface.setAvoidHighway(avoidHighway);
   res = m_settingsInterface.setAvoidTollRoad(avoidTollRoad);
   res = m_settingsInterface.setCheckRouteForTrafficChanges(
      checkRouteForTrafficChanges);
   res = m_settingsInterface.setDistanceUnit(distanceUnit);
   res = m_settingsInterface.setRouteCost(routeCost);
   res = m_settingsInterface.setVoiceVerbosity(voiceVerbosity);
   res = m_settingsInterface.setTrafficInformationUpdateTime(
      trafficInformationUpdateTime);
   res = m_settingsInterface.setUsePositionForSearchCategoriesRequests(
      usePosForSearchCatRequest);
}

void
SettingsTester::eulaAvailable(bool updated, WFString filename)
{
   cout << "SettingsTester::eulaAvailable: " << endl;
   cout << "Eula updated: " << updated << endl;
   cout << "Filename: " << filename << endl;

   if(updated) {
      string eulafileName(DATAPATH);
      eulafileName.append(filename.c_str());

      FILE * pFile;
      long lSize;
      char * buffer;
      size_t result;

      pFile = fopen ( eulafileName.c_str() , "rb" );
      if (pFile==NULL) {fputs ("File error",stderr); exit (1);}

      // obtain file size:
      fseek (pFile , 0 , SEEK_END);
      lSize = ftell (pFile);
      rewind (pFile);

      // allocate memory to contain the whole file:
      buffer = (char*) malloc (sizeof(char)*lSize);
      if (buffer == NULL) {fputs ("Memory error",stderr); exit (2);}

      // copy the file into the buffer:
      result = fread (buffer,1,lSize,pFile);
      if (long(result) != lSize) {fputs ("Reading error",stderr); exit (3);}

      /* the whole file is now loaded in the memory buffer. */
      
      cout << "Eula:" << endl;
      cout << buffer << endl;
      
      // terminate
      fclose (pFile);
      free (buffer);

   } else {
      cout << "No new eula availble" << endl;
   }
}

void
SettingsTester::newVersionAvailable(const WFAPI::WFString& versionNumber,
                                    const WFAPI::WFString& versionUrl,
                                    const WFAPI::WFString& versionString,
                                    bool force)
{
   cout << "Got newVersionAvailable with parameters: " << endl 
        << "versionNumber: " << versionNumber.c_str() << endl
        << "versionUrl: " << versionUrl.c_str() << endl
        << "versionString: " << versionString.c_str() << endl
        << "force: " << force << endl << endl;
}


void
SettingsTester::error(const AsynchronousStatus& status)
{
   printErrorStatus(&status, "SettingsTester");
}

void
SettingsTester::checkUsername()
{
   WFString username;
   SynchronousStatus res = m_settingsInterface.getUsername(username);

   cout << "Username: " << username << endl;
}

void
SettingsTester::setPassword()
{
   string password = "";
   
   cout << "Enter new password: ";
   getline(cin, password);
   cout << "You entered: " << password << endl;
   SynchronousStatus res = m_settingsInterface.setPassword(password.c_str());

}

void
SettingsTester::setUIN()
{
   WFString uin = "11111111";
   cout << "Change UIN to: " << uin << endl;

   SynchronousStatus res = m_settingsInterface.setUIN(uin);

   cout << "Result: " << res.getStatusCode() << endl;
   
}

void
SettingsTester::getVersionInfo()
{
   WFString version;
   WFString url;
   WFString versionString;
   bool force;
   m_settingsInterface.getNewVersionInformation(version, url,
                                             versionString, force);

   cout << "getVersionInfo returned: " << endl 
        << "version: " << version.c_str() << endl
        << "url: " << url.c_str() << endl
        << "versionString: " << versionString.c_str() << endl
        << "force: " << force << endl << endl;
}
