/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#define CORE_LOGGING_MODULE_NAME "ImageMapModule"
#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE
#include "Profiler.h"
#include "CoreDebugPrint.h"
#include "BitBuffer.h"
#include "GfxConstants.h"
#include "PNGImageLoader.h"
#include "ImageMap/ImageMapModule.h"
#include "ImageMap/ImageMapGrid.h"
#include "ImageMap/ImageMapGridConverter.h"
#include "BufferRequester/HttpReqSettings.h"
#include "BufferRequester/BufferRequester.h"
#include "MapDrawingCallback.h"
#include "ExternalRenderer.h"
#include "TextureBlockManager.h"
#include "TextureBlock.h"
#include "ImageMap/WF/WFMapParamFactory.h"
#include "ImageMap/OSM/OSMMapParamFactory.h"
#include "WorkDispatching/WorkPerformer.h"
#include "WorkDispatching/WorkThread.h"
#include "WorkDispatching/DirectDispatcher.h"
#include "Camera.h"
#include <algorithm>
#include <set>
#include "TileMapUtil.h"
#include "PALGraphics.h"

#define USE_THREAD_DISPATCHER
// #define DISABLE_CANCELLATION
using namespace WFAPI;

#define BACKGROUND_TEXTURE_STRING "grid.png"

struct PNGJob {
   PNGJob(BitBuffer* dataBuf,
          const MC2SimpleString& descr,
          const MapParam param,
          OriginType origin) :
      timeStamp(TileMapUtil::currentTimeMillis()),
      dataBuf(dataBuf),
      descr(descr),
      param(param),
      origin(origin),
      width(),
      height()
   {
      
   }
   unsigned int timeStamp;
   BitBuffer* dataBuf;
   pal::ResultBuffer resBuf;

   const MC2SimpleString descr;
   const MapParam param;
   OriginType origin;
   
   uint32 width, height;
   pal::pstatus status;
};

class PNGWorkPerformer : public WorkPerformer {
public:
   virtual void performWork(void* job) {
      PNGJob* pjob = static_cast<PNGJob*>(job);
      pal::GraphicsContext ctx = NULL;

      pjob->status =   
         pal::loadImageFromMemory(ctx,
                                  pjob->dataBuf->getBufferAddress(),
                                  pjob->dataBuf->getBufferSize(),
                                  pal::PNG,
                                  pal::RGBA8,
                                  0, 0,
                                  &pjob->width,
                                  &pjob->height,
                                  &pjob->resBuf);
      
   }
};

struct ImageMapModule::Impl {
   Impl(MapProjection& projection,
        MapParamFactory* factory,
        BufferRequester& bufferRequester) :
      projection(projection),
      factory(factory),
      bufferRequester(bufferRequester),
      renderer(NULL),
      drawCallback(NULL),
      gridStorage(),
      backgroundTexture(NULL),
      backgroundTextureRequested(false),
      drawn(false),
      totalReqTimeMs(0.0f),
      numReqTimes(0),
#ifdef USE_THREAD_DISPATCHER
      pngLoader(new WorkThread(&pngPerformer, 0, 5))
#else
      pngLoader(new DirectDispatcher(&pngPerformer))
#endif
   {
      // Request directly from root, not from /TMap
      bufferRequester.setHttpReqSettings(HttpReqSettings("/",
                                                         true,
                                                         factory->getTestHostAndPort()));
   }

   ~Impl() {
      delete pngLoader;
   }
   
   MapProjection& projection;   
   MapParamFactory* factory;
   BufferRequester& bufferRequester;
   ExternalRenderer* renderer;
   MapDrawingCallback* drawCallback;
   GridStorage<ImageTile> gridStorage;
   set<MapParam> reqQueue;
   TextureBlock* backgroundTexture;
   bool backgroundTextureRequested;
   bool drawn;
   PNGWorkPerformer pngPerformer;
   float totalReqTimeMs;
   int numReqTimes;
   WorkDispatcher* pngLoader;
};


