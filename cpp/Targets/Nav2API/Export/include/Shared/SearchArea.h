/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_SEARCH_AREA_H
#define WFAPI_SEARCH_AREA_H

#include "WFAPIConfig.h"
#include "SearchAreaTypeEnum.h"
#include "WFString.h"

namespace WFAPI {


/**
 * A search match for an area.
 */
class SearchArea
{

public:

   /**
    * Constructor.
    *
    * @param name The name of the match.
    * @param id The identification string of the match, can be used
    *           to do a refined search in this specific area.
    * @param type The type of area.
    */
   SearchArea(const WFString& name, const WFString& id, 
              SearchAreaType type);

   /**
    * Destructor.
    */
   virtual ~SearchArea();

   /**
    * Get the name.
    * 
    * @return The name of the match.
    */
   const WFString& getName() const;

   /**
    * Get the id.
    * 
    * @return The id of the match.
    */
   const WFString& getID() const;

   /**
    * Get the type.
    * 
    * @return The type of match.
    */
   SearchAreaType getType() const;

private:
   /// The name of the match.
   WFString m_name;

   /// The id of the match.
   WFString m_id;

   /// The type of area for this match.
   SearchAreaType m_type;
   
};

} // End namespace WFAPI

#endif // End WFAPI_SEARCH_AREA_H


