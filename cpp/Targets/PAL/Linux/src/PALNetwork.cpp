/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "PALNetwork.h"
#include <string.h>
#include <stdio.h>
#include <set>

// Includes for creating a socket.
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#define CORE_LOGGING_MODULE_NAME "PALNETWORK"
#define CORE_LOGGING_MODULE_LEVEL CL_ALL 

#include "CoreDebugPrint.h"

namespace pal {

struct NetworkCallbackFunction {
   NetworkCallbackFunction(NetworkCallbackFunc cf, void* d) : 
      callbackFunc(cf), data(d) {}
   
   NetworkCallbackFunc callbackFunc;
   void* data;
};

struct LinuxNetworkContext {
   LinuxNetworkContext() :
      networkState(UNINITIALIZED)
      {}
   
   NetConnectionState networkState;
   std::set<NetworkCallbackFunction*> callbackFunctions;
};

struct LinuxSocketContext {
   LinuxSocketContext() :
      fd(0), serverAddress(NULL),
      serverPort(0) {}

   int fd;
   char* serverAddress;
   unsigned int serverPort;
};



pstatus createNetworkContext(NetworkContext* networkContext)
{
   *networkContext = new LinuxNetworkContext();
   
   return PAL_OK;
}

pstatus setNetworkContextParameters(NetworkContext networkContext,
                                    char* connectionParameters)
{
   return PAL_OK;
}

pstatus initNetworkContext(NetworkContext networkContext)
{
   LinuxNetworkContext* linuxNetworkContext =
      static_cast<LinuxNetworkContext*>(networkContext);
   linuxNetworkContext->networkState = INITIALIZED;

   for(std::set<NetworkCallbackFunction*>::iterator it =
          linuxNetworkContext->callbackFunctions.begin();
       it != linuxNetworkContext->callbackFunctions.end(); ++it) {
      (*it)->callbackFunc(linuxNetworkContext,
                          linuxNetworkContext->networkState,
                          NULL,
                          0,
                          (*it)->data);
   }
   
   return PAL_OK;
}

pstatus destroyNetworkContext(NetworkContext networkContext)
{
   LinuxNetworkContext* linuxNetworkContext =
      static_cast<LinuxNetworkContext*>(networkContext);

   linuxNetworkContext->networkState = UNINITIALIZED;

   for(std::set<NetworkCallbackFunction*>::iterator it =
          linuxNetworkContext->callbackFunctions.begin();
       it != linuxNetworkContext->callbackFunctions.end(); ++it) {
      (*it)->callbackFunc(linuxNetworkContext,
                          linuxNetworkContext->networkState,
                          NULL,
                          0,
                          (*it)->data);
   }

   //Remove all registered callback functions to be on the safe side.
   for(std::set<NetworkCallbackFunction*>::iterator it =
          linuxNetworkContext->callbackFunctions.begin();
       it != linuxNetworkContext->callbackFunctions.end(); ++it) {
      delete (*it);
      linuxNetworkContext->callbackFunctions.erase(it);
   }
   
   delete linuxNetworkContext;
   linuxNetworkContext = NULL;

   return PAL_OK;
}

pstatus addNetworkCallbackFunc(NetworkContext networkContext,
                               NetworkCallbackFunc netCallbackFunc,
                               void* data)
{
   LinuxNetworkContext* linuxNetworkContext =
      static_cast<LinuxNetworkContext*>(networkContext);

   linuxNetworkContext->callbackFunctions.insert(
      new NetworkCallbackFunction(netCallbackFunc, data));

   return PAL_OK;
}

pstatus removeNetworkCallbackFunc(NetworkContext networkContext,
                                  NetworkCallbackFunc netCallbackFunc,
                                  void* data)
{
   LinuxNetworkContext* linuxNetworkContext =
      static_cast<LinuxNetworkContext*>(networkContext);
   
   for(std::set<NetworkCallbackFunction*>::iterator it =
          linuxNetworkContext->callbackFunctions.begin();
       it != linuxNetworkContext->callbackFunctions.end(); ++it) {
      if((*it)->callbackFunc == netCallbackFunc) {
         delete (*it);
         linuxNetworkContext->callbackFunctions.erase(it);
      }
   }
   return PAL_OK;
}

pstatus createSocket(SocketContext* socketContext)
{
   
   *socketContext = new LinuxSocketContext();

   LinuxSocketContext* linuxSocketContext =
      static_cast<LinuxSocketContext*>(*socketContext);

   int result = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
   if(result == -1) {
      switch (errno) {
      case EACCES:
         return PAL_SOCKET_PERMISSION_DENIED;
      case EINVAL:
      case EPROTONOSUPPORT:
         return PAL_PROTOCOL_NOT_SUPPORTED;
      case ENOBUFS:
      case ENOMEM:
         return PAL_OUT_OF_MEMORY;
      default:
         return PAL_SOCKET_ERROR;
         
      }
   }

   linuxSocketContext->fd = result;

   
   return PAL_OK;
}

pstatus setSocketContextParameters(SocketContext socketContext,
                                   ServerParameters serverParameters)
{
   LinuxSocketContext* linuxSocketContext =
      static_cast<LinuxSocketContext*>(socketContext);

   linuxSocketContext->serverAddress = new
      char[strlen(serverParameters.serverAddress) + 1];
   strcpy( linuxSocketContext->serverAddress, serverParameters.serverAddress);

   linuxSocketContext->serverPort = serverParameters.serverPort;
   
   return PAL_OK;
}

pstatus connect(SocketContext socketContext)
{
   LinuxSocketContext* linuxSocketContext =
      static_cast<LinuxSocketContext*>(socketContext);

   if(linuxSocketContext->serverAddress == NULL) {
      coreprintln("No socketContextParameters has been set.");
      return PAL_NO_SOCKET_CONTEXT_PARAMS;
   }

   struct sockaddr_in sin;

   memset(&sin, 0, sizeof(sin));
   sin.sin_family = AF_INET;
   sin.sin_port = htons((u_short)linuxSocketContext->serverPort);
   struct hostent *hostEntry;
   hostEntry = gethostbyname(linuxSocketContext->serverAddress);
   if (hostEntry != 0) {
      memcpy(&(&sin)->sin_addr, hostEntry->h_addr, hostEntry->h_length);
   } else {
      return PAL_CONNECT_ERROR;
   }
   
   int status = connect(linuxSocketContext->fd,
                        (struct sockaddr*)&sin, sizeof(struct sockaddr_in));
   
   if(status != 0) {
      switch (errno) {
      case ECONNREFUSED:
         return PAL_CONNECTION_REFUSED;
      case EISCONN:
         return PAL_SOCKET_ALREADY_CONNECTED;
      case ETIMEDOUT:
         return PAL_CONNECTION_ATTEMPT_TIMEDOUT;
      default:
         return PAL_CONNECT_ERROR;
      }
   }
      
   return PAL_OK;
}

pstatus disconnect(SocketContext socketContext)
{
   LinuxSocketContext* linuxSocketContext =
      static_cast<LinuxSocketContext*>(socketContext);
   
   int status = shutdown(linuxSocketContext->fd, SHUT_RDWR);

   if(status != 0) {
      switch (errno) {
      case ENOTCONN:
         return PAL_SOCKET_NOT_CONNECTED;
      default:
         return PAL_DISCONNECT_ERROR;
      }
   }

   status = close(linuxSocketContext->fd);
   if(status == -1) {
      return PAL_DISCONNECT_ERROR;
   }
   linuxSocketContext->fd = 0;
   
   delete linuxSocketContext;
   linuxSocketContext = NULL;
   
   return PAL_OK;
}

pstatus select(SocketContext socketContext,
               int* timeout, 
               bool* readReady,
               bool* writeReady)
{

   LinuxSocketContext* linuxSocketContext =
      static_cast<LinuxSocketContext*>(socketContext);

   
   // Setup the file descriptors.
   fd_set fdescs;
   FD_ZERO(&fdescs);
   FD_SET(linuxSocketContext->fd, &fdescs);

   struct timeval tv;
   if(timeout != NULL) {
      tv.tv_sec = *timeout;
      tv.tv_usec = 0;
   }

   fd_set* read=NULL, *write=NULL;

   if(readReady != NULL && *readReady) {
      read = &fdescs;
      write = (fd_set *)0;
   } else if(writeReady != NULL && *writeReady){
      write = &fdescs;
      read = (fd_set *)0;
   }
   
   int status = select(getdtablesize(),
                       read,
                       write,
                       (fd_set *)0, &tv);
   if(status == -1) {
      return PAL_SELECT_ERROR;
   }
   if(readReady != NULL && *readReady){
      status = FD_ISSET(linuxSocketContext->fd, read);
      if(status) {
         // This means the socket is ready to be read.
         *readReady = true;
      } else {
         // This means there is nothing to read on the socket.
         *readReady = false;
      }
   } else if(writeReady != NULL && *writeReady){
      status = FD_ISSET(linuxSocketContext->fd, write);
      if(status) {
         // This means the socket is ready to be written to.
         *writeReady = true;
      } else {
         // This means it is not possible to write to the socket.
         *writeReady = false;
      }      
   }
   
   return PAL_OK;
}

pstatus read(SocketContext socketContext,
             char* buffer,
             unsigned int bytesToRead,
             unsigned int* bytesRead)
{
   LinuxSocketContext* linuxSocketContext =
      static_cast<LinuxSocketContext*>(socketContext);

   int result = ::read(linuxSocketContext->fd,
                       buffer,
                       (size_t)bytesToRead);
   
   if(result == -1) {
      return PAL_READ_ERROR;
   }
   *bytesRead = result;
   
   return PAL_OK;
}

pstatus write(SocketContext socketContext,
              const char* buffer,
              unsigned int sizeToWrite,
              unsigned int* sizeWritten)
{
   LinuxSocketContext* linuxSocketContext =
      static_cast<LinuxSocketContext*>(socketContext);

   int result  = ::write(linuxSocketContext->fd,
                         buffer,
                         (size_t)sizeToWrite);
   if(result == -1) {
      return PAL_WRITE_ERROR;
   }
   *sizeWritten = result;

   return PAL_OK;
}

} // End PAL namespace.