void ImageMapModule::projectionUpdated(projectionEvent trigger)
{
   unsigned int startTimeMs = TileMapUtil::currentTimeMillis();
   
   // coreprintln("Received projection event: %s",
   //             MapProjectionListener::toString(trigger));   
   
//   BoundingBox box(m->projection.getBoundingBox());
   std::vector<MC2BoundingBox> drawingBoxes;
   m->projection.getDrawingBBoxes( drawingBoxes );
   
   // coreprintln("Nbr drawing boxes: %d", drawingBoxes.size());
   // for (uint32 i = 0; i < drawingBoxes.size(); ++i ) {
   //    cout << drawingBoxes[i] << endl;
   // }

   static vector<MapParam> reqParams;
   reqParams.clear();
   
   for (uint32 i = 0; i < drawingBoxes.size(); ++i ) {
      m->factory->getParamsFromWorldBox(reqParams,
                                             drawingBoxes[i],
                                             m->projection.getPixelScale());
   }

   requestParams(reqParams);

   // printf("elapsed time projectionUpdated: %u ms\n",
   //        TileMapUtil::currentTimeMillis() - startTimeMs);

   m->renderer->cameraUpdated();
   requestDraw();
}

// template<typename ContainerType>
// void printContainer(ContainerType& t, const char* name) {
//    unsigned int i = 0;

//    for(typename ContainerType::iterator itr = t.begin();
//        itr != t.end();
//        itr++,i++)
//    {
//       std::cout << name << "[" << i << "] = " << itr->getString() << endl;
//    }
// }

void ImageMapModule::requestParams(vector<MapParam>& reqParams) 
{
   Prof(reqParams);
   
   unsigned int startTimeMs = TileMapUtil::currentTimeMillis();
   
   if (m->backgroundTexture == NULL &&
       !m->backgroundTextureRequested) {
      m->backgroundTextureRequested = true;
      m->bufferRequester.requestBuffer(BACKGROUND_TEXTURE_STRING,
                                            this,
                                            CachePolicy::onlyResDir);
   }
   
   if(reqParams.empty()) {
      return;
   }

   int index = reqParams[0].getZoomIndex();

#ifdef DISABLE_CANCELLATION
   for(unsigned int i = 0; i < reqParams.size(); i++) {
      if (!m->gridStorage.hasMap(reqParams[i], index)) {
         m->reqQueue.insert(reqParams[i]);
         // printf("About to request %s\n", itr->getString().c_str());
         m->bufferRequester.requestBuffer(reqParams[i].getString(), this);
      }
   }

#else
   
   // This set will contain all the requests that are needed to
   // represent the current view.
   static set<MapParam> inserted;
   inserted.clear();

   // First, see which maps are already in the grid storage
   for(unsigned int i = 0; i < reqParams.size(); i++) {
      if (!m->gridStorage.hasMap(reqParams[i], index)) {
         inserted.insert(reqParams[i]);
      }
   }

   // inserted now represents the entire set of parameters that needs to
   // be requested to represent the current view. However, some of these
   // parameters are likely to be already requested and waited upon.
   //
   // And some are requested and waited upon, but no longer valid. The
   // loop below iterates through the request queue and compares element
   // element to inserted. If it exists in both, we are already waiting
   // for it, and nothing needs to be done. It is therefore removed from
   // inserted.
   // 
   // If it only exists in the request queue, it is not needed for the current
   // view and we can safely cancel the request.
   
   for(set<MapParam>::iterator itr = m->reqQueue.begin();
       itr != m->reqQueue.end(); )
   {
      set<MapParam>::iterator searchItr = inserted.find(*itr);
      
      if(searchItr == inserted.end()) {
         m->bufferRequester.cancelRequest(itr->getString(), this);
         m->reqQueue.erase(itr++);
      } else {
         inserted.erase(searchItr);
         itr++;
      }
   }

   if(!inserted.empty()) {   
      vector<MapParam> vec(inserted.begin(), inserted.end());
   
      random_shuffle(vec.begin(), vec.end());
   
      // Now, inserted represents the final few buffers that needs to be
      // requested. They are inserted into the request queue and the request is
      // formally performed.
      for(vector<MapParam>::iterator itr = vec.begin(); itr != vec.end();
          itr++)
      {
         m->reqQueue.insert(*itr);
         // printf("About to request %s\n", itr->getString().c_str());
         m->bufferRequester.requestBuffer(itr->getString(), this);
      }

      m->totalReqTimeMs+=(TileMapUtil::currentTimeMillis() - startTimeMs);
      m->numReqTimes++;
      // printf("Average req-time: %fms\n", m->totalReqTimeMs / m->numReqTimes);
      if(m->numReqTimes == 20) {
         m->numReqTimes = 0;
         m->totalReqTimeMs = 0;
      }
      
   }
#endif // DISABLE_CANCELLATION
}

