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
#include "ImageMap/MapParam.h"
#include "MC2SimpleString.h"

/**
 *  Class used for geospatial storage of data.
 *  MapType must support i.e. operation getBBox() and getParam()
 */ 
template<typename MapType>
class GridStorage {
public:

   /// Data for a certain zoom level
   typedef std::vector<MapType*> GridVec;

   /// All zoom levels
   typedef std::vector<GridVec> GridVecVec;
            
   /**
    *  Constructor.
    *
    *  @param numLevels The number of zoom levels to init the storage with.
    */
   GridStorage() 
   {
      m_levels.resize(25);
   }

   /**
    *  Add a map to a certain zoom level to the storage.
    *  Figure out ownership of map later on
    *
    *  @param map The map to add.
    *  @param zoomIndex The zoom level.
    */ 
   void addMap(MapType* map, int zoomIndex) {
      if(zoomIndex >= m_levels.size()) {
         m_levels.resize(zoomIndex + 1);
      }
      
      m_levels[zoomIndex].push_back(map);
      garbageCollect();
   }

   /**
    *  Clears the entire cache and all its loaded resources.
    */ 
   void clear() {
      for(int i = 0; i < m_levels.size(); i++) {
         for(int j = 0; j < m_levels[i].size(); j++) {
            delete m_levels[i][j];
         }
         
         m_levels[i].clear();
      }
   }

   
   
   /**
    *  Get all maps covering the specified bounding box and zoom level.
    *  Parameter should perhaps be projection
    *
    *  @param bbox The bounding box.
    *  @param zoomIndex The zoom level.
    *  @return A vector with all maps covering the specified area.
    */
   GridVec getMaps(const BoundingBox& bbox, int zoomIndex) {

      GridVec ret = extractFromBBox(bbox, zoomIndex);

      int maxDepth = 7;
      int curDepth = 1;
      
      while(ret.empty() && curDepth != maxDepth) {
         ret = extractFromBBox(bbox, zoomIndex + curDepth);

         if(!ret.empty()) {
            break;
         }
         
         ret = extractFromBBox(bbox, zoomIndex - curDepth);
         curDepth++;
      }
      
      return ret;
   }

   GridVec extractFromBBox(const BoundingBox& bbox, int zoomIndex) {

      if(zoomIndex >= m_levels.size() || zoomIndex < 0) {
         return GridVec();
      }
      
      GridVec& vec = m_levels[zoomIndex];

      GridVec ret;

      for(unsigned int i = 0; i < vec.size(); i++) {
         if(vec[i]->getBBox().overlaps(bbox)) {
            ret.push_back(vec[i]);
         }
      }

      return ret;
   }

   void garbageCollect() {
      GridVec vec;

#ifdef IPHONE
      static const int MAX_IN_STORAGE = 25;
      static const int SLACK = 5;
#else
      static const int MAX_IN_STORAGE = 100;
      static const int SLACK = 50;
#endif
      

      
      for(int i = 0; i < m_levels.size(); i++) {
         for(int j = 0; j < m_levels[i].size(); j++) {
            vec.push_back(m_levels[i][j]);
         }
      }

      if(vec.size() <= (MAX_IN_STORAGE + SLACK)) {
         return;
      }

      // printf("[GridStorage]: Garbage collecting %d items..\n",
      //        SLACK);
      
      int tmp = 0;

      typename MapType::Comparator cmp;

      sort(vec.begin(), vec.end(), cmp);
      reverse(vec.begin(), vec.end());

      for(int i = MAX_IN_STORAGE; i < vec.size(); i++) {
         delete vec[i];
      }

      vec.resize(MAX_IN_STORAGE);
      
      for(int i = 0; i < m_levels.size(); i++) {
         m_levels[i].clear();
      }

      for(int i = 0; i < vec.size(); i++) {
         m_levels[vec[i]->getParam().getZoomIndex()].push_back(vec[i]);
      }
   }
   
   /**
    *  Check if a map is present by checking it's param string.
    *
    *  @param descr The param string for the map to check.
    *  @param zoomIndex The zoom level to check for.
    *  @return If the map was found in the storage.
    */
   bool hasMap(const MapParam& descr, int zoomIndex) const {
      for (unsigned int i = 0; i < m_levels[zoomIndex].size(); i++) {
         if (m_levels[zoomIndex][i]->getParam() == descr) {
            return true;
         }
      }

      return false;
   }
   
private:
   //typedef std::vector<MapType*> GridVec;
   //typedef std::vector<GridVec> GridVecVec;
   //TODO: add range configuration support   

   /// The zoom levels
   GridVecVec m_levels;
};

#endif /* GRIDSTORAGE_H */
