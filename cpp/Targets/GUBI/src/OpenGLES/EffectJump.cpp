/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "fmath.h"
#include "Core/Platform.h"
#include "Core/IDevice.h"
#include "Widget/IWidget.h"
#include "Signal/SignalUtils.h"
#include "Effects/IEffect.h"
#include "Effects/EffectInterpolatePace.h"
#include "Common/SmartPointer.h"
#include "Property/PropertyUtils.h"
#include "OpenGLES/EffectJump.h"

EffectJump::EffectJump(gubi::IDevice *device,
                       gubi::IWidget *widget,
                       float startX, float startY, float startZ,
                       float endX, float endY, float endZ)
{
   m_device = device;
   m_widget = widget;

   // position
   m_start_position[0] = startX;
   m_start_position[1] = startY;
   m_start_position[2] = startZ;
   m_end_position[0] = endX;
   m_end_position[1] = endY;
   m_end_position[2] = endZ;

   m_lerpTimeFirst = 0;
   m_lerpTimeSecond = 0;
   m_lerpTimeThird = 0;

   m_is_lerp_third_created = false;
   m_is_lerp_third_running = false;
}

EffectJump::~EffectJump()
{
  
}

bool EffectJump::init(int delay)
{
   if (m_device == NULL || m_widget == NULL) {
      return false;
   }

   // new effect uses one wave an a interpolation
   // old effect uses three interpolations
   bool useNewEffect = false;
   
   // set position 
   m_widget->setTranslation(fmath::tofixed(m_start_position[0]),
                            fmath::tofixed(m_start_position[1]),
                            fmath::tofixed(m_start_position[2]));

   if (useNewEffect) {
      gubi::IEffect *effect = NULL;
      gubi::SmartPointer<gubi::PropertyBag> propBag(new gubi::PropertyBag);
   
      // Y direction
      gubi::InsertProperty(propBag,
                           L"amplitude",
                           fmath::abs(fmath::tofixed(m_end_position[1]-m_start_position[1] )));
   
      int period = abs(static_cast<int>( (m_end_position[0]-m_start_position[0])*1000.0f ) );
      //      printf("period: %d\n",period);
      gubi::InsertProperty(propBag, L"period", period<<1);
   
      if (m_device->CreateEffect(L"wave", propBag, gubi::EffectStartOption_Stopped, &m_effect)) {
         gubi::SignalConnect(m_effect, L"onUpdate", this, &EffectJump::waveUpdate, NULL);
      }

      // X direction
      gubi::InsertProperty(propBag, L"duration", period);
      gubi::InsertProperty(propBag, L"startvalue", fmath::tofixed(0));
      gubi::InsertProperty(propBag, L"endvalue", fmath::tofixed(m_end_position[0]));
      gubi::InsertProperty(propBag, L"delay", delay);
   
      if (m_device->CreateEffect(L"interpolate", propBag, gubi::EffectStartOption_Stopped, &effect)) {
         gubi::SignalConnect(effect, L"onUpdate", this, &EffectJump::interpolateUpdate, NULL);
         gubi::SignalConnect(effect, L"onFinish", this, &EffectJump::interpolateFinish, NULL);
         gubi::SignalConnect(effect, L"onStart", this, &EffectJump::interpolateStart, NULL);
         effect->Start();
      }
   } else {
      int lerpTimeOffset = 50;
      m_lerpTimeFirst = 600;
      m_lerpTimeThird = 400;
      m_lerpTimeSecond = (m_lerpTimeFirst + m_lerpTimeThird)+lerpTimeOffset;

      // Effect in Y negative "up" direction
      // Deaccelerate 
      gubi::IEffect *effect = NULL;
      gubi::SmartPointer<gubi::PropertyBag> propBag(new gubi::PropertyBag);

      float dist = m_end_position[1] - m_start_position[1];
      float half = m_end_position[1] - dist*0.5f;
      
      //      printf("dist: %f,%f\n",dist,half);
      
      gubi::InsertProperty(propBag, L"startvalue", fmath::tofixed(m_start_position[1]));
      gubi::InsertProperty(propBag, L"endvalue", fmath::tofixed(4));
      gubi::InsertProperty(propBag, L"delay", delay);
      gubi::InsertProperty(propBag, L"duration", m_lerpTimeFirst);
      gubi::InsertProperty(propBag, L"pace", gubi::EffectInterpolatePace_DeceleratingPow2);
      
      if (m_device->CreateEffect(L"interpolate",
                                 propBag,
                                 gubi::EffectStartOption_Stopped,
                                 &m_effectLerpFirst)) {
         gubi::SignalConnect(m_effectLerpFirst, L"onUpdate", this, &EffectJump::lerpFirst, NULL);
      }

      dist = m_end_position[0] - m_start_position[0];
      half = m_end_position[0] - dist/2;
      //      printf("distx: %f, %f\n", dist,half);
      
      // Effect in X direction
      // linear
      gubi::UpdateProperty(propBag, L"startvalue", fmath::tofixed(m_start_position[0]));
      gubi::UpdateProperty(propBag, L"endvalue", fmath::tofixed(m_end_position[0]));
      gubi::UpdateProperty(propBag, L"delay", delay);
      gubi::UpdateProperty(propBag, L"duration", m_lerpTimeSecond);
      gubi::UpdateProperty(propBag, L"pace", gubi::EffectInterpolatePace_Constant);
      if (m_device->CreateEffect(L"interpolate",
                                 propBag,
                                 gubi::EffectStartOption_Stopped,
                                 &m_effectLerpSecond)) {
         gubi::SignalConnect(m_effectLerpSecond, L"onUpdate", this, &EffectJump::lerpSecond, NULL);
      }
      m_effectLerpFirst->Start();
      m_effectLerpSecond->Start();
   }
   
  return true;
}