void ImageMapModule::draw()
{
   static vector<MapParam> paramsAtRightZoomlevel;
   static std::vector<MC2BoundingBox> drawingBoxes;

   {
      Prof(graySetup);
      paramsAtRightZoomlevel.clear();
      drawingBoxes.clear();

      if (m->backgroundTexture != NULL) {
         m->projection.getDrawingBBoxes( drawingBoxes );
   
         for (uint32 i = 0; i < drawingBoxes.size(); ++i ) {
            m->factory->getParamsFromWorldBox(paramsAtRightZoomlevel,
                                              drawingBoxes[i],
                                              m->projection.getPixelScale());
         }
      }
   }
   
   if(m->renderer) {
      Prof(drawMap);
   
      // printf("ImageMapModule::draw()\n");
      uint32 curTimeStamp = TileMapUtil::currentTimeMillis();


      // printf("Current scale: %lf\n",
      //        m->projection.getPixelScale());
   

      m->renderer->beginFrame();
      m->renderer->clearScreen();
      // m->renderer->cameraUpdated();
      m->renderer->setupPerspectiveProjection();
         
      m->renderer->offsetPerspective(MC2Coordinate(0, 0));

      int zoomIndex = m->factory->worldScaleToZoomIndex(
         m->projection.getPixelScale());

      MC2BoundingBox bbox = m->projection.getBoundingBox();
      
      vector<ImageTile*> tiles = m->gridStorage.getMaps(
         BoundingBox(bbox), zoomIndex);

      // coreprintln("About to draw %u tiles from zoom index %d", tiles.size(), zoomIndex);

//////////
      // Get the params for the right zoom level and draw them as background.
      // FIXME: Only draw background tiles when needed.

      unsigned int numGray = 0;
      unsigned int numNormal = 0;

      m->renderer->enableImageTileState();

      if(m->backgroundTexture) {
         m->renderer->setImageTileTexture(m->backgroundTexture);
                                       
         for (unsigned int i = 0; i < paramsAtRightZoomlevel.size(); ++i) {
            m->renderer->drawImageTile(paramsAtRightZoomlevel[i].getWorldBox());
            numGray++;
         }
      }
      
      for (uint32 i = 0; i < tiles.size(); ++i)  {
         tiles[i]->updateTimeStamp(curTimeStamp);
         m->renderer->setImageTileTexture(static_cast<TextureBlock*> (tiles[i]->getBitMap()));
         m->renderer->drawImageTile(tiles[i]->getParam().getWorldBox());
         numNormal++;
      }

      m->renderer->disableImageTileState();
         
      // printf("Drew %u gray and %u normal\n", numGray, numNormal);
         
      m->renderer->resetOffsetPerspective();

//       // m->renderer->drawBBox(bbox);
      
      m->renderer->setupOrthographicProjection();      

      // m->renderer->drawHorizon(
      //    m->projection.getCamera().getHorizonHeight() );
      
      m->renderer->endFrame();
      m->drawn = true;      
   } else {
      coreprintln("Warning: no external renderer set, skipping draw");
   }
}

ImageMapModule::ImageMapModule(MapProjection& projection,
                               MapParamFactory* factory,
                               BufferRequester& bufferRequester,
                               ExternalRenderer* renderer)
   : m(new Impl(projection, factory, bufferRequester))
{
   projection.setProjectionType(MapProjection::MERCATOR_PROJECTION);
   m->renderer = renderer;
   
   projection.addListener(this);
   projection.setPixelScale(15000.0f);
}

