/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "SymbianTextUtil.h"
#include <utf.h>
#include <stdlib.h>
#include <stdio.h>
#include <utf.h>
#include <coemain.h>
#include <string.h>


char * SymbianTextUtil::toUtf8(const TDesC& inbuf)
{
   char *result = NULL;
   HBufC8* temp = NULL;

   temp = HBufC8::NewLC(inbuf.Length()*4);
   TPtr8 temp_ptr = temp->Des();
   TInt truncated = CnvUtfConverter::ConvertFromUnicodeToUtf8(temp_ptr, inbuf);
   if (truncated != KErrNone) {
      /* truncated now contains either an error or the number of */
      /* characters left untranslated. */
      /* Implement */
   }
   TInt len = temp->Length();
   result = new (ELeave) char[len+4];
   int i = 0;
   while (i < len) {
      result[i] = (*temp)[i];
      i++;
   }
   result[i] = 0;
   CleanupStack::PopAndDestroy(temp);

   return result; 
}

HBufC* SymbianTextUtil::toHBufC(const char* utf8str)
{
   HBufC *unicodeStr = HBufC::NewL(strlen(utf8str)+1);
   TPtr temp_ptr = unicodeStr->Des(); /* Yes, this variable is necessary. */

   // TInt res = Utf8ToTDes(utf8str, temp_ptr);
   
   TPtrC8 tmp;
   tmp.Set(reinterpret_cast<const unsigned char*>(utf8str), strlen(utf8str));

   CnvUtfConverter::ConvertToUnicodeFromUtf8(temp_ptr, tmp);

   return unicodeStr;
}


