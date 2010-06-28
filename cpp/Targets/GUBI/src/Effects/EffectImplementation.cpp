/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "Effects/EffectImplementation.h"
#include "Property/PropertyBag.h"
#include "Property/PropertyUtils.h"

namespace gubi {

static const gubi::Identifier g_vEffectSignal[] = {
   L"OnStart",
   L"OnUpdate",
   L"OnFinish",
   L"OnPause",
   L"OnContinue",
   L"OnTerminate"
};

EffectImplementation::EffectImplementation(
   void ) : gubi::SignalCollection(g_vEffectSignal,
   (sizeof(g_vEffectSignal) / sizeof(gubi::Identifier))),
   m_pPropertyBag(new gubi::PropertyBag)
{
   m_eEffectState = gubi::EffectState_Unknown;

   if (!m_pPropertyBag.IsEmpty())
   {
      InsertProperty(m_pPropertyBag, L"value", 0);
   }
}

EffectImplementation::~EffectImplementation(
   void )
{

}


unsigned int EffectImplementation::AddReference(
   void ) const
{
   return InternalAddReference();
}


unsigned int EffectImplementation::Release(
   void ) const
{
   return InternalRelease();
}


bool EffectImplementation::GetSignal(
   const gubi::Identifier& eSignalId,
   gubi::ISignalHub** ppSignalHub )
{
   return InternalGetSignal(eSignalId, ppSignalHub);
}


bool EffectImplementation::Start(
   void )
{
   bool bReturn = false;

   if (m_eEffectState == gubi::EffectState_Unknown)
   {
      SetState(gubi::EffectState_Idle);

      bReturn = true;
   }

   return bReturn;
}


void EffectImplementation::Stop(
   void )
{
   InternalDispatchSignal(L"OnTerminate", m_pPropertyBag);

   SetState(gubi::EffectState_Remove);
}


bool EffectImplementation::Pause(
   void )
{
   bool bReturn = false;

   switch (GetState())
   {
      case gubi::EffectState_Run:
      {
         SetState(gubi::EffectState_Paused);

         bReturn = true;

         InternalDispatchSignal(L"OnPause", m_pPropertyBag);
      }
      break;

      default:
      {
         // Nop
      }
      break;
   }

   return bReturn;
}


bool EffectImplementation::Continue(
   void )
{
   bool bReturn = false;

   switch (GetState())
   {
      case gubi::EffectState_Paused:
      {
         SetState(gubi::EffectState_Run);

         bReturn = true;

         InternalDispatchSignal(L"OnContinue", m_pPropertyBag);
      }
      break;

      default:
      {
         // Nop
      }
      break;
   }

   return bReturn;
}


void EffectImplementation::SetState(
   const gubi::EffectState& eState )
{
   if (eState == gubi::EffectState_Start &&
       m_eEffectState == gubi::EffectState_Idle)
   {
      InternalDispatchSignal(L"OnStart", m_pPropertyBag);

      m_eEffectState = gubi::EffectState_Run;
   }
   else if(eState == gubi::EffectState_Finish)
   {
      InternalDispatchSignal(L"OnFinish", m_pPropertyBag);

      Stop();
   }
   else
   {
      m_eEffectState = eState;
   }
}

gubi::EffectState EffectImplementation::GetState(
   void )
{
   return m_eEffectState;
}

unsigned int EffectImplementation::GetStartTime(
   void )
{
   return 0;
}

}
