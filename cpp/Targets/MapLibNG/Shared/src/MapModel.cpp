/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "MapModel.h"
#include <vector>
#include "BufferRequester/BufferRequester.h"
#include "BufferRequester/DiskCacheSettings.h"
#include "BufferRequester/MemCacheSettings.h"
#include "MapProjection/MapProjection.h"
#include "ImageMap/ImageMapModule.h"
#include "MapProjection/MapProjectionProxy.h"
#include "MapOperationHandler.h"
#include "ImageMap/WF/WFMapParamFactory.h"
#include "ImageMap/OSM/OSMMapParamFactory.h"
#include "MapManipulator.h"
#include "ImageMap/NullDrawingInterface.h"
#include "MapLibKeyHandler.h"
#include "AnimatedMapManipulator.h"
#include "PlatformFactory.h"
#include "DrawingContextProxy.h"
#include "DrawingContextsConcrete.h"
#include "ExternalRenderer.h"
#include "BufferRequester/BufferRequesterSettings.h"
#include "KeepAliveRequester.h"
#include "MapProjection/ProjectionInterface.h"

/**
 *  The different components that constitute the map model.
 */ 
struct MapModel::Impl {
   // The factories represent the different ways we
   // can request maps (OSM, WF, etc)
   std::vector<MapParamFactory*> factories;

   // The module used to request and draw image maps
   ImageMapModule* imgModule;

   // The projection used to project geometry and calculate bounds
   MapProjection* projection;

   // The handler responsible for performing direct actions upon the map projection
   MapOperationHandler* mapOperationHandler;

   // The buffer requesting class, used to fetch all map geometry
   BufferRequester* bufRequester;

   // Kludge used to bridge compatibility between old design and new
   NullDrawingInterface* nullDrawInterface;

   // Collection of classes implementing functionality that manipulates
   // the look of the map
   MapManipulator* mapManipulator;

   // Class which deals with key input
   MapLibKeyHandler* keyHandler;

   // Class used to keep connection alive under dire circumstances.
   KeepAliveRequester* kaRequester;

   // Class used to expose coordinate projection functions to applications
   ProjectionInterface* projectionInterface;
};

MapModel::~MapModel()
{
   delete m_impl->bufRequester;
   delete m_impl->projection;
   delete m_impl->imgModule;

   for(unsigned int i = 0; i < m_impl->factories.size(); i++) {
      delete m_impl->factories[i];
   }
   
   delete m_impl->mapOperationHandler;
   delete m_impl->nullDrawInterface;
   delete m_impl->mapManipulator;
   delete m_impl->keyHandler;
   delete m_impl->projectionInterface;
   delete m_impl;
}

MapModel::MapModel(const BufferRequesterSettings& modelSettings,
                   WFAPI::DBufConnection* connection,
                   WFAPI::DrawingContext* drawContext)
{
   m_impl = new Impl;
   
   // Create 
   TileMapToolkit* toolkit = PlatformFactory::createToolkit();

   m_impl->bufRequester =
      new BufferRequester(toolkit,
                          connection,
                          modelSettings.getMemCacheSettings(),
                          modelSettings.getDiskCacheSettings(),
                          modelSettings.getResourceDirectorySettings());

   m_impl->bufRequester->setReqAmountLimitEnabled(false);
   
   m_impl->projection = new MapProjection;

   /**
    *  Create our map protocol backends
    */ 
   m_impl->factories.push_back(new OSMMapParamFactory);
   m_impl->factories.push_back(new WFMapParamFactory);

   /**
    *  Extract external renderer from the drawing context, get
    *  screen size, and pass it on to the image map module.
    */
   DrawingContextProxy* proxy =
      static_cast<DrawingContextProxy*>( drawContext );

   assert(proxy->type == DrawingContextProxy::EXTERNAL);

   ExternalContextConcrete* extCont =
      static_cast<ExternalContextConcrete*>( drawContext );
   ExternalRenderer* renderer = extCont->renderer;

   isab::Rectangle screenDims;
   renderer->getMapSizePixels(screenDims);


   m_impl->projectionInterface =
      new ProjectionInterface(*m_impl->projection, *renderer);
   
   m_impl->projection->setScreenSize(MC2Point(screenDims.getWidth(),
                                              screenDims.getHeight()));

   m_impl->projection->setUsingFOVBoundingBox(true);
   renderer->setCamera(&m_impl->projection->getCamera());
   renderer->setProjection(m_impl->projection);

#if 0
   m_impl->kaRequester = new KeepAliveRequester(toolkit,
                                                *m_impl->bufRequester);
#endif
   
   /**
    *  Create our image map module.
    */
   m_impl->imgModule = new ImageMapModule(*m_impl->projection,
                                          m_impl->factories[0],
                                          *m_impl->bufRequester,
                                          renderer);

   m_impl->mapOperationHandler =
      new MapOperationHandler(m_impl->projection, renderer);
   
   // All this stuff is unfortunately needed for the key handler.
   m_impl->nullDrawInterface = new NullDrawingInterface();
   m_impl->mapManipulator = new MapManipulator(m_impl->nullDrawInterface, toolkit);
   m_impl->mapManipulator->setMapOperationInterface(m_impl->mapOperationHandler);
   m_impl->keyHandler = new MapLibKeyHandler(*m_impl->mapManipulator, NULL, NULL);
}

void MapModel::setMapEventListener(MapEventListener* listener)
{
   m_impl->imgModule->addListener(listener);    
}

MapManipulator& MapModel::getMapManipulator()
{
   return *m_impl->mapManipulator;
}

MapLibKeyHandler& MapModel::getKeyHandler()
{
   return *m_impl->keyHandler;
}

void MapModel::setMapBackend(BackendType type)
{
   m_impl->imgModule->setMapParamFactory(m_impl->factories[type]); 
}

MapProjection& MapModel::getMapProjection()
{
   return *m_impl->projection;
}

MapOperationHandler& MapModel::getMapOperationHandler()
{
   return *m_impl->mapOperationHandler; 
}

void MapModel::renderMap()
{
   m_impl->imgModule->draw();    
}

void MapModel::setMapDrawingCallback(WFAPI::MapDrawingCallback* callback)
{
   m_impl->imgModule->setMapDrawingCallback(callback);
}

ProjectionInterface& MapModel::getProjectionInterface()
{
   return *m_impl->projectionInterface; 
}

