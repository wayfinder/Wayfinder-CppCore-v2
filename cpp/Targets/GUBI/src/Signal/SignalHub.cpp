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
#include "Signal/ISignal.h"
#include "Signal/SignalHub.h"

#define SIGNAL_TO_CONNECTIONID(_s_) (gubi::Identifier((unsigned int)(_s_)))

namespace gubi {

SignalHub::SignalHub(
   void )
{

}

SignalHub::~SignalHub(
   void )
{
   for (signal_map::iterator it = m_mapSignal.begin();
      it != m_mapSignal.end();
      it++)
   {
      gubi::ISignal* pSignal = it->second;

      if (pSignal != NULL)
      {
         pSignal->Release();
      }
   }
}

unsigned int SignalHub::AddReference(
   void ) const
{
   return InternalAddReference();
}

unsigned int SignalHub::Release(
   void ) const
{
   return InternalRelease();
}

bool SignalHub::Connect(
   gubi::ISignal* pSignal,
   gubi::Identifier* pIdentifier )
{
   bool bReturn = false;

   if (pSignal != NULL)
   {
      gubi::Identifier eConnectionId = SIGNAL_TO_CONNECTIONID(pSignal);
      signal_map::iterator it        = m_mapSignal.find(eConnectionId);

      if (it == m_mapSignal.end())
      {
         pSignal->AddReference();

         m_mapSignal.insert(std::make_pair(eConnectionId, pSignal));

         if (pIdentifier != NULL)
         {
            *pIdentifier = eConnectionId;
         }

         bReturn = true;
      }
   }

   return bReturn;
}

bool SignalHub::Disconnect(
   gubi::Identifier eIdentifier )
{
   bool bReturn            = false;
   signal_map::iterator it = m_mapSignal.find(eIdentifier);

   if (it != m_mapSignal.end())
   {
      gubi::ISignal* pSignal = it->second;

      m_mapSignal.erase(it);

      if (pSignal != NULL)
      {
         pSignal->Release();
      }

      bReturn = true;
   }

   return bReturn;
}

void SignalHub::Dispatch(
   gubi::IPropertyBag* pPropertyBag )
{
   for (signal_map::iterator it = m_mapSignal.begin();
      it != m_mapSignal.end();
      it++)
   {
      gubi::ISignal* pSignal = it->second;

      if (pSignal != NULL)
      {
         pSignal->Dispatch(pPropertyBag);
      }
   }
}

}
