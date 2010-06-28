/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "Common/ITime.h"
#include "Input/IInputListener.h"
#include "Input/InputManager.h"

namespace gubi {

InputManager::InputManager(
   gubi::IInputListener* pListener,
   gubi::ITime* pTime ) :
m_pListener(pListener),
m_pTime(pTime)
{

}


InputManager::~InputManager(
   void )
{

}


void InputManager::OnPointerDown(
   unsigned int iIndex,
   int x,
   int y )
{
   if (m_pTime != NULL)
   {
      m_vEvents.push_back(gubi::InputEvent(gubi::InputEventState_Down,
                                           iIndex,
                                           x,
                                           y,
                                           m_pTime->GetTime()));

   }
}


void InputManager::OnPointerMove(
   unsigned int iIndex,
   int x,
   int y )
{
   if (m_pTime != NULL)
   {
      m_vEvents.push_back(gubi::InputEvent(gubi::InputEventState_Move,
                                           iIndex,
                                           x,
                                           y,
                                           m_pTime->GetTime()));
   }
}


void InputManager::OnPointerUp(
   unsigned int iIndex,
   int x,
   int y )
{
   if (m_pTime != NULL)
   {
      m_vEvents.push_back(gubi::InputEvent(gubi::InputEventState_Up,
                                           iIndex,
                                           x,
                                           y,
                                           m_pTime->GetTime()));
   }
}


void InputManager::Update(
   void )
{
   if (m_pListener != NULL)
   {   
      std::vector<gubi::InputEvent>::iterator it = m_vEvents.begin();

      while (it != m_vEvents.end())
      {
         gubi::InputIndexState* pState = GetIndexState((*it).getIndex());

         if (pState != NULL)
         {
            pState->PushEvent((*it).getState(),
                              (*it).getX(),
                              (*it).getY(),
                              (*it).getTime());

            gubi::InputEventState eState = gubi::InputEventState_Invalid;
            int x                        = 0;
            int y                        = 0;
            
            if (pState->TranslateEvent(eState, x, y))
            {
               m_pListener->OnInputEvent(eState, x, y);
            }
         }

         it = m_vEvents.erase(it);
      }
   }
   else
   {
      m_vEvents.clear();
   }
}


gubi::InputIndexState* InputManager::GetIndexState(
   unsigned int iIndex )
{
   gubi::InputIndexState* pState = NULL;
   std::map<unsigned int,gubi::InputIndexState>::iterator it =
      m_mapState.find(iIndex);

   if (it != m_mapState.end())
   {
      pState = &it->second;
   }
   else
   {
      m_mapState.insert(std::make_pair(iIndex, InputIndexState()));
      
      it = m_mapState.find(iIndex);

      pState = &it->second;
   }
   
   return pState;
}

}
