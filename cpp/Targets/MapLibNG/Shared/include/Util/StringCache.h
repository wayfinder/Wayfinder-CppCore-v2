/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef STRINGCACHE_H
#define STRINGCACHE_H

#include "MapPlotterConfig.h"

class NativeTextInterface;
class MC2SimpleString;

class StringCache {
public:
   StringCache(NativeTextInterface* textInterface);
   
   ~StringCache();

   /**
    *  Will attempt to retrieve the corresponding STRING* from the
    *  cache. If no such string is found, a new one is created and
    *  stored in the cache for future retrieval.
    *
    *  @param utf8Text The UTF8 string from which the platform specific
    *                  string will be derived.  
    */
   const STRING* getOrCreateString(const MC2SimpleString& utf8Text);

   /**
    *  Clears the internal cache and deallocates all the associated strings.
    *  Requires that the text interface which was supplied is still valid.
    */   
   void clearCache();

   /**
    *  Changes the text interface. The previously used text interface
    *  must still be valid when calling this function as the cache is
    *  cleared automatically.
    */ 
   void setTextInterface(NativeTextInterface* textInterface);
private:
private:
   /**
    * Hinder copying.
    */ 
   StringCache(const StringCache& rhs);

   /**
    * Hinder copying.
    */ 
   StringCache& operator=(const StringCache& rhs);

   
   struct Impl;
   Impl* m_impl;
};

#endif /* STRINGCACHE_H */