void EffectJump::lerpFirst(gubi::IPropertyBag *bag)
{
  if (bag != NULL && m_widget != NULL) {
     fmath::fixed value = 0;
     fmath::fvec3 translation;
     m_widget->getTranslation(translation);

     gubi::IProperty *prop = NULL;
     if (bag->Find(L"value", &prop)) {
        if (gubi::GetDataByType(prop, &value)) {
           if (!m_is_lerp_third_running) {
              m_widget->setTranslation(translation[0], value, translation[2]);
           }

           // we need to check where the poi is
             
           float dist = 0;
           float half = 0;
           float xpos = fmath::tofloat(translation[0]);
           //printf("xpos: %.2f\n", xpos);
           bool create_third = false;
 
           
           if (m_start_position[0] < m_end_position[0]) {
              dist = m_end_position[0] - m_start_position[0];
              half = m_end_position[0] - dist/2;
              if (xpos > half) {
                 create_third = true;
                 //printf("createthird: %f\n",xpos);
              } else {
                 //printf("xpos: %2.f\n",xpos);
              }
           } else {
              dist = m_start_position[0] - m_end_position[0];
              half = m_end_position[0] + dist/2;
              if (xpos < half) {
                 create_third = true;
              } else {
                 //printf("xpos: %2.f\n",xpos);
              }
           }
           
           if (create_third && !m_is_lerp_third_created) {
              // Accelerate
              //printf("lerpFirst : creating third\n");
              m_is_lerp_third_created = true;
              gubi::IEffect *effect = NULL;
              gubi::SmartPointer<gubi::PropertyBag> propBag(new gubi::PropertyBag);
                   
              gubi::InsertProperty(propBag, L"startvalue", value);
              gubi::InsertProperty(propBag, L"endvalue", fmath::tofixed(m_end_position[1]));
              gubi::InsertProperty(propBag, L"delay", 0);
              gubi::InsertProperty(propBag, L"duration", m_lerpTimeThird);
              gubi::InsertProperty(propBag, L"pace", gubi::EffectInterpolatePace_AcceleratingPow2);
              
              if (m_device->CreateEffect(L"interpolate", propBag, gubi::EffectStartOption_Directly, &effect)) {
                 gubi::SignalConnect(effect, L"onUpdate", this, &EffectJump::lerpThird, NULL);
              }

           }
        }
        prop->Release();
     }
  } else {
     printf("lerpFirst : Failed\n");
  }
}

void EffectJump::lerpSecond(gubi::IPropertyBag *bag)
{
   // lerp in x dir
   if (bag != NULL && m_widget != NULL) {
      fmath::fixed value = 0;
      fmath::fvec3 translation;
      m_widget->getTranslation(translation);

      gubi::IProperty *prop = NULL;
      if (bag->Find(L"value", &prop)) {
         if (gubi::GetDataByType(prop, &value)) {
            m_widget->setTranslation(value, translation[1], translation[2]);
         }
         prop->Release();
      }
   }
}

void EffectJump::lerpThird(gubi::IPropertyBag *bag)
{
  if (bag != NULL && m_widget != NULL) {
     m_is_lerp_third_running = true;
     fmath::fixed value = 0;
     fmath::fvec3 translation;
     m_widget->getTranslation(translation);

     gubi::IProperty *prop = NULL;
     if (bag->Find(L"value", &prop)) {
        if (gubi::GetDataByType(prop, &value)) {
           m_widget->setTranslation(translation[0], value, translation[2]);
        }
        prop->Release();
     }
  }
}

void EffectJump::waveUpdate(gubi::IPropertyBag *bag)
{
   if (bag != NULL && m_widget != NULL) {
      fmath::fixed value = 0;
      fmath::fvec3 translation;
      m_widget->getTranslation(translation);

      gubi::IProperty *prop = NULL;
      if (bag->Find(L"value", &prop)) {
         if (gubi::GetDataByType(prop, &value)) {
            m_widget->setTranslation(translation[0],
                                     value + fmath::tofixed(m_start_position[1]),
                                     translation[2]);
         }
         prop->Release();
      }
   }
}

void EffectJump::interpolateUpdate(gubi::IPropertyBag *bag)
{
   if (bag != NULL && m_widget != NULL) {
      fmath::fixed value = 0;
      fmath::fvec3 translation;
      m_widget->getTranslation(translation);

      gubi::IProperty *prop = NULL;
      if (bag->Find(L"value", &prop)) {
         if (gubi::GetDataByType(prop, &value)) {
            m_widget->setTranslation(value, translation[1], translation[2]);
         }
         prop->Release();
      }
   }
}

void EffectJump::interpolateFinish(gubi::IPropertyBag *bag)
{
   if (m_effect != NULL) {
      m_effect->Stop();
   }
}

void EffectJump::interpolateStart(gubi::IPropertyBag *bag)
{
   if (m_effect != NULL) {
      m_effect->Start();
   }
}
