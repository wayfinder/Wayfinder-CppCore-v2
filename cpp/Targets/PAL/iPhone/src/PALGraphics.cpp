/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "PALGraphics.h"
#include <cassert>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <cstdlib>
#include "stb_image.cpp"

#define DBG "[PALGraphicsiPhone]: "

namespace pal {

pstatus loadImageFromMemory(GraphicsContext context,
                            const wf_uint8* source,
                            wf_uint32 sourceSize,
                            ImageType type,
                            BufFormat format,
                            wf_uint32 desiredWidthPixels,
                            wf_uint32 desiredHeightPixels,
                            wf_uint32* widthPixels,
                            wf_uint32* heightPixels,
                            ResultBuffer* buffer)
{
   if(type != PNG) {
      return PAL_ERR_IMG_TYPE_NOT_SUPPORTED;
   }

   if(format != RGBA8) {
      return PAL_ERR_BUFFER_FORMAT_NOT_SUPPORTED;
   }

   assert(sizeof(stbi_uc) == sizeof(wf_uint8));

   int reqComp  = 4; // Always read RGBA
   int comp     = 0;

   pal::wf_int32 width = 0;
   pal::wf_int32 height = 0;

   wf_uint8* buf =
      stbi_load_from_memory(source,
                            sourceSize,
                            &width,
                            &height,
                            &comp,
                            reqComp);

   assert(width >= 0);
   assert(height >= 0);
   
   *widthPixels = width;
   *heightPixels = height;

   // printf("{stb}: comp = %d, reqComp = %d\n", comp, reqComp);
   // printf("{stb}: dimensions = %dx%d\n", width, height);

   // Some archane PNG:s are not possible to read
   if(width == 0 || height ==0) {
      free(buf);
      return PAL_ERR_IMG_TYPE_NOT_SUPPORTED;
   }

   // assert(comp == reqComp);
   
   buffer->init(buf, (*widthPixels) * (*heightPixels) * 4, free);
   
   return PAL_OK;
}

pstatus loadImageFromFile(GraphicsContext context,
                          FILE* source,
                          ImageType type,
                          BufFormat format,
                          wf_uint32 desiredWidthPixels,
                          wf_uint32 desiredHeightPixels,
                          wf_uint32* widthPixels,
                          wf_uint32* heightPixels,
                          ResultBuffer* buffer)
{
   if(type != PNG) {
      return PAL_ERR_IMG_TYPE_NOT_SUPPORTED;
   }

   if(format != RGBA8) {
      return PAL_ERR_BUFFER_FORMAT_NOT_SUPPORTED;
   }

   assert(sizeof(stbi_uc) == sizeof(wf_uint8));

   int reqComp  = 4; // Always read RGBA
   int comp     = 0;

   pal::wf_int32 width = 0;
   pal::wf_int32 height = 0;

   unsigned char* buf =
      stbi_load_from_file(source, &width, &height, &comp, reqComp);

   *widthPixels = width;
   *heightPixels = height;

   // assert(comp == reqComp);

   buffer->init(buf, (*widthPixels) * (*heightPixels) * 4, free);

   return PAL_OK;
}

pstatus measureTextLine(GraphicsContext context,
                        const wf_char* utf8Text,
                        wf_int32 charPosition,
                        wf_int32 length,
                        BufFormat format,
                        BitmapMetrics *metrics )
{
   return PAL_OK;
}   
   

pstatus drawTextLineToBuffer(GraphicsContext context,
                             const wf_char* utf8Text,
                             wf_int32 charPosition,
                             wf_int32 length,
                             BufFormat format,
                             BitmapMetrics *metrics,
                             ResultBuffer* buffer)
{
   return PAL_OK;
}

pstatus createGraphicsContext(GraphicsContext* context) {
   *context = NULL;
   return PAL_OK;
}

pstatus destroyGraphicsContext(GraphicsContext context) {
   return PAL_OK;
}

pstatus setFontColorForContext( GraphicsContext context,
                                pal::wf_uint8 r,
                                pal::wf_uint8 g,
                                pal::wf_uint8 b)
{
   return PAL_OK;
}

pstatus setFontOutlineColorForContext( GraphicsContext context,
                                       pal::wf_uint8 r,
                                       pal::wf_uint8 g,
                                       pal::wf_uint8 b )
{
   return PAL_OK;
}

pstatus setFontSizeForContext( GraphicsContext context,
                               pal::wf_uint32  size )
{
   return PAL_OK;
}

// default is 1.5
pstatus setFontBoldnessForContext( GraphicsContext context,
                                   float           boldness )
{
   return PAL_OK;
}

// default is 1.0
pstatus setFontStrokeWidthForContext( GraphicsContext context,
                                      float           strokewidth )
{
   return PAL_OK;
}


pstatus setFontStyleForContext( GraphicsContext context,
                                pal::wf_uint32  style )
{
   return PAL_OK;
}

pstatus setFontMatrixForContext( GraphicsContext context,
                                 pal::wf_int32   xx,
                                 pal::wf_int32   xy,
                                 pal::wf_int32   yx,
                                 pal::wf_int32   yy )
{
   return PAL_OK;
}

pstatus setFontForContext(GraphicsContext context,
                          const pal::wf_char* fontFamily,
                          const pal::wf_char* fontStyle,
                          pal::wf_uint32 fontSize)
{
   return PAL_OK;
}

pstatus setFontFaceForContext(GraphicsContext  context,
                              const pal::wf_char* path)
{
   return PAL_OK;
}




} // End of namespace pal
