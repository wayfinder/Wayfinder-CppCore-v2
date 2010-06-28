/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _FILESYSTEM_H_
#define _FILESYSTEM_H_

#include <stdio.h>

namespace FileSystem {

/**
 * @return True if the file at location path exists.
 */ 
bool pathExists(const char* path);

/**
 * Prints to file using vfprintf.
 *
 * @param file The FILE pointer to be written to, should be open for
 *             writing.
 */
void corefprintf(FILE* file, const char *format, ...);

void core_simple_fprintf(const char* fileName, const char* format, ...);

/**
 * Opens a file for writing and appending.
 *
 * @param fileName The path and name of the file to open.
 * @return A pointer to a FILE object, the opened file.
 */
FILE* openFile(const char* fileName);

/**
 * Closes a file.
 *
 * @param file The file to be closed.
 */
void closeFile(FILE* file);

/**
 * Writes a string to a file, the file should be opened for
 * writing.
 *
 * @param file The file that should be written to.
 * @param data The data that should be written to the file.
 */
void corefwrite(FILE* file, const char* data);

}

#endif /* _FILESYSTEM_H_ */
