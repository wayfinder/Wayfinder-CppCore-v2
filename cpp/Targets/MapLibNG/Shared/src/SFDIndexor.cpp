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

#include <string.h>

#include <algorithm>

#include "SFDIndexor.h"



bool
SFDIndexor::operator<( const char* other ) const
{
   return strcmp( getStr(), other ) < 0;
}

bool
SFDIndexor::operator<( const SFDIndexor& other ) const
{
   return *this < other.getStr();
}

bool
SFDIndexor::operator<( const MC2SimpleString& other ) const
{
   return *this < other.c_str();
}

bool operator<( const MC2SimpleString& str1,
                const SFDIndexor& str2 )
{
   return str1 < str2.getStr();
}

void
SFDIndexor::swap( SFDIndexor& other )
{
   std::swap( m_str, other.m_str );
   std::swap( m_strNbr, other.m_strNbr );
   std::swap( m_strOffset, other.m_strOffset );
   std::swap( m_strAllocSize, other.m_strAllocSize );
   std::swap( m_stringRead, other.m_stringRead );
}

void SFDIndexor::allocStr( int size )
{
   delete [] m_str;
   m_str = new char[ size ];
   m_strAllocSize = size;
}

SFDIndexor&
SFDIndexor::operator=( const SFDIndexor& other )
{
   // Slower?
   if ( this != &other ) {      
      // We can maybe re-use the old string.
      char* old_str = m_str;
      uint16 old_size = m_strAllocSize;
      // Do the base assignment.
      SFDIndexorBase::operator=(other);
      if ( ! m_str ) {
         delete [] old_str;
      } else {
         if ( old_size >= m_strAllocSize ) {
            m_strAllocSize = old_size;         
         } else {
            delete [] old_str;
            old_str = new char[m_strAllocSize];
         }
         strcpy( old_str, m_str );
         m_str = old_str;
      }
   }
   return *this;
}

SFDIndexorBase::~SFDIndexorBase()
{
   delete [] m_str;
}
