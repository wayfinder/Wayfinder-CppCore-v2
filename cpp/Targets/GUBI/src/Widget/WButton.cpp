/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "Widget/WButton.h"
#include "Property/PropertyUtils.h"
#include "Renderer/IImage.h"

namespace gubi {

WidgetConfiguration WButton::_config = {
   L"gubi.render.image"
};

const gubi::WidgetConfiguration& WButton::Config(
   void )
{
   return _config;
}


gubi::IWidgetInternal* WButton::Create(
   gubi::IWidgetDestructor* pDestructor,
   gubi::IRenderFunction* pRenderFunction,
   const gubi::IPropertyBag* pProperties )
{
   gubi::WButton* pWidget = NULL;

   if (pDestructor != NULL && pRenderFunction != NULL && pProperties != NULL)
   {
      gubi::IImage* pImage[2]          = {NULL, NULL};
      const gubi::IProperty* pProperty = NULL;

      if (pProperties->Find(L"image_up", &pProperty))
      {
         if (gubi::GetDataByType(pProperty, &pImage[0]))
         {
            pImage[0]->AddReference();
         }

         pProperty->Release();
      }

      if (pProperties->Find(L"image_down", &pProperty))
      {
         if (gubi::GetDataByType(pProperty, &pImage[1]))
         {
            pImage[1]->AddReference();
         }

         pProperty->Release();
      }

      pWidget = new WButton(pDestructor, pRenderFunction);

      if (pWidget != NULL)
      {
         gubi::InsertProperty(pWidget->m_propertyBag, L"image",      pImage[0]);
         gubi::InsertProperty(pWidget->m_propertyBag, L"image_up",   pImage[0]);
         gubi::InsertProperty(pWidget->m_propertyBag, L"image_down", pImage[1]);
      }
      else
      {
         if (pImage[0] != NULL)
         {
            pImage[0]->Release();
         }

         if (pImage[1] != NULL)
         {
            pImage[1]->Release();
         }
      }
   }

   return pWidget;
}


WButton::WButton(
   gubi::IWidgetDestructor* pDestructor,
   gubi::IRenderFunction* pRenderFunction ) :
WidgetImplementation(pDestructor, pRenderFunction)
{
   InternalAddSignal(L"OnClick");
}


WButton::~WButton(
   void )
{
   gubi::IProperty* pImageUp   = NULL;
   gubi::IProperty* pImageDown = NULL;   

   if (!m_propertyBag.IsEmpty())
   {
      if (m_propertyBag->Find(L"image_up", &pImageUp))
      {
         gubi::IImage* pImage = NULL;
         if (gubi::GetDataByType(pImageUp, &pImage))
         {
            pImage->Release();
         }
         
         pImageUp->Release();
      }

      if (m_propertyBag->Find(L"image_down", &pImageDown))
      {
         gubi::IImage* pImage = NULL;
         if (gubi::GetDataByType(pImageDown, &pImage))
         {
            pImage->Release();
         }

         pImageDown->Release();
      }      
   }   
}


bool WButton::OnInputEvent(
   const gubi::InputEventState& eState,
   int x,
   int y )
{
   bool bReturn = false;

   if (!m_propertyBag.IsEmpty())
   {   
      switch (eState)
      {
         case gubi::InputEventState_Down:
         {
            gubi::IImage* pImageDown   = NULL;
            gubi::IProperty* pProperty = NULL;
            if (m_propertyBag->Find(L"image_down", &pProperty))
            {
               if (gubi::GetDataByType(pProperty, &pImageDown))
               {
                  if (gubi::UpdateProperty(m_propertyBag, L"image", pImageDown))
                  {
                     bReturn = true;
                  }
               }

               pProperty->Release();
            }
         }
         break;

         case gubi::InputEventState_Up:
         case gubi::InputEventState_Click:
         {
            gubi::IImage* pImageDown   = NULL;
            gubi::IProperty* pProperty = NULL;
            if (m_propertyBag->Find(L"image_up", &pProperty))
            {
               if (gubi::GetDataByType(pProperty, &pImageDown))
               {
                  if (gubi::UpdateProperty(m_propertyBag, L"image", pImageDown))
                  {
                     bReturn = true;

                     InternalDispatchSignal(L"OnClick", NULL);
                  }
               }

               pProperty->Release();
            }
         }
         break;
      }
   }
   
   return bReturn;
}

}
