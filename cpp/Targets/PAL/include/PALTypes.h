/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef PALTYPES_H
#define PALTYPES_H

#include "PALMachineTypes.h"

namespace pal {

/**
 *  Used to encapsulate a buffer allocated from
 *  within PAL. Upon destruction, it will ensure
 *  that the correct deallocation mechanism is called.
 */ 
class ResultBuffer {
public:

   /**
    *  Creates a new empty result buffer. This can be
    *  passed to PAL functions. PAL will then change
    *  the internal structure as needed.
    */
   ResultBuffer();

   /**
    *  Will deallocate the buffer that is held internally.
    */ 
   ~ResultBuffer();

   /**
    *  @return A byte array with data which is owned by the
    *          instance of this object.
    */ 
   const wf_uint8* getData() const;

   /**
    *  @return The size of the byte array in bytes.
    */ 
   wf_uint32 getSize() const;

   /**
    * This function will initialize the result buffer with
    * proper data. Assumes ownership of the data parameter.
    *
    * @param data The data that the buffer will keep
    * @param size The size of the buffer
    * @param func The correct function for deallocating
    *             the buffer.
    */
   void init(wf_uint8* data, pal::wf_uint32 size, wf_dealloc_func func);
private:
   /**
    *  Deallocates the buffer and resets other attribues.
    */ 
   void reset();
   
   /**
    * Hinder copying.
    */ 
   ResultBuffer(const ResultBuffer& rhs);

   /**
    * Hinder copying.
    */ 
   ResultBuffer& operator=(const ResultBuffer& rhs);

   /// The internal data buffer
   wf_uint8* m_data;

   /// The size of the buffer 
   wf_uint32 m_size;

   /// The deallocator which is responsible for deallocating
   /// the instance of this class
   wf_dealloc_func m_deallocFunc;
};

} // End of namespace pal



#endif /* PALTYPES_H */
