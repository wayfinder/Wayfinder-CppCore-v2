/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "Effects/EffectPeriodic.h"
#include "Property/PropertyUtils.h"

namespace gubi {

gubi::IEffectInternal* EffectPeriodic::Create(
   gubi::IPropertyBag* pPropertyBag,
   unsigned int iTime )
{
   EffectPeriodic* pEffect = NULL;

   if (pPropertyBag != NULL)
   {
      unsigned int iPeriod       = 0;
      unsigned int iDelay        = 0;
      gubi::IProperty* pProperty = NULL;

      if (pPropertyBag->Find(L"period", &pProperty))
      {
         gubi::GetDataByType(pProperty, &iPeriod);

         pProperty->Release();
      }

      if (pPropertyBag->Find(L"delay", &pProperty))
      {
         gubi::GetDataByType(pProperty, &iDelay);

         pProperty->Release();
      }

      if (iPeriod != 0)
      {
         pEffect = new EffectPeriodic(iTime + iDelay + iPeriod, iPeriod);
      }
   }

   return pEffect;
}

EffectPeriodic::EffectPeriodic(
   unsigned int iNextUpdate,
   unsigned int iPeriod )
{
   m_iNextUpdate = iNextUpdate;
   m_iPeriod     = iPeriod;
}

EffectPeriodic::~EffectPeriodic(
   void )
{

}

void EffectPeriodic::Update(
   unsigned int iTime )
{
   if (m_iPeriod == 0)
   {
      SetState(gubi::EffectState_Finish);
   }
   else if (iTime >= m_iNextUpdate)
   {
      do
      {
         m_iNextUpdate += m_iPeriod;
      }
      while (m_iNextUpdate <= iTime);

      InternalDispatchSignal(L"OnUpdate", m_pPropertyBag);
   }
}

}
