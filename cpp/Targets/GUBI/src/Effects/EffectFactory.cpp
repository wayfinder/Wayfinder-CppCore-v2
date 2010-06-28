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
#include "Effects/IEffectInternal.h"
#include "Effects/EffectFactory.h"

namespace gubi {

EffectFactory::EffectFactory(
   void )
{

}

EffectFactory::~EffectFactory(
   void )
{

}


bool EffectFactory::RegisterEffect(
   const gubi::Identifier& eEffectId,
   EffectCreator pCreator )
{
   bool bReturn             = false;
   creator_map::iterator it = m_mapCreators.find(eEffectId);

   if (pCreator != NULL && it == m_mapCreators.end())
   {
      m_mapCreators.insert(std::make_pair(eEffectId, pCreator));

      bReturn = true;
   }

   return bReturn;
}


bool EffectFactory::UnregisterEffect(
   const gubi::Identifier& eEffectId )
{
   bool bReturn             = false;
   creator_map::iterator it = m_mapCreators.find(eEffectId);

   if (it != m_mapCreators.end())
   {
      m_mapCreators.erase(it);

      bReturn = true;
   }

   return bReturn;
}


bool EffectFactory::CreateEffect(
   unsigned int iTime,
   const gubi::Identifier& eEffectId,
   gubi::IPropertyBag* pPropertyBag,
   gubi::IEffectInternal** ppEffect )
{
   bool bReturn = false;

   if (pPropertyBag != NULL && ppEffect != NULL)
   {
      creator_map::iterator it = m_mapCreators.find(eEffectId);

      if (it != m_mapCreators.end())
      {
         EffectCreator pCreator = it->second;

         if (pCreator != NULL)
         {
            gubi::IEffectInternal* pEffect = pCreator(pPropertyBag, iTime);

            if (pEffect != NULL)
            {
               pEffect->AddReference();

               *ppEffect = pEffect;
               bReturn   = true;
            }
         }
      }
   }

   return bReturn;
}

}
