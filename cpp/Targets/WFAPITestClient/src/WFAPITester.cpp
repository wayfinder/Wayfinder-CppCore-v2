/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "WFAPITester.h"

#include "StartupData.h"
#include "LinuxNav2API.h"

#include "RouteTester.h"
#include "SettingsTester.h"
#include "FavouriteTester.h"
#include "SearchTester.h"
#include "ImageTester.h"
#include "LocationTester.h"
#include "NetworkTester.h"
#include "BillingTester.h"
#include "ServiceWindowTester.h"
#include "UIControl.h"
#include "NetworkLinkID.h"
#include "NetworkInterface.h"

#include "ImageDimension.h"
#include "ImageExtension.h"

#include <gtk/gtk.h>


//VF ES proxy:
//#define HARD_IAP_PROXY_HOST "212.73.32.10"
//#define HARD_IAP_PROXY_PORT 80
//VF NL proxy:
//#define HARD_IAP_PROXY_HOST "192.168.251.150"
//#define HARD_IAP_PROXY_PORT 8799
//VF PT proxy
//#define HARD_IAP_PROXY_HOST "172.16.19.50"
//#define HARD_IAP_PROXY_PORT 8799

 

#if ((defined MAEMO4) || (defined MAEMO5))
#include "MaemoNav2API.h"
#else
#include "LinuxNav2API.h"
#endif

#include <unistd.h>
#include <iostream>

#ifdef MAEMODATAPATH
# define DATAPATH "/usr/share/WFAPITestClient/nav2data/"
#else
# define DATAPATH "./nav2data/"
#endif

using namespace std;
using namespace WFAPI;


WFAPITester::WFAPITester(bool& stop, bool enableGtk, TextLanguage textLanguage,
                         VoiceLanguage::VoiceLanguage voiceLang)
   : m_stop(stop),
     m_startUpStatus(0, OK, "", ""),
     m_nav2Started(false),
     m_enableGtk(enableGtk),
     m_textLang(textLanguage),
     m_voiceLang(voiceLang)
{
   cout << "WFAPITester::WFAPITester()" << endl;
   // Initialise tester objects to NULL, they are created in startNav2 using
   // the initTesters function.
   m_routeTester = NULL;
   m_favouriteTester = NULL;
   m_settingsTester = NULL;
   m_currentTester = NULL;
   m_searchTester = NULL;
   m_imageTester = NULL;
   m_locationTester = NULL;
   m_networkTester = NULL;
   m_billingTester = NULL;
   m_serviceWindowTester = NULL;
   
   m_uiControl = NULL;

   // Add the command options.
   addCommandOptions();

   // Create the Nav2API object.

#if ((defined MAEMO4) || (defined MAEMO5))
   m_nav2API = new MaemoNav2API(); 
#else
   m_nav2API = new LinuxNav2API();
#endif
   cout << "WFAPITester::WFAPITester: m_nav2API created." << endl;
}

WFAPITester::~WFAPITester()
{
   delete m_routeTester;
   delete m_favouriteTester;
   delete m_settingsTester;
   delete m_searchTester;
   delete m_imageTester;
   delete m_locationTester;
   delete m_networkTester;
   delete m_billingTester;
   delete m_serviceWindowTester;
   delete m_nav2API;
}

void
WFAPITester::initTesters(const char* dataPath)
{
   // Create the testers.
   m_routeTester = new RouteTester(this,
                                   getNav2API().getRouteInterface(),
                                   getNav2API().getNavigationInterface(),
                                   dataPath, "Vorbis", "ogg", // Hardcoded here
                                   getNav2API().getVoiceLanguageAsString(
                                      m_voiceLang, true/*uppercase*/));

   m_favouriteTester = new FavouriteTester(
         getNav2API().getFavouriteInterface());

   m_settingsTester = new SettingsTester(
      getNav2API().getSettingsInterface());

   m_searchTester = new SearchTester(
      getNav2API().getSearchInterface());

   m_imageTester = new ImageTester(
      getNav2API().getImageInterface(), dataPath);

   m_locationTester = new LocationTester(getNav2API().getLocationInterface());

   m_networkTester = new NetworkTester(getNav2API().getNetworkInterface());
   
   m_billingTester = new BillingTester(getNav2API().getBillingInterface());

   m_serviceWindowTester = new ServiceWindowTester(getNav2API().getTunnelInterface());
}

