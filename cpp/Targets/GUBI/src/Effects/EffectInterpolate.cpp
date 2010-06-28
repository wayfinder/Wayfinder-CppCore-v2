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
#include "Effects/EffectInterpolate.h"
#include "Property/PropertyUtils.h"

#include "fmath.h"

namespace gubi {

IEffectInternal* EffectInterpolate::Create(
   gubi::IPropertyBag* pPropertyBag,
   unsigned int iTime )
{
   EffectInterpolate* pEffect = NULL;

   if (pPropertyBag != NULL)
   {
      gubi::IProperty* pProperty              = NULL;
      int iStartValue                         = 0;
      int iEndValue                           = 0;
      unsigned int iDuration                  = 0;
      unsigned int iDelay                     = 0;
      gubi::EffectInterpolatePace eEffectPace =
         gubi::EffectInterpolatePace_Constant;      

      if (pPropertyBag->Find(L"startvalue", &pProperty))
      {
         gubi::GetDataByType(pProperty, &iStartValue);

         pProperty->Release();
      }

      if (pPropertyBag->Find(L"endvalue", &pProperty))
      {
         gubi::GetDataByType(pProperty, &iEndValue);

         pProperty->Release();
      }

      if (pPropertyBag->Find(L"duration", &pProperty))
      {
         gubi::GetDataByType(pProperty, &iDuration);

         pProperty->Release();
      }

      if (pPropertyBag->Find(L"delay", &pProperty))
      {
         gubi::GetDataByType(pProperty, &iDelay);

         pProperty->Release();
      }

      if (pPropertyBag->Find(L"pace", &pProperty))
      {
         gubi::GetDataByType(pProperty, &eEffectPace);

         pProperty->Release();
      }

      pEffect = new gubi::EffectInterpolate(iStartValue,
                                            iEndValue,
                                            iDuration,
                                            iTime + iDelay,
                                            iTime + iDelay + iDuration,
                                            eEffectPace);
   }

   return pEffect;
}

EffectInterpolate::EffectInterpolate(
   int iStartValue,
   int iEndValue,
   unsigned int iDuration,
   unsigned int iStartTime,
   unsigned int iEndTime,
   gubi::EffectInterpolatePace eEffectPace )
{
   m_iStartValue = iStartValue;
   m_iEndValue   = iEndValue;
   m_iDuration   = iDuration;
   m_iStartTime  = iStartTime;
   m_iEndTime    = iEndTime;
   m_eEffectPace = eEffectPace;
}

EffectInterpolate::~EffectInterpolate(
   void )
{

}

void EffectInterpolate::Update(
   unsigned int iTime )
{
   if (!m_pPropertyBag.IsEmpty())
   {
      // Prevent possible division by zero.
      int iInterpolationPeriod = m_iEndTime - m_iStartTime;

      if (iInterpolationPeriod == 0)
      {
         UpdateProperty(m_pPropertyBag,
            L"value",
            Interpolate(1 << 16));
      }
      else
      {
         UpdateProperty(m_pPropertyBag,
            L"value",
            Interpolate(((iTime - m_iStartTime) << 16) /
            iInterpolationPeriod));
      }

      InternalDispatchSignal(L"OnUpdate", m_pPropertyBag);

      if (iTime >= m_iEndTime)
      {
         SetState(gubi::EffectState_Finish);
      }
   }
}

unsigned int EffectInterpolate::GetStartTime(
   void )
{
   return m_iStartTime;
}

int EffectInterpolate::Interpolate(
   unsigned int iDeltaTime )
{
   int iInterpolation = 0;

   switch (m_eEffectPace)
   {
      case gubi::EffectInterpolatePace_AcceleratingPow2:
      {
         iDeltaTime = fmath::multiply(iDeltaTime, iDeltaTime);
      }
      break;

      case gubi::EffectInterpolatePace_AcceleratingPow4:
      {
         int iTime  = fmath::multiply(iDeltaTime, iDeltaTime);
         iDeltaTime = fmath::multiply(iTime, iTime);
      }
      break;

      case gubi::EffectInterpolatePace_DeceleratingPow2:
      {
         int iTime  = fmath::static_int2fix<1>::value -
            iDeltaTime;
         iDeltaTime = fmath::static_int2fix<1>::value -
            fmath::multiply(iTime, iTime);
      }
      break;

      case gubi::EffectInterpolatePace_DeceleratingPow4:
      {
         int iTime  = fmath::static_int2fix<1>::value -
            iDeltaTime;
         iTime      = fmath::multiply(iTime, iTime);
         iDeltaTime = fmath::static_int2fix<1>::value -
            fmath::multiply(iTime, iTime);
      }
      break;
   }

   iInterpolation = (int)((long long)m_iStartValue +
                         (((((long long)iDeltaTime *
                         (m_iEndValue - m_iStartValue)) >>
                         (fix_precision - 1)) + 1) >> 1));

   return iInterpolation;
}

}
