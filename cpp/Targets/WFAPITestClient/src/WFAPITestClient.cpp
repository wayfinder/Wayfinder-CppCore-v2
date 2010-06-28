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
#include "CommandOption.h"
#include "WFAPITester.h"
#include "RouteTester.h"
#include "FavouriteTester.h"
#include "SettingsTester.h"
#include "SearchTester.h"
#include "ScreenPoint.h"

#include "LocationInterface.h"
#include "FavouriteInterface.h"
#include "DetailedConfigInterface.h"
#include "ConfigInterface.h"
#include <string.h>
#include <stdlib.h>

#include <gtk/gtk.h>
#include "GtkDevControl.h"

#include "TextLanguage.h"
#include "VoiceLanguage.h"
#include "MapLibStartupListener.h"
#include "MapLibAPI.h"
#include "WFGfxUtil.h"
#include "UIControl.h"

#include <iostream>
#include <ctime>
#include <string>
#include <map>
#include <math.h>

using namespace std;


/**
 * The main function that starts the test application.
 *
 * The possible arguments are:
 *   -n  If not to start MapLib and just have the command line 
 *       interface.
 */
int
main(int argc, char **argv)
{
   cout << "Test application for Wayfinder API!" << endl;
#if 0
   // Test the distance calculation between two coordinates.
   
   WFAPI::WGS84Coordinate c1(55.59, 13.0078);
   WFAPI::WGS84Coordinate c2(55.565, 12.9763);

   cout << "Distance between " << c1 << " and " << c2 << " is "
        << sqrt(WFGfxUtil::squareDistBetweenCoords(c1,c2))
        << " meters."
        << endl;
#endif

   // If to use MapLib and set language for MapLib
   TextLanguage textLanguage = ENGLISH; //Default is English
   VoiceLanguage::VoiceLanguage voiceLanguage = VoiceLanguage::ENGLISH;
   bool enableMapLib = true;
   for (int i = 1; i < argc; ++i) {
      if (strcmp(argv[i], "-n") == 0) {
         enableMapLib = false;
      }
      else if(strcmp(argv[i], "-l") == 0) {
         textLanguage = static_cast<TextLanguage>(atoi(argv[i+1]));
         voiceLanguage = VoiceLanguage::VoiceLanguage(atoi(argv[i+1]));
      }
   }

   if (enableMapLib) {
      // Initialize GTK+
      if (!g_thread_supported()) g_thread_init(NULL);
      if (!g_thread_supported()){
         // If we don't have thread support in Gdk we can't use the
         // thread synchronisation, GtkReadyToRunListener, in WFAPITester.
         cout << "No threads available..." << endl;
         return 1;
      }

      gdk_threads_init();
      gdk_threads_enter();
   }
   // We need gtk for getting the imei even
   // if we run the app without MapLib
   gtk_init(&argc, &argv);

   cout << "Starting Nav2, please wait..." << endl;  

   bool stopped = false;
   WFAPITester* apiTester = new WFAPITester(stopped, enableMapLib, 
                                            textLanguage, voiceLanguage);

   // Try to start nav2, if an error is returned the status is
   // printed and the program will then be terminated.
   const AsynchronousStatus& status = apiTester->startNav2();
   if (status.getStatusCode() != OK) {
      cout << "Could not start Nav2, status: " << status.getStatusCode() 
           << endl;
      cout << "Program is terminated..." << endl;
      return 0;
   }
   // Startup was ok

   GtkDevControl* mapControl = NULL;

   while(!apiTester->isNav2Started()) {
      gtk_main_iteration_do(false);
   }
   
   cout << "Nav2 was successfully started" << endl;
   
   if (enableMapLib) {
      // If we are using MapLib then start it here after Nav2 has been started.
      cout << "Enabling MapLib." << endl;

      // Select the image directory
      const char* imageDir = "./images/";
#ifdef IMAGE_DIR
      imageDir = IMAGE_DIR;
#endif
      // Create the map control.
      mapControl = new GtkDevControl(imageDir,
                                     apiTester->getNav2API().getDBufConnection(),
                                     apiTester);
      
      apiTester->getNav2API().connectMapLib(mapControl->getMapLibAPI());

      mapControl->getMapLibAPI()->getConfigInterface()->getDetailedConfigInterface()->showCopyright( true );
      mapControl->getMapLibAPI()->getConfigInterface()->getDetailedConfigInterface()->setCopyrightPos( ScreenPoint( 0, 0 ) );
      
      // Create a UI controller.
      UIControl uiCtrl(mapControl);
      // Add UI control to the api tester.
      apiTester->setUIControl(&uiCtrl);
      apiTester->printHelp();
      // Start main loop that will run until gtk_main_quit is called.
      gtk_main();
   } else {
      // Run the command line interface for the test classes.
      cout << "\n\nSelect the test you wish to perform, type h for help" 
           << endl;   

      bool quitting = false;
      cout << "-------------- Main menu ---------------" << endl;
      // apiTester->printHelp();
     
      while (! quitting) {
         // Continue until user selects quit
         string input;
         getline(cin, input);
         if (!apiTester->handleInput(input.c_str(), quitting)) {
            cout << "No such command: " << input << endl;
            apiTester->printCurrentHelp();
         }
      }
   } // End else enableMapLib
   
   // If we started gtk above leave it here.
   if (enableMapLib) {
      gdk_threads_leave();
   }

   cout << "Starting shutdown of Nav2." << endl;
   // Start the shutdown of Nav2.
   apiTester->getNav2API().stop();

   // So now we should quit. But wait, we can't. We need to run
   // more gtk iterations so that the listeners will finish nicely.
   while(!stopped) {
      gtk_main_iteration_do(false);
   }
   
   // Delete the newed WFAPITester.
   delete mapControl;
   delete apiTester;

   return 0;
}
