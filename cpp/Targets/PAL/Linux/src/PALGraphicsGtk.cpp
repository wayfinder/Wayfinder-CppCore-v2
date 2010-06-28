/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef OPENGL_PAL_TEXT_RENDERING

#include "PALGraphics.h"
#include <gtk/gtk.h> 
#include <cassert>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <cstdlib>

#include "stb_image.cpp"

#define DBG "[PALGraphicsLinux]: "

namespace pal {

struct PALGraphicsLinuxContext {
   PALGraphicsLinuxContext() :
      m_context( gdk_pango_context_get() ), 
      m_layout( pango_layout_new( m_context ) ),
      m_colorMap( NULL ),
      m_visual( NULL)
      {
         m_visual = gdk_visual_get_best_with_depth( 32 );
         assert( m_visual );
         m_colorMap = gdk_colormap_new( m_visual, true );
         assert( m_colorMap );
      }

   ~PALGraphicsLinuxContext() {
      g_object_unref( m_colorMap );
      g_object_unref( m_visual );
      g_object_unref( m_layout );
      g_object_unref( m_context );
   }
   
   PangoContext* m_context;
   PangoLayout* m_layout;
   GdkColormap* m_colorMap;
   GdkVisual* m_visual;
   unsigned int m_curColor;
};


/**
 *   Returns the pixel size for a given layout and a string.
 */

static void getTextSize( PangoLayout *layout, 
                         const char *str,
                         int &width, int &height )
{
   pango_layout_set_text( layout, str, -1 );
   pango_layout_get_pixel_size( layout, &width, &height );
}

/**
 * Get UTF-8 substring from an UTF-8 string
 * @param str utf-8 string
 * @param start start index 
 * @param length of sub string
 * @return allocated utf-8 sub string
 */

static char *subStringUTF8(const char *str, int startIdx, int length) {

   gchar *start = g_utf8_offset_to_pointer( str, startIdx );
   gchar *end = g_utf8_offset_to_pointer( str, startIdx + length );

   char *subText = new char[end - start + 2]; // +2 for some extra safety :P
   memset(subText, 0, end - start + 2); 
   return g_utf8_strncpy( subText, start, length);   
}

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
   
   assert(comp == reqComp);
   
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
                        BitmapMetrics *metrics)
{
   PALGraphicsLinuxContext* linuxContext = (PALGraphicsLinuxContext*)(context);

   if( utf8Text == NULL ) {
      return PAL_ERR_TEXT_INVALID;
   }
   
   int char_length;
   
   if( length == -1 ) {   
      char_length = g_utf8_strlen( utf8Text, -1 );
      pango_layout_set_text( linuxContext->m_layout, utf8Text, - 1 );
   } else {
      // text iterator, get start
      char *text_i = g_utf8_offset_to_pointer( utf8Text, charPosition );
      
      const char *endOfString = g_utf8_offset_to_pointer( text_i, length );
      // length of first utf-8 character
      char_length = g_utf8_strlen( text_i, endOfString - text_i );
   }

   metrics->m_Width = 0;
   metrics->m_Height = 0;
   metrics->m_OffsetToPenX = 0;
   metrics->m_OffsetToPenY = 0;
   metrics->m_CharCount = char_length;

   return PAL_OK;
}

