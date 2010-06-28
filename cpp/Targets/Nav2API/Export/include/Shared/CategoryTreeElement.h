/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_CATEGORY_TREE_ELEMENT_H
#define WFAPI_CATEGORY_TREE_ELEMENT_H

#include "WFAPIConfig.h"
#include "WFString.h"

namespace WFAPI {

/**
 * A category tree element / node / category.
 */
class CategoryTreeElement
{
public:

   /** 
    * Default constructor
    */
   CategoryTreeElement();
   
   /** 
    * Constructor.
    * 
    * @param id The unique id of this catgory, provided by server.
    * @param name The name of this category.
    * @param imageName The name of the image for this category.
    */
   CategoryTreeElement(wf_int32 categoryId,
                       const WFString& name,
                       const WFString& imageName);

   /** 
    * Copy constructor. Copies members from other to
    * own members.
    * 
    * @param other The CategoryTreeElement object that we should copy. 
    */
   CategoryTreeElement(const CategoryTreeElement& other);

   /**
    * Destructor.
    */
   virtual ~CategoryTreeElement();

   /**
    * Get the categoryID for this category.
    * 
    * @return The categoryID this category.
    */
   wf_int32 getCategoryID() const;
   
   /**
    * Get the name of this category.
    * 
    * @return The name of this category.
    */
   const WFString& getName() const;

   /**
    * Get the image name to use for this category.
    * 
    * @return The image name to use for this category.
    */
   const WFString& getImageName() const;

   /** 
    * Assignment operator, assigns the values from other
    * to this.
    *
    * @param other The CategoryTreeElement to assign the data from.
    * 
    * @return A reference to the new assigned member.
    */
   CategoryTreeElement& operator= (const CategoryTreeElement& other);
   
private:
   /** 
    * Internal swap function, used for exception safe assigment
    * in assigment operator.
    * 
    * @param other The object to swap values from.
    */
   void swap(CategoryTreeElement& other);
   
private:
   
   /// The category id.
   wf_int32 m_categoryID;
   
   /// The name of this category.
   WFString m_name;

   /// The image name for this category.
   WFString m_imageName;
};

} // End namespace WFAPI

#endif // End WFAPI_CATEGORY_TREE_ELEMENT_H


