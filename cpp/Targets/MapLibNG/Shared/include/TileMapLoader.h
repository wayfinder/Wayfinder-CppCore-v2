/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef TILEMAPLOADER_H
#define TILEMAPLOADER_H

#include <vector>
#include "TileMapParams.h"
#include "MC2SimpleString.h"
#include <set>

class TileMapHandler;
class MC2SimpleString;
class BitBuffer;
class TileMapParams;
class TileMap;
class TileMapFormatDesc;

class TileMapLoadingListener {
public:
   virtual void tileMapLoaded(const MC2SimpleString& descr,
                              const TileMapParams& params,
                              TileMap* tileMap,
                              BitBuffer* buffer) = 0;
};


/**
 * This class loads TileMaps in a load-balanced fashion.
 * 
 */ 
class TileMapLoader {
public:
   TileMapLoader();

   void setFormatDesc(TileMapFormatDesc* desc);

   void setListener(TileMapLoadingListener* listener);
   
   void loadTileMap(const MC2SimpleString& descr,  
                    BitBuffer* dataBuffer);

   /**
    * Performs a single iteration of the TileMap loading
    */ 
   void run();

   /**
    * Performs a profiling run which is designed to load all
    * the bitbuffers that has been previously stored. This is
    * to gauge the performance of loading the tilemaps of a
    * specific scene.
    */
   void profileRun();

   typedef std::map<MC2SimpleString, BitBuffer*> BufferMap;

   struct LoadedData {
      TileMapParams params;
      MC2SimpleString descr;
      TileMap* tileMap;
      BitBuffer* buffer;
   };

   bool hasWork() const;

   /**
    * Will fill the toFill vector with the currently loaded data,
    * and clear the internal set of loaded data.
    */ 
   void acquire(std::vector<LoadedData>& toFill);

   uint32 timeSinceLastRunMillis() const;

   enum Constant { MIN_LOAD_INTERVAL_MS = 100, START_PHASE_MS = 20 * 1000 };

   void acquireUnloaded(BufferMap& toFill);

   bool insideStartPhase() const;

   bool inLoadingQueue(const MC2SimpleString& descr) const;
private:
   
   void performLoad(const MC2SimpleString& descr, BitBuffer* bufferData);

   BufferMap m_perfDump;
   std::vector<TileMap*> m_toDelete;

   BufferMap m_unloaded;
   std::vector<LoadedData> m_loaded;
   TileMapLoadingListener* m_listener;
   TileMapFormatDesc* m_formatDesc;

   uint32 m_prevTime;
   uint32 m_startTime;
};


#endif // TILEMAPLOADER_H
