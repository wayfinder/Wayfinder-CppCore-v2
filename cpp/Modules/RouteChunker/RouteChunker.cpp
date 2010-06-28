/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* A debugging module to load a route from a file and send it to
 * NavTask the same way NavServerCom does.
 */

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdlib.h>


#include "arch.h"
#include "RouteChunker.h"

#define DEBUG(...)  m_log->debug   (__VA_ARGS__)
#define INFO(...)   m_log->info    (__VA_ARGS__)
#define WARN(...)   m_log->warning (__VA_ARGS__)
#define ERROR(...)  m_log->error   (__VA_ARGS__)

#undef  DEBUG
#define DEBUG(...)


namespace isab {


   RouteChunker::RouteChunker(const char *filename)
      : Module("RouteChunker")
   {
      DEBUG("RouteChunker::RouteChunker\n");

      m_navTaskProvider = new NavTaskProviderPublic(this);
      m_timerId = m_queue->defineTimer(2000);
      m_filename = new char[strlen(filename)+1];
      strcpy(m_filename, filename);
   }


   MsgBuffer * RouteChunker::dispatch(MsgBuffer *buf)
   {
      DEBUG("RouteChunker::dispatch : %i\n", buf->getMsgType());
      buf=m_ctrlHubAttachedDecoder.dispatch(buf, this);
      if (!buf)
         return buf;
      buf=Module::dispatch(buf);
      return buf;
   }


   CtrlHubAttachedPublic * RouteChunker::newPublicCtrlHub()
   {
      DEBUG("newPublicCtrlHub()\n");
      return new CtrlHubAttachedPublic(m_queue);

   }


   void RouteChunker::decodedExpiredTimer(uint16 timerid)
   {
      uint8 *addr;
      uint8 *p;
      int fd;
      struct stat statbuf;
      uint32 length;

      if (timerid != m_timerId) {
         Module::decodedExpiredTimer(timerid);
         return;
      }
      DEBUG("Opening route\n");
      fd = open(m_filename, O_RDONLY);
      if (fd < 0) {
         perror("Failed to open route file");
         exit(1);
      }
      fstat(fd, &statbuf);
      length = statbuf.st_size;
      addr = (uint8 *)(mmap(NULL, length, PROT_READ, MAP_SHARED, fd, 0));
      if (addr == MAP_FAILED) {
         perror("Failed to mmap route");
      }
      INFO("File %s  was successfully mmap:ed", m_filename);

      p = addr;
      PacketHeader hdr;
      hdr.stx      = *p++;
      DEBUG("4 bytes at p: %0#hhx %0#hhx %0#hhx %0#hhx", *p, p[1], p[2], p[3]);
      DEBUG("p read as uint32*: %0#10x", *reinterpret_cast<uint32*>(p));
      hdr.length   = ntohl(*reinterpret_cast<uint32*>(p)); p += 4;
      hdr.protover = *p++;
      hdr.type     = ntohs(*reinterpret_cast<uint16*>(p)); p += 2;
      hdr.reqId    = *p++;
      hdr.crc      = ntohs(*reinterpret_cast<uint16*>(p)); p += 2;
      hdr.navId    = ntohl(*reinterpret_cast<uint32*>(p)); p += 4;
      DEBUG("Stored %d bytes in PacketHeader. Should be %d bytes.", p - addr,
          3*sizeof(uint8) + 2*sizeof(uint16) + 2*sizeof(uint32));
      DEBUG("hdr::stx      : 0x%02hhx", hdr.stx);
      DEBUG("hdr::length   : 0x%08x     (file length : 0x%08x)", hdr.length, length);
      DEBUG("hdr::protover : 0x%02hhx", hdr.protover);
      DEBUG("hdr::type     : 0x%04hx", hdr.type);
      DEBUG("hdr::reqId    : 0x%02hhx", hdr.reqId);
      DEBUG("hdr::crc      : 0x%04hx", hdr.crc);
      DEBUG("hdr::navId    : 0x%08x", hdr.navId);

      uint8 requestStatus = *p++;
      INFO("request status      : 0x%0hhx", requestStatus);

      if (hdr.length > length) {
         WARN("hdr.length > length!!!!");
      }
      length = hdr.length;
      length -= 16;
      m_navTaskProvider->newRoute(0, 0, length, addr);
      while (length) {
         int chunkSize = (length < MaxChunkSize ) ? length : MaxChunkSize;
         DEBUG("  Sending chunk len %i, pos %i, remining %i", chunkSize, p-addr, length);
         m_navTaskProvider->routeChunk(false, chunkSize, p);
         p+=chunkSize;
         length-=chunkSize;
      }

   }


} /* namespace isab */

