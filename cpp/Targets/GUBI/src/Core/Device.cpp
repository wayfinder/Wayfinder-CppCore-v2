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
#include "Core/Device.h"
#include "Common/ITime.h"
#include "Core/IDrawCallback.h"
#include "View/View.h"

namespace gubi {

bool CreateDevice(
   const gubi::DeviceFlags& eDeviceFlags,
   gubi::ITime* pTime,
   gubi::IRenderBridge* pBridge,
   gubi::IDrawCallback* pDrawCallback,
   gubi::IDevice** ppDevice )
{
   bool bReturn = false;

   if (ppDevice != NULL)
   {
      Device* pDevice = new Device(pTime,
                                   pBridge,
                                   pDrawCallback);

      if (pDevice != NULL)
      {
         pDevice->AddReference();

         *ppDevice = pDevice;
         bReturn   = true;
      }
   }

   return bReturn;
}

Device::Device(
   gubi::ITime* pTime,
   gubi::IRenderBridge* pBridge,
   gubi::IDrawCallback* pDrawCallback ) :
m_pTime(pTime),
m_renderSystem(pBridge),
m_effectManager(pTime),
m_widgetManager(&m_renderSystem),
m_inputManager(&m_widgetManager, pTime)
{
   m_pDrawCallback = pDrawCallback;
}


Device::~Device(
   void )
{
   m_pDrawCallback = NULL;
}


unsigned int Device::AddReference(
   void ) const
{
   return InternalAddReference();
}


unsigned int Device::Release(
   void ) const
{
   return InternalRelease();
}


void Device::Update(
   void )
{
   // Update the input manager
   m_inputManager.Update();

   // Update the effect manager and all its effects
   if (m_effectManager.Update() && m_pDrawCallback != NULL)
   {
      m_pDrawCallback->gubiShouldBeDrawn();
   }
}


void Device::Render(
   void )
{
   // Update the render system
   if (m_renderSystem.update() && m_pDrawCallback != NULL)
   {
      m_pDrawCallback->gubiShouldBeDrawn();
   }

   // Render
   m_renderSystem.render();
}


bool Device::CreateView(
   const gubi::Identifier& eViewName,
   gubi::IView** ppView )
{
   bool bReturn = false;

   if (ppView != NULL)
   {
      view_map::iterator it = m_mapView.find(eViewName);

      if (it == m_mapView.end())
      {
         gubi::View* pView = new gubi::View;

         if (pView != NULL)
         {
            pView->AddReference();

            m_mapView.insert(std::make_pair(eViewName,
                                            (gubi::IViewInternal*)pView));

            *ppView = pView;
            bReturn = true;

            // TODO: Remove this and change to a view-render-queue
            m_renderSystem.addView(pView);
         }
      }
   }
   
   return bReturn;
}

bool Device::GetView(
   const gubi::Identifier& eViewName,
   gubi::IView** ppView )
{
   bool bReturn = false;

   if (ppView != NULL)
   {
      view_map::iterator it = m_mapView.find(eViewName);

      if (it != m_mapView.end())
      {
         gubi::IViewInternal* pView = it->second;

         if (pView != NULL)
         {
            pView->AddReference();

            *ppView = (gubi::IView*)pView;
            bReturn = true;
         }
      }
   }

   return bReturn;
}

bool Device::CreateWidget(
      const gubi::Identifier& eWidgetType,
      const gubi::IPropertyBag* pProperties,
      gubi::IWidget** ppWidget )
{
   return m_widgetManager.CreateWidget(eWidgetType,
                                       pProperties,
                                       ppWidget);
}


bool Device::CreateWidgetArray(
   const gubi::Identifier& eWidgetType,
   const gubi::IPropertyBag* pProperties,
   unsigned int iCount,
   gubi::IWidgetArray** ppWidgetArray )
{
   return false;
}


bool Device::CreateEffect(
   const gubi::Identifier& eEffectType,
   gubi::IPropertyBag* pPropertyBag,
   const gubi::EffectStartOption& eStartOption,
   gubi::IEffect** ppEffect )
{
   return m_effectManager.CreateEffect(eEffectType, 
                                       pPropertyBag,
                                       eStartOption,
                                       ppEffect);
}


void Device::OnPointerDown(
   unsigned int iIndex,
   int x,
   int y )
{
   m_inputManager.OnPointerDown(iIndex, x, y);
}


void Device::OnPointerMove(
   unsigned int iIndex,
   int x,
   int y )
{
   m_inputManager.OnPointerMove(iIndex, x, y);
}


void Device::OnPointerUp(
   unsigned int iIndex,
   int x,
   int y )
{
   m_inputManager.OnPointerUp(iIndex, x, y);
}

}