pstatus drawTextLineToBuffer(GraphicsContext context,
                             const wf_char* utf8Text,
                             wf_int32 charPosition,
                             wf_int32 length,
                             BufFormat format,
                             BitmapMetrics* metrics,
                             ResultBuffer* buffer)
{
   // printf("[PALGraphicsLinux]: %s\n", __FUNCTION__);
   
   PALGraphicsLinuxContext* linuxContext = (PALGraphicsLinuxContext*)(context);

   if( utf8Text == NULL ) {
      return PAL_ERR_TEXT_INVALID;
   }
   
   int char_length;
   
   if( length == -1 ) {   
      char_length = g_utf8_strlen( utf8Text, -1 );
      pango_layout_set_text( linuxContext->m_layout, utf8Text, - 1 );
   } else {
      // text iterator, get start
      char *text_i = g_utf8_offset_to_pointer( utf8Text, charPosition );
      
      const char *endOfString = g_utf8_offset_to_pointer( text_i, length );
      // length of first utf-8 character
      char_length = g_utf8_strlen( text_i, endOfString - text_i );
   
      pango_layout_set_text( linuxContext->m_layout,
                             text_i,
                             endOfString - text_i );
   }

   // get sub string from utf-8
   char *subText = subStringUTF8(utf8Text, charPosition, char_length );
   int width, height;
   getTextSize( linuxContext->m_layout, subText, width, height );
   delete[] subText;
   
   if( width == 0 ) {
      return PAL_ERR_TEXT_INVALID;
   } else if( height == 0 ) {
      return PAL_ERR_TEXT_INVALID;
   }

   GdkPixmap* pixMap = gdk_pixmap_new( NULL, width, height, 32 );
   
   GdkGC* gc = gdk_gc_new( pixMap );
   
   gdk_drawable_set_colormap( pixMap, linuxContext->m_colorMap );
       
   gdk_draw_rectangle( pixMap, gc, 
                       true,  // filled
                       0, 0, -1, -1 );
   
   GdkColor fontColor;

   // transform range from 0-255 to 0-65535

   int r = ( linuxContext->m_curColor >> 24) & 0xff;
   int g = ( linuxContext->m_curColor >> 16) & 0xff;
   int b = ( linuxContext->m_curColor >>  8) & 0xff;
   
   fontColor.red = static_cast<int>(r/ 256.0 * 0xFFFF);
   fontColor.green = static_cast<int>(g / 256.0 * 0xFFFF);
   fontColor.blue = static_cast<int>(b / 256.0 * 0xFFFF);
   
   gdk_draw_layout_with_colors( pixMap, gc, 0, 0, linuxContext->m_layout,
                                &fontColor, NULL);

   
   GdkImage* img = gdk_image_get( pixMap, 0, 0, width, height );

   g_object_unref( pixMap );
   g_object_unref( gc );

   wf_uint8* buf = (wf_uint8*)malloc(width * height * 4);
   buffer->init(buf, width * height * 4, free);

   metrics->m_Width = width;
   metrics->m_Height = height;
   metrics->m_OffsetToPenX = 0;
   metrics->m_OffsetToPenY = 0;
   metrics->m_CharCount = 0;
   
   wf_uint32* asInt = reinterpret_cast<wf_uint32*>(buf);
   
   for ( int y = 0; y < img->height; y++ ) {
      for ( int x = 0; x < img->width; x++ ) {
         *asInt++ = gdk_image_get_pixel( img, x, y );
      }
   }
   
   return PAL_OK;
}

pstatus createGraphicsContext(GraphicsContext* context) {
   //TODO: Change to malloc
   *context = new PALGraphicsLinuxContext;
   return PAL_OK;
}

pstatus destroyGraphicsContext(GraphicsContext context) {
   //TODO: Change to free
   delete ((PALGraphicsLinuxContext*)context);
   return PAL_OK;
}

pstatus setFontForContext(GraphicsContext context,
                          const pal::wf_char* fontFamily,
                          const pal::wf_char* fontStyle,
                          pal::wf_uint32 fontSize)
{
   return PAL_OK;
}

pstatus setFontColorForContext(GraphicsContext context,
                               pal::wf_uint8 r,
                               pal::wf_uint8 b,
                               pal::wf_uint8 g)
{
   return PAL_OK;
}

pstatus runGraphicsTests(GraphicsContext context) {
   return PAL_OK;
}

pstatus setFontFaceForContext(GraphicsContext  context,
                              const pal::wf_char* path)
{
   return PAL_OK;
}


} // End of namespace pal
#endif
