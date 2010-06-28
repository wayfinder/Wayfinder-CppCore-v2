/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_POICATEGORIES_H
#define WFAPI_POICATEGORIES_H

#include "PALMachineTypes.h"
#include "WFString.h"
#include <vector>

namespace WFAPI {

/**
 * Class representing a POI category with possibility
 * to enable or disable visibility.
 */
class POICategory {
public:
   /**
    * The constructor. Must be supplied with a name, a unique id and
    * whether the category should be visible or not by default.
    *
    * @param catName The name of the category.
    * @param id The id of the category. 
    * @param visible True if the category should be visible by default.
    */
   POICategory(const WFString& catName, wf_uint32 id, bool visible);
   
   /**
    * Returns the id of the category.
    *
    * @return The id of the category.
    */
   wf_uint32 getID() const;

   /**
    * Returns the name of the category in UTF8.
    *
    * @return The name of the category.
    */
   const WFString& getName() const;

   /**
    * Returns true if the category is visible, false otherwise.
    */
   bool isVisible() const;

   /**
    * Set to true if the category should be visible, false otherwise.
    *
    * @param visible True if category should be visible, false otherwise.
    */
   void setVisible(bool visible);

private:

   WFString m_catName;
   wf_uint32 m_id;
   bool m_visible;
};

/**
 * POI categories typedef.
 */
typedef std::vector<POICategory> POICategories;

} // End of namespace WFAPI.

#endif // WFAPI_POICATEGORIES_H
