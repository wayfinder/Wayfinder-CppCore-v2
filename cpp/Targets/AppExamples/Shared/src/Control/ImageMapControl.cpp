/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#define CORE_LOGGING_MODULE_NAME "ImageMapControl"
#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE
#include "CoreDebugPrint.h"

// GUBI includes
#include "OpenGLES/GUBIRenderBridge.h"
#include "OpenGLES/EffectJump.h"
#include "Core/IDevice.h"
#include "Window/AppWindow.h"
#include "KeyEvent.h"
#include "Common/TimeDefault.h"
#include "Common/SmartPointer.h"
#include "Property/PropertyUtils.h"
#include "View/IView.h"
#include "Signal/SignalUtils.h"
#include "Effects/IEffect.h"
#include "Widget/IWidget.h"

// MapLib includes
#include "Window/AppWindow.h"
#include "Control/ImageMapControl.h"
#include "Control/Input/InputControl.h"
#include "MapModel.h"
#include "MapProjection/MapProjection.h"

#include "MapOperationHandler.h"
#include "DrawingContext.h"
#include "TileMapUtil.h"
#include "MC2Coordinate.h"
#include "WGS84Coordinate.h"

// AppEx includes
#include "KeyEvent.h"
#include "AppEventListener.h"
#include "PointerEvent.h"

// PAL includes
#include "PALRunLoop.h"

#include <vector>
#include <map>

struct EffectDemo {
   gubi::IWidget *widget;
   EffectJump *effect;
};
#define NUMBEROFPOIS 2

using namespace WFAPI;

struct ImageMapControl::Impl {
   Impl(AppWindow* window,
        const BufferRequesterSettings& modelSettings,
        DBufConnection* connection,
        AppEventListener* appListener) :
      window(window),
      model(modelSettings, connection, window->getDrawingContext()),
      inputControl(&model, appListener)
   {
      time = new gubi::TimeDefault;
#ifdef USE_OPENGL_ES
      renderBridge = new GUBIRenderBridge;
#else
      renderBridge = NULL;
#endif
      pal::pstatus status = pal::createRunLoopContext(&runLoopContext);
      assert(pal::PAL_OK == status);
   }

   ~Impl() {
      // TODO:: Release run loop context
      // TODO: Release GUBI stuff
   }
   
   /// The window that is used to draw the maps
   AppWindow* window;
   
   /// The map model.
   MapModel model;

   // The input control
   InputControl inputControl;

   // Our GUBI library instance
   gubi::IDevice* gubiDevice;

   // The renderer that GUBI will use
   GUBIRenderBridge *renderBridge;

   // The time instance that GUBI uses
   gubi::ITime* time;

   // A test widget
   //gubi::IWidget *widget;
   std::vector<EffectDemo> demoList;

   // The context we use to create timers
   pal::RunLoopContext* runLoopContext;
};

