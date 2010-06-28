/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_TESTER_H
#define WFAPI_TESTER_H

#include "Nav2StatusListener.h"
#include "AsynchronousStatus.h"
#include "Nav2API.h"
#include "ExternalKeyConsumer.h"
#include "GenericTester.h"
#include "TextLanguage.h"
#include "VoiceLanguage.h"

// Forward
namespace WFAPI {
class ReadyToRunListener;
}

//Forward
class RouteTester;
class FavouriteTester;
class SettingsTester;
class SearchTester;
class ImageTester;
class LocationTester;
class NetworkTester;
class BillingTester;
class ServiceWindowTester;
class UIControl;

using namespace WFAPI;

/**
 * Tester for starting Nav2 and MapLib, also handles the key inputs
 * and navigation between the different GenericTesters.
 */
class WFAPITester : public Nav2StatusListener, 
                    public ExternalKeyConsumer,
                    public GenericTester
{
public:
   /**
    * Constructor.
    *
    * @param stop Out parameter. Will be set to true when
    *             nav2 is stopped.
    * @param enableGtk If gtk can be used.
    * @param textLang The text language to use.
    * @param voiceLang The voice language to use.
    */
   WFAPITester(bool& stop, bool enableGtk, TextLanguage textLanguage,
               VoiceLanguage::VoiceLanguage voiceLanguage);

   /**
    * Destructor.
    */
   ~WFAPITester();

   /**
    * Start Nav2 and wait for the startup to complete and
    * return the startup status.
    *
    * @return The status of the startup of Nav2API.
    */
   const AsynchronousStatus& startNav2();

   /**
    * Get the Nav2API.
    *
    * @return The Nav2API.
    */
   Nav2API& getNav2API();

   /**
    * Print the help for the current GenericTester.
    */
   void printCurrentHelp();

   /**
    * Set the UI control.
    *
    * @param uiControl The UI control to set.
    */
   void setUIControl(UIControl* uiControl);

public: // From base classes

   /**
    * From Nav2StatusListener.
    */
   void startupComplete();

   /**
    * From Nav2StatusListener.
    */
   void mapLibStartupComplete();

   /**
    * From Nav2StatusListener.
    */
   void stopComplete();

   /**
    * From Nav2StatusListener.
    */
   void error(const AsynchronousStatus& status);

   /**
    * Handle the key and return true if it was consumed.
    *
    * @param keyValue An integer key value representing the key.
    * @return True if the key was consumed, false otherwise.
    */
   virtual bool handleKey(int keyValue);

   /**
    * @see GenericTester.
    */
   bool handleInput(const char* input, bool& goBack);

   /**
    * Used to tell UIControl that we want to snap to positions.
    */
   void snapToPosition();

   /**
    * Returns true if nav2 is properly started.
    */ 
   bool isNav2Started() const;
protected:
   
   /**
    * Add the command options.
    */
   virtual void addCommandOptions();

private:

   /**
    * Inititalize the testers.
    *
    * @param dataPath The path to the directory of the nav2 data.
    */
   void initTesters(const char* dataPath);
   
   /**
    * The route tester.
    */
   RouteTester* m_routeTester;

   /**
    * The favorite tester.
    */
   FavouriteTester* m_favouriteTester;

   /**
    * The settings tester.
    */
   SettingsTester* m_settingsTester;

   /**
    * The settings tester.
    */
   SearchTester* m_searchTester;

   /**
    * The image tester
    */
   ImageTester* m_imageTester;

   /**
    * The location tester.
    */
   LocationTester* m_locationTester;

   /**
    * The network tester.
    */
   NetworkTester* m_networkTester;

   /**
    * The billing tester.
    */
   BillingTester* m_billingTester;

   /**
    * The service window tester.
    */
   ServiceWindowTester* m_serviceWindowTester;

   /**
    * The currently selected tester.
    */
   GenericTester* m_currentTester;

   /**
    * Reference to the bool that WFAPITestClient is waiting on during
    * stop.
    */
   bool& m_stop;

   /// Pointer to the Nav2API
   Nav2API* m_nav2API;

   /// The resulting status of the startup of Nav2.
   AsynchronousStatus m_startUpStatus;

   /// If Nav2 has been started or not.
   bool m_nav2Started;

   /// The ready to run listener, synchronizing callbacks to current thread.
   ReadyToRunListener* m_readyToRunListener;

   /// If gtk should be enabled.
   bool m_enableGtk;

   /// The UI Control.
   UIControl* m_uiControl;

   /// The text Language to use.
   TextLanguage m_textLang;

   /// The voice language to use.
   VoiceLanguage::VoiceLanguage m_voiceLang;
};

#endif
