/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "WinMFDBufRequester.h"
#include "TextConv.h"
#include "LogFile.h"

inline void
WinFileHandler::init()
{
   m_file = NULL;
   openFile();
   m_initialized = true;
   m_state = IDLE;
   if ( m_status == 0 ) {
      setPos( 0 );
   }
}

WinFileHandler::WinFileHandler( const char* fileName,
                                bool bReadOnly,
                                int createFile,
                                int initNow ) :
      FileHandler(fileName),
      m_dataPtr( NULL ),
      m_createFile( createFile ),
      m_bReadOnly(bReadOnly)
{
   m_file = NULL;
   m_status = 0;
   m_fileName = new wchar_t[64];

   TextConverter::UTF2Unicode(fileName, m_fileName, 128);

   m_initialized = false;
   if ( initNow ) {
      init();
   }
}

WinFileHandler::~WinFileHandler()
{
   if(m_file && INVALID_HANDLE_VALUE != m_file)
      CloseHandle(m_file);

   delete[] m_fileName;   
}

int
WinFileHandler::status() const
{
   if ( m_status == 0 ) {
      return 0;
   } else {
      return -1;
   }
}

bool
WinFileHandler::openFile()
{
   if ( m_createFile ) {
      // Create the directory where the file should be if it does not
      // exist.

      // Get current directory 
      WCHAR wbuf[256];
      for(int i = wcslen(m_fileName) - 1; i >= 0; i--) {
         if(m_fileName[i] == '\\') {
            wcsncpy(wbuf, m_fileName, i);
            wbuf[i] = '\0';
            break;
         }
      }

      bool b = CreateDirectory(wbuf, NULL);
   }

   DWORD dwAccess = m_bReadOnly ? GENERIC_READ : (GENERIC_READ | GENERIC_WRITE);
 
   // Try to open the file.
   {  
      m_file = CreateFile ( m_fileName, 
                            dwAccess, 
                            0, NULL, OPEN_EXISTING, 
                            FILE_FLAG_RANDOM_ACCESS, 
                            NULL);
      
      if(INVALID_HANDLE_VALUE == m_file)
         m_status = -1;
      else 
         m_status = 0;

      if( m_status == 0 )
         return true;
   }

   // Try creating it if we should
   if ( m_createFile ) {
      m_file = CreateFile ( m_fileName, 
                            dwAccess, 
                            0, NULL, CREATE_ALWAYS, 
                            FILE_FLAG_RANDOM_ACCESS, 
                            NULL);

      if(INVALID_HANDLE_VALUE == m_file)
         m_status = -1;
      else 
         m_status = 0;
   }
   
   return m_status == 0;
}

void
WinFileHandler::clearFile()
{
   if ( ! m_initialized ) {
      init();
   }
   CloseHandle(m_file);
   
   // And create empty file.
   m_file = CreateFile ( m_fileName, 
                         GENERIC_READ | GENERIC_WRITE, 
                         0, NULL, CREATE_ALWAYS, 
                         FILE_FLAG_RANDOM_ACCESS, 
                         NULL);

   if(INVALID_HANDLE_VALUE == m_file)
      m_status = -1;
   else 
      m_status = 0;
}

int
WinFileHandler::read( uint8* bytes,
                      int maxLength,
                      FileHandlerListener* listener )
{
   if ( ! m_initialized ) {
      init();
   }

   m_state = READING;   
   DWORD dwBytesRead;
   m_dataPtr = bytes;
   BOOL bResult =  ReadFile( m_file, m_dataPtr, maxLength, &dwBytesRead, NULL );

   if(bResult)
      m_status = 0;
   else
      m_status = -1;

   m_currentListener = listener;

   if ( listener ) {
      m_state = IDLE;
      if ( m_status == 0 ) {
         m_currentListener->readDone( dwBytesRead );
      } else {
         m_currentListener->readDone( -1 );
      } 
   } else {
      m_state = IDLE;
      if ( m_status == 0 ) {
         return dwBytesRead;
      } else {
         return -1;
      }
   }
   return 0;
}

