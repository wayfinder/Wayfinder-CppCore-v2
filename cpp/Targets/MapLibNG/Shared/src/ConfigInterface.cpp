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
#include "ConfigInterface.h"
#include "DetailedConfigInterface.h"
#include "ScaleConfigInterface.h"
#include "MapLib.h"
#include "MapLibSettingsNotifier.h"


namespace WFAPI {

ConfigInterface::ConfigInterface( MapLib* mapLib )
   : m_mapLib( mapLib )
{
   CORE_FUNCTION_PROLOGUE();
   
   m_detailedConfig = new DetailedConfigInterface(m_mapLib);
   m_scaleConfig = new ScaleConfigInterface(m_mapLib);
}

ConfigInterface::~ConfigInterface()
{
   CORE_FUNCTION_PROLOGUE();
   
   delete m_detailedConfig;
   delete m_scaleConfig;
}

void ConfigInterface::set3dMode( bool enabled )
{
   CORE_FUNCTION_PROLOGUE();
   
   m_mapLib->set3dMode( enabled );
}

bool ConfigInterface::get3dMode() const
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_mapLib->get3dMode(); 
}

void WFAPI::ConfigInterface::setNightMode(bool on)
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_mapLib->setNightMode(on); 
}

bool WFAPI::ConfigInterface::getNightMode() const
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_mapLib->getNightMode(); 
}

void
ConfigInterface::setMapScaleAdjustment(float level){
   CORE_FUNCTION_PROLOGUE();

   int levelInt = static_cast<int>(level + 0.5);
   double currentScale = m_mapLib->getDPICorrectedScale();
   m_mapLib->setDPICorrectionFactor(levelInt);
   m_mapLib->setDPICorrectedScale(currentScale);
}

DetailedConfigInterface* 
ConfigInterface::getDetailedConfigInterface() const
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_detailedConfig;
}

ScaleConfigInterface* 
ConfigInterface::getScaleConfigInterface() const
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_scaleConfig;
}

void WFAPI::ConfigInterface::addSettingsListener(MapLibSettingsListener* listener)
{
   CORE_FUNCTION_PROLOGUE();
   
   m_mapLib->getMapLibSettingsNotifier()->addSettingsListener(listener);
}

void WFAPI::ConfigInterface::removeSettingsListener(MapLibSettingsListener* listener)
{
   CORE_FUNCTION_PROLOGUE();
   
   m_mapLib->getMapLibSettingsNotifier()->removeSettingsListener(listener);
}

void ConfigInterface::setVariable3dMode(float step)
{
   CORE_FUNCTION_PROLOGUE();
   
   m_mapLib->setVariable3dMode(step);
}

}
