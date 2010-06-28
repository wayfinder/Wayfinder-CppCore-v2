/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include <algorithm>
#include "Core/Platform.h"
#include "Widget/IWidgetInternal.h"
#include "Widget/WidgetManager.h"

namespace gubi {


WidgetManager::WidgetManager(
   gubi::IRenderSystem* pRenderSystem ) : 
m_widgetFactory(pRenderSystem)
{

}

WidgetManager::~WidgetManager(
   void )
{

}

bool WidgetManager::RegisterWidget(
   const gubi::Identifier& eWidgetId,
   WidgetConfig pWidgetConfig,
   WidgetCreator pWidgetCreator )
{
   return m_widgetFactory.RegisterWidget(eWidgetId,
                                         pWidgetConfig,
                                         pWidgetCreator);
}

bool WidgetManager::UnregisterWidget(
   const gubi::Identifier& eWidgetId )
{
   return m_widgetFactory.UnregisterWidget(eWidgetId);
}

bool WidgetManager::CreateWidget(
   const gubi::Identifier& eWidgetId,
   const gubi::IPropertyBag* pProperties,
   IWidget** ppWidget )
{
   bool bReturn                   = false;
   gubi::IWidgetInternal* pWidget = NULL;

   if (ppWidget != NULL)
   {   
      if (m_widgetFactory.CreateWidget(this,
                                       eWidgetId,
                                       pProperties,
                                       &pWidget))
      {
         m_lWidget.push_back(pWidget);

         *ppWidget = static_cast<gubi::IWidget*>(pWidget);
         bReturn   = true;
      }
   }
   
   return bReturn;
}


void WidgetManager::OnWidgetDestroy(
   const gubi::IWidgetInternal* pWidget )
{
   std::remove(m_lWidget.begin(), m_lWidget.end(), pWidget);
}


void WidgetManager::OnInputEvent(
   const gubi::InputEventState& eState,
   int x,
   int y )
{
   std::list<gubi::SmartPointer<gubi::IWidgetInternal> >::iterator it =
      m_lWidget.begin();

   while (it != m_lWidget.end())
   {
      gubi::SmartPointer<gubi::IWidgetInternal> pWidget = (*it);

      if (!pWidget.IsEmpty())
      {
         if (pWidget->OnInputEvent(eState, x, y))
         {
            it = m_lWidget.end();
         }
         else
         {
            it++;
         }
      }
      else
      {
         it++;
      }
   }
}

}
