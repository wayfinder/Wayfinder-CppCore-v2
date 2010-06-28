/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "Input/InputIndexState.h"

namespace gubi {

#define INPUT_CLICK_TIME   (70)

InputIndexState::InputIndexState(
   void )
{
   m_eState    = gubi::InputEventState_Invalid;
   m_x         = 0;
   m_y         = 0;
   m_iDownTime = 0;
}


void InputIndexState::PushEvent(
   const gubi::InputEventState& eState,
   int x,
   int y,
   unsigned int iTime )
{
   switch (eState)
   {
      case gubi::InputEventState_Move:
      {
         if (m_iDownTime != 0)
         {
            m_x      = x;
            m_y      = y;
            m_eState = gubi::InputEventState_Move;
         }
      }
      break;

      case gubi::InputEventState_Down:
      {
         m_x         = x;
         m_y         = y;
         m_iDownTime = iTime;
         m_eState    = gubi::InputEventState_Down;
      }
      break;

      case gubi::InputEventState_Up:
      {
         m_x         = x;
         m_y         = y;
         m_iDownTime = 0;
         m_eState    = (iTime - m_iDownTime) < INPUT_CLICK_TIME ?
            gubi::InputEventState_Click : gubi::InputEventState_Up;
      }
      break;
   }
}


bool InputIndexState::TranslateEvent(
   gubi::InputEventState& eState,
   int& x,
   int& y )
{
   bool bReturn = false;

   if (m_eState != gubi::InputEventState_Invalid &&
       (m_eState == gubi::InputEventState_Up || m_iDownTime != 0))
   {
      bReturn = true;
      eState  = m_eState;
      x       = m_x;
      y       = m_y;
   }

   return bReturn;
}

}
