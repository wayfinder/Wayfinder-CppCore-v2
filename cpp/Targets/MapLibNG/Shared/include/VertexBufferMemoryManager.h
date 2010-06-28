/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef VERTEXBUFFERMEMORYMANAGER_H
#define VERTEXBUFFERMEMORYMANAGER_H

#include "Vector2d.h"

class VertexBufferMemoryManager {
public:
   typedef void* Handle;

   /**
    *   Stores the vertex buffer into (possibly) faster memory.
    *
    *   @param   handle   The return handle to the newly created vertex buffer.
    *                     Shall be set to 0 if memory cannot be allocated.
    *   @param   color    The color of the vertex buffer.
    *   @param   buf      The vertex buffer data to store in the memory.
    *   
    */

   virtual void storeVertexBuffer( Handle& handle,
                                   unsigned int color,
                                   const VertexBuffer& buf ) = 0;

   /**
    *   Update the vertex buffer using the previously created handle.
    *
    *   @param   handle   The previously stored handle. It shall be asserted
    *                     that handle is not 0.
    *   @param   color    The color of the vertex buffer.
    *   @param   buf      The vertex buffer data to update from.
    *                     NOTE: The size of the new data MUST match the size
    *                     of the buffer initially passed to storeVertexBuffer.
    *   
    */

   virtual void updateVertexBuffer( Handle& handle,
                                    unsigned int color,
                                    const VertexBuffer& buf ) = 0;
   
   /**
    *   Releases a previously loaded vertex buffer from memory.
    *
    *   @param   handle   The handle to the vertex buffer. It shall be asserted
    *                     that handle is not 0.
    *
    */
   
   virtual void releaseVertexBuffer( Handle& handle ) = 0;
   
};

#endif /* VERTEXBUFFERMEMORYMANAGER_H */
