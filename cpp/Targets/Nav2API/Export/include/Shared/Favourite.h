/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_FAVOURITE_H
#define WFAPI_FAVOURITE_H

#include "WFAPIConfig.h"
#include "WGS84Coordinate.h"
#include "WFString.h"
#include "ItemInfoEntryArray.h"

namespace WFAPI {

/**
 * A Favourite describes a favourite location that can be used 
 * to return to or show that location.
 */
class Favourite
{

public:


   /// The favourite id that is used for new favourites.
   static const wf_uint32 INVALID_FAVOURITE_ID;

   /**
    * Constructor, creates a new Favourite with INVALID_FAVOURITE_ID.
    *
    * @param id The id number of the favourite, use INVALID_FAVOURITE_ID
    *           for new favourites.
    */
   explicit Favourite(const wf_uint32 id = INVALID_FAVOURITE_ID);

   /**
    * Constructor.
    *
    * @param id The id number of the favourite.
    * @param position The location of the favourite.
    * @param name The name of the favourite.
    * @param description The description of the favourite.
    * @param infos The additional information about the favourite.
    */
   Favourite(wf_uint32 id,
             WGS84Coordinate position,
             const WFString& name,
             const WFString& description,
             ItemInfoArray infos );
   /**
    * Destructor.
    */
   virtual ~Favourite();

   /**
    * The identification number of the favourite.
    */
   wf_uint32 getID() const;

   /**
    * The position of the favourite.
    * 
    * @return The location of the favourite.
    */
   WGS84Coordinate getPosition() const;

   /**
    * Set the position of the favourite.
    * 
    * @param position The new location of the favourite.
    */
   void setPosition(const WGS84Coordinate& position);

   /**
    * The name of the favourite.
    *
    * @return The name as string.
    */
   const WFString& getName() const;

   /**
    * Set the name of the favourite.
    *
    * @param name The new name.
    */
   void setName(const WFString& name);

   /**
    * The description of the favourite.
    *
    * @return The description as string.
    */
   const WFString& getDescription() const;

   /**
    * Set the description of the favourite.
    *
    * @param description The new description.
    */
   void setDescription(const WFString& description);

   /**
    * Get the ItemInfoArray which contains additional 
    * information items about this favourite.
    */
   const ItemInfoArray& getInformationArray() const;

   /**
    * Add an ItemInfoEntry to the favourite.
    *
    * @param info 
    */
   void addItemInfoEntry(const ItemInfoEntry& info);

private:

   /**
    * The identification number of the favourite.
    */
   wf_uint32 m_id;

   /**
    * The location of the favourite.
    */
   WGS84Coordinate m_position;

   /**
    * The name of the favourite.
    */
   WFString m_name;

   /**
    * The description of the favourite.
    */
   WFString m_description;

   /**
    * The additional information about the favourite.
    */
   ItemInfoArray m_infos;
};

} // End namespace WFAPI

#endif // End WFAPI_FAVOURITE_H
