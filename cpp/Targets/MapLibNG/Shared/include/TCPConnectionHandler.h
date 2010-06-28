#ifndef TCPCONNECTIONHANDLER_H
#define TCPCONNECTIONHANDLER_H

class TCPClientConnection;


class TCPConnectionHandler {
public:
   
   virtual ~TCPConnectionHandler() {}
   
   /**
    *   Tells the underlying layer to connect to the
    *   supplied host and port. When connected, connected
    *   should be called.
    *   Should call ccon->connectDone when done.
    *   Max one connect per ClientConnection is allowed simultaneously.
    */
   virtual void connect(const char* host, unsigned int port,
                        TCPClientConnection* ccon) = 0;

   /**
    *   Tells the underlying layer to disconnect the
    *   current connection.
    */
   virtual void disconnect(TCPClientConnection* ccon) = 0;

   /**
    *   Tells the underlying layer to write the supplied
    *   bytes. The underlying layer must keep the order
    *   of the write requests.
    *   Should call ccon->writeDone when done.
    *   Max one write per ClientConnection is allowed.
    *   Not allowed if not connected.
    */
   virtual void write(const byte* bytes, int length,
                      TCPClientConnection* ccon) = 0;
      
   /**
    *   Tells the underlying layer to start receiving reads
    *   with up to maxLength bytes from the 
    *   socket. Max <code>length</code> bytes.
    *   Should call the ccon->readDone when done.
    *   Max one read per ClientConnection is allowed.
    *   Not allowed if not connected.
    */
   virtual void read(int maxLength,
                     TCPClientConnection* ccon) = 0;

   /**
    *   Removes the connection from the internal list of
    *   connections in the TCPConnectionHandler.
    */
   virtual void remove(TCPClientConnection* conn) = 0;

   /**
    *   Requests a timeout for the connection conn.
    *   After the timeout, the connectionhandler should call
    *   timerExpired on the connection.
    *   @param nbrMillis The number of milliseconds to wait before
    *                    sending the timeout.
    *   @param conn      The connection that should have the timeout.
    *   @return ID of the timeout.
    */
   virtual int requestTimer(uint32 nbrMillis,
                            TCPClientConnection* conn) = 0;

   /**
    *   Cancels the timer with id timerID.
    */
   virtual void cancelTimer(int timerID) = 0;
};


#endif /* TCPCONNECTIONHANDLER_H */
