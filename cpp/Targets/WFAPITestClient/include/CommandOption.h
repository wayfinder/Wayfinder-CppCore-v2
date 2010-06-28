/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef COMMAND_OPTION_H
#define COMMAND_OPTION_H

#include <map>
#include <string>

/**
 * The list of all commands in the test client.
 */
enum CommandType
{
   /// Print the help.
   help,
   /// Go into the route tester.
   route,
   /// Go into the favourite tester.
   favourites,
   /// Go into the setting tester.
   settings,
   /// Go into the search tester.
   searchTest,
   /// Go to the image tester menu.
   imageTest,
   /// Go to the location tester menu.
   locationTest,
   /// Go to network tester menu.
   networkTest,
   /// Go to billing tester menu.
   billingTest,
   /// Go to service window tester menu.
   serviceWindowTest,
   /// Start map snapping to the current position.
   snapToPosition,
   /// Quit the application.
   quit,
   /// Go back one menu.
   back,
   // Route sub menu
   /// Make the default route.
   cmdDefaultRoute,
   /// Make a route between two coordintes that has to be inputed.
   cmdRouteBetweenCoordinates,
   /// Make a route to a coordinate using the current GPS position as start.
   cmdRouteToCoordinate,
   /// Make a route from current position to the current route's destination.
   cmdReroute,
   /// Start simulate moving along the current route.
   cmdRouteSimStart,
   /// Stop simulate moving along the current route.
   cmdRouteSimStop,
   /// Pause simulate moving along the current route.
   cmdRouteSimPause,
   /// Resume simulation after a pause simulation.
   cmdRouteSimResume,
   /// Increase speed of simulation.
   cmdRouteSimInc,
   /// Decrease speed of simulation.
   cmdRouteSimDec,
   /// Start map snapping to the current position.
   cmdRouteSnapToPosition,
   /// Clear the current route.
   cmdRouteClear,
   /// List the turns in the current route.
   cmdRouteList,

   // Favourites sub menu
   /// Add a favourite, asking the user for values.
   cmdAddFavourite,
   ///Add a favourite using the users current position.
   cmdCurrentPosAsFavourite,
   /// Delete a favourite.
   cmdDeleteFavourite,
   /// Synchronise the favourites with the server.
   cmdSyncFavourites,
   /// Get a listof the current favourites in the client.
   cmdGetFavourites,

   // Settings menu
   /// Set the setings to set A.
   cmdSettingsA,
   /// Set the setings to set B.
   cmdSettingsB,
   /// Show the settings.
   cmdSettingsShow,
   /// Check for updated EULA.
   cmdCheckEula,
   /// Check username
   cmdCheckUsername,
   /// Set password
   cmdSetPassword,
   /// Set UIN
   cmdSetUIN,
   /// Fetches version information
   cmdGetVersionInfo,

   // Search menu
   /// Do a search, asking the user for input.
   cmdSearch,
   /// Do a search, using the default values.
   cmdSearchDefault,
   /// Display the current search headings.
   cmdSearchHeadings,
   /// Display the current search results.
   cmdSearchResults,
   /// List all search results.
   cmdListAll,
   /// Get more details for search hits.
   cmdGetDetails,
   /// Get more results for a heading.
   cmdGetMoreResults,
   /// Get the historic search input values.
   cmdGetSearchHistory,
   /// Do a search using users position.
   cmdSearchByPosition,

   cmdGetSearchCategories,

   //Image menu
   /// Download the test image to file.
   cmdDownloadImageToFile,
   /// Download the test image to a buffer.
   cmdDownloadImageToBuffer,

   // Location menu
   /// Start LBS.
   cmdStartLBS,
   /// Stop LBS.
   cmdStopLBS,
   /// Is lbs started.
   cmdIslbsStarted,
   /// Request reverse geocoding.
   cmdRevGeocoding,

   // Network menu
   /// Set network link id
   cmdSetNetworkLinkID,
   /// Get network link id
   cmdGetNetworkLinkID,

   // Billing menu
   /// Verify third party transaction
   cmdVerifyThirdPartyTransaction,

   // Service Window menu
   /// Request Data
   cmdRequestData,
};

/**
 * Struct holding the help text and CommandType for a command.
 */
struct CommandHolder 
{
   /**
    * Construct a CommandHolder.
    *
    * @param text The help text for the command.
    * @param commandType The command type.
    */
   CommandHolder(std::string text, CommandType commandType) 
      : m_helpText(text),
        m_commandType(commandType) { }

   /// The help text.
   std::string m_helpText;

   /// The command.
   CommandType m_commandType;
};

/// The container of the relationship from string to command.
typedef std::map<std::string, CommandHolder> StringMap;

/// Print the help for the options in the StringMap.
void printHelp(StringMap optionsMap); 

#endif
