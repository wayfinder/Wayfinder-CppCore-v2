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
#include "Effects/EffectWave.h"
#include "Property/PropertyUtils.h"

#include "fmath.h"

namespace gubi {

gubi::IEffectInternal* EffectWave::Create(
   gubi::IPropertyBag* pPropertyBag,
   unsigned int iTime )
{
   EffectWave* pEffect = NULL;

   if (pPropertyBag != NULL)
   {
      unsigned int iPeriod       = 0;
      int iAmplitude             = 0;
      int iPhase                 = 0;
      gubi::IProperty* pProperty = NULL;

      if (pPropertyBag->Find(L"period", &pProperty))
      {
         gubi::GetDataByType(pProperty, &iPeriod);

         pProperty->Release();
      }

      if (pPropertyBag->Find(L"amplitude", &pProperty))
      {
         gubi::GetDataByType(pProperty, &iAmplitude);

         pProperty->Release();
      }        

      if (pPropertyBag->Find(L"phase", &pProperty))
      {
         gubi::GetDataByType(pProperty, &iPhase);

         pProperty->Release();
      }

      if (iPeriod > 0)
      {
         pEffect = new gubi::EffectWave(iPeriod,
                                        iAmplitude,
                                        iPhase);
      }
   }

   return pEffect;
}

EffectWave::EffectWave(
   unsigned int iPeriod,
   int iAmplitude,
   int iPhase )
{
   m_bHasStartTime = false;
   m_iStartTime    = 0;
   m_iPeriod       = iPeriod;
   m_iAmplitude    = iAmplitude;
   m_iPhase        = iPhase;
}

EffectWave::~EffectWave(
   void )
{

}

void EffectWave::Update(
   unsigned int iTime )
{
   if (m_iPeriod != 0)
   {
      if (!m_bHasStartTime)
      {
         m_iStartTime    = iTime;
         m_bHasStartTime = true;
         printf("%d\n", m_iStartTime);
      }

      int iValue = (int)(((((long long)m_iAmplitude *
         fmath::sin(m_iPhase +
         ((((long long)iTime - m_iStartTime) << 1) *
         fpi / m_iPeriod))) << 1) + (1 << 16)) >> 17);

      if (UpdateProperty(m_pPropertyBag, L"value", iValue))
      {
         InternalDispatchSignal(L"OnUpdate", m_pPropertyBag);
      }
   }
}

}
