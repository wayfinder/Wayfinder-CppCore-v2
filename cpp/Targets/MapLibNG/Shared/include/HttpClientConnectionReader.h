#ifndef HTTPCLIENTCONNECTIONREADER_H
#define HTTPCLIENTCONNECTIONREADER_H

#include "TCPClientConnection.h"
#include "TCPConnectionHandler.h"
#include "SharedHttpHeader.h"

class HttpClientConnection;
class HttpClientConnectionListener;

class HttpClientConnectionReader {
public:
   /**
    *   Creates a new HttpClientConnectionReader
    */
   HttpClientConnectionReader(HttpClientConnection* connection,
                              HttpClientConnectionListener* listener);

   /// Destroys the internal stuff.
   ~HttpClientConnectionReader();

   /**
    *   Handles that a read is done.
    */
   TCPClientConnection::status_t
   readDone(TCPClientConnection::status_t status,
            const byte* bytes, int nbrBytes);

   /**
    *
    */
   void setListener(HttpClientConnectionListener* listener ) {
      m_listener = listener;
   }
   
public:

   /// The connection
   HttpClientConnection* m_connection;

   /// The connection listener
   HttpClientConnectionListener* m_listener;
   
   /// The header
   SharedHttpHeader m_header;
   
   enum state_t {
      IDLE           = 0,
      READING_HEADER = 1,
      READING_BODY   = 2,
   } m_state;

   /// The content length.
   int m_contentLength;
   /// The content length left.
   int m_contentLengthLeft;
   /// The current position in the body
   int m_curBodyPos;
   /// Max length to read
   int m_readLength;
   /// Read buffer.
   byte* m_readBuffer;
};

#endif /* HTTPCLIENTCONNECTIONREADER_H */
