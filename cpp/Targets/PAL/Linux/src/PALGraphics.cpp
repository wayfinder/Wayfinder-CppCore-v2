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
#include "PALFont.h"
#include <gtk/gtk.h>
#include <cassert>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <cstdlib>
#include "stb_image.cpp"

#define DBG "[PALGraphicsLinux]: "

namespace pal {

typedef struct {
   void *fontContext;
} PALGraphicsLinuxContext;


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
   
   // printf("comp = %d, reqComp = %d\n", comp, reqComp);
   
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

   assert(comp == reqComp);

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
   PALGraphicsLinuxContext* linuxContext = (PALGraphicsLinuxContext*)(context);

   int            err = 0;
   int            aPenX;
   int            aPenY;
   int            ttWidth;
   int            ttHeight;
   unsigned int*  ttBitmap = NULL;
   PalFont_BBox_t aBBox;
   int            nChars;
   
   int x, y;
      
   err = PalFont_RenderText( linuxContext->fontContext,
                        NULL, // unsigned     *aBitmap,
                        0, //int           aBitmapWidth,
                        0, //int           aBitmapHeight,
                        utf8Text, //char         *text,
                        charPosition,
                        length, //int           textLength,
                        NULL, //int          *aPen_x,
                        NULL, //int          *aPen_y,
                        &aBBox, //       *aBBox,
                        &nChars,
                        1 ); //int           resetBox );
   
   if ( err != 0 ) {
      return (pstatus)err;
   }
   
   // bounding box is in fractional units so round up to integer pixels
   ttWidth  = ( ( aBBox.xMax + 63 ) >> 6 ) - ( aBBox.xMin >> 6 );
   ttHeight = ( ( aBBox.yMax + 63 ) >> 6 ) - ( aBBox.yMin >> 6 );
   
   // return the metrics
   if ( metrics != NULL ) {
      metrics->m_Width        = ttWidth;
      metrics->m_Height       = ttHeight;
      metrics->m_OffsetToPenX = - ( aBBox.xMin >> 6 ) << 6;
      metrics->m_OffsetToPenY = - ( aBBox.yMin >> 6 ) << 6;
      metrics->m_CharCount    = nChars;
   }
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
   PALGraphicsLinuxContext* linuxContext = (PALGraphicsLinuxContext*)(context);

   int            err = 0;
   int            aPenX;
   int            aPenY;
   int            ttWidth;
   int            ttHeight;
   unsigned int*  ttBitmap = NULL;
   PalFont_BBox_t aBBox;
   int            nChars;
   
   int x, y;
      
   err = PalFont_RenderText( linuxContext->fontContext,
                        NULL, // unsigned     *aBitmap,
                        0, //int           aBitmapWidth,
                        0, //int           aBitmapHeight,
                        utf8Text, //char         *text,
                        charPosition,
                        length, //int           textLength,
                        NULL, //int          *aPen_x,
                        NULL, //int          *aPen_y,
                        &aBBox, //       *aBBox,
                        &nChars,
                        1 ); //int           resetBox );
   
   if ( err != 0 ) {
      return (pstatus)err;
   }

   // bounding box is in fractional units so round up to integer pixels
   ttWidth  = ( ( aBBox.xMax + 63 ) >> 6 ) - ( aBBox.xMin >> 6 );
   ttHeight = ( ( aBBox.yMax + 63 ) >> 6 ) - ( aBBox.yMin >> 6 );
   
   // allocate a bitmap just big enough
   ttBitmap = ( unsigned int * )malloc( 4 * ttWidth * ttHeight );
   if ( ttBitmap == NULL ) {
      return PAL_OUT_OF_MEMORY;
   }   
   memset(ttBitmap, 0, ttWidth * ttHeight * 4);

   // make sure we position the pen correctly
   aPenX = - ( aBBox.xMin >> 6 ) << 6;
   aPenY = - ( aBBox.yMin >> 6 ) << 6;
   
   err = PalFont_RenderText( linuxContext->fontContext, ttBitmap, ttWidth, ttHeight,
                  utf8Text, charPosition, length, &aPenX, &aPenY, &aBBox, NULL, 1 );

   // return the metrics
   if ( metrics != NULL ) {
      metrics->m_Width        = ttWidth;
      metrics->m_Height       = ttHeight;
      metrics->m_OffsetToPenX = - ( aBBox.xMin >> 6 ) << 6;
      metrics->m_OffsetToPenY = - ( aBBox.yMin >> 6 ) << 6;
      metrics->m_CharCount    = nChars;
   }

#if 0
   printf("widthPixels: %u\n", *widthPixels);
   printf("heightPixels: %u\n", *heightPixels);
   for ( y = 0; y < ttHeight; y++ ) {
      for ( x = 0; x < ttWidth; x++ ) {
         printf( "0x%08x", ttBitmap[ y * ttWidth + x ] );
      }
      printf( "\n" );
   }
#endif   
   buffer->init(reinterpret_cast<uint8*>(ttBitmap), ttWidth * ttHeight * 4, free);
   
   return PAL_OK;
}

