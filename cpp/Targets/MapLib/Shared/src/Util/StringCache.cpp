/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "Util/StringCache.h"
#include "NativeTextInterface.h"
#include "MC2SimpleString.h"
#include <map>

struct StringCache::Impl {
   Impl(NativeTextInterface* textInterface) :
      textInterface(textInterface)
      {}

   /**
    *  The text interface that will create the strings
    */
   NativeTextInterface* textInterface;

   /** 
    *    Cache of already created strings.
    */
   std::map<MC2SimpleString, STRING*> strings;
};

StringCache::StringCache(NativeTextInterface* textInterface) :
   m_impl(new Impl(textInterface))
{
   
}

const STRING* StringCache::getOrCreateString(const MC2SimpleString& utf8Text)
{
   if(!m_impl->textInterface) {
      return NULL;
   }
   
   std::map<MC2SimpleString, STRING*>::const_iterator it =
      m_impl->strings.find( utf8Text );
   
   if ( it != m_impl->strings.end() ) {
      // Got cache hit, return new string
      return it->second;
   } else {
      // No STRING found, create one, insert it and then return it
      STRING* ret = m_impl->textInterface->createString( utf8Text.c_str() ); 
      m_impl->strings.insert(std::make_pair( utf8Text, ret ));
      return ret;
   } 
}

StringCache::~StringCache()
{
   clearCache();
   delete m_impl;
}

void StringCache::setTextInterface(NativeTextInterface* textInterface)
{
   clearCache();
   m_impl->textInterface = textInterface;
}

void StringCache::clearCache()
{
   for ( std::map<MC2SimpleString, STRING*>::iterator it =
            m_impl->strings.begin();
         it != m_impl->strings.end();
         ++it )
   {
      if(m_impl->textInterface) {
         m_impl->textInterface->deleteString( it->second );         
      }
   }

   m_impl->strings.clear();
}