const AsynchronousStatus& 
WFAPITester::startNav2()
{
   const char* dataPath = DATAPATH;
   cout << "WFAPITester::startNav2, before StartupData creation." << endl;  
   StartupData startupData(dataPath,
                           dataPath,
                           dataPath,
                           m_textLang,
                           m_voiceLang);
   cout << "WFAPITester::startNav2, after StartupData creation." << endl;

   startupData.setImageExtension(PNG);
   startupData.setImageDimension(DIMENSION_50X50);

   // Set the voice type directory name
   m_nav2API->setAudioTypeDirName("Vorbis");

   // Start the startup of Nav2API, this call will return after
   // initiating the startup.
   cout << "WFAPITester::startNav2, calling m_nav2API->start" << endl;  
   m_startUpStatus =  m_nav2API->start(this, &startupData);
   // Wait for the startup of Nav2API to finish. m_nav2Started is set
   // in startupComplete that Nav2API calls when it is started.


   return m_startUpStatus;
}

Nav2API& 
WFAPITester::getNav2API()
{
   return *m_nav2API;
}

void
WFAPITester::printCurrentHelp()
{
   if (m_currentTester != NULL) {
      m_currentTester->printHelp();
   } else {
      printHelp();
   }
}

void
WFAPITester::setUIControl(UIControl* uiControl)
{
   m_uiControl = uiControl;
   if (m_locationTester != NULL && m_uiControl != NULL) {
      m_locationTester->addLocationListener(m_uiControl);
   
   }
   if(m_routeTester != NULL && m_uiControl != NULL) {
      m_routeTester->addNavigationInfoUpateListener(m_uiControl);
   }
}

void 
WFAPITester::startupComplete()
{
   const char* dataPath = DATAPATH;
   
   cout << "WFAPITester::startNav2, starting m_nav2API done." << endl;  

#if defined HARD_IAP_PROXY_HOST && defined HARD_IAP_PROXY_PORT
   cout << "Doing setNetworkLinkID with proxy host " << HARD_IAP_PROXY_HOST << ", port " << HARD_IAP_PROXY_PORT << endl;
   getNav2API().getNetworkInterface().setNetworkLinkID(
      WFAPI::NetworkLinkID(
                           2, //ignored in linux case
                           HARD_IAP_PROXY_PORT,
                           HARD_IAP_PROXY_HOST));

   cout << "doing setHttpProto(true) to ensure proxy usage works" << endl;
   m_nav2API->setHttpProto(true);
#endif


   // Create the testers.
   initTesters(dataPath);

   // Register interesting listeners.
   if (m_routeTester != NULL && m_uiControl != NULL ) {
      m_routeTester->addNavigationInfoUpateListener(m_uiControl);
   }

   if (m_networkTester != NULL) {
     //m_networkTester->testServerConnection();
   }
   // Nav2API calls this when startup has completed. If there was an error
   // during startup the error function is called and not this function.
   cout << "WFAPITester::startupComplete." << endl;  
   m_nav2Started = true;
}

void 
WFAPITester::mapLibStartupComplete()
{
   cout << "WFAPITester::mapLibStartupComplete." << endl;  
}

void 
WFAPITester::stopComplete()
{
   // Called from Nav2API when Nav2API has finished shutting down and is
   // ready to be deleted.
   cout << "WFAPITester::stopComplete." << endl;
   m_stop = true;   
}

void
WFAPITester::error(const AsynchronousStatus& status)
{
   printErrorStatus(&status, "WFAPITester");
   // An error has occured, assumes that it is during startup.
   m_startUpStatus = status;
   m_nav2Started = true;
}
   