int
WinFileHandler::write( const uint8* bytes,
                       int length,
                       FileHandlerListener* listener )
{
   if ( ! m_initialized ) {
      init();
   }

   m_state = WRITING;   
   DWORD dwBytesWritten;
   m_dataPtr = (uint8*)bytes;
   BOOL bResult = WriteFile( m_file, m_dataPtr, length, &dwBytesWritten, NULL );

   if(bResult)
      m_status = 0;
   else
      m_status = -1;

   m_currentListener = listener;

   if ( listener ) {
      m_state = IDLE;
      if ( m_status == 0 ) {
         m_currentListener->writeDone( dwBytesWritten );            
      } else {
         m_currentListener->writeDone( -1 );
      }      
   } else {
      m_state = IDLE;
      if ( m_status == 0 ) {
         return length;
      } else {
         return -1;
      }
   }
   return 0;
}

void
WinFileHandler::cancel()
{
}

void
WinFileHandler::setPos( int pos )
{   
   if ( ! m_initialized ) {
      init();
   }
   if ( pos >= 0 ) {
      int tmpPos = pos;
      SetFilePointer(m_file, tmpPos, NULL, FILE_BEGIN);
   } else {
      int tmpPos = 0;
      SetFilePointer(m_file, tmpPos, NULL, FILE_END);
   }
}
   
void 
WinFileHandler::setSize( int size )
{
   if ( ! m_initialized ) {
      init();
   }
   
   // Do nothing
}

int
WinFileHandler::tell()
{
   if ( ! m_initialized ) {
      init();
   }

   return SetFilePointer(m_file, 0, NULL, FILE_CURRENT);
}

uint32
WinFileHandler::getModificationDate() const
{
   FILETIME creationTime, lastAccessedTime, lastWriteTime;
   SYSTEMTIME fileTime;
   int nResult = GetFileTime( m_file, &creationTime, &lastAccessedTime, &lastWriteTime );
   if(!nResult)
      return MAX_UINT32;
   FileTimeToSystemTime(&lastWriteTime, &fileTime);

   return (fileTime.wYear - 1970) * 31536000 + (fileTime.wMonth * 30 + fileTime.wDay) * 86400 + 
          fileTime.wHour * 3600 + fileTime.wMinute * 60 + fileTime.wSecond;
}

uint32
WinFileHandler::getAvailableSpace() const
{
   ULARGE_INTEGER iFreeBytesAvailableToCaller;
   ULARGE_INTEGER iTotalNumberOfBytes;
   ULARGE_INTEGER iTotalNumberOfFreeBytes;

   if(!GetDiskFreeSpaceEx(L".\\", &iFreeBytesAvailableToCaller, &iTotalNumberOfBytes, &iTotalNumberOfFreeBytes)) {
      return MAX_UINT32;
   }

   return (uint32)iFreeBytesAvailableToCaller.QuadPart;
}

int 
WinFileHandler::getFileSize() 
{
   if ( m_initialized ) {
      return GetFileSize(m_file, NULL);
   } else {
      WIN32_FILE_ATTRIBUTE_DATA fileData;
      if (::GetFileAttributesEx(m_fileName,  GetFileExInfoStandard, &fileData)) {
         return fileData.nFileSizeLow;
      }
      return -1;
   }
}

void
WinFileHandler::DoCancel()
{
   switch ( m_state ) {
      case READING:
         m_currentListener->readDone( -1 );
         break;
      case WRITING:
         m_currentListener->writeDone( -1 );
         break;
      case IDLE:
         break;
   }
}

// -- WinMFDBufRequester

const char*
WinMFDBufRequester::createUTF8Path( const wchar_t* unicodePath )
{
   char utf8buf[256];
   TextConverter::Unicode2UTF(unicodePath, utf8buf, 256);
   
   m_utf8Path = new char[ strlen(utf8buf) + 1 ];
   strcpy(m_utf8Path, utf8buf);
   // Remove trailing slashes.
   while ( m_utf8Path[ strlen( m_utf8Path ) - 1 ]  == '\\' ) {
      m_utf8Path[ strlen( m_utf8Path ) - 1 ] = '\0';
   }
   return m_utf8Path;
}
