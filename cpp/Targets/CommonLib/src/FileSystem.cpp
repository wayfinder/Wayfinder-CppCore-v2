/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "FileSystem.h"

#if defined _WIN32_WCE
#define FS_WINDOWS_API
#elif defined __SYMBIAN32__
#define FS_POSIX_API
#endif

#ifdef __linux__
#include <time.h>
#endif

#include <string.h>
#include <stdarg.h>

# if defined(FS_WINDOWS_API)
#   include <windows.h>
#elif defined __SYMBIAN32__
# include <sys/unistd.h>
# include <sys/stat.h>
# include <dirent.h>
# include <unistd.h>
# if !(defined(NAV2_CLIENT_UIQ) || defined(NAV2_CLIENT_UIQ3))
#  include <sysutil.h>
# endif
# include <f32file.h>
#else //Linux etc
#   include <sys/types.h>
#   ifdef __APPLE__
#      include <stdio.h>
#      include <sys/types.h>
#      include <sys/stat.h>
#    endif
#   if !defined(__APPLE__) && !defined(__OpenBSD__) && !defined (__SYMBIAN32__)
#     include <sys/statvfs.h>
#   elif !defined(__SYMBIAN32__)
#     include <sys/mount.h>
#   endif
#   include <dirent.h>
#   include <unistd.h>
#   include <fcntl.h>
#   include <utime.h>
#   include <limits.h>
#   include <errno.h>
# endif

bool FileSystem::pathExists(const char* path)
{
#ifdef FS_WINDOWS_API

   WCHAR tmpFileName[MAX_PATH];

   MultiByteToWideChar(CP_ACP, 0, path, -1, tmpFileName, MAX_PATH);
   
   DWORD attr( ::GetFileAttributes( tmpFileName ) );
   DWORD ec = 0;
   
   if ( attr == 0xFFFFFFFF )
   {
      ec = ::GetLastError();
      if ((ec == ERROR_FILE_NOT_FOUND)
          || (ec == ERROR_PATH_NOT_FOUND)
          || (ec == ERROR_INVALID_NAME) // "tools/jam/src/:sys:stat.h", "//foo"
          || (ec == ERROR_INVALID_DRIVE) // USB card reader with no card inserted
          || (ec == ERROR_INVALID_PARAMETER) // ":sys:stat.h"
          || (ec == ERROR_BAD_PATHNAME) // "//nosuch" on Win64
          || (ec == ERROR_BAD_NETPATH)) // "//nosuch" on Win32
      {
         // these are not considered errors;
         // the status is considered not found
         return false;
      }
      else if ((ec == ERROR_SHARING_VIOLATION))
      {
         return false;
         // ec = ok; // these are not considered errors;
         // // the file exists but the type is not known 
         // return fs::file_status( fs::type_unknown );
      }
      return false;
      // return fs::file_status( fs::status_unknown );
   }

   if(attr & FILE_ATTRIBUTE_DIRECTORY) {
      //Exists and is a file
      return true;
   } else {
      //Exists and is a directory
      return true;  
   }
# else // FS_POSIX_API
   struct stat path_stat;
   if ( ::stat( path, &path_stat ) != 0 )
   {
#ifndef __SYMBIAN32__
      if ( errno == ENOENT || errno == ENOTDIR )
      {
         return false;
         // File not found
      }
#endif
      return false;
      // Status unknown
   }

   return true;

   // if ( S_ISDIR( path_stat.st_mode ) )
   //    return fs::file_status( fs::directory_file );
   // if ( S_ISREG( path_stat.st_mode ) )
   //    return fs::file_status( fs::regular_file );
   // if ( S_ISBLK( path_stat.st_mode ) )
   //    return fs::file_status( fs::block_file );
   // if ( S_ISCHR( path_stat.st_mode ) )
   //    return fs::file_status( fs::character_file );
   // if ( S_ISFIFO( path_stat.st_mode ) )
   //    return fs::file_status( fs::fifo_file );
   // if ( S_ISSOCK( path_stat.st_mode ) )
   //    return fs::file_status( fs::socket_file );
   // return fs::file_status( fs::type_unknown );
#endif
}

void
FileSystem::corefprintf(FILE* file, const char *format, ...)
{
   va_list args;
   va_start(args, format);
   vfprintf(file, format, args);
   fflush(file);
   va_end(args);
}

FILE*
FileSystem::openFile(const char* fileName)
{
   return fopen(fileName, "a");
}

void
FileSystem::closeFile(FILE* file)
{
   if (file != NULL) {
      fclose(file);
   }
}

void
FileSystem::corefwrite(FILE* file, const char* data)
{
   if (file != NULL) {
      // write to file
      fprintf(file, "%s\n", data);
      fflush(file);
   }
}

void FileSystem::core_simple_fprintf(const char* fileName, const char* format, ...)
{
#if 0
   FILE* file = openFile(fileName);
   
   va_list args;
   va_start(args, format);
   
#ifdef __linux__
   if(strcmp(format, "") == 0) {
      fprintf(file, "\n");
   } else {
      const char* extFormat = "[%d]";

      time_t tt;
      struct tm* tm;

      ::time(&tt);
      tm = localtime(&tt);

      char tmpFormat[512];

      sprintf(tmpFormat,
              "[%04d-%02d-%02d %02d:%02d:%02d]: %s",
              tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
              tm->tm_hour, tm->tm_min, tm->tm_sec,
              format);
   
      vfprintf(file, tmpFormat, args);      
   }
#else
   vfprintf(file, format, args);      
#endif

   fflush(file);
   va_end(args);

   closeFile(file);
#endif
}







