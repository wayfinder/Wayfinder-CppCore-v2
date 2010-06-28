#ifndef _TCPLOGGER_H_
#define _TCPLOGGER_H_

#include "HttpClientConnection.h"
#include "NetworkLogging.h"

class TCPClientConnection;

class TCPLogger : public TCPClientConnection,
                  public NetworkLogger {
public:
   TCPLogger( );
   virtual ~TCPLogger();
   void init( TCPConnectionHandler* connectionHandler,
              const char* host, int port );

   void sendMessage( const char* msg );

   bool finishedSendingMessages();

   bool connectDone();
   
   /**
    *   To be called when the connection is established or has
    *   failed.
    */
   virtual void connectDone(status_t status);

   /**
    *   To be called when the last read is done.
    *   @param status The status of the read.
    */
   virtual void readDone(status_t status,
                         const byte* bytes,
                         int nbrBytes);

   /**
    *   Informs the ClientConnection that the last write is completed.
    *   @param status The status of the write.
    */
   virtual void writeDone(status_t status);

   /**
    *   Informs the ClientConnection that the connection is closed.
    */
   virtual void connectionClosed(status_t status);

   /**
    *   Informs the ClientConnection that the timer with id
    *   timerID has expired.
    */
   virtual void timerExpired(int timerID);


   void attemptFlush();
private:
   const char* m_host;
   char* m_curData;
   int m_port;

   bool m_connectDone;
   
   TCPConnectionHandler* m_connHandler;

   int m_msgCount;
};

#endif /* _TCPLOGGER_H_ */
