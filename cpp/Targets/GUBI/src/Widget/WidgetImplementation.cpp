/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "Renderer/IRenderFunction.h"
#include "Widget/IWidgetDestructor.h"
#include "Widget/WidgetImplementation.h"
#include "Property/PropertyUtils.h"

namespace gubi {

WidgetImplementation::WidgetImplementation(
   gubi::IWidgetDestructor* pDestructor,
   gubi::IRenderFunction* pRenderFunction ) :
m_pDestructor(pDestructor),
m_pRenderFunction(pRenderFunction)
{
   addRenderNodeFlag(RenderNodeFlag_Renderable);
}


WidgetImplementation::~WidgetImplementation(
   void )
{
   if (m_pDestructor != NULL)
   {
      m_pDestructor->OnWidgetDestroy(this);
      m_pDestructor = NULL;
   }
   
   if (m_pRenderFunction != NULL)
   {
      m_pRenderFunction->Release();
      m_pRenderFunction = NULL;
   }
}


bool WidgetImplementation::GetSignal(
   const gubi::Identifier& eSignalId,
   gubi::ISignalHub** ppSignalHub )
{
   return InternalGetSignal(eSignalId, ppSignalHub);
}


void WidgetImplementation::render(
   void )
{
   if (m_pRenderFunction != NULL)
   {
      m_pRenderFunction->PerformRender(this);
   }
}


void WidgetImplementation::setSize(
   fmath::fixed width,
   fmath::fixed height )
{
   gubi::UpdateProperty(m_propertyBag, L"width",  width);
   gubi::UpdateProperty(m_propertyBag, L"height", height);
}


void WidgetImplementation::getSize(
   fmath::fixed& width,
   fmath::fixed& height )
{
   gubi::IProperty* pWidth  = NULL;
   gubi::IProperty* pHeight = NULL;

   if (!m_propertyBag.IsEmpty())
   {
      if (m_propertyBag->Find(L"width", &pWidth))
      {
         gubi::GetDataByType(pWidth, &width);

         pWidth->Release();         
      }
      
      if (m_propertyBag->Find(L"height", &pHeight))
      {
         gubi::GetDataByType(pHeight, &height);

         pHeight->Release();         
      }
   }   
}


bool WidgetImplementation::OnInputEvent(
   const gubi::InputEventState& eState,
   int x,
   int y )
{
   // The default implementation dosen't do anything

   return false;
}

}
