/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "CoreFunctionPrologue.h"
#include "config.h"
#include "ScaleConfigInterface.h"
#include "MapLib.h"
#include "UserDefinedScaleFeature.h"
#include "UserDefinedFeature.h"
#include <vector>

namespace WFAPI {

ScaleConfigInterface::ScaleConfigInterface(MapLib* mapLib)
   : m_mapLib(mapLib)
{
   CORE_FUNCTION_PROLOGUE();
   
}

ScaleConfigInterface::~ScaleConfigInterface()
{
   CORE_FUNCTION_PROLOGUE();
   
}

void
ScaleConfigInterface::enableScale(bool enable)
{
   CORE_FUNCTION_PROLOGUE();
   
   std::vector<UserDefinedFeature*>* userDefinedFeatures =
      m_mapLib->getUserDefinedFeatures();

   std::vector<UserDefinedFeature*>::iterator it =
      userDefinedFeatures->begin();
   std::vector<UserDefinedFeature*>::const_iterator itEnd =
      userDefinedFeatures->end();
   for(; it != itEnd; it++) {
      if((*it)->getType() == UserDefinedFeature::scale)
      {
         static_cast<UserDefinedScaleFeature*>((*it))->setVisible(enable);
      }
   }
}

void
ScaleConfigInterface::setDistanceUnit(DistanceUnit unit)
{
   CORE_FUNCTION_PROLOGUE();
   
   std::vector<UserDefinedFeature*>* userDefinedFeatures =
      m_mapLib->getUserDefinedFeatures();

   std::vector<UserDefinedFeature*>::iterator it =
      userDefinedFeatures->begin();
   std::vector<UserDefinedFeature*>::const_iterator itEnd =
      userDefinedFeatures->end();
   for(; it != itEnd; it++) {
      if((*it)->getType() == UserDefinedFeature::scale)
      {
         switch(unit) {
         case KM:
            static_cast<UserDefinedScaleFeature*>((*it))->
               setScale(UserDefinedScaleFeature::getMeterSettings());
            break;
         case MILES_FEET:
            static_cast<UserDefinedScaleFeature*>((*it))->
               setScale(UserDefinedScaleFeature::getMilesFeetSettings());
            break;
         case MILES_YARD:
            static_cast<UserDefinedScaleFeature*>((*it))->
               setScale(UserDefinedScaleFeature::getMilesYardsSettings());
            break;
         }
      }
   }
   //Called to make the map to redraw so the scale is updated.
   m_mapLib->invalidateMap();
}

void
ScaleConfigInterface::setMaxWidth(wf_uint16 maxScaleWidth)
{
   CORE_FUNCTION_PROLOGUE();
   
   std::vector<UserDefinedFeature*>* userDefinedFeatures =
      m_mapLib->getUserDefinedFeatures();

   std::vector<UserDefinedFeature*>::iterator it =
      userDefinedFeatures->begin();
   std::vector<UserDefinedFeature*>::const_iterator itEnd =
      userDefinedFeatures->end();
   for(; it != itEnd; it++) {
      if((*it)->getType() == UserDefinedFeature::scale) {
         static_cast<UserDefinedScaleFeature*>((*it))->
            setScaleMaxWidth(maxScaleWidth);
      }
   }

}

void
ScaleConfigInterface::setAlignment(ScaleAlignment scaleAlignment)
{
   CORE_FUNCTION_PROLOGUE();
   
   std::vector<UserDefinedFeature*>* userDefinedFeatures =
      m_mapLib->getUserDefinedFeatures();

   std::vector<UserDefinedFeature*>::iterator it =
      userDefinedFeatures->begin();
   std::vector<UserDefinedFeature*>::const_iterator itEnd =
      userDefinedFeatures->end();
   for(; it != itEnd; it++) {
      if((*it)->getType() == UserDefinedFeature::scale) {
         switch(scaleAlignment) {
         case LEFTALIGNMENT:
            static_cast<UserDefinedScaleFeature*>((*it))->
               setAlignment(UserDefinedScaleFeature::LEFT_ALIGNMENT);
            break;
         case RIGHTALIGNMENT:
            static_cast<UserDefinedScaleFeature*>((*it))->
               setAlignment(UserDefinedScaleFeature::RIGHT_ALIGNMENT);
            break;
         }
      }
   }
}

void
ScaleConfigInterface::setPosition(wf_uint16 posX, wf_uint16 posY)
{
   CORE_FUNCTION_PROLOGUE();
   
   std::vector<UserDefinedFeature*>* userDefinedFeatures =
      m_mapLib->getUserDefinedFeatures();
   std::vector<UserDefinedFeature*>::iterator it =
      userDefinedFeatures->begin();
   std::vector<UserDefinedFeature*>::const_iterator itEnd =
      userDefinedFeatures->end();
   for(; it != itEnd; it++) {
      if((*it)->getType() == UserDefinedFeature::scale) {
         static_cast<UserDefinedScaleFeature*>((*it))->setPosition(posX, posY);
      }
   }
}

void
ScaleConfigInterface::setHeight(wf_uint16 scaleHeight)
{
   CORE_FUNCTION_PROLOGUE();
   
   std::vector<UserDefinedFeature*>* userDefinedFeatures =
      m_mapLib->getUserDefinedFeatures();
   std::vector<UserDefinedFeature*>::iterator it =
      userDefinedFeatures->begin();
   std::vector<UserDefinedFeature*>::const_iterator itEnd =
      userDefinedFeatures->end();
   for(; it != itEnd; it++) {
      if((*it)->getType() == UserDefinedFeature::scale) {
         static_cast<UserDefinedScaleFeature*>((*it))->setScaleHeight(scaleHeight);
      }
   }
}

wf_uint16
ScaleConfigInterface::getMaxWidth() const
{
   CORE_FUNCTION_PROLOGUE();
   
   std::vector<UserDefinedFeature*>* userDefinedFeatures =
      m_mapLib->getUserDefinedFeatures();
   std::vector<UserDefinedFeature*>::iterator it =
      userDefinedFeatures->begin();
   std::vector<UserDefinedFeature*>::const_iterator itEnd =
      userDefinedFeatures->end();
   for(; it != itEnd; it++) {
      if((*it)->getType() == UserDefinedFeature::scale) {
         return static_cast<UserDefinedScaleFeature*>((*it))->
            getScaleMaxWidth();
      }
   }
   return WF_MAX_UINT16;
}

wf_uint16
ScaleConfigInterface::getHeight() const
{
   CORE_FUNCTION_PROLOGUE();
   
   std::vector<UserDefinedFeature*>* userDefinedFeatures =
      m_mapLib->getUserDefinedFeatures();
   std::vector<UserDefinedFeature*>::iterator it =
      userDefinedFeatures->begin();
   std::vector<UserDefinedFeature*>::const_iterator itEnd =
      userDefinedFeatures->end();
   for(; it != itEnd; it++) {
      if((*it)->getType() == UserDefinedFeature::scale) {
         return static_cast<UserDefinedScaleFeature*>((*it))->
            getScaleHeight();
      }
   }
   return WF_MAX_UINT16;
}
wf_int32
ScaleConfigInterface::getPositionXLed() const
{
   CORE_FUNCTION_PROLOGUE();
   
   std::vector<UserDefinedFeature*>* userDefinedFeatures =
      m_mapLib->getUserDefinedFeatures();
   std::vector<UserDefinedFeature*>::iterator it =
      userDefinedFeatures->begin();
   std::vector<UserDefinedFeature*>::const_iterator itEnd =
      userDefinedFeatures->end();
   for(; it != itEnd; it++) {
      if((*it)->getType() == UserDefinedFeature::scale) {
         return static_cast<UserDefinedScaleFeature*>((*it))->
            getPositionXLed();
      }
   }
   return WF_MAX_UINT32;
}

wf_int32
ScaleConfigInterface::getPositionYLed() const
{
   CORE_FUNCTION_PROLOGUE();
   
   std::vector<UserDefinedFeature*>* userDefinedFeatures =
      m_mapLib->getUserDefinedFeatures();
   std::vector<UserDefinedFeature*>::iterator it =
      userDefinedFeatures->begin();
   std::vector<UserDefinedFeature*>::const_iterator itEnd =
      userDefinedFeatures->end();
   for(; it != itEnd; it++) {
      if((*it)->getType() == UserDefinedFeature::scale) {
         return static_cast<UserDefinedScaleFeature*>((*it))->
            getPositionYLed();
      }
   }
   return WF_MAX_UINT32;
}

} // End namespace WFAPI.
