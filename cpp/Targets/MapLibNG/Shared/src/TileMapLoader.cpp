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
#include "TileMapLoader.h"
#include "TileMapHandler.h"
#include "TileMapConfig.h"
#include "TileMapUtil.h"


#include <algorithm>

TileMapLoader::TileMapLoader() :
   m_formatDesc(NULL),
   m_prevTime(TileMapUtil::currentTimeMillis())
{
   m_startTime = m_prevTime;
}

void TileMapLoader::loadTileMap(const MC2SimpleString& descr, 
                                BitBuffer* dataBuffer)
{
   
   BufferMap::iterator itr =
      m_unloaded.find(descr);
   
   MC2_ASSERT(itr == m_unloaded.end());

   m_unloaded.insert(std::make_pair(descr, dataBuffer));
   
   // if(TileMapConfig::PROFILE_LOADING) {

   //    if(m_perfDump.find(descr) == m_perfDump.end()) {
   //       BitBuffer* copy = new BitBuffer(*dataBuffer);
   //       m_perfDump[descr] = copy;
   //    }      
   // }
}

void TileMapLoader::run()
{   
   // std::cout << "[TML]: Performing loading run, "
   //           << m_unloaded.size()
   //           << " items in queue. Items in perf-dump: "
   //           << m_perfDump.size() 
   //           << std::endl;
   
   // Wait until we have a format desc..
   if(m_formatDesc == NULL) {
      return;
   }

   m_prevTime = TileMapUtil::currentTimeMillis();
   
   // We only do a maximum of half the buffers in queue each run,
   // but at least 1.
#if defined(IPHONE) || defined(__x86_64__)
   // std::max is templated and needs the exact same type in both
   // arguments, on iPhone we need to typecast the 1 to ensure this.
   unsigned int todo = std::max( m_unloaded.size() / 2,
                                 (unsigned long) 1);
#else
   unsigned int todo = std::max( m_unloaded.size() / 2, 1U );
#endif
   // if(!m_unloaded.empty()) {
   //    std::cout << "Processing " << todo << " items" << std::endl;      
   // }
   BufferMap::iterator it = m_unloaded.begin();
   
   while (it != m_unloaded.end()) {

      performLoad(it->first, it->second);
      
      m_unloaded.erase(it++);
      
      todo--;

      if(todo == 0) {
         break;
      }
   }
}

bool isBufferLoadedFromFile(const MC2SimpleString& descr);

void TileMapLoader::performLoad(const MC2SimpleString& descr,
                                BitBuffer* bufferData)
{
   
   TileMapParams tileMapParams( descr );
   TileMap* curMap = new TileMap;

   if (!curMap->load( *bufferData, *m_formatDesc, tileMapParams ) )
   {
      // Could not load map! 
      delete curMap;
      curMap = NULL;
   }

   LoadedData loaded = { tileMapParams, descr, curMap, bufferData };

   m_loaded.push_back(loaded);
}

void TileMapLoader::setFormatDesc(TileMapFormatDesc* desc)
{
   m_formatDesc = desc; 
}

void TileMapLoader::acquire(std::vector<LoadedData>& toFill)
{
   //acquire mutex
   toFill.clear();
   toFill.swap(m_loaded);
   //release mutex
}

void TileMapLoader::profileRun()
{
   {
      for(BufferMap::iterator itr = m_perfDump.begin(); itr != m_perfDump.end(); itr++)
      {
         TileMap* curMap = new TileMap;
         curMap->load( *itr->second, *m_formatDesc, itr->first );
         itr->second->reset();
         m_toDelete.push_back(curMap);
      }      
   }

   for(std::vector<TileMap*>::iterator itr = m_toDelete.begin();
       itr != m_toDelete.end(); itr++)
   {
      delete (*itr);
   }

   m_toDelete.clear();
}

bool TileMapLoader::hasWork() const
{
   return !m_unloaded.empty(); 
}

uint32 TileMapLoader::timeSinceLastRunMillis() const
{
   return TileMapUtil::currentTimeMillis() - m_prevTime; 
}

void TileMapLoader::acquireUnloaded(BufferMap& toFill)
{
   toFill.clear();
   toFill.swap(m_unloaded);
}

bool TileMapLoader::insideStartPhase() const
{
   unsigned int timeSinceStartMillis =
      TileMapUtil::currentTimeMillis() - m_startTime;

   bool ret = timeSinceStartMillis < START_PHASE_MS;

//     if(ret) {
//        std::cout << "Still inside startup phase @ "
//                  << (timeSinceStartMillis / 1000) << "s" << std::endl;
//     } else {
//        std::cout << "outside startup phase @ "
//                  << (timeSinceStartMillis / 1000) << "s" << std::endl;
//     }

   return ret;
}

bool TileMapLoader::inLoadingQueue(const MC2SimpleString& descr) const {
   return m_unloaded.find(descr) != m_unloaded.end();
}
