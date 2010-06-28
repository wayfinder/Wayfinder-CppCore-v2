/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_GDKPIXBUFIMAGESPEC_H
#define WFAPI_GDKPIXBUFIMAGESPEC_H

#include <gtk/gtk.h>
#include "ImageSpecInternal.h"
#include "BitMapTypes.h"

/**
 * Sub class of ImageSpec using GdkPixbuf as underlying Image data.
 */
class GdkPixbufImageSpec : public ImageSpecInternal {
public:
   /**
    * Constructor. Will create its own reference of the GdkPixbuf.
    *
    * @param gdkPixBuf The GdkPixBuf to use. 
    */
   GdkPixbufImageSpec(GdkPixbuf* gdkPixBuf) {
      m_pixBuf = gdkPixBuf;
      g_object_ref(gdkPixBuf);
      m_width = gdk_pixbuf_get_width(gdkPixBuf);
      m_height = gdk_pixbuf_get_height(gdkPixBuf);
   }
   
   virtual WFAPI::wf_uint32 getWidth() const {
      return m_width;
   }

   virtual WFAPI::wf_uint32 getHeight() const {
      return m_height;
   }

   virtual int load( RGBA32BitMap& bitMap ) const {
      bitMap.imgWidth = bitMap.memWidth = m_width;
      bitMap.imgHeight = bitMap.memHeight = m_height;
      bitMap.data.reserve(bitMap.memWidth * bitMap.memHeight);

      int width, height, rowstride, n_channels;
      guchar *p;
      
      n_channels = gdk_pixbuf_get_n_channels (m_pixBuf);
      g_assert (gdk_pixbuf_get_colorspace (m_pixBuf) == GDK_COLORSPACE_RGB);
      g_assert (gdk_pixbuf_get_bits_per_sample (m_pixBuf) == 8);
      g_assert (gdk_pixbuf_get_has_alpha (m_pixBuf));
      g_assert (n_channels == 4);
      width = gdk_pixbuf_get_width (m_pixBuf);
      height = gdk_pixbuf_get_height (m_pixBuf);
      rowstride = gdk_pixbuf_get_rowstride (m_pixBuf);
      p = gdk_pixbuf_get_pixels (m_pixBuf);

      int rowOffset = (rowstride - bitMap.memWidth * n_channels);
      
      for(unsigned int y = 0; y < bitMap.memHeight; y++) {
         for(unsigned int x = 0; x < bitMap.memWidth; x++) {
            //                    a         b         b        r
            bitMap.data.push_back(p[3] << 24 | p[2] << 16 | p[1] << 8 | p[0]);

            p += n_channels;
         }
         p += rowOffset;
      }
      
      return 0;
   }
   
   /**
    * Destructor.
    */ 
   virtual ~GdkPixbufImageSpec() {
      g_object_unref(m_pixBuf);
   }
   
   /// Our relevant pix buffer.
   GdkPixbuf* m_pixBuf;

   /// The width of the pixbuf
   unsigned int m_width;

   /// The height of the pixbuf
   unsigned int m_height;
};

#endif // WFAPI_GDKPIXBUFIMAGESPEC_H
