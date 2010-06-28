/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <config.h>

#define CORE_LOGGING_MODULE_NAME "BufferRequesterChain"
// #define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE
#include "CoreDebugPrint.h"

#include "BufferRequester/BufferRequesterChain.h"
#include "BufferRequester/MemCacheSettings.h"
#include "BufferRequester/DiskCacheSettings.h"
#include "BufferRequester/HttpReqSettings.h"
#include "BufferRequester/ResourceDirectorySettings.h"
#include "DirectoryDBufRequester.h"
#include "DBufRequester.h"
#include "NetDBufRequester.h"
#include "MemoryDBufRequester.h"
#include "MultiFileDBufRequester.h"
#include "MemTracker.h"
#include <vector>
#include "PlatformFactory.h"
#include "PlatformMFDBufRequester.h"
#include "HttpDBufConnection.h"

class FileHandlerContext;

using namespace pal;

struct BufferRequesterChain::Impl {
   Impl(TileMapToolkit* toolkit,
        FileHandlerContext* fileContext) :
      toolkit(toolkit),
      fileContext(fileContext),
      memTracker(1024*1024, 1024*1024)
   {
      root = NULL;
      memTracker.setToolkit(toolkit);
   }
   TileMapToolkit* toolkit;
   FileHandlerContext* fileContext;
   DBufRequester* root;
   MemTracker memTracker;
};

DBufRequester* BufferRequesterChain::getRequesterOfExactType( wf_uint32 typeMask ) {
   vector<DBufRequester*> requesters;
   getRoot()->getAllRequesters( requesters );
   
   for ( vector<DBufRequester*>::const_iterator it = requesters.begin();
         it != requesters.end();
         ++it ) {
      if ( WFAPI::wf_uint32( (*it)->getType() ) == typeMask ) {
         return *it;
      }
   }
   
   return NULL;
}


BufferRequesterChain::~BufferRequesterChain()
{
   vector<DBufRequester*> requesters;
   getRoot()->getAllRequesters( requesters );
   
   for ( vector<DBufRequester*>::iterator it = requesters.begin();
         it != requesters.end();
         ++it )
   {
      delete (*it);
   }
   
   requesters.clear();
   
   delete m_impl->fileContext;
   delete m_impl;
}

BufferRequesterChain::BufferRequesterChain(
   TileMapToolkit* toolkit,
   WFAPI::DBufConnection* netConnection,
   const MemCacheSettings& memCacheSettings,
   const DiskCacheSettings& diskCacheSettings,
   const ResourceDirectorySettings& resDirSettings)
{
   m_impl = new Impl(toolkit, PlatformFactory::createFileHandlerContext());
   
   // The order should be (from the bottom)
   // * Internet
   // * Read/write multifile cache
   // * Read-only  multifile caches
   // * Single file caches
   // * Memory cache
   // * Internal requester in TileMapHandler.

   // Internet
      
   if(netConnection) {
      // If we have a network connection, create a buffer requester for it which
      // will reside at the top of the request chain
      m_impl->root = new NetDBufRequester(netConnection);
   } else {
      // Otherwise, just put dead end.
      m_impl->root = new DBufRequester;
   }

   setMemCacheSettings(memCacheSettings);
   setDiskCacheSettings(diskCacheSettings);
   setResourceDirectorySettings(resDirSettings);
}

bool BufferRequesterChain::addPreloadedMapsCache(const char* fileName)
{
   return false;
}

void BufferRequesterChain::clearMemoryCache()
{
   DBufRequester* req = getRequesterOfExactType( OriginType::MEMORY );
   
   if ( req != NULL ) {
      static_cast<MemoryDBufRequester*>(req)->clearCache();
   } else {
      mc2dbg << "[BufferRequesterChain::clearMemoryCache]: Couldn't find mem cache" << endl;
   } 
}

void BufferRequesterChain::clearReadWriteCache()
{
   // Get requester that is permanent, but not READ_ONLY
   DBufRequester* req = getRequesterOfExactType( OriginType::READ_WRITE_CACHE );
   if ( req != NULL ) {
      static_cast<MultiFileDBufRequester*>(req)->clearCache();
   } else {
      mc2dbg << "[BufferRequesterChain::setDiskCacheSize]: Couldn't find dsk cache" << endl;
   }  
}

DBufRequester* BufferRequesterChain::getRoot()
{
   return m_impl->root;
}

int BufferRequesterChain::addReadWriteCache(const char* path,
                                            const char* uin,
                                            WFAPI::wf_uint32 maxSize,
                                            int read_only )
{
   return 0;
}

bool
BufferRequesterChain::removeRequester( DBufRequester* req )
{
   if(!req) {
      coreprintln("removeRequester bailing, req == NULL");
      return false;
   }

   if(req == getRoot()) {
      DBufRequester* toRemove = getRoot();      
      m_impl->root = toRemove->getParent();
      delete toRemove;
   } else {
      vector<DBufRequester*> requesters;
      getRoot()->getAllRequesters( requesters );

      vector<DBufRequester*>::iterator childit = requesters.end();
      for ( vector<DBufRequester*>::iterator it = requesters.begin();
            it != requesters.end();
            ++it ) {
         if ( (*it)->getParent() == req ) {
            childit = it;
            // Found the one to be removed.
            break;
         }
      }

      if ( childit == requesters.end() ) {
         coreprintln("Failed to find requester! (examined %u requesters)\n", requesters.size());
         return false;
      }

      DBufRequester* toRemove = (*childit)->getParent();
      coreprintln("Removing %p of type %s", toRemove->toString());
      (*childit)->setParent( (*childit)->getParent()->getParent() );
   
      // Prevent parents from being deleted
      toRemove->setParent( NULL );
      delete toRemove;
   }
   
   // Done
   return true;
}

