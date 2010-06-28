/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#define CORE_LOGGING_MODULE_NAME "OpenGlEsTextureBlockManager"
// #define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE

#include "CoreDebugPrint.h"
#include "config.h"
#include "OpenGlEsTextureBlockManager.h"
#include "OpenGlEsTextureBlock.h"
#include "BitMapLoader.h"
#include "BitMapUtil.h"
#include "BitMapPixelBoxAlgorithms.h"
#include "OpenGlEsUtil.h"


TextureBlock* 
OpenGlEsTextureBlockManager::allocateFromRawData(const pal::wf_uint8* srcData,
                                                 pal::wf_uint32 srcWidth,
                                                 pal::wf_uint32 srcHeight)
{  
   static RGBA32BitMap pow2Tex;
   pow2Tex.data.clear();
   pow2Tex.imgWidth = pow2Tex.imgHeight = 0;
   pow2Tex.memWidth = pow2Tex.memHeight = 0;
   
   // The expander will re-initialize the members of pow2Tex.
   BitMapUtil::expandPow2(srcData, srcWidth, srcHeight, pow2Tex);
   
   GLuint texHandle;
   glGenTextures( 1, &texHandle );

   glBindTexture( GL_TEXTURE_2D, texHandle );
   glTexImage2D( GL_TEXTURE_2D, 0,
                 GL_RGBA,
                 pow2Tex.memWidth, pow2Tex.memHeight,
                 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, &pow2Tex.data[0] );

   OpenGlEsUtil::assertNoErrors();
   
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   
   OpenGlEsUtil::assertNoErrors();

   MC2Point anchor = MC2Point( pow2Tex.imgWidth / 2,
                               pow2Tex.imgHeight / 2 );

   PixelBox boundingBox = BitMapPixelBoxAlgorithms::getColoredArea( pow2Tex );
   
   OpenGlEsTextureBlock* ret =
      new OpenGlEsTextureBlock( anchor,
                                boundingBox,
                                this,
                                texHandle,
                                pow2Tex.memWidth,
                                pow2Tex.memHeight );

   return ret;
}

void OpenGlEsTextureBlockManager::releaseTextureBlock( TextureBlock* handle )
{
   OpenGlEsTextureBlock* texBlock = static_cast<OpenGlEsTextureBlock*>( handle );

   glDeleteTextures(1, &texBlock->texHandle );
   
   OpenGlEsUtil::assertNoErrors();
}
