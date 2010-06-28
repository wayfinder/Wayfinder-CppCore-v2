/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef TEXTUREBLOCKMANAGER_H
#define TEXTUREBLOCKMANAGER_H

#include "BitMapLoader.h"
#include "PALMachineTypes.h"

class TextureBlock;
class RGBA32BitMap;

class TextureBlockManager {
public:
   virtual ~TextureBlockManager();

   /**
    *   Stores the texture into (possibly) faster memory.
    *
    *   @param   handle   The return handle to the newly created texture.
    *                     Shall be set to 0 if memory cannot be allocated.
    *   @param   tex      The RGBA texture in system memory.
    *   
    */
   TextureBlock* allocateFromBitMap( const RGBA32BitMap& tex );

   
   /**
    *   Stores the texture into (possibly) faster memory.
    *
    *   @param   handle    The return handle to the newly created texture.
    *                      Shall be set to 0 if memory cannot be allocated.
    *   @param   srcData   The RGBA texture in system memory.
    *   @param   srcWidth  The width of the source in pixels
    *   @param   srcHeight The height of the source in pixels
    *   
    */   
   virtual TextureBlock* allocateFromRawData(const pal::wf_uint8* srcData,
                                             pal::wf_uint32 srcWidth,
                                             pal::wf_uint32 srcHeight) = 0;
                                               
   friend class TextureBlock;
protected:

   /**
    *   Releases a previously loaded texture from memory.
    *
    *   @param   handle   The handle to the memory. It shall be asserted
    *                     that handle is not 0.
    *
    */
   
   virtual void releaseTextureBlock( TextureBlock* handle ) = 0;
};

#endif /* TEXTUREBLOCKMANAGER_H */
