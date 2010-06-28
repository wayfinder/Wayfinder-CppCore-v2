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
#include "HttpClientConnectionReader.h"
#include "HttpClientConnection.h"
#include "HttpClientConnectionListener.h"
#include "SharedBuffer.h"
#include "SharedHttpHeader.h"

HttpClientConnectionReader::
HttpClientConnectionReader(HttpClientConnection* connection,
                           HttpClientConnectionListener* listener)
{
   m_connection = connection;
   m_listener   = listener;
   m_readLength = 1024;
   m_state      = IDLE;
}

HttpClientConnectionReader::~HttpClientConnectionReader()
{
   setListener(NULL);
   if ( m_connection ) {
      m_connection->setListener(NULL);
   }
}

struct BytesReceivedData {
   int requestNbr;
   int httpStatusCode;
   const WFAPI::wf_uint8* bytes;
   int startOffset;
   int nbrBytes;
   int contentLength;
};

void notifyReceivedData(const std::vector<BytesReceivedData>& rdata,
                        HttpClientConnectionListener* listener)
{
   if(!listener) {
      return;
   }

   for(unsigned int i = 0; i < rdata.size(); i++) {
      listener->bytesReceived(rdata[i].requestNbr,
                              rdata[i].httpStatusCode,
                              rdata[i].bytes,
                              rdata[i].startOffset,
                              rdata[i].nbrBytes,
                              rdata[i].contentLength);
   }
}

TCPClientConnection::status_t
HttpClientConnectionReader::readDone(TCPClientConnection::status_t status,
                                     const byte* bytes,
                                     int nbrBytes)
{
   std::vector<BytesReceivedData> rdata;
   
   mc2dbg << "[HttpClientConnectionReader::readDone called with status = "
          << int(status) << " and nbr bytes = " << nbrBytes
          << endl;
   
   if ( m_state == IDLE ) {
      if ( m_connection->getNbrSent() ) {
         m_state = READING_HEADER;
         m_header.reset();
         m_curBodyPos = 0;
      } else {
         return TCPClientConnection::ERROR;
      }      
   }

   // FIXME: Divide this into more functions.
   if ( status == TCPClientConnection::OK ) {
      int headerBytes = 0;
      while ( nbrBytes > 0 ) {
         int currentRequest = m_connection->getCurrentRequestNbr();
         if ( m_state == READING_HEADER ) {
            mc2dbg << "[HttpClientConnectionReader]: Adding max "
                   << nbrBytes
                   << "bytes to header"
                   << endl;
            headerBytes = m_header.addBytes(bytes, nbrBytes);
            if ( headerBytes < 0 ) {
               m_state = IDLE;
               m_connection->connectionClosed(TCPClientConnection::ERROR);
               notifyReceivedData(rdata, m_listener);
               return TCPClientConnection::ERROR;
            }
            bytes +=    headerBytes;
            nbrBytes -= headerBytes;
            if ( headerBytes < 0 ) {
               // ERROR
               m_state = IDLE;
               notifyReceivedData(rdata, m_listener);
               return TCPClientConnection::ERROR;
            }
            if ( m_header.complete() ) {
               mc2dbg << "[HttpClientConnectionReader]: HTTP-status is "
                      << m_header.getStatusCode() << endl;
               // Status is ok.
               m_contentLength = 0;
               const char* contentLengthStr =
                  m_header.getHeaderValue("Content-Length");
               if ( contentLengthStr ) {
                  m_contentLength = atoi(contentLengthStr);
               }
               m_contentLengthLeft = m_contentLength;
               mc2dbg << "[Http]: Content-length = "
                      << m_contentLength << endl;
               // Tell the listener that there are bytes.

               BytesReceivedData b = { currentRequest,
                                       m_header.getStatusCode(),
                                       NULL, 0, 0,
                                       m_contentLength };
               rdata.push_back(b);
               
               m_state = READING_BODY;
               m_curBodyPos = 0;
            }
         }
         mc2dbg << "[HttpClientConnectionReader]: headerBytes = "
                << headerBytes << endl;
         
         if ( m_state == READING_BODY ) {
            if ( nbrBytes ) {
               int nbrBytesToListener = MIN(nbrBytes, m_contentLengthLeft);
               BytesReceivedData b = { currentRequest,
                                       m_header.getStatusCode(),
                                       bytes,
                                       m_curBodyPos,
                                       nbrBytesToListener,
                                       m_contentLength };
               rdata.push_back(b);
               
               m_contentLengthLeft -= nbrBytesToListener;
               m_curBodyPos        += nbrBytesToListener;
               nbrBytes            -= nbrBytesToListener;
               bytes               += nbrBytesToListener;
            }
            mc2dbg << "[HttpClientConnectionReader]: m_contentLengthLeft = "
                   << m_contentLengthLeft << endl;
            if ( (m_contentLength != 0) && (m_contentLengthLeft == 0) ) {
               m_connection->oneRequestComplete();
               if ( m_connection->getNbrSent() == 0 ) {
                  mc2dbg << "[HttpClientConnectionReader]: Should be IDLE now"
                         << endl;
                  m_state = IDLE;
               } else {
                  m_state = READING_HEADER;
                  m_curBodyPos = 0;
                  m_contentLengthLeft = 0;
                  m_contentLength = 0;
                  m_header.reset();
               }
            }
         }

         // Happens when disconnected during read
         if( m_state == IDLE) {
            break;
         }
      }
   } else if ( ( status == TCPClientConnection::CLOSED ) &&
               ( m_state == READING_BODY ) &&
               ( m_contentLength == 0) ) {
      const int currentRequest = m_connection->getCurrentRequestNbr();         
      // The server didn't send content length.
      // We're done when the connection is closed.
      BytesReceivedData b =  { currentRequest,
                               m_header.getStatusCode(),
                               bytes,
                               m_curBodyPos,
                               nbrBytes,
                               m_curBodyPos+nbrBytes};
      rdata.push_back(b);

      m_curBodyPos += nbrBytes;
      bytes += nbrBytes;
      m_connection->oneRequestComplete();
      m_state = READING_HEADER;
      m_curBodyPos = 0;
      m_header.reset();
   } else {
      // Error!
      notifyReceivedData(rdata, m_listener);
      m_state = IDLE;
      return TCPClientConnection::ERROR;
   }

   notifyReceivedData(rdata, m_listener);

   // Always check for more data.
   m_connection->readNextRequest();

   return status;
}

