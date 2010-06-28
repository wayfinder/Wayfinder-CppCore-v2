/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "ImageMap/ImageMapGridConverter.h"
#include <vector>
#include "GfxConstants.h"
#include <math.h>

using namespace std;

struct ImageMapGridConverter::Impl {
   Impl(const ImageMapGrid& grid) :
      grid(grid)
   {}

   ImageMapGrid grid;
   vector<double> invZoomRanges;
   vector<double> switchZoomRanges;
   vector<int>   gridWidths;
};

ImageMapGridConverter::ConversionDetails
ImageMapGridConverter::getConversionDetails(int index) const
{
   ConversionDetails ret = {0, 0};

   ret.scalingFactor = MAX_UINT32 / (double)m_impl->gridWidths[index];
   ret.zoomIndex = index;

   return ret;
}

int ImageMapGridConverter::worldScaleToZoomIndex(double scale) const
{
   int i = 0;

   for(i = 0; i < m_impl->switchZoomRanges.size(); i++) {
      if (scale >= m_impl->switchZoomRanges[i] ) {
         break;
      }
   }

   if(i == m_impl->switchZoomRanges.size()) {
      i--;
   }

   return i;
}

ImageMapGridConverter::ImageMapGridConverter(const ImageMapGrid& grid)
{
   m_impl = new Impl(grid);

   for(int i = 0; i < grid.getNumLevels(); i++) {
      BoundingBox p = grid.getCompleteImageGridBox(i);

      int width = (p.bottomRight.getX() - p.topLeft.getX());

      double scale = (MAX_UINT32 * GfxConstants::MC2SCALE_TO_METER) / (double)width;

      // printf("[%d] Width: %d\n", i, width);
      // printf("[%d] Scale: %lf\n", i, scale);
      // printf("[%d] Switch-scale: %lf\n", i, scale / sqrt(2));
      m_impl->invZoomRanges.push_back(scale);
      m_impl->switchZoomRanges.push_back(scale / sqrt(2));
      m_impl->gridWidths.push_back(width);
   }
}

ImageMapGridConverter::~ImageMapGridConverter()
{
   delete m_impl;
}


GridBox
ImageMapGridConverter::worldToGrid(const BoundingBox& worldBox, double scale) const
{
   // cout << "worldToGrid: " << worldBox << " @ " << scale << endl;
   ConversionDetails t = getConversionDetails(worldScaleToZoomIndex(scale));

   UPoint topLeft = worldBox.topLeft;
   UPoint bottomRight = worldBox.bottomRight;

   topLeft.getX() =
      static_cast<int>(rint(topLeft.getX() / t.scalingFactor));
   topLeft.getY() =
      static_cast<int>(rint(topLeft.getY() / t.scalingFactor));
   bottomRight.getX() =
      static_cast<int>(rint(bottomRight.getX() / t.scalingFactor));
   bottomRight.getY() =
      static_cast<int>(rint(bottomRight.getY() / t.scalingFactor));

   GridBox res = { BoundingBox(topLeft, bottomRight), t.zoomIndex };

   // printf("%20.20f\n", t.scalingFactor);
   // cout << "worldBox" << worldBox << endl;
   // cout << "imageGridBox: " << res.box << endl;

   return res;
}

BoundingBox ImageMapGridConverter::gridToWorld(GridBox res)
{
   // cout << "gridToWorld: " << res << endl;
   ConversionDetails t = getConversionDetails(res.zoomIndex);

   UPoint topLeft = res.box.topLeft;
   UPoint bottomRight = res.box.bottomRight;

   // cout << "Translation factor for zoom " << res.zoomIndex << " is: "
   //      << t.scalingFactor << endl;

   topLeft.getX() = static_cast<int>((topLeft.getX() * t.scalingFactor));
   topLeft.getY() = static_cast<int>((topLeft.getY() * t.scalingFactor));
   bottomRight.getX() =
      static_cast<int>((bottomRight.getX() * t.scalingFactor));
   bottomRight.getY() =
      static_cast<int>((bottomRight.getY() * t.scalingFactor));



   return BoundingBox(topLeft, bottomRight);
}

