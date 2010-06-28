#ifndef UNIXFILEHANDLER_H
#define UNIXFILEHANDLER_H

#include "config.h"
#include <stdio.h>
#include "MC2SimpleString.h"
#include "FileHandler.h"
#include "MultiFileDBufRequester.h"

class UnixFileHandler : public FileHandler {
public:
   /**
    *   Creates a filehandler that handles the supplied file.
    *   Should create the file if it is not there already
    *   if createFile is true.
    */
   UnixFileHandler( const char* fileName, bool createFile );

   /**
    *   Destroys the file handler and closes the file.
    */
   virtual ~UnixFileHandler();

   /**
    *   Returns 0 if status is ok.
    */
   int status() const;
   
   /**
    *   Clears the file to zero length.
    */
   void clearFile();
   
   /**
    *   Reads maxLength bytes from the file.
    *   Calls the listener when done.
    */
   virtual int read( uint8* bytes,
                     int maxLength,
                     FileHandlerListener* listener );

   /**
    *   Sets the read and write position of the stream to the sent-in value.
    */
   void setPos( int pos );

   /**
    *   Returns the position in the stream.
    */
   int tell();

   /**
    *   Returns the modification date of the file or MAX_UINT32 if error.
    */
   uint32 getModificationDate() const;
   
   /**
    *   Writes bytes to the file of the FileHandler.
    *   Calls the listener when it is done.
    */
   virtual int write( const uint8* bytes,
                      int length,
                      FileHandlerListener* listener );
   /**
    *   Returns the amount of available space on the drive
    *   where the file is located.
    *   Currently returns a lot.
    */
   uint32 getAvailableSpace() const;

   /**
    *   Cancels outstanding reads/writes.
    */
   virtual void cancel() {}

   /**
    *   Sets the size of the file.
    */
   void setSize(int size);
   
private:
   FILE* m_file;
   /// Variable to keep track so that no functions are called before done.
   int m_working;
};


#endif /* UNIXFILEHANDLER_H */
