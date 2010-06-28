/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_ITEM_INFO_ENTRY_H
#define WFAPI_ITEM_INFO_ENTRY_H

#include "WFAPIConfig.h"
#include "ItemInfoEnums.h"
#include "WFString.h"

namespace WFAPI {


/**
 * A piece of information about an item.
 */
class ItemInfoEntry
{

public:
   
   /**
    * Constructs a new ItemInfoEntry from the parameters.
    *
    * @param key The key string.
    * @param val The value string.
    * @param infoType The type of info that the key represents.
    */
   ItemInfoEntry(const WFString& key,
                 const WFString& val,
                 ItemInfoType infoType);

   /**
    * Returns the key string.
    *
    * @return The string with the key name.
    */
   const WFString& getKey() const;
   
   /**
    * Returns the value string.
    *
    * @return The string value. 
    */
   const WFString& getVal() const;

   /**
    * Returns the type of information.
    *
    * @return The type of information contained in this ItemInfoEntry.
    */
   ItemInfoType getInfoType() const;
   
private:

   /**
    * The key string.
    */
   WFString m_key;

   /**
    * The value string.
    */
   WFString m_val;

   /**
    * The type of the ItemInfoEntry.
    */
   ItemInfoType m_infoType;
};

} // End namespace WFAPI

#endif // WFAPI_ITEM_INFO_ENTRY_H
