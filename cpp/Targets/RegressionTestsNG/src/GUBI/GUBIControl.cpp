/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "GUBIControl.h"
#include "OpenGLES/GUBIRenderBridge.h"
#include "Core/IDevice.h"
#include "Window/AppWindow.h"
#include "KeyEvent.h"

#define CORE_LOGGING_MODULE_NAME "GUBIDemoTest"
#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE
#include "CoreDebugPrint.h"
#include "Common/TimeDefault.h"
#include "Common/SmartPointer.h"
#include "Property/PropertyUtils.h"
#include "View/IView.h"
#include "Signal/SignalUtils.h"
#include "Effects/IEffect.h"
#include "Widget/IWidget.h"

struct GUBIControl::Impl {
   AppWindow* window;
   gubi::IDevice* gubiDevice;
   AppEventListener* appListener;
   gubi::ITime* time;
   GUBIRenderBridge *renderBridge;
};

gubi::IWidget *g_Widget = NULL;

GUBIControl::GUBIControl(AppWindow* window,
                         AppEventListener* appListener)
   : NotCopyable()
{
   m_impl = new Impl;
   m_impl->window = window;
   m_impl->time = new gubi::TimeDefault;
   m_impl->renderBridge = new GUBIRenderBridge;
   
   m_impl->appListener = appListener;
      
   gubi::CreateDevice(gubi::DeviceFlags_Default,
                      m_impl->time,
                      m_impl->renderBridge,
                      this,
                      &m_impl->gubiDevice);
                                           
   m_impl->window->setListener(this);

   // make a test widget with an image and a view
   gubi::IView *view = NULL;
   if (m_impl->gubiDevice->CreateView(L"testView", &view)) {
         gubi::IImage *image = NULL;
         if (m_impl->renderBridge->createImage("resources/","bar_plus.png", &image)) {
            gubi::SmartPointer<gubi::PropertyBag> propBag(new gubi::PropertyBag);
            gubi::InsertProperty(propBag, L"image", image);
            if (m_impl->gubiDevice->CreateWidget(L"gubi.ui.image", propBag, &g_Widget)) {
               view->AddWidget(g_Widget, gubi::ViewSpace_Overlay);
            }
         }
   }

   gubi::SmartPointer<gubi::PropertyBag> bag(new gubi::PropertyBag);
   gubi::IEffect *effect = NULL;

   gubi::InsertProperty(bag, L"startValue", fmath::tofixed(10));
   gubi::InsertProperty(bag, L"endValue", fmath::tofixed(100));
   gubi::InsertProperty(bag, L"duration", 3000);
   gubi::InsertProperty(bag, L"period", 500);
   gubi::InsertProperty(bag, L"amplitude", fmath::tofixed(100));

   if (m_impl->gubiDevice->CreateEffect(L"wave", bag, gubi::EffectStartOption_Stopped, &effect)) {
      gubi::SignalConnect(effect, L"onUpdate", this, &GUBIControl::onEffectUpdate, NULL);
      effect->Start();
   }
   
}

GUBIControl::~GUBIControl()
{
   delete m_impl; 
}

void GUBIControl::onKeyDown(const KeyEvent& event)
{
   switch(event.code) {
   case KeyEvent::ML_e:
      if(m_impl->appListener) {
         m_impl->appListener->onExitRequest(0);
      }
   }
}

void GUBIControl::onKeyUp(const KeyEvent& event)
{
    
}

void GUBIControl::onWindowClosed()
{
   printf("onWindowClosed\n");
   
   m_impl->appListener->onExitRequest(0);
}

void GUBIControl::onPointerDown(const PointerEvent& event)
{
    
}

void GUBIControl::onPointerMove(const PointerEvent& event)
{
    
}

void GUBIControl::onPointerUp(const PointerEvent& event)
{
    
}

void GUBIControl::onWindowResize(int newWidth, int newHeight)
{
    
}

void GUBIControl::gubiShouldBeDrawn()
{
   m_impl->window->requestDraw();
}

void GUBIControl::onRedraw()
{
   coreprintln("onRedraw");
                              
   m_impl->gubiDevice->Update();
   m_impl->gubiDevice->Render();

   m_impl->window->requestDraw();
}

void GUBIControl::onEffectUpdate(gubi::IPropertyBag *bag)
{
#ifdef USE_OPENGL_ES
   if (bag != NULL && g_Widget != NULL) {
      gubi::IProperty *prop = NULL;
      printf("effect\n");
      if (bag->Find(L"value", &prop)) {
         int value = 0;
         if (gubi::GetDataByType(prop, &value)) {
            g_Widget->setTranslation(0, value, 0);
            printf("v: %d\n",value);
         }
         prop->Release();
      }
   } else {
      printf("fail\n");
   }
#endif
}
