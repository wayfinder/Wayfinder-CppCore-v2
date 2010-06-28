#ifndef GTKFILEHANDLER_H
#define GTKFILEHANDLER_H

#include "config.h"
#include "UnixFileHandler.h"

#include <gtk/gtk.h>

// -- GtkFileHandler

class GtkFileHandler : public UnixFileHandler {
public:
   /**
    *   Creates a filehandler that handles the supplied file.
    *   Should create the file if it is not there already.
    */
   GtkFileHandler( const char* fileName,
                   bool createFiles );

   ~GtkFileHandler();

   /**
    *   Reads maxLength bytes from the file.
    *   Calls the listener when done.
    */
   int read( uint8* bytes,
             int maxLength,
             FileHandlerListener* listener );

   /**
    *   Writes bytes to the file of the FileHandler.
    *   Calls the listener when it is done.
    */
   int write( const uint8* bytes,
              int length,
              FileHandlerListener* listener );
   
   /**
    *   Cancels outstanding reads/writes.
    */
   void cancel() {}
   
private:

   struct IdleData_t;

   /**
    * The ID of the read idle event source.
    */
   guint m_readIdle;

   /**
    * The associated data for the readIdle event source.
    */
   IdleData_t* m_readIdleData;
   
   /**
    * The ID of the read idle event source.
    */
   guint m_writeIdle;

   /**
    * The associated data for the writeIdle event source.
    */
   IdleData_t* m_writeIdleData;


   /// True if an idle operation is active.
   int m_active;
   
   /// Called when read is done.
   static gboolean on_read_idle( gpointer data );
   /// Called when read is done.
   static gboolean on_write_idle( gpointer data );
};


#endif /* GTKFILEHANDLER_H */
