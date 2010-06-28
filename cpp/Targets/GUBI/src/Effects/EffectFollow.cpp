/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "Effects/EffectFollow.h"
#include "Property/PropertyUtils.h"

namespace gubi {

IEffectInternal* EffectFollow::Create(
   gubi::IPropertyBag* pPropertyBag,
   unsigned int iTime )
{
   EffectFollow* pEffect = NULL;

   if (pPropertyBag != NULL)
   {
      int iTarget                = 0;
      unsigned char iSourceSpeed = 0xFF;
      unsigned char iTargetSpeed = 0x01;
      gubi::IProperty* pProperty = NULL;

      if (pPropertyBag->Find(L"target", &pProperty))
      {
         gubi::GetDataByType(pProperty, &iTarget);

         pProperty->Release();
      }

      if (pPropertyBag->Find(L"sourcespeed", &pProperty))
      {
         gubi::GetDataByType(pProperty, &iSourceSpeed);

         pProperty->Release();
      }

      if (pPropertyBag->Find(L"targetspeed", &pProperty))
      {
         gubi::GetDataByType(pProperty, &iTargetSpeed);

         pProperty->Release();
      }

      pEffect = new gubi::EffectFollow(iTarget, iSourceSpeed, iTargetSpeed);
   }

   return pEffect;
}


EffectFollow::EffectFollow(
   int iTarget,
   unsigned char iSourceSpeed,
   unsigned char iTargetSpeed )
{
   m_iValue       = 0;
   m_iTarget      = iTarget;
   m_iWaypoint    = 0;
   m_iSourceSpeed = iSourceSpeed;
   m_iTargetSpeed = iTargetSpeed;
}


EffectFollow::~EffectFollow(
   void )
{

}


void EffectFollow::Update(
   unsigned int iTime )
{
   m_iWaypoint += (int)(((long long)(m_iTarget - m_iWaypoint) *
      m_iTargetSpeed) >> 8);
   m_iValue    += (int)(((long long)(m_iWaypoint - m_iValue) *
      m_iSourceSpeed) >> 8);

   if (UpdateProperty(m_pPropertyBag, L"value", m_iValue))
   {
      InternalDispatchSignal(L"OnUpdate", m_pPropertyBag);
   }
}

}
