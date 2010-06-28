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
#include "Signal/SignalHub.h"
#include "Signal/SignalCollection.h"

namespace gubi {

SignalCollection::SignalCollection(
   void )
{

}

SignalCollection::SignalCollection(
   const gubi::Identifier* pSignalElements,
   unsigned int iSignalElements )
{
   for (unsigned int i = 0; i < iSignalElements; ++i)
   {
      InternalAddSignal(pSignalElements[i]);
   }
}

SignalCollection::~SignalCollection(
   void )
{

}

bool SignalCollection::InternalAddSignal(
   const gubi::Identifier& eSignalId )
{
   bool bReturn         = false;
   hub_map::iterator it = m_mapSignalHub.find(eSignalId);

   if (it == m_mapSignalHub.end())
   {
      ISignalHub* pSignalHub = new SignalHub;

      if (pSignalHub != NULL)
      {
         m_mapSignalHub.insert(std::make_pair(eSignalId, pSignalHub));

         bReturn = true;
      }
   }

   return bReturn;
}

bool SignalCollection::InternalRemoveSignal(
   const gubi::Identifier& eSignalId )
{
   bool bReturn         = false;
   hub_map::iterator it = m_mapSignalHub.find(eSignalId);

   if (it != m_mapSignalHub.end())
   {
      m_mapSignalHub.erase(it);

      bReturn = true;
   }

   return bReturn;
}

void SignalCollection::InternalDispatchSignal(
   const gubi::Identifier& eSignalId,
   gubi::IPropertyBag* pPropertyBag )
{
   hub_map::iterator it = m_mapSignalHub.find(eSignalId);

   if (it != m_mapSignalHub.end())
   {
      gubi::SmartPointer<gubi::ISignalHub> pSignalHub = it->second;

      if (!pSignalHub.IsEmpty())
      {
         pSignalHub->Dispatch(pPropertyBag);
      }
   }
}

bool SignalCollection::InternalGetSignal(
   const gubi::Identifier& eSignalId,
   gubi::ISignalHub** ppSignalHub )
{
   bool bReturn         = false;
   hub_map::iterator it = m_mapSignalHub.find(eSignalId);

   if (ppSignalHub != NULL && it != m_mapSignalHub.end())
   {
      gubi::SmartPointer<gubi::ISignalHub> pSignalHub = it->second;

      if (!pSignalHub.IsEmpty())
      {
         pSignalHub->AddReference();

         *ppSignalHub = pSignalHub;
         bReturn      = true;
      }
   }

   return bReturn;
}

}