void ImageMapModule::requestReceived(const MC2SimpleString& descr,
                                     BitBuffer* dataBuffer,
                                     OriginType origin)
{
   // coreprintln("Recv %p (size %u) / %s and origin %s",
   //             dataBuffer,
   //             dataBuffer ? dataBuffer->getBufferSize() : 0,
   //             descr.c_str(),
   //             OriginType::toString(origin));
   
   if (dataBuffer == NULL) {
      // coreprintln("Received NULL buffer.");
      requestDraw();
      
      return;
   }

   PNGJob* pjob = NULL;
                             
   if (descr == BACKGROUND_TEXTURE_STRING) {
      printf("Received background texture.\n");
      
      pjob = new PNGJob(dataBuffer,
                        descr,
                        MapParam(),
                        origin);
   } else {
      MapParam p = m->factory->getParamFromString(descr);

      set<MapParam>::iterator itr = m->reqQueue.find(p);

      if(itr == m->reqQueue.end()) {
         //TODO: Fix this, appears since we do release below.
         //TODO: Should never happen.
      
         m->bufferRequester.release(descr, dataBuffer);
      
         requestDraw();
         return;
      }

      m->reqQueue.erase(itr);

      pjob = new PNGJob(dataBuffer, descr, p, origin);
   }

   m->pngLoader->scheduleWork(this, pjob);         
}

void ImageMapModule::setMapParamFactory(MapParamFactory* factory)
{
   m->gridStorage.clear();
   // Request directly from root, not from /TMap
   m->bufferRequester.setHttpReqSettings(
      HttpReqSettings("/", true, factory->getTestHostAndPort()));
   m->factory = factory;
   projectionUpdated(MapProjectionListener::UNKNOWN);
}

void ImageMapModule::requestDraw()
{
   if(m->drawCallback && m->drawn) {
      m->drawn = false;
      m->drawCallback->mapIsReadyToBeDrawn();
   }
}

void ImageMapModule::setMapDrawingCallback(WFAPI::MapDrawingCallback* callback)
{   
   m->drawCallback = callback; 
}

void ImageMapModule::onWorkDone(void* work)
{
   PNGJob* pjob = static_cast<PNGJob*>(work);
   
   // cout << tmpBox << endl;

   const MapParam& p = pjob->param;

   TextureBlock* texture = NULL;
   TextureBlockManager* texMgr = m->renderer->getTextureBlockManager();
   
   if(pjob->status == PAL_OK) {
      // printf("File loaded from thread, roundtrip = %u ms\n",
      //        TileMapUtil::currentTimeMillis() - pjob->timeStamp);
      
      texture = texMgr->allocateFromRawData(
         pjob->resBuf.getData(), pjob->width, pjob->height);
   } else {
      // Load it using PNGImageLoader instead.
      PNGImageLoader pngLoader;
      pngLoader.setParameters(pjob->dataBuf->getBufferAddress(),
                              pjob->dataBuf->getBufferSize());
      RGBA32BitMap tmpBmp;

      int err = pngLoader.load(tmpBmp);

      if(err == 0) {
         texture = texMgr->allocateFromBitMap(tmpBmp);
      } else {
         // Give up
         printf("PNG failed to load, error code = %d\n", err);
         texture = NULL;
      }
   }      

   if (texture) {
      if(p.isValid()) {
         ImageTile* tile = new ImageTile(texture, p.getWorldBox(), p);
         // coreprintln("Adding map to zoom index: %d", p.getZoomIndex());
         m->gridStorage.addMap(tile, p.getZoomIndex());               
      } else if (pjob->descr == BACKGROUND_TEXTURE_STRING) {
         m->backgroundTexture = texture;
         m->backgroundTextureRequested = false;
         
         if (m->backgroundTexture == NULL) {
            printf("Failed to load background texture\n");
         }
      }
   }

   // Done with loading the buffer->tile, release it
   m->bufferRequester.release(p.getString(), pjob->dataBuf);

   delete pjob;
   
   // coreprintln("ImageTile created: %p", tile);
   
   if(m->reqQueue.empty()) {      
      notifyMapCompletelyLoaded();
   }

   requestDraw(); 
}




