/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_ITEM_INFO_ENUMS_H
#define WFAPI_ITEM_INFO_ENUMS_H

#include "WFAPIConfig.h"

namespace WFAPI {

/**
 * The types of values for ItemInfo items. Used by SearchDetails and 
 * Favourites.
 */
enum ItemInfoType { 
   /// Free text content, no special handling needed.
   TEXT,

   /// An URL.
   URL,

   /// A phone number.
   PHONE_NUMBER,

   /// An image URL
   IMAGE_URL,

   /// An email address.
   EMAIL_ADDRESS,

   /**
    * A long text describing the item, is so long that special
    * care might be needed.
    */
   LONG_DESCRIPTION,

   /**
    * A piece of information that is not for end user to see,
    * only for the application to use if it knows how else to skip.
    */
   DONT_SHOW,

   SHORT_INFO,

   ADDRESS,

   HOUSE_NUMBER,

   PARSED_ZIP_CODE,

   COMPLETE_ZIP,

   PARSED_ZIP_AREA,

   FULL_ADDRESS,
   
   CITYPART,

   SUPPLIER,

   POST_ADDRESS,

   POST_ZIP_AREA,

   POST_ZIP_CODE,

   NEIGHBORHOOD,
   
   /**
    * DEPRECATED, do not use.
    *
    * If creating a favourite that has this as a additional info type,
    * it will not be synced. This option will be removed in the next release.
    */
   SPECIAL_FLAG,
};

} // End namespace WFAPI

#endif // WFAPI_ITEM_INFO_ENUMS_H
