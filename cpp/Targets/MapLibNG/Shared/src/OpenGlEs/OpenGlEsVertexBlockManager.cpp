/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#define CORE_LOGGING_MODULE_NAME "OpenGlEsVertexBlockManager"
// #define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE

#include "CoreDebugPrint.h"
#include "OpenGlEsVertexBlockManager.h"
#include "Vector2d.h"
#include "OpenGlEsVertexBlock.h"
#include "OpenGlEsUtil.h"


// #ifdef USE_VBO
// #undef USE_VBO
// #endif

void OpenGlEsVertexBlockManager::writeToSystemBufferBlock(OpenGlEsVertexBlock* block,
                                                          const vbuf3x& buf)
{
   if(block->location == OpenGlEsVertexBlock::SYSTEM_MEMORY) {
      delete[] block->data.bufferVector;
      block->data.bufferVector = new vec3x[ buf.size() ];
      std::copy(buf.begin(), buf.end(), block->data.bufferVector);
   }

}

void OpenGlEsVertexBlockManager::updateVertexBlock(VertexBlock* handle,
                                                   const vbuf3x& buf)
{
   OpenGlEsVertexBlock* block = getBufferBlock(handle);

   block->size = buf.size();
   
#ifdef USE_VBO
   if (block->location == OpenGlEsVertexBlock::VIDEO_MEMORY) {
      glBindBuffer(GL_ARRAY_BUFFER, block->data.bufferHandle);

      glBufferData(GL_ARRAY_BUFFER, buf.size() * sizeof( vec3x),
                    &buf[0], GL_STATIC_DRAW );
      
      glBindBuffer(GL_ARRAY_BUFFER, 0);

      GLint err = glGetError();
      if(err != GL_NO_ERROR) {
         block->location = OpenGlEsVertexBlock::SYSTEM_MEMORY;
         writeToSystemBufferBlock(block, buf);
      }
         
   } else {
      writeToSystemBufferBlock(block, buf);
   }
#else
   writeToSystemBufferBlock(block, buf);
#endif

   OpenGlEsUtil::assertNoErrors();
}


VertexBlock*
OpenGlEsVertexBlockManager::allocateVertexBlock(const vbuf3x& buf)
{
   OpenGlEsVertexBlock* block = new OpenGlEsVertexBlock(this);
   block->data.bufferVector = NULL;
#ifdef USE_VBO
   glGenBuffers( 1, &block->data.bufferHandle);
   GLint err = glGetError();
   if (err == GL_NO_ERROR) {
      block->location = OpenGlEsVertexBlock::VIDEO_MEMORY;
   } else {
      block->location = OpenGlEsVertexBlock::SYSTEM_MEMORY;
   }

#else
   block->data.bufferVector = NULL;
   block->location = OpenGlEsVertexBlock::SYSTEM_MEMORY;
#endif
   
   OpenGlEsUtil::assertNoErrors();
   VertexBlock* vBlock = block;
   updateVertexBlock(vBlock, buf);
   return vBlock;
}

void
OpenGlEsVertexBlockManager::releaseVertexBlock(VertexBlock* handle)
{
   OpenGlEsVertexBlock* block = getBufferBlock(handle);
   
   if (block == NULL){
      return;
   }
   
#ifdef USE_VBO
   if (block->location == OpenGlEsVertexBlock::VIDEO_MEMORY) {
      glDeleteBuffers(1, &block->data.bufferHandle);
   }

   OpenGlEsUtil::assertNoErrors();
#endif
   
   if (block->location == OpenGlEsVertexBlock::SYSTEM_MEMORY){
      delete[] block->data.bufferVector;
   }
   
}


OpenGlEsVertexBlock*
OpenGlEsVertexBlockManager::getBufferBlock(VertexBlock* bufHandle)
{
   OpenGlEsVertexBlock* ret = static_cast<OpenGlEsVertexBlock*>(bufHandle);

   return ret;
}
