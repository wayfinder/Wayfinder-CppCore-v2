/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef GRIDSTORAGE_H
#define GRIDSTORAGE_H

#include <vector>
#include "MC2BoundingBox.h"
#include "Grid/ZoomRange.h"

/**
 *  MapType must support i.e. operation getBBox();
 */ 
template<typename MapType>
class GridStorage {
public:
   GridStorage(ZoomRange& zoomRange) :
      m_zoomRange(zoomRange)
   {
      m_levels.resize(zoomRange.getNumLevels());
   }

   /**
    * Figure out ownership of map later on
    */ 
   void addMap(int level, MapType* map) {
      m_levels[level].push_back(map);
   }

   /**
    *  Parameter should perhaps be projection
    */

   // 75.843 1:1 
   std::vector<MapType*>& getMaps(const MC2BoundingBox& bbox, float level) {
      int index = m_zoomRange.getLevelForZoom(level);

      return m_levels[index];
   }
   
private:
   
   typedef std::vector<MapType*> GridVec;
   typedef std::vector<GridVec> GridVecVec;

   GridVecVec m_levels;
   ZoomRange m_zoomRange;
};

#endif /* GRIDSTORAGE_H */
