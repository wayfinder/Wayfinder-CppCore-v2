/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "Renderer/IRenderSystem.h"
#include "Renderer/IRenderFunction.h"
#include "Widget/IWidget.h"
#include "Widget/WidgetFactory.h"

#include "Widget/WImage.h"
#include "Widget/WButton.h"

namespace gubi {

struct _static_widget 
{
   gubi::Identifier eWidgetId;
   WidgetConfig     pConfig;
   WidgetCreator    pCreator;
};

static const _static_widget g_vStaticWidget[] = {
   // Widget id          WidgetConfig               WidgetCreator
   {L"gubi.ui.image",    &gubi::WImage::Config,     &gubi::WImage::Create  },
   {L"gubi.ui.button",   &gubi::WButton::Config,    &gubi::WButton::Create },
};

WidgetFactory::WidgetFactory(
   gubi::IRenderSystem* pRenderSystem )
{
   m_pRenderSystem = pRenderSystem;

   for (unsigned int i = 0;
        i < (sizeof(g_vStaticWidget) / sizeof(_static_widget));
        ++i)
   {
      RegisterWidget(g_vStaticWidget[i].eWidgetId,
                     g_vStaticWidget[i].pConfig,
                     g_vStaticWidget[i].pCreator);
   }
}

WidgetFactory::~WidgetFactory(
   void )
{
   m_pRenderSystem = NULL;
}

bool WidgetFactory::RegisterWidget(
   const gubi::Identifier& eWidgetId,
   WidgetConfig pWidgetConfig,
   WidgetCreator pWidgetCreator )
{
   bool bReturn = false;

   if (pWidgetConfig != NULL && pWidgetCreator != NULL)
   {
      widget_map::iterator it = m_mapWidget.find(eWidgetId);

      if (it == m_mapWidget.end())
      {
         m_mapWidget.insert(std::make_pair(eWidgetId,
                            std::make_pair(pWidgetConfig, pWidgetCreator)));

         bReturn = true;
      }
   }
   
   return bReturn;
}

bool WidgetFactory::UnregisterWidget(
   const gubi::Identifier& eWidgetId )
{
   bool bReturn = false;
   widget_map::iterator it = m_mapWidget.find(eWidgetId);

   if (it != m_mapWidget.end())
   {
      m_mapWidget.erase(it);
      
      bReturn = true;
   }
   
   return bReturn;
}

bool WidgetFactory::CreateWidget(
   gubi::IWidgetDestructor* pDestructor,
   const gubi::Identifier& eWidgetId,
   const gubi::IPropertyBag* pProperties,
   gubi::IWidgetInternal** ppWidget )
{
   bool bReturn = false;

   if (pDestructor != NULL && ppWidget != NULL && m_pRenderSystem != NULL)
   {
      widget_map::iterator it = m_mapWidget.find(eWidgetId);

      if (it != m_mapWidget.end())
      {
         std::pair<WidgetConfig,WidgetCreator>& pairWidget = it->second;
         WidgetConfig pConfig                              = pairWidget.first;
         WidgetCreator pCreator                            = pairWidget.second;

         if (pConfig != NULL && pCreator != NULL)
         {
            IRenderFunction* pRenderFunction        = NULL;
            const WidgetConfiguration& widgetConfig = pConfig();

            if (m_pRenderSystem->GetRenderFunction(widgetConfig.eRenderFunction,
                                                   &pRenderFunction))
            {
               gubi::IWidgetInternal* pWidget = pCreator(pDestructor,
                                                         pRenderFunction,
                                                         pProperties);

               if (pWidget != NULL)
               {
                  pWidget->AddReference();

                  *ppWidget = pWidget;
                  bReturn   = true;
               }
               else
               {
                  pRenderFunction->Release();
               }
            }
         }
      }
   }
   
   return bReturn;
}

}
