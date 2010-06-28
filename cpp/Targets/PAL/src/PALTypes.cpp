/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "PALTypes.h"
#include <cstdlib>

namespace pal {

ResultBuffer::ResultBuffer() :
   m_data(NULL),
   m_size(0),
   m_deallocFunc(NULL)
{
   
}

ResultBuffer::~ResultBuffer()
{
   reset();
}

const wf_uint8* ResultBuffer::getData() const {
   return m_data;
}

wf_uint32 ResultBuffer::getSize() const {
   return m_size;
}

void ResultBuffer::reset() {
   if(m_deallocFunc != NULL) {
      (*m_deallocFunc)(m_data);
   }
   
   m_deallocFunc = NULL;
   m_data = NULL;
   m_size = 0;
}

void ResultBuffer::init(wf_uint8* data, wf_uint32 size, wf_dealloc_func func) {
   reset();
   
   m_deallocFunc = func;
   m_data = data;
   m_size = size;
}

} // End of namespace pal

