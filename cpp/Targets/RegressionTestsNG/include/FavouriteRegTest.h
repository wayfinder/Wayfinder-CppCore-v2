/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _FAVOURITEREGTEST_H_
#define _FAVOURITEREGTEST_H_

#include "FavouriteInterface.h"
#include "FavouriteListener.h"
#include "RegressionTest.h"
#include "Favourite.h"
#include "RequestID.h"
#include "WGS84Coordinate.h"
#include "ItemInfoEnums.h"

#include "Favourite.h"


#include <string>
#include <map>
#include <utility>

class Nav2Session;



/**
 * This class creates a numkber of favourites, then sync them with the
 * server and then checks if they are returned correctly.
 */

class FavouriteRegTest : public WFAPI::FavouriteListener,
                         public RegressionTest
{
public:

   enum state_t { UNINITIALIZED,
                  ADD_FAV,
                  ADD_FAV_DONE,
                  SYNC_FAV,
                  SYNC_FAV_DONE,
                  SYNC_FAV_ONGOING,
                  CHECK_DONE,
                  ERROR,
                  START_SYNC_TEST,
                  DONE };
   
   /**
    * Constructor.
    *
    * @param session The Nav2Session to run within.
    * @param name
    */
   FavouriteRegTest(Nav2Session& session, const char* name);

   /**
    * Destructor.
    */
   virtual ~FavouriteRegTest();

   /**
    * Starts the test.
    */
   virtual void startTest();

   /**
    * Checks if the test is finished or not.
    *
    * @return True if done, false if not.
    */
   bool finished();

   /**
    * From favourites listener.
    * 
    * Called when the synchronisation of favrourites with the server
    * has completed. The synchronisation is started by the syncFavourites
    * function in FavouriteInterface.
    * 
    * @param requestID The identification of the syncFavourites request
    *                  sent from the FavouriteInterface.
    */
   virtual void favouritesSynced(WFAPI::RequestID requestID);

   /**
    * From favourites listener.
    * 
    * Called by the API when the favrourites has been changed.
    */
   virtual void favouritesChanged();

   /**
    * From Favourite listener, called when a favourite operation has triggered
    * an error in some way.
    *
    * @param status The status containing the error message.
    */
   virtual void error(const WFAPI::AsynchronousStatus& status);

   typedef std::vector<WFAPI::RequestID> t_reqIdVec;

private:

   /**
    * To print all information in the favourite.
    *
    * @param fav The favourite to print.
    */
   void printFavourite(WFAPI::Favourite fav);

   /**
    * This favourite checks all created favourites agains the synced ones.
    * If they are not correct the test failes.
    */
   void checkSyncedFavourites();

   /**
    * This functions creates a number of favourites.
    */
   void createFavourites();

   /**
    * This functions calls the addFavourite function for each cretated
    * favourite.
    */
   void addFavourites();

   void startSyncTest();

   /**
    * Creates a favourite with dummy data.
    *
    * @param name The name of the favourite
    * @param desc The description of the favourite
    *
    * @return A favourite with dummy data.
    */
   WFAPI::Favourite createFavourite(std::string name, std::string desc);

   bool checkIfSyncTestIsDone();
   /// Typedef for holding  all favourites added.
   typedef std::map<int, WFAPI::Favourite> t_favouriteMap;

   /// A set containing the favourites 
   t_favouriteMap m_favTestMap;

   /// Keeps track of the current state
   state_t m_state;

   /// The interface to handled favourites.
   WFAPI::FavouriteInterface& m_favouriteInterface;

   /// The number of favourites that has been added.
   unsigned int m_numFavAdded;

   t_reqIdVec m_reqIdVec;

   bool m_gotGoodReplyForSync;

   bool m_gotOkErrorMsgForSync;

   t_reqIdVec m_allReqId;
};

#endif
