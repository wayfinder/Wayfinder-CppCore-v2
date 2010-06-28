#ifndef HTTPDBUFCONNECTION_H
#define HTTPDBUFCONNECTION_H

#include "config.h"
#include "NetDBufRequester.h"
#include "DBufConnection.h"

class HttpClientConnection;
class MC2SimpleString;

class HttpDBufConnection : public WFAPI::DBufConnection {
public:
   /**
    *    Creates a new HttpDBufConnection
    *    which will add the prefix to the get and posts.
    *    @param urlParams       [Optional] The url parameters 
    *                           (for instance containing the session).
    *                           Should begin with a "?".
    */
   HttpDBufConnection(HttpClientConnection* clientConnetion,
                                         const MC2SimpleString& prefix,
                                         const MC2SimpleString& urlParams);

   /**
    *    Destructor.
    */
   virtual ~HttpDBufConnection() {}
   
   /**
    *    Requests one databuffer. The value returned here should
    *    also be used when sending replies to the listeners.
    *    @param descr Buffer to request.
    *    @return -1 if too many requests have been sent already.
    */
   int requestOne(const char* descr);

   /**
    *    Requests many databuffers. (e.g. using post in http).
    *    The value returned here should
    *    also be used when sending replies to the listeners.
    *    @param body Body of data as described in NetDBufRequester
    *                and ParserThread. (For multiple databufs).
    *    @return -1 if too many requests have been sent already.
    */
   int requestMany(const uint8* buf, int nbrBytes);

   /**
    *    Sets listener.
    */
   void setListener(HttpClientConnectionListener* listener);

   /**
    *    Sets the POST directory that the HTTP requester should use.
    */ 
   void setPostDirectory(const char* dir);

   /**
    *    Sets the server to request against.
    */ 
   void setHostAndPort(const char* host);

   /**
    *    Issues a forced disconnect request to the underlying connection.
    */ 
   void forceDisconnect();
   
private:
   /// Client connection        to use.
   HttpClientConnection* m_clientConnection;
   /// prefix to use.
   MC2SimpleString m_prefix;
   /// The urlParams.
   MC2SimpleString m_urlParams;
};

#endif /* HTTPDBUFCONNECTION_H */
