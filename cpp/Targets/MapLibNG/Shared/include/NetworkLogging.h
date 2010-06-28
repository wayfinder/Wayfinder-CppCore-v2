#ifndef _NETWORKLOGGING_H_
#define _NETWORKLOGGING_H_

class NetworkLogger {
public:
   virtual ~NetworkLogger() {}
   
   void print( const char* format, ... );
private:
   virtual void sendMessage( const char* msg ) = 0;
};

namespace NetLog {
   /**
    *   Changes the global network logger. The
    *   default logger will not perfom any data
    *   sending.
    */ 
   
   void setInstance( NetworkLogger* newLogger );

   /** 
    *   Returns an instance to the currently registered
    *   network logger.
    */
   
   NetworkLogger& instance();
}

#endif /* _NETWORKLOGGING_H_ */
