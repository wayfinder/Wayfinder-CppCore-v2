/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "Common/Identifier.h"

namespace gubi {

Identifier::Identifier(
   const std::wstring& strIdentity )
{
   m_iIdentity = HashString(strIdentity.c_str(), strIdentity.length());
}

Identifier::Identifier(
   const wchar_t* pIdentity )
{
   if (pIdentity != NULL)
   {
      m_iIdentity = HashString(pIdentity, wcslen(pIdentity));
   }
   else
   {
      m_iIdentity = 0;
   }
}

Identifier::Identifier(
   unsigned int iIdentity )
{
   m_iIdentity = iIdentity;
}

bool Identifier::operator<(
   const Identifier& refIdentifier ) const
{
   return m_iIdentity < refIdentifier.m_iIdentity;
}

Identifier::operator unsigned int(
   void ) const
{
   return m_iIdentity;
}

unsigned int Identifier::HashString(
   const wchar_t* pIdentity,
   unsigned int iLength )
{
   unsigned int iHash = 5381;

   for (unsigned int i = 0; i < iLength; ++i)
   {
      iHash = iHash * 33 + tolower(pIdentity[i]);
   }

   return iHash;
}

}
