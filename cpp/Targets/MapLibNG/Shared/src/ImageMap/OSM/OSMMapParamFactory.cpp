/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "ImageMap/OSM/OSMMapParamFactory.h"
#include "ImageMap/ImageMapGrid.h"
#include "ImageMap/ImageMapGridConverter.h"

#define FORMAT_STR "%d/%d/%d.png"

struct OSMMapParamFactory::Impl {
   Impl() :
      // The following numbers specify the WF grid division
      grid(18, 0,
           -128, -128, -128, -128,
           256, 2),
      converter(grid)
      {}
   
   ImageMapGrid grid;
   ImageMapGridConverter converter;
};

OSMMapParamFactory::OSMMapParamFactory()
   : MapParamFactory()
{
   m_impl = new Impl;
}

OSMMapParamFactory::~OSMMapParamFactory()
{
   delete m_impl;
}

MapParam OSMMapParamFactory::getParamFromString(const MC2SimpleString& string)
{
   int x, y, zoomIndex;

   sscanf(string.c_str(), FORMAT_STR, &zoomIndex, &x, &y);
   
   return MapParam(zoomIndex,
                   getWorldBoxForParam(x, y, zoomIndex),
                   string,
                   x,
                   y);
}

BoundingBox OSMMapParamFactory::getWorldBoxForParam(int x, int y, int zoomIndex)
{
   int gz = m_impl->grid.getGridSize();

   BoundingBox b(UPoint(x * gz, y * gz + gz),
                 UPoint(x * gz + gz, y * gz));

   // cout << "b initial: " << b << endl;
   
   GridBox res = {b, zoomIndex};

   invInvertGridBoxYAxis(res);

   // cout << "b after invaxis: " << b << endl;
   
   invOffsetGridBox(res.box, res.zoomIndex);

   // cout << "b after offset: " << b << endl;
   
   BoundingBox worldBox = m_impl->converter.gridToWorld(res);

   // cout << "b after projection: " << worldBox << endl;
   
   return worldBox;    
}

GridBox OSMMapParamFactory::getGridBox(const BoundingBox& bbox, double worldScale)
{
   return m_impl->converter.worldToGrid(bbox, worldScale);
}

UPoint OSMMapParamFactory::getTranslationOffsets(int zoomIndex)
{
   BoundingBox wholeBox = m_impl->grid.getCompleteImageGridBox(zoomIndex);

   // We increment by one since we want the complete width and not the
   // distance between two pixels
   int extentX = (1 + wholeBox.bottomRight.getX()) - wholeBox.topLeft.getX();
   int extentY = (1 + wholeBox.topLeft.getY()) - wholeBox.bottomRight.getY();
   
   int translateX = extentX / 2;
   
   int translateY = extentY / 2;

   return UPoint(translateX, translateY);
}

void OSMMapParamFactory::offsetGridBox(BoundingBox& bbox, int zoomIndex) {

   UPoint tOffsets(getTranslationOffsets(zoomIndex));
   
   bbox.topLeft.getX() += tOffsets.getX();
   bbox.topLeft.getY() += tOffsets.getY();
   bbox.bottomRight.getX() += tOffsets.getX();
   bbox.bottomRight.getY() += tOffsets.getY();
}

void OSMMapParamFactory::invOffsetGridBox(BoundingBox& bbox, int zoomIndex)
{
   UPoint tOffsets(getTranslationOffsets(zoomIndex));
   
   bbox.topLeft.getX() -= tOffsets.getX();
   bbox.topLeft.getY() -= tOffsets.getY();
   bbox.bottomRight.getX() -= tOffsets.getX();
   bbox.bottomRight.getY() -= tOffsets.getY();
}

void OSMMapParamFactory::invertGridBoxYAxis(GridBox& gb)
{
   BoundingBox wholeBox = m_impl->grid.getCompleteImageGridBox(gb.zoomIndex);
   
   int extentY = wholeBox.topLeft.getY() - wholeBox.bottomRight.getY();

   gb.box.topLeft.getY() = extentY - gb.box.topLeft.getY();
   gb.box.bottomRight.getY() = extentY - gb.box.bottomRight.getY();

   swap(gb.box.topLeft.getY(), gb.box.bottomRight.getY()); 
}

void OSMMapParamFactory::invInvertGridBoxYAxis(GridBox& gb)
{
   // Yes, this is correct. The inverse of f(x) is f(f(x))
   invertGridBoxYAxis(gb);
}

void
OSMMapParamFactory::getParamsFromGridBox(std::vector<MapParam>& out,
                                         const GridBox& box)
{
   GridBox t(box);

      
   // cout << "GridBox BO: " << t << endl;
   offsetGridBox(t.box, t.zoomIndex);
   // cout << "GridBox BI: " << t << endl;
   invertGridBoxYAxis(t);
   // cout << "GridBox DO: " << t << endl;
   
   // BoundingBox b = m_impl->grid.getCompleteImageGridBox(t.zoomIndex);
   // offsetGridBox(b, t.zoomIndex);

   t.box.topLeft.getY()--;
   t.box.bottomRight.getX()--;
   
   int gz = m_impl->grid.getGridSize();
   
   int firstGridX = (t.box.topLeft.getX() / gz);
   int lastGridX = (t.box.bottomRight.getX() / gz);
   int firstGridY = (t.box.bottomRight.getY() / gz);
   int lastGridY = (t.box.topLeft.getY() / gz);

   // cout << "firstGridX = " << firstGridX << endl;
   // cout << "firstGridY = " << firstGridY << endl;
   // cout << "lastGridX = " << lastGridX << endl;
   // cout << "lastGridY = " << lastGridY << endl;

   char buf[255];
   
   for(int x = firstGridX; x <= lastGridX; x++) {
      for(int y = firstGridY; y <= lastGridY; y++) {
         sprintf(buf, FORMAT_STR, t.zoomIndex, x, y);

         MapParam p(t.zoomIndex,
                    getWorldBoxForParam(x, y, t.zoomIndex),
                    MC2SimpleString(buf),
                    x,
                    y);
         
         out.push_back(p);         
      }
   }
}

void
OSMMapParamFactory::getParamsFromWorldBox(std::vector<MapParam>& ret,
                                          const BoundingBox& bbox,
                                          double worldScale)
{
   GridBox t = getGridBox(bbox, worldScale);
   getParamsFromGridBox(ret, t);
}

int OSMMapParamFactory::worldScaleToZoomIndex(double worldScale)
{
   return m_impl->converter.worldScaleToZoomIndex(worldScale);
}

const char* OSMMapParamFactory::getTestHostAndPort()
{
   return "b.tile.openstreetmap.org:80";
}


