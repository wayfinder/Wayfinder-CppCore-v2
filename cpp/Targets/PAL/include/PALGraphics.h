/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef PALGRAPHICS_H
#define PALGRAPHICS_H

#include <stdio.h>

#include "PAL.h"
#include "PALTypes.h"

// FIXME PALFont is not specific to Linux and should be moved
// FIXME or the types should be defined elsewhere...
#include "../Linux/include/PALFont.h"


/**
 * This file contains declarations for various functions dealing
 * with graphics related operations. 
 */
namespace pal {

/**
 *  The context for the graphics related operations. Various platforms
 *  can implement context setup and caching using this type as a
 *  session variable.
 */ 
typedef void* GraphicsContext;

/**
 *  An enumeration type describing the size and layouts of the components
 *  in a (typically) output buffer.
 */ 
enum BufFormat { RGBA8 };

/**
 *  The image types that can be loaded using the functions below.
 */ 
enum ImageType { PNG, SVG_T };


/**
 * Type to return font alpha mask metrics
 * 
 * use the offsets to realize where the pen is inside the bitmap for instance
 * when you draw a lower case g then you need the m_OffsetToPenY value
 * to be able to place the g so its not sitting too high
 * 
 */
typedef struct {
   int m_Width;
   int m_Height;
   int m_OffsetToPenX;   // fixed point 24.6
   int m_OffsetToPenY;   // fixed point 24.6
   int m_CharCount;   
} BitmapMetrics;


/**
 *  FUNCTION: createGraphicsContext
 *
 *  Initializes a new graphics context to use for the various
 *  graphics related operations.
 *
 *  @param context This value will be set to the newly created context.
 *  
 *  @return PAL_OK if no error.
 */ 
pstatus createGraphicsContext(GraphicsContext* context);

/**
 *  FUNCTION: destroyGraphicsContext
 *   
 *  Destroys a context previously created using initContext.
 *
 *  @param context The graphics context to destroy.
 *
 *  @return PAL_OK if no error.
 */ 
pstatus destroyGraphicsContext(GraphicsContext context);

/**
 *  FUNCTION: loadImageFromFile
 *   
 *  Loads an RGBA buffer from an image stored in a file.
 *  Could be implemented in terms of loadImageToRgbaFromMemory (by
 *  first loading the buffer into memory and then decoding it).
 *
 *
 *  @param context The graphics context to use as environment.
 *  @param source  The file to use as source for the image
 *  @param type    The image type of the file.
 *  @param format  The buffer format of the resulting texture.
 *  @param desiredWidthPixels Vector-based formats can render to any
 *                            dimension. If possible, the function will
 *                            try to load the file using the desired*
 *                            parameters and then widthPixels will be
 *                            set to the that value. If not successful
 *                            (for non-vector formats) widthPixels will be
 *                            set to the width of the source. Using a
 *                            value of 0 means to disregard this argument.
 *  @param desiredHeightPixels See documention for desiredWidthPixels.                           
 *  @param widthPixels   Will contain the width of the output image.
 *  @param heightPixels  Will contain the height of the output image.
 *  @param buffer        The buffer that will contain the output.
 *
 *  ®return PAL_OK if no error.
 *  @return PAL_ERR_UNABLE_TO_DECODE if the image could not be decoded.
 *  @return PAL_ERR_IMG_TYPE_NOT_SUPPORTED if the image type is not
 *                                         supported for this platform.
 *  @return PAL_ERR_BUFFER_FORMAT_NOT_SUPPORTED if the buffer format is not
 *                                              supported for this platform.
 */
pstatus loadImageFromFile(GraphicsContext context,
                          FILE* source,
                          ImageType type,
                          BufFormat format,
                          wf_uint32 desiredWidthPixels,
                          wf_uint32 desiredHeightPixels,
                          wf_uint32* widthPixels,
                          wf_uint32* heightPixels,
                          ResultBuffer* buffer);

/**
 *  FUNCTION: loadImageFromMemory
 *  
 *  Loads an RGBA buffer from an image stored in memorye.
 *
 *  @param context     The graphics context to use as environment.
 *  @param source      The memory buffer to use as source for the image.
 *  @param sourceSize  The size of the memory buffer. 
 *  @param type        The image type of the file.
 *  @param format      The buffer format of the resulting texture.
 *  @param desiredWidthPixels Vector-based formats can render to any
 *                            dimension. If possible, the function will
 *                            try to load the file using the desired*
 *                            parameters and then widthPixels will be
 *                            set to the that value. If not successful
 *                            (for non-vector formats) widthPixels will be
 *                            set to the width of the source.
 *  @param desiredHeightPixels See documention for desiredWidthPixels.                           
 *  @param widthPixels   Will contain the width of the output image.
 *  @param heightPixels  Will contain the height of the output image.
 *  @param buffer        The buffer that will contain the output.
 *
 *  ®return PAL_OK if no error.
 *  @return PAL_ERR_UNABLE_TO_DECODE if the image could not be decoded.
 */
pstatus loadImageFromMemory(GraphicsContext context,
                            const wf_uint8* source,
                            wf_uint32 sourceSize,
                            ImageType type,
                            BufFormat format,
                            wf_uint32 desiredWidthPixels,
                            wf_uint32 desiredHeightPixels,
                            wf_uint32* widthPixels,
                            wf_uint32* heightPixels,
                            ResultBuffer* buffer);


/**
 *  FUNCTION: measureTextLine
 *
 *  This function will measure a single line of text and return the result in metrics structure
 *  
 *  @param context  The graphics context to use as environment.
 *  @param utf8Text A proper UTF-8 text string which is null terminated and
 *                  contains no line breaks.
 *  @param charPosition For substring operations. This represents the first
 *                      character in the string to draw. It is not equivalent
 *                      to the array index, since utf8 chars might span multiple
 *                      bytes.
 *  @param length   The length of the substring that starts at pos. Like
 *                  the previous parameter this denotes the number of
 *                  characters (not bytes). If set to -1, it will use
 *                  the entire string starting from pos.
 *  @param format   The buffer format of the resulting texture.
 *  @param metrics  Will contain the width, height and pen offsets
 *  @return PAL_OK if successfully drawn.
 *  @return PAL_ERR_XXX if error condition
 */
pstatus measureTextLine(GraphicsContext context,
                        const wf_char* utf8Text,
                        wf_int32 charPosition,
                        wf_int32 length,
                        BufFormat format,
                        BitmapMetrics *metrics);
                        
/**
 *  FUNCTION: drawTextLineToBuffer
 *
 *  This function will draw a single line of text unto a buffer in memory.
 *  
 *  @param context  The graphics context to use as environment.
 *  @param utf8Text A proper UTF-8 text string which is null terminated and
 *                  contains no line breaks.
 *  @param charPosition For substring operations. This represents the first
 *                      character in the string to draw. It is not equivalent
 *                      to the array index, since utf8 chars might span multiple
 *                      bytes.
 *  @param length   The length of the substring that starts at pos. Like
 *                  the previous parameter this denotes the number of
 *                  characters (not bytes). If set to -1, it will use
 *                  the entire string starting from pos.
 *  @param format   The buffer format of the resulting texture.
 *  @param metrics  Will contain the width, height and pen offsets
 *  @param buffer   The buffer that will contain the output.
 *
 *  @return PAL_OK if successfully drawn.
 *  @return PAL_ERR_OUT_OF_MEMORY if the buffer could not be allocated
 *          due to memory conditions.
 */
pstatus drawTextLineToBuffer(GraphicsContext context,
                             const wf_char* utf8Text,
                             wf_int32 charPosition,
                             wf_int32 length,
                             BufFormat format,
                             BitmapMetrics* metrics,
                             ResultBuffer* buffer);

/**
 *  FUNCTION: setFontForContext
 *
 *  This function will attempt to change the current font
 *  for a specified graphics context. Subsequent calls to functions
 *  like drawTextToBuffer will use this new font.
 *
 *  Practically, since we do not yet have a common font rasterizer for
 *  all platforms, the implementation this function and subsequent
 *  context behavior is different depending upon the platform. As such,
 *  a font family which is valid for Windows Mobile might not be valid
 *  at all for Linux Mobile.
 *
 *  @param context The graphics context to use as environment.
 *  @param fontFamily The font family of the desired style. E.g. "Verdana".
 *  @param fontStyle  The font style. The four which are mandatory to work
 *                    are "Regular", "Bold", "Italic" and "Bold Italic".
 *  @param fontSize The size of the font.
 *
 *  @return PAL_OK if no error occured.
 *  @return PAL_ERR_TEXT_FONT_FAMILY_INVALID If an invalid font family
 *                                           was specified.
 *  @return PAL_ERR_TEXT_FONT_STYLE_INVALID If an invalid font family
 *                                           was specified.
 */
pstatus setFontForContext(GraphicsContext context,
                          const pal::wf_char* fontFamily,
                          const pal::wf_char* fontStyle,
                          pal::wf_uint32 fontSize);
                          


/**
 *  TODO: Comment
 */ 
pstatus setFontFaceForContext(GraphicsContext  context,
                              const pal::wf_char* path);


pstatus setFontSizeForContext( GraphicsContext context,
                               pal::wf_uint32  size );
                               

pstatus setFontStyleForContext( GraphicsContext context,
                                pal::wf_uint32  style );
                                
pstatus setFontMatrixForContext( GraphicsContext context,
                                 pal::wf_int32   xx,
                                 pal::wf_int32   xy,
                                 pal::wf_int32   yx,
                                 pal::wf_int32   yy );
                               

   
/**
 *  FUNCTION: setFontColorForContext
 *
 *  This function will attempt to change the current font color
 *  for a specified graphics context. Subsequent calls to functions
 *  like drawTextToBuffer will use this new color.
 *
 *  @param context The graphics context to use as environment.
 *  @param r The desired red component color of the font, 0-255.
 *  @param g The desired green component color of the font, 0-255.
 *  @param b The desired blue component color of the font, 0-255.
 *  
 *  @return PAL_OK if no error occured.
 */ 
pstatus setFontColorForContext(GraphicsContext context,
                               pal::wf_uint8 r,
                               pal::wf_uint8 g,
                               pal::wf_uint8 b);
                               
                               
pstatus setFontOutlineColorForContext(GraphicsContext context,
                                      pal::wf_uint8 r,
                                      pal::wf_uint8 g,
                                      pal::wf_uint8 b);
                                      
// default is 1.5
pstatus setFontBoldnessForContext( GraphicsContext context,
                                   float           boldness );

// default is 1.0
pstatus setFontStrokeWidthForContext( GraphicsContext context,
                                      float           strokewidth );


/**
 *  FUNCTION: runGraphicsTests
 *  
 *  This function will execute platform specific tests for a
 *  given context. This is used that a platform specific
 *  implementation behaves as it should. Ideally this should
 *  test all platform specific functionality not covered by
 *  the external regression test tool.
 *
 *  @return PAL_OK if no error occured.
 */ 
pstatus runGraphicsTests(GraphicsContext context);

} // End of namespace PALGraphics

#endif /* PALGRAPHICS_H */
