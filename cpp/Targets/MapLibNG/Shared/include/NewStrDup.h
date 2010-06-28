/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "config.h"

#ifndef NEWSTRDUP_FILEN_H 
#define NEWSTRDUP_FILEN_H 

#include <string.h>

#ifdef MC2_SYSTEM
#include <MC2String.h>
#endif

/**
 *   Class containing function for copying a string.
 */
class NewStrDup {
public:
   /**
    *   Returns a copy of the string <code>orig</code> allocated with new.
    *   Zero terminates the string if len is set.
    *   @param orig Original string to copy.
    *   @return Copy of <code>orig</code> allocated with new.
    */
   static char* newStrDup(const char* orig, int len = -1);

   /**
    *   Creates a copy of the string and returns the length
    *   of the newly allocated string incl \0
    */
   static uint32 newStrDupLen(char*& orig_dest);

#ifdef MC2_SYSTEM
   /**
    *   Returns a copy of the string <code>orig</code> allocated with new.
    *   Only copies upto the length of the string, and then zero terminates.
    *   @param orig Original string to copy.
    *   @return Copy of <code>orig</code> allocated with new.
    */
   inline static char* newStrDup( const MC2String& orig );
#endif
};



#ifdef MC2_SYSTEM
char*
NewStrDup::newStrDup( const MC2String& str )
{
   return newStrDup( str.c_str(), str.length() );
}
#endif

#endif
