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

#include "MapLibAPI.h"
#include "MapLib.h"
#include "ConfigInterface.h"
#include "MapOperationInterface.h"
#include "OverlayView.h"
#include "OverlayViewLayerInterface.h"
#include "SelectedMapObjectNotifier.h"
#include "MapDrawingInterface.h"


namespace WFAPI {

struct MapLibAPI::Impl {
   MapLib* m_maplib;
   ConfigInterface* m_configInterface;
};

void 
MapLibAPI::setStartupListener(MapLibStartupListener* startupListener) 
{
   CORE_FUNCTION_PROLOGUE();
   
   if(m_impl) {
      m_impl->m_maplib->setStartupListener(startupListener);
      m_impl->m_maplib->start();      
   } else {
      m_impl->m_maplib = NULL; // For our sake, we want this to crash. 
   }
}

bool
MapLibAPI::isMapLibStarted() const 
{
   CORE_FUNCTION_PROLOGUE();
   
   if(m_impl) {
      return m_impl->m_maplib->isInitialized();
   } else {
      return false;
   }
}

ConfigInterface* MapLibAPI::getConfigInterface()
{
   CORE_FUNCTION_PROLOGUE();
   
   if(m_impl) {
      return m_impl->m_configInterface;
   } else {
      return NULL;
   }
}

MapLibAPI::MapLibAPI(MapLib* maplib)
{
   m_impl = new Impl;

   m_impl->m_maplib = maplib;
   m_impl->m_configInterface = new ConfigInterface( m_impl->m_maplib );
   // Set the settings from MapLibInitialConfig.
   m_impl->m_maplib->initializeSettings();

}

MapDrawingInterface* 
MapLibAPI::getMapDrawingInterface() const
{
   CORE_FUNCTION_PROLOGUE();
   
   if(m_impl) {
      return m_impl->m_maplib->getMapDrawingInterface();
   } else {
      return NULL;
   }
}

void 
MapLibAPI::setDrawingContext( DrawingContext* context )
{
   CORE_FUNCTION_PROLOGUE();
   
   if(m_impl) {
      m_impl->m_maplib->setDrawingContext( context );      
   }
}

MapLibAPI::~MapLibAPI()
{
   if(m_impl) {
      delete m_impl->m_configInterface;
      delete m_impl->m_maplib;
      delete m_impl;  
   }
}

MapLibKeyInterface* 
MapLibAPI::getKeyInterface()
{
   CORE_FUNCTION_PROLOGUE();
   
   if(m_impl) {
      return m_impl->m_maplib->getKeyInterface();
   } else {
      return NULL;
   }
}

WFAPI::InternalConfig* MapLibAPI::getInternalConfig()
{
   CORE_FUNCTION_PROLOGUE();
   
   if(m_impl) {
      return m_impl->m_maplib->getInternalConfig();
   } else {
      return NULL;
   }
}

WFAPI::MapOperationInterface* MapLibAPI::getMapOperationInterface()
{
   CORE_FUNCTION_PROLOGUE();
   
   if(m_impl) {
      return m_impl->m_maplib->getMapOperationInterface();      
   } else {
      return NULL;
   }
}

OverlayInterface* WFAPI::MapLibAPI::getOverlayInterface()
{
   CORE_FUNCTION_PROLOGUE();
   
   if(m_impl) {
      return &m_impl->m_maplib->getOverlayView()->getLayerInterface();      
   } else {
      return NULL;
   }
}

MapObjectInterface* WFAPI::MapLibAPI::getMapObjectInterface()
{
   CORE_FUNCTION_PROLOGUE();
   
   if(m_impl) {
      return m_impl->m_maplib->getSelectedMapObjectNotifier();      
   } else {
      return NULL;
   }
}

void MapLibAPI::stop()
{
   CORE_FUNCTION_PROLOGUE();
   
   if(m_impl) {
      delete m_impl->m_configInterface;
      delete m_impl->m_maplib;
      delete m_impl;      
   }
   
   m_impl = NULL;
}

MapLib* 
MapLibAPI::getMapLib()
{
   CORE_FUNCTION_PROLOGUE();
   
   if(m_impl) {
      return m_impl->m_maplib;      
   } else {
      return NULL;
   }
}

} // End of namespace WFAPI

