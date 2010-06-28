/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef WFAPI_SEARCH_CATEGORY_H
#define WFAPI_SEARCH_CATEGORY_H

#include "WFAPIConfig.h"
#include "WFString.h"

namespace WFAPI {

/**
 * A search category containing name, image name and id. The category can
 * be used when doing a search, enabling the user to search for categories,
 * e.g a search for airports. Note that the id should be used when triggering
 * the search, the name for displaying the name of the category.
 */
class SearchCategory
{
public:

   /**
    * Constructor
    *
    * @param name The name of the category. The name should be used for
    *             visible display, for example in a list in an application.
    * @param id The id of the category. This id should be used when triggering'
    *           a search in the categegory.
    * @param intId The id as an int. Should be used when making a search
    *              by categories.
    * @param imageName The name of the icon that should be used if displaying
    *                  an icon together with the name, for example in a list.
    */
   SearchCategory(const WFString& name,
                  const WFString& id,
                  wf_int32 intId,
                  const WFString& imageName);

   /**
    * Destructor.
    */
   virtual ~SearchCategory();

   /**
    * Get the name of this category.
    *
    * @return The name of this category.
    */
   const WFString& getName() const;

   /**
    * Get the id of this category.
    *
    * @return The id of this category.
    */
   const WFString& getId() const;

   /**
    * Get the id of this category as an int.
    *
    * @return The id of this category.
    */
   wf_int32 getIntId() const;

   /**
    * Get the image name for this category.
    *
    * @return The image name for this category.
    */
   const WFString& getImageName() const;

   /**
    * Set the name of this category.
    *
    * @param name The name of this category.
    */
   void setName(const WFString& name);

   /**
    * Set the id of this category.
    *
    * @param id The id of this category.
    */
   void setId(const WFString& id);

   /**
    * Set the id of this category as an int.
    *
    * @param intId The id of this category.
    */
   void setIntId(wf_int32 intId);

   /**
    * Set the image name for this category.
    *
    * @param imageName The image name for this category.
    */
   void setImageName(const WFString& imageName);

private:
   
   /// Name of this category
   WFString m_name;

   /// The id for this category.
   WFString m_id;

   /// The ud for this category, as an int.
   wf_int32 m_intId;

   /// The image name for this category.
   WFString m_imageName;
};

}

#endif
