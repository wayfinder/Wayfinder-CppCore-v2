#ifndef TCPCLIENTCONNECTION_H
#define TCPCLIENTCONNECTION_H

/**
 *   Class describing a asynchronous connection using TCP.
 */
class TCPClientConnection {
public:

   virtual ~TCPClientConnection() {}
   
   /**
    *   Enum describing the status of the different calls.
    */
   enum status_t {
      /// All is done.
      OK                =  0,
      /// Some error occured.
      ERROR             = -1,
      /// A timout occured
      TIMEOUT           = -2,
      /// An operation of the same kind has been requested already.
      ALREADY_REQUESTED = -3,
      /// The connection was closed
      CLOSED            = -4,
   };
   
   /**
    *   To be called when the connection is established or has
    *   failed.
    */
   virtual void connectDone(status_t status) = 0;

   /**
    *   To be called when the last read is done.
    *   @param status The status of the read.
    */
   virtual void readDone(status_t status,
                         const byte* bytes,
                         int nbrBytes) = 0;

   /**
    *   Informs the ClientConnection that the last write is completed.
    *   @param status The status of the write.
    */
   virtual void writeDone(status_t status) = 0;

   /**
    *   Informs the ClientConnection that the connection is closed.
    */
   virtual void connectionClosed(status_t status) = 0;

   /**
    *   Informs the ClientConnection that the timer with id
    *   timerID has expired.
    */
   virtual void timerExpired(int timerID) = 0;
};

#endif /* TCPCLIENTCONNECTION_H */