ImageMapControl::ImageMapControl(AppWindow* window,
                                 const std::string& resourcePath,
                                 const BufferRequesterSettings& modelSettings,
                                 DBufConnection* connection,
                                 AppEventListener* appListener)
   : WindowEventListener()
{
   printf("ImageMapControl constructor\n");
   m_impl = new Impl(window,
                     modelSettings,
                     connection,
                     appListener);
#ifdef USE_OPENGL_ES
   gubi::CreateDevice(gubi::DeviceFlags_Default,
                      m_impl->time,
                      m_impl->renderBridge,
                      this,
                      &m_impl->gubiDevice);
#endif
   
   m_impl->window->setListener(this);

   // Update position and zoom level.
   // Hack to set mercator coord.
   MC2Coordinate paddington = WFAPI::WGS84Coordinate(51.515713, -0.17503);
   MercatorCoord paddingtonMerc = MercatorCoord(paddington);
   MC2Coordinate paddingtonMC2Merc(paddingtonMerc.lat, paddingtonMerc.lon);
   m_impl->model.getMapProjection().setCenter(paddingtonMC2Merc);
   m_impl->model.getMapOperationHandler().setZoomLevel(1.194329);

   m_impl->model.setMapDrawingCallback(this);

   // Initialize some GUBI stuff
   using namespace gubi;
   
   // make a test widget with an image and a view
   // IView *view = NULL;
   // if (m_impl->gubiDevice->CreateView(L"testView", &view)) {
   //    IImage *image = NULL;

   //    //TODO: Remove "/"
   //    if (m_impl->renderBridge->createImage(resourcePath + "/","overlay_bg1.png", &image)) {
   //       printf("Successfully created image\n");
   //       SmartPointer<PropertyBag> propBag(new PropertyBag);
   //       InsertProperty(propBag, L"image", image);

   //       for (int i = 0; i < 15; ++i) {
   //          EffectDemo poi;
   //          if (m_impl->gubiDevice->CreateWidget(L"gubi.ui.image",
   //                                               propBag,
   //                                               &poi.widget)) {
   //             poi.widget->setScale(fmath::tofixed(0.5f),
   //                                  fmath::tofixed(0.5f),
   //                                  fmath::tofixed(1.0f));
                                    
   //             view->AddWidget(poi.widget, ViewSpace_3d);

   //             float startX = 0.0f;
   //             float startY = -5.0f;
   //             float startZ = -10.0f;
   //             float endX = ((rand() % 400)-200)/100.0f;
   //             float endY = ((rand() % 600)-300)/100.0f;
   //             float endZ = -10.0f;
   //             poi.effect = new EffectJump(m_impl->gubiDevice,
   //                                         poi.widget,
   //                                         startX, startY, startZ,
   //                                         endX, endY, endZ);
   //             poi.effect->init(30*i + 3000);
               
   //             m_impl->demoList.push_back(poi);
   //          }
   //       }
   //    } else {
   //       printf("Failed to load resources/bar_plus.png\n");
   //    }
   // }
   /*
   gubi::SmartPointer<gubi::PropertyBag> bag(new gubi::PropertyBag);
   gubi::IEffect *effect = NULL;

   gubi::InsertProperty(bag, L"startValue", fmath::tofixed(10));
   gubi::InsertProperty(bag, L"endValue", fmath::tofixed(100));
   gubi::InsertProperty(bag, L"duration", 3000);
   gubi::InsertProperty(bag, L"period", 5000);
   gubi::InsertProperty(bag, L"amplitude", fmath::tofixed(80.0f));

   if (m_impl->gubiDevice->CreateEffect(L"wave", bag, gubi::EffectStartOption_Stopped, &effect)) {
      gubi::SignalConnect(effect, L"onUpdate", this, &ImageMapControl::onEffectUpdate, NULL);
      effect->Start();
      printf("effect start\n");
   }
   */
   unsigned int id = 0;
   pal::requestTimer(m_impl->runLoopContext,
                     16,
                     ImageMapControl::timerCallback,
                     this,
                     &id);
}

ImageMapControl::~ImageMapControl()
{
   delete m_impl;
}

void ImageMapControl::onWindowClosed()
{
   
}

void ImageMapControl::onKeyUp(const KeyEvent& event)
{
   m_impl->inputControl.onKeyUp(event);
}

void ImageMapControl::onKeyDown(const KeyEvent& event)
{
   m_impl->inputControl.onKeyDown(event);
}

void ImageMapControl::onPointerDown(const PointerEvent& event)
{   
   m_impl->inputControl.onPointerDown(event);
}

void ImageMapControl::onPointerMove(const PointerEvent& event)
{   
   m_impl->inputControl.onPointerMove(event);
}

void ImageMapControl::onPointerUp(const PointerEvent& event)
{
   m_impl->inputControl.onPointerUp(event);
}

void ImageMapControl::onWindowResize(int newWidth, int newHeight)
{
   m_impl->model.getMapProjection().setScreenSize(
      MC2Point(newWidth, newHeight));    
}

void ImageMapControl::onRedraw()
{
   m_impl->model.renderMap();
   // m_impl->gubiDevice->Render();
}

void ImageMapControl::mapIsReadyToBeDrawn()
{
   m_impl->window->requestDraw();
}

void ImageMapControl::gubiShouldBeDrawn()
{
   m_impl->window->requestDraw();   
}

void ImageMapControl::onEffectUpdate(gubi::IPropertyBag *bag)
{
   /*
   if (bag != NULL && m_impl->widget != NULL) {
      gubi::IProperty *prop = NULL;
      
      if (bag->Find(L"value", &prop)) {
         int value = 0;
         if (gubi::GetDataByType(prop, &value)) {
            m_impl->widget->setTranslation(0, 0, fmath::tofixed(-15.0f));
            m_impl->widget->setRotation(0, value, 0);
         }
         prop->Release();
      }
   }
   */
}

void ImageMapControl::updateLogic()
{
   m_impl->gubiDevice->Update();
   // gubiShouldBeDrawn();
   
   unsigned int id = 0;
   // Request again.. and again, and again, and again
   pal::requestTimer(m_impl->runLoopContext,
                     16,
                     ImageMapControl::timerCallback,
                     this,
                     &id);
}

void ImageMapControl::timerCallback(unsigned int id, void* data) {
   ImageMapControl* mapControl = static_cast<ImageMapControl*>(data);
   mapControl->updateLogic();
}
