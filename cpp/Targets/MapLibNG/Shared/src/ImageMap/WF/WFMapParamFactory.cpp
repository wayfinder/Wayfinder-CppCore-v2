/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "ImageMap/WF/WFMapParamFactory.h"
#include "ImageMap/ImageMapGrid.h"
#include "ImageMap/ImageMapGridConverter.h"

#define FORMAT_STR "LMMap?x=%d&y=%d&zoom=%d"

struct WFMapParamFactory::Impl {
   Impl() :
      // The following numbers specify the WF grid division
      grid(15, 1,
           -1280, -1280, 1024, 1024,
           256, 2),
      converter(grid)
   {}
   
   ImageMapGrid grid;
   ImageMapGridConverter converter;
};

WFMapParamFactory::WFMapParamFactory()
   : MapParamFactory()
{
   m_impl = new Impl;
}

WFMapParamFactory::~WFMapParamFactory()
{
   delete m_impl;
}

MapParam WFMapParamFactory::getParamFromString(const MC2SimpleString& string)
{
   int x, y, zoomIndex;
   
   sscanf(string.c_str(), FORMAT_STR,
          &x, &y, &zoomIndex);
   
   zoomIndex--;

   return MapParam(zoomIndex,
                   getWorldBoxForParam(x, y, zoomIndex),
                   string,
                   x,
                   y);
}

BoundingBox WFMapParamFactory::getWorldBoxForParam(int x, int y, int zoomIndex)
{
   int gz = m_impl->grid.getGridSize();

   BoundingBox b(UPoint(x, y + gz),
                 UPoint(x + gz, y));
   
   // cout << "B before getMercatorBox: " << b << endl;
   GridBox res = {b, zoomIndex};

   // B before getMercatorBox: [(0,65792),(256,65536)]
      
   BoundingBox worldBox = m_impl->converter.gridToWorld(res);
   
   return worldBox;    
}

void WFMapParamFactory::getParamsFromWorldBox(std::vector<MapParam>& out,
                                              const BoundingBox& bbox,
                                              double worldScale)
{
   // cout << "Building params for box: " << bbox << endl;
   
   GridBox t =
      m_impl->converter.worldToGrid(bbox, worldScale);

   t.box.topLeft.getY()--;
   t.box.bottomRight.getX()--;
   
   // cout << "Corresponding imggrid-box is: " << t.box << endl;
  
   // MC2Coordinate mc2Coord1 =
   //    MC2Coordinate(MercatorCoored(bbox.topLeft.getY(), bbox.topLeft.getX()));
   // MC2Coordinate mc2Coord2 =
   //    MC2Coordinate(MercatorCoord(bbox.bottomRight.getY(),
   //    bbox.bottomRight.getX()));

   
   // cout << "Corresponding MC2Coordinate box is: " << mc2Coord1 << ", " << mc2Coord2 << endl;

   BoundingBox b = m_impl->grid.getCompleteImageGridBox(t.zoomIndex);
   
   int gz = m_impl->grid.getGridSize();
   
   int firstGridX = (t.box.topLeft.getX() / gz) * gz;
   int lastGridX = (t.box.bottomRight.getX() / gz) * gz;
   int firstGridY = (t.box.bottomRight.getY() / gz) * gz;
   int lastGridY = (t.box.topLeft.getY() / gz) * gz;
   
   // PRINT_VAR(firstGridX);
   // PRINT_VAR(lastGridX);
   // PRINT_VAR(firstGridY);
   // PRINT_VAR(lastGridY);
   
   char buf[255];
   
   for(int x = firstGridX; x <= lastGridX; x+=gz) {
      for(int y = firstGridY; y <= lastGridY; y+=gz) {
         sprintf(buf, FORMAT_STR, x, y, t.zoomIndex + 1);

         MapParam p(t.zoomIndex,
                    getWorldBoxForParam(x, y, t.zoomIndex),
                    MC2SimpleString(buf),
                    x,
                    y);
         
         // char buf2[355];
         // sprintf(buf2, "wget -O %d_%d.png \"http://dt2:12211/%s\"",
         //         x, y, buf);

         // printf("%s\n", buf2);
         out.push_back(p);
// #if 0
//          BoundingBox tmpBox = getBoundingBox(p);
//          cout << "Snapped imggrid-box is: " << tmpBox << endl;
        
//          MC2Coordinate mc2CoordTL = MC2Coordinate(MercatorCoord(tmpBox.topLeft.getY(), tmpBox.topLeft.getX()));
//          MC2Coordinate mc2CoordBR = MC2Coordinate(MercatorCoord(tmpBox.bottomRight.getY(), tmpBox.bottomRight.getX()));
//          cout << "Snapped corresponding MC2Coordinate box is: " << mc2CoordTL << ", " << mc2CoordBR << endl;
//          WFAPI::WGS84Coordinate wgsTL = WFAPI::WGS84Coordinate(mc2CoordTL);
//          WFAPI::WGS84Coordinate wgsBR = WFAPI::WGS84Coordinate(mc2CoordBR);

//          cout << "Snapped corresponding WGS84 box is: " << wgsTL << ", " << wgsBR << endl;
//          coreprintln("http://pafciu17.dev.openstreetmap.org/?module=map&bbox=%f,%f,%f,%f&width=255", 
//                      wgsTL.lonDeg, wgsTL.latDeg, wgsBR.lonDeg, wgsBR.latDeg);
//          cout << p << endl;
// #endif 
         
      }
   }
}

int WFMapParamFactory::worldScaleToZoomIndex(double worldScale)
{
   return m_impl->converter.worldScaleToZoomIndex(worldScale);
}

const char* WFMapParamFactory::getTestHostAndPort()
{
   return "xml-1.services.wayfinder.com:12211";
}
