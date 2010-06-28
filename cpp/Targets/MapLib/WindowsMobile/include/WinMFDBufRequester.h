/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef WINMFDBUFREQUESTER_H
#define WINMFDBUFREQUESTER_H

#include "config.h"
#include <stdio.h>
#include "MC2SimpleString.h"
#include "FileHandler.h"
#include "MultiFileDBufRequester.h"
#include "LogFile.h"

// -- WinFileHandler

class WinFileHandler : public FileHandler {
public:
   /**
    *   Creates a filehandler that handles the supplied file.
    *   Should create the file if it is not there already if
    *   create file is not zero.
    *   @param initNow  If the file should be initialized (opened)
    *                   immediately or delayed until the file
    *                   is actually used.
    */
   WinFileHandler( const char* fileName,
                   bool bReadOnly,
                   int createFile,
                   int initNow );

   /**
    *   Destroys the file handler and closes the file.
    */
   virtual ~WinFileHandler();

   /**
    *   Clears the file to zero length.
    */
   void clearFile();

   /// Returns 0 if everything is ok.
   int status() const;
   
   /**
    *   Reads maxLength bytes from the file.
    *   Calls the listener when done.
    *   If listener is NULL the write will be sync and the number of
    *   written bytes returned.
    */
   int read( uint8* bytes,
             int maxLength,
             FileHandlerListener* listener );

   /**
    *   Sets the read and write position of the stream to the sent-in value.
    */
   void setPos( int pos );

   /**
    *   Sets the size of the file.
    *   @param size  The new size in bytes.
    */
   void setSize( int size );

   /**
    *   Returns the position in the stream.
    */
   int tell();
   
   /**
    *   Writes bytes to the file of the FileHandler.
    *   Calls the listener when it is done.
    *   If listener is NULL the write will be sync and the number of
    *   written bytes returned.
    */
   int write( const uint8* bytes,
              int length,
              FileHandlerListener* listener );

   /**
    *   Cancels outstanding reads/writes.
    */
   void cancel();
   
   /**
    *   Returns the time of last modification of the file.
    */
   uint32 getModificationDate() const;

   /**
    *   Returns the available space for the drive that the
    *   file is located on.
    */
   uint32 getAvailableSpace() const;
   
   /**
    *   Returns the size of the file.
    *   Overrides superclass implementation.
    */
   int getFileSize();
   
private:

   /// Called when pending jobs are done.
   void RunL();

   /// Called when the object is cancelled
   void DoCancel();
   
   /**
    *   Will initialize the file, i.e. open it.
    */
   inline void init();
   
   /**
    *   Tries to create the file. Returns true if the file
    *   is opened ok.
    */
   bool openFile();

   enum state_t {
      /// We are not doing anything
      IDLE,
      /// We are reading
      READING,
      /// We are writing
      WRITING,
   } m_state ;
   
   wchar_t* m_fileName;
   HANDLE m_file;
   uint8* m_dataPtr;
   FileHandlerListener* m_currentListener;

   /// Status == 0 if ok
   int m_status;
   /// True if files should be created.
   int m_createFile;

   /// True if the file has been initilized (i.e. init() has been called)
   int m_initialized;

   bool m_bReadOnly;
};

// -- WinMFDBufRequester

class WinMFDBufRequester : public MultiFileDBufRequester {
public:
   WinMFDBufRequester(DBufRequester* parent,
                          const wchar_t* path,
                          MemTracker* memTracker,
                          uint32 maxSize,
                          int readOnly = false ) :
         MultiFileDBufRequester(parent,
                                createUTF8Path(path),memTracker,
                                maxSize,
                                -1,-1,
                                readOnly) {
      // Delete the temporarily used utf8 path.
      delete[] m_utf8Path;
      init();
   }

   FileHandler* createFileHandler(const char* fileName,
                                  bool createIfNotExists,
                                  bool initNow ) {
      return new WinFileHandler( fileName,
                                 false,
                                 createIfNotExists,
                                 initNow );
   }

   const char* getPathSeparator() const {
      return "\\";
   }
private:
   /// Creates m_utf8 path.
   const char* createUTF8Path( const wchar_t* des );
   /// Temporary utf7 path
   char* m_utf8Path;
};

#endif