bool 
WFAPITester::handleKey(int keyValue)
{
  // Called when running in gtk mode.
   string str;
   str += char(keyValue);
   bool quit = false;
   bool handled = handleInput(str.c_str(), quit);
  
   if (quit) {
      std::cout << "Press e to quit in Gtk window." << std::endl; 
   }
   return handled; 
}

bool 
WFAPITester::handleInput(const char* input, bool& goBack)
{
   // Handle a input.
  

   // If there is subtester set let it handle the input 
   if (m_currentTester != NULL) {
      // Supply input to current tester.
      bool back = false;
      bool handled = m_currentTester->handleInput(input, back);
  
      if (back) {
         printHelp();
         m_currentTester = NULL;
      }
      return handled;
   }
  
   // Else, it is for me.

   // Search for the commnd among the supported ones.
   StringMap::const_iterator it = m_commandOptions.find(input);
   if (it == m_commandOptions.end()) {
      // No such command, return false to indicate that it is not handled.
     // cout << "Could NOT find the command in the commad Map return false" << endl;
      return false;
   }

   // Handle the command.
   switch((*it).second.m_commandType) {
      case help:
         printHelp();
         break;
      case route: {
	m_currentTester = m_routeTester;
	break;
      }
      case favourites: {        
         m_currentTester = m_favouriteTester;
         break;
      }   
      case settings: {        
         m_currentTester = m_settingsTester;
         break;
      }
      case searchTest: {
         m_currentTester = m_searchTester;
         break;
      }
      case imageTest: {
         m_currentTester = m_imageTester;
         break;
      }
      case locationTest:
         m_currentTester = m_locationTester;
         break;
      case networkTest:
         m_currentTester = m_networkTester;
         break;
      case billingTest:
         m_currentTester = m_billingTester;
         break;
   case serviceWindowTest:
      m_currentTester = m_serviceWindowTester;
      break;
      case quit:
         goBack = true;
         break;
      default:
         cout << "Invalid input, please try again." << endl;
         return false;
         break;
   }

   // If a tester was selected then print it's help.
   if (m_currentTester != NULL) {
      m_currentTester->printHelp();
   }

   // If we got this far, we consumed the key.
   return true;
}

void 
WFAPITester::addCommandOptions()
{
   // Add the commands 
   m_commandOptions.
      insert(make_pair("h",
                       CommandHolder("Displays this help menu.", help)));
   m_commandOptions.
      insert(make_pair("r", 
                       CommandHolder("Route option, you should specify "
                                     "origin and destination in WGS84 "
                                     "Coordinates.", route)));
   m_commandOptions.
      insert(make_pair("f", 
                       CommandHolder("Favourite option. Add, delete, "
                                     "sync and get Favourites.", favourites)));
   m_commandOptions.
      insert(make_pair("d", 
                       CommandHolder("Image test menu. ", imageTest)));
   m_commandOptions.
      insert(make_pair("g",
                       CommandHolder("Location test menu. ", locationTest)));
   m_commandOptions.
      insert(make_pair("n",
                       CommandHolder("Network test menu. ", networkTest)));
   m_commandOptions.
      insert(make_pair("p", 
                       CommandHolder("Settings test menu.", settings)));
   m_commandOptions.
      insert(make_pair("s", 
                       CommandHolder("Search test menu.", searchTest)));

   m_commandOptions.
      insert(make_pair("i", 
                       CommandHolder("Billing test menu.", billingTest)));
   m_commandOptions.
      insert(make_pair("w", 
                       CommandHolder("service Window test menu.", serviceWindowTest)));

   m_commandOptions.
      insert(make_pair("q", 
                       CommandHolder("Quits.", quit)));
}

void WFAPITester::snapToPosition()
{
   if (m_uiControl != NULL){
      m_uiControl->snapToPosition();
   }
}

bool WFAPITester::isNav2Started() const
{
   return m_nav2Started; 
}


