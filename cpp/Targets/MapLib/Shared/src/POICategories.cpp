/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "PALMachineTypes.h"
#include "POICategories.h"
#include "WFString.h"


namespace WFAPI {


/**
 * This class is very similar to TileCategory. But since this needs to be
 * publicly used via the MapLibAPI, we allow some code duplication in favor
 * of stirring around the TileCategory class.
 *
 * When changing this class, be sure to update TileCategory
 * accordingly - if needed.
 */
POICategory::POICategory(const WFString& catName, wf_uint32 id, bool visible) :
   m_catName(catName), m_id(id), m_visible(visible)
{
   
}

wf_uint32 WFAPI::POICategory::getID() const
{
   return m_id; 
}

const WFString& WFAPI::POICategory::getName() const
{
   return m_catName; 
}

bool WFAPI::POICategory::isVisible() const
{
   return m_visible; 
}

void WFAPI::POICategory::setVisible(bool visible)
{
   m_visible = visible; 
}

} // End of namespace WFAPI

