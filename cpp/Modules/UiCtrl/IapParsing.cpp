/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "IapParsing.h"
#include <string.h>
#include <algorithm>
#include "nav2util.h"
#include <cstdio>

using namespace isab;

namespace IapParsing {

char* toString(isab::IapData* iapData)
{

   char* tmpStr = new char[ 40 + (
         iapData->m_proxyHost ? strlen( iapData->m_proxyHost ) : 0)
                            + 1 ];
   if (iapData->m_proxyPort != MAX_UINT32 &&
       iapData->m_proxyHost != NULL &&
       strcmp(iapData->m_proxyHost, "") != 0) {
      sprintf( tmpStr, "%"PRIi32",%s:%"PRIi32"",
               iapData->m_iapID,
               iapData->m_proxyHost,
               iapData->m_proxyPort);
   } else {
      sprintf( tmpStr, "%"PRIi32"", iapData->m_iapID );
   }

   return tmpStr;

}

isab::IapData* fromString(const char* str)
{
   IapData* iapData = new IapData;
   
   const char* p0 = str;
   const char* p1 = strchr( p0, ',' );

   iapData->m_iapID = atoi(p0);
   
   if(p1) {
      p1++; // To move beyond the comma
      
      const char* p2 = strchr(p1, ':');
      
      unsigned int hostLength = p2 - p1;

      iapData->m_proxyHost =  new char[hostLength + 1];

      std::copy( p1, p2, iapData->m_proxyHost);
      iapData->m_proxyHost[hostLength] = '\0';
      
      p2++; // To move beyond the colon
      
      iapData->m_proxyPort = atoi(p2);
   } else {
      iapData->m_proxyPort = MAX_UINT32;
      iapData->m_proxyHost = strdup_new("");
   }
   return iapData;
}


} // End of namespace IapParsing
