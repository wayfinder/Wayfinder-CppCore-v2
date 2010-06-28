/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef SETTINGS_TESTER_H
#define SETTINGS_TESTER_H

#include "WFAPIConfig.h"
#include "SettingsInterface.h"
#include "DistanceUnit.h"
#include "RouteCost.h"
#include "VoiceVerbosity.h"
#include "GenericTester.h"
#include "SettingsListener.h"

using namespace WFAPI;


/**
 * Test class that implements settings testing.
 */
class SettingsTester : public SettingsListener,
                       public GenericTester
{

public:

   /**
   * Destructor.
   */
   ~SettingsTester();

   /**
    * Constructor.
    *
    * @param settingsInterface The settings interface which to use in the 
    *                          tests.
    */
   SettingsTester(SettingsInterface& settingsInterface);

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
    * Retreives the version information from the settings interface
    * and prints the result.
    */
   void getVersionInfo();

public: // From base classes

   /**
    * Called when an error has occured.
    */
   virtual void error(const AsynchronousStatus& status);
   
   /**
    * Called when a check against the server for a new EULA has been done.
    *
    * @param updated True when a new unseen EULA is available, false if EULA
    * available is the same as before the check against the server.
    * @param filename The filename of the EULA.
    */
   virtual void eulaAvailable(bool updated, WFString filename);

   virtual void newVersionAvailable(const WFAPI::WFString& versionNumber,
                                    const WFAPI::WFString& versionUrl,
                                    const WFAPI::WFString& versionString,
                                    bool force);
private:

   /**
    * Adds the command options valid for this class. The storage is a map
    * where the key is a String and the value is a CommandHolder.
    * The CommandHolder holds the actual command to perform so by finding
    * in the map after user input we can map this against a real command.
    */
   void addCommandOptions();

   /**
    * Show the current settings.
    */
   void settingsShow();

   /**
    * To check if a new eula is available.
    */
   void checkEula();
   
   /**
    * Set the A values for settings.
    */
   void setSettingsA();

   /**
    * Set the B values for settings.
    */
   void setSettingsB();

   /**
    * Check the username for this user.
    */
   void checkUsername();

   /**
    * Set the web password for this user.
    */
   void setPassword();

   /**
    * Set the UIN for this user.
    */
   void setUIN();

   void setSettings(bool autoReroute,
                    bool avoidHighway,
                    bool avoidTollRoad,
                    bool checkRouteForTrafficChanges,
                    DistanceUnit distanceUnit,
                    RouteCost routeCost,
                    VoiceVerbosity voiceVerbosity,
                    wf_uint32 trafficInformationUpdateTime,
                    bool usePosForSearchCatRequest);

   /// Reference to the SettingsInterface
   SettingsInterface& m_settingsInterface;

};

#endif // SETTINGS_TESTER_H

