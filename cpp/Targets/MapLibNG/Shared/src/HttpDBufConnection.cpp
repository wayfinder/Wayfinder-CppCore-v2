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

#include "BitBuffer.h"
#include "HttpDBufConnection.h"
#include "HttpClientConnection.h"
#include "DBufConnection.h"

#include <algorithm>
#include <vector>
#include <string>

HttpDBufConnection::
HttpDBufConnection(HttpClientConnection* clientConnetion,
                                      const MC2SimpleString& prefix,
                                      const MC2SimpleString& urlParams)
      : m_clientConnection(clientConnetion),
        m_prefix(prefix), m_urlParams(urlParams)
{
}

int
HttpDBufConnection::requestOne(const char* descr)
{
   return m_clientConnection->get(m_prefix.c_str(),
                                  "/",
                                  descr, 
                                  m_urlParams.c_str());
}

int
HttpDBufConnection::requestMany(const byte* buf,
                                                   int nbrBytes)
{
   SharedBuffer tempBuf(const_cast<uint8*>(buf), nbrBytes);
   
   return m_clientConnection->post(m_prefix.c_str(), "",
                                   "x", tempBuf, m_urlParams.c_str());
}

void
HttpDBufConnection::
setListener(HttpClientConnectionListener* listener)
{
   m_clientConnection->setListener(listener);
}

void HttpDBufConnection::setPostDirectory(const char* dir)
{
   m_prefix = dir;
}


void HttpDBufConnection::setHostAndPort(const char* host)
{
   std::string str(host);

   size_t index = str.find_first_of(":");

   if(index == string::npos) {
      return;
   }
   
   std::string hostStr = str.substr(0, index);
   std::string portStr = str.substr(index+1);

   int port;
   
   sscanf(portStr.c_str(), "%d", &port);

   printf("Parsed host: %s and port: %d\n", hostStr.c_str(), port);
   
   m_clientConnection->setHostAndPort(hostStr.c_str(), port);
}

void HttpDBufConnection::forceDisconnect()
{
   cout << "[HttpDBufConnection]: forceDisconnect()" << endl;
   m_clientConnection->forceDisconnect();
}
