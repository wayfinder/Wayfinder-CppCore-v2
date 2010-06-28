/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_SEARCH_DESC_H
#define WFAPI_SEARCH_DESC_H

#include "WFAPIConfig.h"
#include "WFString.h"

namespace WFAPI {


/**
 * A search desc describes a possible heading in search results.
 */
class SearchDesc
{

public:

   /**
    * Constructor.
    *
    * @param name The name of the heading.
    * @param headingID The identification id of this heading.
    * @param topRegionID The specific top region for this heading,
    *                    WF_MAX_UINT32 if not limited to a specific top region.
    * @param imageName The image for this heading.
    */
   SearchDesc(const WFString& name, wf_uint32 headingID, 
              wf_uint32 topRegionID, const WFString& imageName);

   /**
    * Destructor.
    */
   virtual ~SearchDesc();

   /**
    * Get the name.
    * 
    * @return The name of the heading.
    */
   const WFString& getName() const;

   /**
    * Get the id.
    * 
    * @return The headingID of the heading.
    */
   wf_uint32 getHeadingID() const;

   /**
    * Get the topRegionID.
    * 
    * @return The topRegionID of the heading.
    */
   wf_uint32 getTopRegionID() const;

   /**
    * Get the image name.
    * 
    * @return The image name of the heading.
    */
   const WFString& getImageName() const;

private:
   /// The headings name.
   WFString m_name;

   /// The ID of this heading.
   wf_uint32 m_headingID;

   /// The specific top region for this heading.
   wf_uint32 m_topRegionID;

   /// The image for this heading.
   WFString m_imageName;
   
};

} // End namespace WFAPI

#endif // End WFAPI_SEARCH_DESC_H


