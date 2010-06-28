/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef OPENGLESVERTEXBUFFERMEMORYMANAGER_H
#define OPENGLESVERTEXBUFFERMEMORYMANAGER_H

#include "VertexBlockManager.h"
#include "OpenGlEs.h"

class OpenGlEsVertexBlock;

#ifdef GL_OES_VERSION_1_1
#define USE_VBO
#endif
#undef USE_VBO


class OpenGlEsVertexBlockManager : public VertexBlockManager {
public:
   /**
    *   @see VertexBlockManager::storeVertexBuffer
    */
   
   virtual VertexBlock* allocateVertexBlock(const vbuf3x& buf);

   /**
    *   @see VertexBlockManager::updateVertexBuffer
    */

   virtual void updateVertexBlock(VertexBlock* handle,
                                  const vbuf3x& buf);

   /**
    *   Returns an OpenGL ES specific block. Please note
    *   that only handles instantiated by this class can
    *   be used with this function!
    *   
    *   @param bufHandle   The generic pointer to the buffer data.
    */ 
   
   static OpenGlEsVertexBlock* getBufferBlock(VertexBlock* bufHandle);

   /// For deallocation purposes.
   friend class OpenGlEsVertexBlock;
   
private:

   /**
    *   @see VertexBlockManager::releaseVertexBlock
    */
   
   virtual void releaseVertexBlock(VertexBlock* handle);


   /**
    *   Copies the vertex data in buf to the system memory block in block.
    *
    *   @param   block   The destination block.
    *   @param   buf     The vertex data.
    *
    */ 

   void writeToSystemBufferBlock(OpenGlEsVertexBlock* block,
                                 const vbuf3x& buf );
};

#endif /* OPENGLESVERTEXBUFFERMEMORYMANAGER_H */
