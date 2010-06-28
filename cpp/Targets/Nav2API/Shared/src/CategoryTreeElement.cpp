/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "CategoryTreeElement.h"
#include <algorithm>

namespace WFAPI {

CategoryTreeElement::CategoryTreeElement() :
   m_categoryID(-1), m_name(NULL), m_imageName(NULL)
{
}

CategoryTreeElement::CategoryTreeElement(wf_int32 categoryId,
                                         const WFString& name,
                                         const WFString& imageName)
   : m_categoryID(categoryId),
     m_name(name),
     m_imageName(imageName)
{
}

CategoryTreeElement::CategoryTreeElement(const CategoryTreeElement& other) :
   m_categoryID(other.m_categoryID),
   m_name(other.m_name),
   m_imageName(other.m_imageName)
{
}

CategoryTreeElement::~CategoryTreeElement()
{
}

wf_int32
CategoryTreeElement::getCategoryID() const
{
   return m_categoryID;    
}

const WFString&
CategoryTreeElement::getName() const
{
   return m_name;
}

const WFString&
CategoryTreeElement::getImageName() const
{
   return m_imageName;
}

CategoryTreeElement&
CategoryTreeElement::operator=(const CategoryTreeElement& other)
{
   if (this == &other) {
      return *this;
   }
   CategoryTreeElement tmp(other);
   tmp.swap(*this);
   return *this;
}

void CategoryTreeElement::swap(CategoryTreeElement& other)
{
   std::swap(m_categoryID, other.m_categoryID);
   std::swap(m_name, other.m_name);
   std::swap(m_imageName, other.m_imageName);
}

} // End namespace WFAPI

