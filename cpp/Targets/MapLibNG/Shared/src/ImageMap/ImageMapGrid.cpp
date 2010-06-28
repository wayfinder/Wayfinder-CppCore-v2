/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "config.h"
#include "ImageMap/ImageMapGrid.h"
#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;

struct ImageMapGrid::Impl {
   vector<BoundingBox> ranges;

   /**
    *  Our map projection has its center at origo(0,).
    *  For image map grid that do not, this offset
    *  should be subtracted in order to map correctly.
    */ 
   vector<int> origoOffsets;
   
   int gridSize;
};


bool ImageMapGrid::operator==(const ImageMapGrid& rhs) const
{
   return
      m_impl->ranges == rhs.m_impl->ranges &&
      m_impl->gridSize == rhs.m_impl->gridSize;
}

ImageMapGrid::ImageMapGrid(const ImageMapGrid& rhs)
{
   m_impl = new Impl;
   m_impl->ranges = rhs.m_impl->ranges;
   m_impl->gridSize = rhs.m_impl->gridSize;
}

ImageMapGrid::ImageMapGrid(int zoomInLevel,
                           int zoomOutLevel,
                           int minX, int minY,
                           int maxX, int maxY,
                           int gridSize,
                           int subfactor)
{
   m_impl = new Impl;
   m_impl->gridSize = gridSize;
   
   int upperLeftX = minX;
   int bottomRightX = maxX + gridSize;

   // Start by assuming y-axis points up
   int upperLeftY = maxY + gridSize;
   int bottomRightY = minY;

   assert(upperLeftX <= 0);   

   assert(zoomOutLevel < zoomInLevel);
   
   for(int i = zoomOutLevel; i <= zoomInLevel; i++) {
      BoundingBox box(UPoint(upperLeftX, upperLeftY),
                      UPoint(bottomRightX, bottomRightY));
      
      m_impl->ranges.push_back(box);
      upperLeftX*=subfactor;
      bottomRightX*=subfactor;
      upperLeftY*=subfactor;
      bottomRightY*=subfactor;
   }
}

ImageMapGrid::~ImageMapGrid()
{
   delete m_impl; 
}

BoundingBox ImageMapGrid::getCompleteImageGridBox(int level) const
{
   MC2_ASSERT(level >= 0 && level < m_impl->ranges.size());
   
   return m_impl->ranges[level];
}

int ImageMapGrid::getNumLevels() const
{
   return m_impl->ranges.size();
}

int ImageMapGrid::getGridSize() const
{
   return m_impl->gridSize;
}

