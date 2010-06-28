/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "View/View.h"

namespace gubi {

View::View(
   void )
{
   m_bVisible = true; // TODO: Remove this and change to a view-render-queue
}


View::~View(
   void )
{

}


unsigned int View::AddReference(
   void ) const
{
   return InternalAddReference();
}


unsigned int View::Release(
   void ) const
{
   return InternalRelease();
}


bool View::GetProperties(
   gubi::IPropertyBag** ppProperties )
{
   return false;
}


bool View::GetProperties(
   const gubi::IPropertyBag** ppProperties ) const
{
   return false;
}


bool View::GetSignal(
   const gubi::Identifier& eSignalId,
   gubi::ISignalHub** ppSignalHub )
{
   return InternalGetSignal(eSignalId, ppSignalHub);
}


bool View::Show(
   const gubi::ViewShowFlag& eFlag )
{
   return m_bVisible = true;
}


void View::Hide(
   void )
{
   m_bVisible = false;
}


bool View::AddWidget(
   gubi::IWidget* pWidget,
   const gubi::ViewSpace& eViewSpace )
{
   bool bReturn = false;

   if (pWidget != NULL)
   {
      std::vector<SmartPointer<gubi::IWidget> >& vRoots = m_vRoots[eViewSpace];

      vRoots.push_back(pWidget);

      bReturn = true;
   }
   
   return bReturn;
}


bool View::AddWidgetArray(
   gubi::IWidgetArray* pWidgetArray,
   const gubi::ViewSpace& eViewSpace )
{
   return false;
}

bool View::FetchNodes(
   const gubi::ViewSpace& eViewSpace,
   gubi::RenderQueue& renderQueue )
{
   bool bReturn                                      = false;
   std::vector<SmartPointer<gubi::IWidget> >& vRoots = m_vRoots[eViewSpace];

   for (unsigned int i = 0; i < vRoots.size(); ++i)
   {
      SmartPointer<gubi::IWidget> pWidget = vRoots[i];

      if (!pWidget.IsEmpty())
      {
         bReturn = pWidget->update(renderQueue, false) ? true : bReturn;
      }
   }

   return bReturn;
}


bool View::Visible(
   void )
{
   return m_bVisible;
}


}
