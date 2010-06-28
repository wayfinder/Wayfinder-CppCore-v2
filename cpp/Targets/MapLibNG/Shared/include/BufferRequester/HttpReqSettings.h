#ifndef HTTPREQSETTINGS_H
#define HTTPREQSETTINGS_H

/**
 *  Class containing settings for the http network communication.
 */
class HttpReqSettings {
public:

   /**
    *  Constructor.
    *
    *  @param postDirectory Post directory path, for instance "/TMap" or "/".
    *  @param forceGet If force using GET instead of POST for the http
    *                  requests. Note that the postDirectory will still
    *                  be used anyways.
    */
   HttpReqSettings(const char* postDirectory,
                   bool forceGet,
                   const char* hostAndPort = NULL)
   {
      m_postDirectory = postDirectory;
      m_forceGet = forceGet;
      m_hostAndPort = hostAndPort;
   }

   /**
    *  If true, force GET style requests.
    */ 
   bool getForceGet() const {
      return m_forceGet;
   }

   /**
    *  @return The post directory path, for instance "/TMap" or "/".
    */
   const char* getPostDirectory() const {
      return m_postDirectory;
   }

   
   const char* getHostAndPort() const {
      return m_hostAndPort;
   }
   
private:
   /**
    *  The post directory path, for instance "/TMap" or "/".
    */
   const char* m_postDirectory;

   /**
    *  If to force GET style requests.
    */
   bool m_forceGet;

   /**
    * Host and port. NULL if to use default.
    */
   const char* m_hostAndPort;
};

#endif /* HTTPREQSETTINGS_H */
