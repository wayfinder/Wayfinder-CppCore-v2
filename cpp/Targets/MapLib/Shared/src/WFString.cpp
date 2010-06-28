/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "WFString.h"
#include <string.h>


namespace WFAPI {

void
WFString::init(const char* str)
{
   if ( str && str[0] != '\0' ) {
      m_len = strlen(str) + 1;
      m_str = new char[m_len];
      // Then copy
      memcpy(m_str, str, m_len);
      --m_len;
   } else {
      // The string is empty. We have a special case for that.
      m_str = const_cast<char*>(c_emptyString);
      m_len = 0;
   }
}

void
WFString::init(const WFString& other)
{
   if ( other.m_str[0] != '\0' ) {
      m_len = other.m_len + 1;
      m_str = new char[m_len];
      // Then copy
      memcpy(m_str, other.m_str, m_len);
      --m_len;
   } else {
      // The string is empty. We have a special case for that.
      m_str = const_cast<char*>(c_emptyString);
      m_len = 0;
   }
}

const char* const WFString::c_emptyString = "";

WFString::WFString()
   : m_str(const_cast<char*>(c_emptyString)), m_len(0)
{
}

WFString::WFString(const char* str)
{
   init(str);
}


WFString::WFString(const WFString& other)
{
   init(other);
}

WFString&
WFString::operator=(const char* str)
{
   if ( m_str != c_emptyString ) {
      delete [] m_str;
   }
   init(str);
   return *this;
}

void
WFString::clear()
{
   if ( m_str != c_emptyString ) {
      delete [] m_str;
   }
   m_str = const_cast<char*>(c_emptyString);
   m_len = 0;
}

WFString&
WFString::operator=(const WFString& other)
{
   if ( &other != this ) {
      if ( m_str != c_emptyString ) {
         delete [] m_str;
      }
      init(other);
   }
   return *this;
}

WFString::~WFString()
{
   if ( m_str != c_emptyString ) {
      delete [] m_str;
   }
}

const char*
WFString::c_str() const
{
   return m_str;
}

unsigned int
WFString::length() const
{
   return m_len;
}

int
WFString::empty() const
{
   return !m_len;
}

bool
WFString::operator<(const WFString& other) const
{
   return strcmp(m_str, other.m_str) < 0;
}

bool
WFString::operator<(const char* str) const
{
   return strcmp(m_str, str) < 0;
}

bool
WFString::operator>(const WFString& other) const
{
   return strcmp(m_str, other.m_str) > 0;
}

bool
WFString::operator>(const char* str) const
{
   return strcmp(m_str, str) > 0;
}

bool
WFString::operator==(const char* other ) const
{
   return strcmp(m_str, other) == 0;
}

bool
WFString::operator==(const WFString& other) const
{   
   return strcmp(m_str, other.m_str) == 0;
}

bool
WFString::operator!=(const WFString& other) const
{
   return !( (*this) == other);
}

bool
WFString::operator!=(const char* other) const
{
   return !( (*this) == other);
}

char
WFString::operator[] (int i) const
{
   return m_str[i];
}

}