void BufferRequesterChain::setMemCacheSettings(const MemCacheSettings& memCacheSettings)
{
   
   if(memCacheSettings.getMaxSize() == 0)  {
      coreprintln("Memory cache size 0, attempting to remove cache");
      // If we have a memory cache, we should now remove it.
      // Otherwise, getRequesterOfExactType will return NULL, and
      // removeRequester will exit directly
      
      removeRequester(getRequesterOfExactType( OriginType::MEMORY ));
      coreprintln("Done removing cache");
   } else {
      DBufRequester* req =
         getRequesterOfExactType( OriginType::MEMORY );

      if(!req) {
         coreprintln("No memory cache set, creating new.");
         MemoryDBufRequester* memReq = new MemoryDBufRequester(NULL,
                                                               memCacheSettings.getMaxSize());

         if(m_impl->root) {
            memReq->setParent(m_impl->root);
         }

         m_impl->root = memReq;
      } else {
         coreprintln("Updating memory cache max size.");
         static_cast<MemoryDBufRequester*>(req)->setMaxSize(
            memCacheSettings.getMaxSize());         
      }
   }
}

void
BufferRequesterChain::setResourceDirectorySettings(
   const ResourceDirectorySettings& resDirSettings)
{
   if(resDirSettings.getPath() == "")  {
      coreprintln("Resource dir empty, attempting to remove cache");
      // If we have a memory cache, we should now remove it.
      // Otherwise, getRequesterOfExactType will return NULL, and
      // removeRequester will exit directly
      
      removeRequester(getRequesterOfExactType( OriginType::RESOURCE_DIRECTORY ));
   } else {
      DBufRequester* memReq =
         getRequesterOfExactType( OriginType::MEMORY );

      DirectoryDBufRequester* dirReq = new DirectoryDBufRequester(
         resDirSettings.getPath().c_str(),
         resDirSettings.getWriteable(),
         m_impl->toolkit);

      DBufRequester* oldParent = memReq->getParent();
      memReq->setParent(dirReq);
      dirReq->setParent(oldParent);
   }
}

void BufferRequesterChain::setDiskCacheSettings(const DiskCacheSettings& diskCacheSettings)
{
   if(diskCacheSettings.getMaxSize() == 0)  {
      coreprintln("Disk cache size 0, attempting to remove cache");
      // If we have a memory cache, we should now remove it.
      // Otherwise, getRequesterOfExactType will return NULL, and
      // removeRequester will exit directly
      
      removeRequester(getRequesterOfExactType( OriginType::READ_WRITE_CACHE ));
   } else {
      DBufRequester* req =
         getRequesterOfExactType( OriginType::READ_WRITE_CACHE );

      if(!req) {
         DBufRequester* memReq =
            getRequesterOfExactType( OriginType::MEMORY );

         coreprintln("No disk cache set, creating new @ %s",
                     diskCacheSettings.getPath().c_str());
         
         // Create new MultifileDBufRequester with the parent of the
         // new child as parent.
         MultiFileDBufRequester* mfdbr =
            new PlatformMFDBufRequester(m_impl->fileContext,
                                        m_impl->toolkit,
                                        NULL,
                                        diskCacheSettings.getPath().c_str(),
                                        &m_impl->memTracker,
                                        diskCacheSettings.getMaxSize(),
                                        false );

         
         if(memReq) {
            // Take memReqs parent and make it mfdbrs parent
            // Set memReq parent to mfdbr
            mfdbr->setParent(memReq->getParent());
            memReq->setParent(mfdbr);
         } else if(m_impl->root) {
            // No memory requester, but m_root
            //
            // Set root to mfdbr. Mfdbrs parent is old m_root
            //            
            mfdbr->setParent(m_impl->root);
            m_impl->root = mfdbr;
         } else {
            // No requester at all.
            m_impl->root = mfdbr;
         }
      } else {
         coreprintln("Updating memory cache max size.");
         static_cast<MultiFileDBufRequester*>(req)->setMaxSize(
            diskCacheSettings.getMaxSize());         
      }
   }
}

void BufferRequesterChain::printChain()
{
   
   vector<DBufRequester*> requesters;
   coreprintln("Root is: %p", getRoot());
   
   getRoot()->getAllRequesters( requesters );

   coreprintln("Request chain: ");


   
   vector<DBufRequester*>::iterator childit = requesters.end();
   for ( vector<DBufRequester*>::iterator it = requesters.begin();
         it != requesters.end();
         ++it )
   {
      DBufRequester* req = *it;
      
      coreprintln("[%p] %16s/%s. Parent: %p",
                  req, req->toString(), OriginType::toString(req->getType()),
                  req->getParent());
   }
}

void BufferRequesterChain::setHttpReqSettings(const HttpReqSettings& reqSettings)
{
   DBufRequester* req = getRequesterOfExactType( OriginType::EXTERNAL );
   
   if(req) {
      NetDBufRequester* hReq =
         dynamic_cast<NetDBufRequester*>(req);
      
      if(hReq) {
         hReq->setForceGet(reqSettings.getForceGet());
         
         HttpDBufConnection* conn =
            dynamic_cast<HttpDBufConnection*>(
               hReq->getDBufConnection());
         
         if(conn) {
            conn->setPostDirectory(reqSettings.getPostDirectory());
            if(reqSettings.getHostAndPort()) {
               conn->setHostAndPort(reqSettings.getHostAndPort());
            }
         }
      }
   }
}

