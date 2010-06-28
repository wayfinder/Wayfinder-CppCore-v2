/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "CoreFunctionPrologue.h"
#include "FontSpec.h"
#include "FontSpecInternal.h"

namespace WFAPI {

FontSpec::~FontSpec()
{
   
}

FontSpec::FontSpec(const WFString& fontString,
                   unsigned int fontSize) :
   m_fontString(fontString),
   m_fontSize(fontSize)
{
   CORE_FUNCTION_PROLOGUE();
}

FontSpec::FontSpec() :
   m_fontString(),
   m_fontSize(0)
{
   CORE_FUNCTION_PROLOGUE();
}

const WFString& FontSpec::getFontString()
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_fontString;
}

unsigned int FontSpec::getFontSize()
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_fontSize;
}

bool FontSpec::operator==(const FontSpec& rhs)
{
   CORE_FUNCTION_PROLOGUE();
   
   return m_fontString == rhs.m_fontString && m_fontSize == rhs.m_fontSize;
}

bool FontSpec::operator!=(const FontSpec& rhs)
{
   CORE_FUNCTION_PROLOGUE();
   
   return !(*this==rhs);
}

bool FontSpec::operator<(const FontSpec& rhs)
{
   CORE_FUNCTION_PROLOGUE();
   
   if(m_fontString < rhs.m_fontString) {
      return true;
   } if(m_fontString > rhs.m_fontString) {
      return false;
   }
   
   return m_fontSize < rhs.m_fontSize;
}

FontSpec* FontSpec::allocate()
{
   CORE_FUNCTION_PROLOGUE();
   
   return new FontSpecInternal;
}

FontSpec* FontSpec::allocate(const WFString& fontString, unsigned int fontSize)
{
   CORE_FUNCTION_PROLOGUE();
   
   return new FontSpecInternal(fontString, fontSize);
}

} // End of namespace WFAPI

