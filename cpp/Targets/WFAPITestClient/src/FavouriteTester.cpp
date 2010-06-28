/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "FavouriteTester.h"
#include "FavouriteInterface.h"
#include "Favourite.h"
#include "FavouriteArray.h"
#include "WGS84Coordinate.h"
#include "WFString.h"
#include "ItemInfoEnums.h"
#include <iostream>
#include <stdlib.h>

using namespace std;
using namespace WFAPI;

FavouriteTester::FavouriteTester(FavouriteInterface& favouriteInterface)
   : m_favouriteInterface(favouriteInterface)
{
   m_favouriteInterface.addFavouriteListener(this);
   addCommandOptions();
}
                   
FavouriteTester::~FavouriteTester()
{

}
   
void
FavouriteTester::addFavourite()
{
   Favourite fav;
   cout << "Enter favourite name: ";
   string name = "";
   cin.clear();
   getline(cin, name);
   fav.setName(WFString(name.c_str()));
   cout << "Enter favourite description: ";
   string desc = "";
   cin.clear();
   getline(cin, desc);
   fav.setDescription(WFString(desc.c_str()));
   cout << "Enter favourite coord latitude: ";
   double lat = 0;
   cin >> lat;
   cout << "Enter favourite coord longitude: ";
   double lon = 0;
   cin >> lon;
   
   fav.setPosition(WGS84Coordinate(lat, lon));   
   fav.addItemInfoEntry(ItemInfoEntry(WFString("key"),
                                       WFString("val"),
                                       URL));
   m_favouriteInterface.addFavourite(fav);
}

void FavouriteTester::addCurrentPositionAsFavourite()
{
   cout << "Enter favourite name: ";
   string name = "";
   cin.clear();
   getline(cin, name);
   cout << "Enter favourite description: ";
   string desc = "";
   cin.clear();
   getline(cin, desc);

   ItemInfoArray ia;
   m_favouriteInterface.addCurrentPositionAsFavourite(WFString(name.c_str()),
                                                      WFString(desc.c_str()),
                                                      ia);
}

void
FavouriteTester::deleteFavourite()
{
   cout << "Enter id of Favourite to delete:";
   string input = "";
   cin.clear();
   getline(cin, input);
   wf_uint32 id = strtoul(input.c_str(), NULL, 10);
   cout << "You entered: " << id << endl;
   m_favouriteInterface.deleteFavourite(id);
}

void
FavouriteTester::getFavourites()
{
   FavouriteArray favouriteArray;
   wf_uint32 totalFavouriteCount = 0;

   // Get the current favourites
   m_favouriteInterface.getFavourites(0, WF_MAX_UINT16, totalFavouriteCount,
                                      favouriteArray);
   cout << "FavouriteTester::getFavourites totalFavouriteCount = "
        << totalFavouriteCount << endl;

   // Print the current favourites
   for (FavouriteArray::iterator it = favouriteArray.begin(); 
        it != favouriteArray.end(); ++it) {
      cout << "================== Favourite =================="<< endl;
      cout << "Name: " << it->getName().c_str() << endl;
      cout << "ID: " << it->getID() << endl;
      cout << "Position: " << it->getPosition() << endl;
      cout << "Description: " << it->getDescription().c_str() << endl;
      if (!(it->getInformationArray().empty())) {
         for (ItemInfoArray::const_iterator itemIt = 
                  it->getInformationArray().begin(); 
              itemIt != it->getInformationArray().end(); ++itemIt) {
            cout << " Extra key: " << itemIt->getKey() << endl;
            cout << " Extra val: " << itemIt->getVal() << endl;
            cout << " Extra type: " << itemIt->getInfoType() << endl;
         }
      }
      cout << "==============================================="<< endl;
   }
}

void
FavouriteTester::syncFavourites()
{
   cout << "Sync Favourites" << endl;
   m_favouriteInterface.syncFavourites();
}

void
FavouriteTester::error(const AsynchronousStatus& status)
{
   printErrorStatus(&status, "FavoriteTester");
}

void
FavouriteTester::favouritesChanged()
{
   cout << "FavouriteTester::favouritesChanged" << endl;
}

void
FavouriteTester::favouritesSynced(RequestID requestID)
{
   cout << "FavouriteTester::favouritesSynced" << endl;
}

bool 
FavouriteTester::handleInput(const char* input, bool& goBack)
{
   StringMap::const_iterator it = m_commandOptions.find(input);

   if (it == m_commandOptions.end()) {
      return false;
   }
   switch((*it).second.m_commandType) {
      case help:  
         printHelp();
         break;
      case cmdAddFavourite:
         addFavourite();
         break;
      case cmdCurrentPosAsFavourite:
         addCurrentPositionAsFavourite();
         break;
      case cmdDeleteFavourite:
         deleteFavourite();
         break;
      case cmdSyncFavourites:
         syncFavourites();
         break;
      case cmdGetFavourites:
         getFavourites();
         break;
      case back:
         goBack = true;
         return false;
         break;
      default:
         cout << "Command not handled here, please try again or select B to "
              << "go back." << endl;
         return false;
         break;
   }

   // If we get here, the key has been consumed.
   return true;
}   

void 
FavouriteTester::addCommandOptions()
{
   m_commandOptions.
      insert(make_pair("h",
                       CommandHolder("Displays this help menu.", help)));
   m_commandOptions.
      insert(make_pair("a", 
                       CommandHolder("Add a new favourite", 
                                     cmdAddFavourite)));
   m_commandOptions.
      insert(make_pair("c", 
                       CommandHolder("Add current position as a favourite", 
                                     cmdCurrentPosAsFavourite)));   
   m_commandOptions.
      insert(make_pair("d", 
                       CommandHolder("Delete a favourite", 
                                     cmdDeleteFavourite)));
   m_commandOptions.
      insert(make_pair("s", 
                       CommandHolder("Sync favourites", 
                                     cmdSyncFavourites)));
   m_commandOptions.
      insert(make_pair("g", 
                       CommandHolder("Get favourites", 
                                     cmdGetFavourites)));
   m_commandOptions.
      insert(make_pair("b", 
                       CommandHolder("Back", 
                                     back)));
}


