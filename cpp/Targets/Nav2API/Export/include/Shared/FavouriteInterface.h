/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_FAVOURITE_INTERFACE_H
#define WFAPI_FAVOURITE_INTERFACE_H

#include "WFAPIConfig.h"
#include "Interface.h"
#include "SynchronousStatus.h"
#include "AsynchronousStatus.h"

namespace WFAPI {

// Forward declarations
class Favourite;
class FavouriteListener;
class FavouriteArray;
class ItemInfoArray;
class Nav2APIImpl;
class FavouriteInterfaceImpl;

/**
 * Interface class for favourites, used to get, add, delete or 
 * synchonise favourites.
 */
class FavouriteInterface : public Interface
{

public:

   /**
    * Destructor.
    */
   virtual ~FavouriteInterface();
   
   /**
    * Add a new favourite. Call syncFavourites to push it to 
    * the server.
    * Do not use the added Favourite as it will get an id while being added.
    * Call getFavourites to get an up to date Favourite object.
    *
    * @param favourite The favourite to add.
    * @return The status of the operation, StatusCodes::OK if operation
    *         succeeded.
    */
   SynchronousStatus addFavourite(const Favourite& favourite);

   /**
    * Add a new favourite using the current position.
    * Call syncFavourites to push it to the server. 
    * Do not use the added Favourite as it will get an id while being added.
    * Call getFavourites to get an up to date Favourite object.
    *
    * @param name The name of favourite to add.
    * @param description The description of the favourite.
    * @param info The additional information about the favourite.
    * @return The status of the operation, StatusCodes::OK if operation
    *         succeeded.
    */
   SynchronousStatus addCurrentPositionAsFavourite(const WFString& name,
                                                   const WFString& description,
                                                   ItemInfoArray& info);

   /**
    * Add a listener for favourite events.
    * @see FavouriteListener
    *
    * @param listener The FavouriteListener to add. 
    *                 It may not be deleted until it is removed by
    *                 calling removeFavouriteListener.
    */
   void addFavouriteListener(FavouriteListener* listener);

   /**
    * Remove a favourite by it's id. Call syncFavourites to push the 
    * change to the server.
    *
    * @param id The id of the favourite to remove.
    * @return The status of the operation, StatusCodes::OK if operation
    *         succeeded.
    */
   SynchronousStatus deleteFavourite(wf_uint32 id);

   /**
    * Get the current list of favourites.
    *
    * @param startIndex The index of the first Favourite in the
    *                   output favouriteArray.
    * @param count The maximum number of Favourites in the favouriteArray.
    *              Use WF_MAX_UINT16 to get all favourites stored in Nav2.
    * @param totalFavouriteCount Set to the total number of favourites
    *                            there is currently.
    * @param favouriteArray The current Favourites are added to this 
    *                       output favouriteArray.
    * @return The status of the operation, StatusCodes::OK if operation
    *         succeeded.
    */
   SynchronousStatus getFavourites(wf_uint32 startIndex, 
                                   wf_uint32 count,
                                   wf_uint32& totalFavouriteCount,
                                   FavouriteArray& favouriteArray);

   /**
    * Remove a listener.
    *
    * @param listener The FavouriteListener to remove.
    *                 It should now be deleted by caller, unless caller
    *                 keeps it around for later use.
    */
   void removeFavouriteListener(FavouriteListener* listener);

   /**
    * This asynchronous function starts a synchronisation of favrourites
    * with the server, when operation is done the added FavouriteListeners
    * will get a callback with the RequestID returned by this call.
    *
    * @return The status and if status is OK the RequestID that will be
    *         returned in the FavouriteListeners favouritesSynced function.
    */
   AsynchronousStatus syncFavourites();

   /**
    * Internal function.
    */
   FavouriteInterfaceImpl* getImpl();
    
private:
   /**
    * Do not create FavouriteInterface get it from the Nav2API class.
    */
   FavouriteInterface(Nav2APIImpl* nav2APIimpl);

   /**
    * We do not allow copies of this interface.
    */
   FavouriteInterface(const FavouriteInterface&);
   const FavouriteInterface& operator=(const FavouriteInterface&);
   
   friend class Nav2APIImpl;

   struct impl;

   impl* m_impl;
};

} // End namespace WFAPI

#endif // End WFAPI_FAVOURITE_INTERFACE_H
