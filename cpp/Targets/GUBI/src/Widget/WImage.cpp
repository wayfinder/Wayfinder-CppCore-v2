/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "Widget/WImage.h"
#include "Property/PropertyUtils.h"
#include "Renderer/IImage.h"

namespace gubi {

WidgetConfiguration WImage::_config = {
   L"gubi.render.image"
};

const WidgetConfiguration& WImage::Config(
   void )
{
   return _config;
}

IWidgetInternal* WImage::Create(
   gubi::IWidgetDestructor* pDestructor,
   gubi::IRenderFunction* pRenderFunction,
   const gubi::IPropertyBag* pProperties )
{
   gubi::WImage* pWidget = NULL;

   if (pDestructor != NULL && pRenderFunction != NULL && pProperties != NULL)
   {
      gubi::IImage* pImage             = NULL;
      const gubi::IProperty* pProperty = NULL;

      if (pProperties->Find(L"image", &pProperty))
      {
         if (gubi::GetDataByType(pProperty, &pImage))
         {
            pImage->AddReference();
         }

         pProperty->Release();
      }
      
      pWidget = new WImage(pDestructor, pRenderFunction);

      if (pWidget != NULL)
      {
         gubi::InsertProperty(pWidget->m_propertyBag, L"image", pImage);
      }
      else
      {
         if (pImage != NULL)
         {
            pImage->Release();
         }
      }
   }
   
   return pWidget;
}

WImage::WImage(
   gubi::IWidgetDestructor* pDestructor,
   gubi::IRenderFunction* pRenderFunction ) :
WidgetImplementation(pDestructor, pRenderFunction)
{

}

WImage::~WImage(
   void )
{
   if (m_propertyBag != NULL)
   {
      gubi::IImage* pImage             = NULL;
      const gubi::IProperty* pProperty = NULL;

      if (m_propertyBag->Find(L"image", &pProperty))
      {
         if (gubi::GetDataByType(pProperty, &pImage))
         {
            pImage->Release();
         }

         pProperty->Release();
      }
   }
}

}
