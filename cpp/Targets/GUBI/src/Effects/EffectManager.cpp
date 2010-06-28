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
#include "Common/ITime.h"
#include "Effects/EffectManager.h"
#include "Effects/EffectInterpolate.h"
#include "Effects/EffectWave.h"
#include "Effects/EffectPeriodic.h"
#include "Effects/EffectFollow.h"

namespace gubi {

#define INTERNAL_EFFECT_FPS     (50)
#define INTERNAL_UPDATE_PERIOD  (1000/INTERNAL_EFFECT_FPS)
#define INTERNAL_THRESHOLD      (500)

struct _static_effect 
{
   gubi::Identifier eEffectId;
   EffectCreator    pCreator;
};

static const _static_effect g_vStaticEffect[] = {
   //  Effect id           Effect creator 
   {L"interpolate",    &EffectInterpolate::Create  },
   {L"wave",           &EffectWave::Create         },
   {L"periodic",       &EffectPeriodic::Create     },
   {L"follow",         &EffectFollow::Create       },
};

EffectManager::EffectManager(
   gubi::ITime* pTime ) :
m_pTime(pTime)
{
   // Register the default effects
   for (unsigned int i = 0;
      i < (sizeof(g_vStaticEffect)/sizeof(_static_effect));
      ++i)
   {
      m_EffectFactory.RegisterEffect(g_vStaticEffect[i].eEffectId,
         g_vStaticEffect[i].pCreator);
   }

   if (!m_pTime.IsEmpty())
   {
      m_iEffectTime = m_pTime->GetTime();
      m_iTimeShift  = 0;
   }
}


EffectManager::~EffectManager(
   void )
{

}


bool EffectManager::CreateEffect(
   const gubi::Identifier& eEffectType,
   gubi::IPropertyBag* pPropertyBag,
   const gubi::EffectStartOption& eStartOption,
   gubi::IEffect** ppEffect )
{
   bool bReturn = false;

   if (pPropertyBag != NULL && !m_pTime.IsEmpty())
   {
      IEffectInternal *pEffect = NULL;

      if (m_EffectFactory.CreateEffect(m_pTime->GetTime(),
                                       eEffectType,
                                       pPropertyBag,
                                       &pEffect))
      {
         switch (eStartOption)
         {
            case gubi::EffectStartOption_Stopped:
            {
               // Nop
            }
            break;

            default:
            {
               //pEffect->SetState(gubi::EffectState_Idle);

               pEffect->Start();
            }
            break;
         }

         m_lEffects.push_back(pEffect);

         if (ppEffect != NULL)
         {
            *ppEffect = (gubi::IEffect*)pEffect;
         }

         bReturn = true;
      }
   }

   return bReturn;
}

bool EffectManager::Update(
   void )
{
   bool bReturn = false;

   if (!m_pTime.IsEmpty())
   {
      int iRealTime = m_pTime->GetTime() - m_iTimeShift;

      if (m_iEffectTime < (iRealTime - INTERNAL_THRESHOLD))
      {
         SyncTime();

         iRealTime = m_pTime->GetTime() - m_iTimeShift;
      }

      while (m_iEffectTime <= iRealTime)
      {
         effect_list::iterator it = m_lEffects.begin();

         while (it != m_lEffects.end())
         {
            IEffectInternal* pEffect = (*it);

            if (pEffect != NULL)
            {
               switch (pEffect->GetState())
               {
                  case gubi::EffectState_Remove:
                  {
                     pEffect->Release();

                     it = m_lEffects.erase(it);
                  }
                  break;

                  case gubi::EffectState_Unknown:
                  case gubi::EffectState_Paused:
                  {
                     it++;
                  }
                  break;

                  default:
                  {
                     bReturn = true;

                     UpdateEffect(pEffect, m_iEffectTime);

                     it++;
                  }
                  break;
               }
            }
            else
            {
               // Shouldn't happen, but we do this for safety
               it = m_lEffects.erase(it);
            }
         }

         m_iEffectTime += INTERNAL_UPDATE_PERIOD;
      }
   }

   return bReturn;
}

void EffectManager::UpdateEffect(
   gubi::IEffectInternal* pEffect,
   unsigned int iTime )
{
   if (pEffect != NULL)
   {
      if (iTime >= pEffect->GetStartTime())
      {
         if (pEffect->GetState() == gubi::EffectState_Idle)
         {
            pEffect->SetState(gubi::EffectState_Start);
         }

         pEffect->Update(iTime);
      }

      if (pEffect->GetState() == gubi::EffectState_Finish)
      {
         pEffect->Stop();
      }
   }
}


void EffectManager::SyncTime(
   void )
{
   if (!m_pTime.IsEmpty())
   {
      m_iTimeShift = m_pTime->GetTime() - m_iEffectTime;
   }
}

}
