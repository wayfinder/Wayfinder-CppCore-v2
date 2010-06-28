/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_SEARCH_HISTORIC_INPUT_INPUT_H
#define WFAPI_SEARCH_HISTORIC_INPUT_INPUT_H

#include "WFAPIConfig.h"
#include "WFString.h"

namespace WFAPI {


/**
 * A historic search input.
 */
class SearchHistoricInput
{

public:

   /**
    * Constructor.
    *
    * @param what The what string.
    * @param where The where string.
    * @param topRegionID The selected top region id.
    * @param categoryID The inputed category id, MAX_UINT32 if none selected.
    */
   SearchHistoricInput(const WFString& what, const WFString& where,
                       wf_uint32 topRegionID, wf_uint32 categoryID );

   /**
    * Destructor.
    */
   virtual ~SearchHistoricInput();

   /**
    * Get the what string.
    * 
    * @return The what string from the search.
    */
   const WFString& getWhat() const;

   /**
    * Get the where string.
    * 
    * @return The where string from the search.
    */
   const WFString& getWhere() const;

   /**
    * Get the top region ID.
    * 
    * @return The topRegionID from the search.
    */
   wf_uint32 getTopRegionID() const;

   /**
    * Get the categoryID.
    * 
    * @return The categoryID from the search.
    */
   wf_uint32 getCategoryID() const;

private:

   /// The what string in the search.
   WFString m_what;

   /// The wher string in the search.
   WFString m_where;

   /// The country id in the search.
   wf_uint32 m_topRegionID;

   /// The heading id in the search.
   wf_uint32 m_categoryID;
};

} // End namespace WFAPI

#endif // End WFAPI_SEARCH_HISTORIC_INPUT_INPUT_H


