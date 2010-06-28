/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "Core/Platform.h"
#include "Property/IPropertyBag.h"
#include "Property/IProperty.h"
#include "Property/PropertyIterator.h"

namespace gubi {

PropertyIterator::PropertyIterator(
   void )
{
   m_pPropertyBag = NULL;
   m_pProperty    = NULL;
   m_bEndOfBag    = true;
}


PropertyIterator::PropertyIterator(
   gubi::IPropertyBag* pPropertyBag )
{
   m_pPropertyBag = pPropertyBag;

   if (m_pPropertyBag != NULL)
   {
      m_pPropertyBag->AddReference();
      m_pPropertyBag->Reset();

      m_bEndOfBag = !m_pPropertyBag->GetNext(&m_pProperty);
   }
}


PropertyIterator::~PropertyIterator(
   void )
{
   if (m_pProperty != NULL)
   {
      m_pProperty->Release();
      m_pProperty = NULL;
   }

   if (m_pPropertyBag != NULL)
   {
      m_pPropertyBag->Release();
      m_pPropertyBag = NULL;
   }
}

PropertyIterator& PropertyIterator::operator=(
   gubi::IPropertyBag* pPropertyBag )
{
   // Save the old bag instance and set the new one
   gubi::IPropertyBag* pOldBag = m_pPropertyBag;        
   m_pPropertyBag              = pPropertyBag;

   // Release any saved property
   if (m_pProperty != NULL)
   {
      m_pProperty->Release();
      m_pProperty = NULL;
   }

   // Get the first new property
   if (m_pPropertyBag != NULL)
   {
      m_pPropertyBag->AddReference();
      m_pPropertyBag->Reset();

      m_bEndOfBag = !m_pPropertyBag->GetNext(&m_pProperty);
   }

   // Release the old bag instance
   if (pOldBag != NULL)
   {
      pOldBag->Release();
   }

   return *this;
}


gubi::IProperty* PropertyIterator::operator*(
   void )
{
   return m_pProperty;
}


PropertyIterator& PropertyIterator::operator++(
   void )
{
   if (m_pPropertyBag != NULL && !m_bEndOfBag)
   {
      if (m_pProperty != NULL)
      {
         m_pProperty->Release();
         m_pProperty = NULL;
      }

      m_bEndOfBag = !m_pPropertyBag->GetNext(&m_pProperty);
   }

   return *this;        
}


bool PropertyIterator::IsValid(
   void )
{
   return !m_bEndOfBag;
}

}