pstatus createGraphicsContext(GraphicsContext* context) {

   PALGraphicsLinuxContext *linuxContext =
      ( PALGraphicsLinuxContext *)malloc( sizeof( PALGraphicsLinuxContext ) );

   if ( linuxContext == NULL ) {
      return PAL_OUT_OF_MEMORY;
   }

   if ( PalFont_Initialise( &linuxContext->fontContext ) != 0 ) {
      return PAL_ERR_FONT_FAILED_TO_INITIALIZE;
   }

   *context = ( GraphicsContext )linuxContext;

   return PAL_OK;
}

pstatus destroyGraphicsContext(GraphicsContext context) {

   pstatus err = PAL_OK;
   PALGraphicsLinuxContext *linuxContext = ( PALGraphicsLinuxContext * )context;

   if ( linuxContext == NULL ) {
      return PAL_ERR_GRAPHICS_CONTEXT_INVALID;
   }

   if(PalFont_Finalise( linuxContext->fontContext ) != 0) {
      err = PAL_ERR_FONT_FAILED_TO_FINALIZE;
   }

   // Free structure
   free ( linuxContext );
   return err;
}

pstatus setFontColorForContext( GraphicsContext context,
                                pal::wf_uint8 r,
                                pal::wf_uint8 g,
                                pal::wf_uint8 b)
{
   PALGraphicsLinuxContext* linuxContext = (PALGraphicsLinuxContext*)(context);
   if ( PalFont_Color_Set( linuxContext->fontContext,  MAKE_ARGB( 0, r, g, b )  ) != 0 ) {
      return PAL_ERR_TEXT_FONT_STYLE_INVALID;
      }
   return PAL_OK;
}

pstatus setFontOutlineColorForContext( GraphicsContext context,
                                       pal::wf_uint8 r,
                                       pal::wf_uint8 g,
                                       pal::wf_uint8 b ) {
   PALGraphicsLinuxContext* linuxContext = (PALGraphicsLinuxContext*)(context);
   if ( PalFont_OutlineColor_Set( linuxContext->fontContext,  MAKE_ARGB( 0, r, g, b )  ) != 0 ) {
      return PAL_ERR_TEXT_FONT_STYLE_INVALID;
      }
   return PAL_OK;
}

pstatus setFontSizeForContext( GraphicsContext context,
                               pal::wf_uint32  size ) {
   PALGraphicsLinuxContext* linuxContext = (PALGraphicsLinuxContext*)(context);
   if ( PalFont_Size_Set( linuxContext->fontContext, size ) ) {
      return PAL_ERR_TEXT_FONT_STYLE_INVALID;
      }
   return PAL_OK;
}

// default is 1.5
pstatus setFontBoldnessForContext( GraphicsContext context,
                                   float           boldness ) {
   PALGraphicsLinuxContext* linuxContext = (PALGraphicsLinuxContext*)(context);
   if ( PalFont_BoldnessFactor_Set( linuxContext->fontContext, ( int )( 65536.0 * boldness ) ) != 0 ) {
      return PAL_ERR_TEXT_FONT_STYLE_INVALID;
      }
   return PAL_OK;
}

// default is 1.0
pstatus setFontStrokeWidthForContext( GraphicsContext context,
                                      float           strokewidth ) {
   PALGraphicsLinuxContext* linuxContext = (PALGraphicsLinuxContext*)(context);
   if ( PalFont_StrokeWidthFactor_Set( linuxContext->fontContext, ( int )( 65536.0 * strokewidth) ) != 0 ) {
      return PAL_ERR_TEXT_FONT_STYLE_INVALID;
      }
   return PAL_OK;
}


pstatus setFontStyleForContext( GraphicsContext context,
                                pal::wf_uint32  style ) {
   PALGraphicsLinuxContext* linuxContext = (PALGraphicsLinuxContext*)(context);
   if ( PalFont_Style_Set( linuxContext->fontContext, style ) != 0 ) {
      return PAL_ERR_TEXT_FONT_STYLE_INVALID;
   }
   return PAL_OK;
}

pstatus setFontMatrixForContext( GraphicsContext context,
                                 pal::wf_int32   xx,
                                 pal::wf_int32   xy,
                                 pal::wf_int32   yx,
                                 pal::wf_int32   yy ) {
   PALGraphicsLinuxContext* linuxContext = (PALGraphicsLinuxContext*)(context);
   PalFont_Matrix_t matrix;
   matrix.xx = xx;
   matrix.xy = xy;
   matrix.yx = yx;
   matrix.yy = yy;   
   PalFont_Matrix_Set( linuxContext->fontContext, matrix );
   return PAL_OK;
}

pstatus setFontForContext(GraphicsContext context,
                          const pal::wf_char* fontFamily,
                          const pal::wf_char* fontStyle,
                          pal::wf_uint32 fontSize)
{
   setFontSizeForContext(context, fontSize);
   //TODO: implement
   return PAL_OK;
}

pstatus setFontFaceForContext(GraphicsContext  context,
                              const pal::wf_char* path)
{
   PALGraphicsLinuxContext* linuxContext = (PALGraphicsLinuxContext*)(context);
   PalFont_Face_Set( linuxContext->fontContext, path );
   return PAL_OK;
}

pstatus runGraphicsTests(GraphicsContext context) {
   int ap = 22;
   ap++;
   return PAL_OK;
}



} // End of namespace pal
