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
#include "Property/IProperty.h"
#include "Property/PropertyBag.h"

namespace gubi {

PropertyBag::PropertyBag(
   void )
{

}


PropertyBag::~PropertyBag(
   void )
{

}


bool PropertyBag::Find(
   const gubi::Identifier& ePropertyId,
   gubi::IProperty** ppProperty )
{
   bool bReturn              = false;
   property_map::iterator it = m_mapProperty.find(ePropertyId);

   if (ppProperty != NULL && it != m_mapProperty.end())
   {
      IProperty* pProperty = it->second;

      if (pProperty != NULL)
      {
         pProperty->AddReference();
      }

      *ppProperty = pProperty;
      bReturn     = true;
   }

   return bReturn;
}

bool PropertyBag::Find(
   const gubi::Identifier& ePropertyId,
   const gubi::IProperty** ppProperty ) const
{
   bool bReturn                    = false;
   property_map::const_iterator it = m_mapProperty.find(ePropertyId);

   if (ppProperty != NULL && it != m_mapProperty.end())
   {
      const IProperty* pProperty = it->second;

      if (pProperty != NULL)
      {
         pProperty->AddReference();
      }

      *ppProperty = pProperty;
      bReturn     = true;
   }

   return bReturn;
}


bool PropertyBag::GetNext(
   gubi::IProperty** ppProperty )
{
   bool bReturn = false;

   if (ppProperty != NULL && m_mapIterator != m_mapProperty.end())
   {
      IProperty* pProperty = m_mapIterator->second;

      if (pProperty != NULL)
      {
         pProperty->AddReference();
      }

      *ppProperty = pProperty;
      bReturn     = true;

      m_mapIterator++;
   }

   return bReturn;
}


bool PropertyBag::GetNext(
   const gubi::IProperty** ppProperty ) const
{
   bool bReturn = false;

   if (ppProperty != NULL && m_mapIterator != m_mapProperty.end())
   {
      IProperty* pProperty = m_mapIterator->second;

      if (pProperty != NULL)
      {
         pProperty->AddReference();
      }

      *ppProperty = pProperty;
      bReturn     = true;

      m_mapIterator++;
   }

   return bReturn;
}


void PropertyBag::Reset(
   void ) const
{
   m_mapIterator = m_mapProperty.begin();
}


bool PropertyBag::Insert(
   const gubi::Identifier& ePropertyId,
   gubi::IProperty* pProperty )
{
   bool bReturn              = false;
   property_map::iterator it = m_mapProperty.find(ePropertyId);

   if (it == m_mapProperty.end() && pProperty != NULL)
   {
      m_mapProperty.insert(std::make_pair(ePropertyId, pProperty));

      Reset();

      bReturn = true;
   }

   return bReturn;
}

unsigned int PropertyBag::AddReference(
   void ) const
{
   return InternalAddReference();
}


unsigned int PropertyBag::Release(
   void ) const
{
   return InternalRelease();
}

}
