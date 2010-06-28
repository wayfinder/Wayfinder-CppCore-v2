/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "OpenGlEs/OpenGlEsStateDumping.h"
#include "OpenGlEs.h"
#include <stdio.h>

struct EnumLiteral {
   GLenum e;
   const char* name;
};

#define EDEF(VAR) { (GLenum)(VAR), #VAR }

static EnumLiteral enums[] = {

/* ClearBufferMask */
   EDEF(GL_DEPTH_BUFFER_BIT),
   EDEF(GL_STENCIL_BUFFER_BIT),
   EDEF(GL_COLOR_BUFFER_BIT),

/* AlphaFunction */
   EDEF(GL_NEVER),
   EDEF(GL_LESS),
   EDEF(GL_EQUAL),
   EDEF(GL_LEQUAL),
   EDEF(GL_GREATER),
   EDEF(GL_NOTEQUAL),
   EDEF(GL_GEQUAL),
   EDEF(GL_ALWAYS),

/* BlendingFactorDest */
   EDEF(GL_SRC_COLOR),
   EDEF(GL_ONE_MINUS_SRC_COLOR),
   EDEF(GL_SRC_ALPHA),
   EDEF(GL_ONE_MINUS_SRC_ALPHA),
   EDEF(GL_DST_ALPHA),
   EDEF(GL_ONE_MINUS_DST_ALPHA),

/* BlendingFactorSrc */
   EDEF(GL_ZERO),
   EDEF(GL_ONE),
   EDEF(GL_DST_COLOR),
   EDEF(GL_ONE_MINUS_DST_COLOR),
   EDEF(GL_SRC_ALPHA_SATURATE),
/*      GL_SRC_ALPHA */
/*      GL_ONE_MINUS_SRC_ALPHA */
/*      GL_DST_ALPHA */
/*      GL_ONE_MINUS_DST_ALPHA */

/* ClipPlaneName */
   EDEF(GL_CLIP_PLANE0),
   EDEF(GL_CLIP_PLANE1),
   EDEF(GL_CLIP_PLANE2),
   EDEF(GL_CLIP_PLANE3),
   EDEF(GL_CLIP_PLANE4),
   EDEF(GL_CLIP_PLANE5),

/* BeginMode */
   EDEF(GL_POINTS),
   EDEF(GL_LINES),
   EDEF(GL_LINE_LOOP),
   EDEF(GL_LINE_STRIP),
   EDEF(GL_TRIANGLES),
   EDEF(GL_TRIANGLE_STRIP),
   EDEF(GL_TRIANGLE_FAN),

/* ColorMaterialFace */
/*      GL_FRONT_AND_BACK */

/* ColorMaterialParameter */
/*      GL_AMBIENT_AND_DIFFUSE */

/* ColorPointerType */
/*      GL_UNSIGNED_BYTE */
/*      GL_FLOAT */
/*      GL_FIXED */

/* CullFaceMode */
   EDEF(GL_FRONT),
   EDEF(GL_BACK),
   EDEF(GL_FRONT_AND_BACK),

/* DepthFunction */
/*      GL_NEVER */
/*      GL_LESS */
/*      GL_EQUAL */
/*      GL_LEQUAL */
/*      GL_GREATER */
/*      GL_NOTEQUAL */
/*      GL_GEQUAL */
/*      GL_ALWAYS */

/* EnableCap */
   EDEF(GL_FOG),
   EDEF(GL_LIGHTING),
   EDEF(GL_TEXTURE_2D),
   EDEF(GL_CULL_FACE),
   EDEF(GL_ALPHA_TEST),
   EDEF(GL_BLEND),
   EDEF(GL_COLOR_LOGIC_OP),
   EDEF(GL_DITHER),
   EDEF(GL_STENCIL_TEST),
   EDEF(GL_DEPTH_TEST),
/*      GL_LIGHT0 */
/*      GL_LIGHT1 */
/*      GL_LIGHT2 */
/*      GL_LIGHT3 */
/*      GL_LIGHT4 */
/*      GL_LIGHT5 */
/*      GL_LIGHT6 */
/*      GL_LIGHT7 */
   EDEF(GL_POINT_SMOOTH),
   EDEF(GL_LINE_SMOOTH),
   EDEF(GL_SCISSOR_TEST),
   EDEF(GL_COLOR_MATERIAL),
   EDEF(GL_NORMALIZE),
   EDEF(GL_RESCALE_NORMAL),
   EDEF(GL_POLYGON_OFFSET_FILL),
   EDEF(GL_VERTEX_ARRAY),
   EDEF(GL_NORMAL_ARRAY),
   EDEF(GL_COLOR_ARRAY),
   EDEF(GL_TEXTURE_COORD_ARRAY),
   EDEF(GL_MULTISAMPLE),
   EDEF(GL_SAMPLE_ALPHA_TO_COVERAGE),
   EDEF(GL_SAMPLE_ALPHA_TO_ONE),
   EDEF(GL_SAMPLE_COVERAGE),

/* ErrorCode */
   // EDEF(GL_NO_ERROR),
   // EDEF(GL_INVALID_ENUM),
   // EDEF(GL_INVALID_VALUE),
   // EDEF(GL_INVALID_OPERATION),
   // EDEF(GL_STACK_OVERFLOW),
   // EDEF(GL_STACK_UNDERFLOW),
   // EDEF(GL_OUT_OF_MEMORY),

/* FogMode */
/*      GL_LINEAR */
   EDEF(GL_EXP),
   EDEF(GL_EXP2),

/* FogParameter */
   EDEF(GL_FOG_DENSITY),
   EDEF(GL_FOG_START),
   EDEF(GL_FOG_END),
   EDEF(GL_FOG_MODE),
   EDEF(GL_FOG_COLOR),

/* FrontFaceDirection */
   EDEF(GL_CW),
   EDEF(GL_CCW),

/* GetPName */
   EDEF(GL_CURRENT_COLOR),
   EDEF(GL_CURRENT_NORMAL),
   EDEF(GL_CURRENT_TEXTURE_COORDS),
   EDEF(GL_POINT_SIZE),
   EDEF(GL_POINT_SIZE_MIN),
   EDEF(GL_POINT_SIZE_MAX),
   EDEF(GL_POINT_FADE_THRESHOLD_SIZE),
   EDEF(GL_POINT_DISTANCE_ATTENUATION),
   EDEF(GL_SMOOTH_POINT_SIZE_RANGE),
   EDEF(GL_LINE_WIDTH),
   EDEF(GL_SMOOTH_LINE_WIDTH_RANGE),
   EDEF(GL_ALIASED_POINT_SIZE_RANGE),
   EDEF(GL_ALIASED_LINE_WIDTH_RANGE),
   EDEF(GL_CULL_FACE_MODE),
   EDEF(GL_FRONT_FACE),
   EDEF(GL_SHADE_MODEL),
   EDEF(GL_DEPTH_RANGE),
   EDEF(GL_DEPTH_WRITEMASK),
   EDEF(GL_DEPTH_CLEAR_VALUE),
   EDEF(GL_DEPTH_FUNC),
   EDEF(GL_STENCIL_CLEAR_VALUE),
   EDEF(GL_STENCIL_FUNC),
   EDEF(GL_STENCIL_VALUE_MASK),
   EDEF(GL_STENCIL_FAIL),
   EDEF(GL_STENCIL_PASS_DEPTH_FAIL),
   EDEF(GL_STENCIL_PASS_DEPTH_PASS),
   EDEF(GL_STENCIL_REF),
   EDEF(GL_STENCIL_WRITEMASK),
   EDEF(GL_MATRIX_MODE),
   EDEF(GL_VIEWPORT),
   EDEF(GL_MODELVIEW_STACK_DEPTH),
   EDEF(GL_PROJECTION_STACK_DEPTH),
   EDEF(GL_TEXTURE_STACK_DEPTH),
   EDEF(GL_MODELVIEW_MATRIX),
   EDEF(GL_PROJECTION_MATRIX),
   EDEF(GL_TEXTURE_MATRIX),
   EDEF(GL_ALPHA_TEST_FUNC),
   EDEF(GL_ALPHA_TEST_REF),
   EDEF(GL_BLEND_DST),
   EDEF(GL_BLEND_SRC),
   EDEF(GL_LOGIC_OP_MODE),
   EDEF(GL_SCISSOR_BOX),
   EDEF(GL_SCISSOR_TEST),
   EDEF(GL_COLOR_CLEAR_VALUE),
   EDEF(GL_COLOR_WRITEMASK),
   EDEF(GL_UNPACK_ALIGNMENT),
   EDEF(GL_PACK_ALIGNMENT),
   EDEF(GL_MAX_LIGHTS),
   EDEF(GL_MAX_CLIP_PLANES),
   EDEF(GL_MAX_TEXTURE_SIZE),
   EDEF(GL_MAX_MODELVIEW_STACK_DEPTH),
   EDEF(GL_MAX_PROJECTION_STACK_DEPTH),
   EDEF(GL_MAX_TEXTURE_STACK_DEPTH),
   EDEF(GL_MAX_VIEWPORT_DIMS),
   EDEF(GL_MAX_TEXTURE_UNITS),
   EDEF(GL_SUBPIXEL_BITS),
   EDEF(GL_RED_BITS),
   EDEF(GL_GREEN_BITS),
   EDEF(GL_BLUE_BITS),
   EDEF(GL_ALPHA_BITS),
   EDEF(GL_DEPTH_BITS),
   EDEF(GL_STENCIL_BITS),
   EDEF(GL_POLYGON_OFFSET_UNITS),
   EDEF(GL_POLYGON_OFFSET_FILL),
   EDEF(GL_POLYGON_OFFSET_FACTOR),
   EDEF(GL_TEXTURE_BINDING_2D),
   EDEF(GL_VERTEX_ARRAY_SIZE),
   EDEF(GL_VERTEX_ARRAY_TYPE),
   EDEF(GL_VERTEX_ARRAY_STRIDE),
   EDEF(GL_NORMAL_ARRAY_TYPE),
   EDEF(GL_NORMAL_ARRAY_STRIDE),
   EDEF(GL_COLOR_ARRAY_SIZE),
   EDEF(GL_COLOR_ARRAY_TYPE),
   EDEF(GL_COLOR_ARRAY_STRIDE),
   EDEF(GL_TEXTURE_COORD_ARRAY_SIZE),
   EDEF(GL_TEXTURE_COORD_ARRAY_TYPE),
   EDEF(GL_TEXTURE_COORD_ARRAY_STRIDE),
   EDEF(GL_VERTEX_ARRAY_POINTER),
   EDEF(GL_NORMAL_ARRAY_POINTER),
   EDEF(GL_COLOR_ARRAY_POINTER),
   EDEF(GL_TEXTURE_COORD_ARRAY_POINTER),
   EDEF(GL_SAMPLE_BUFFERS),
   EDEF(GL_SAMPLES),
   EDEF(GL_SAMPLE_COVERAGE_VALUE),
   EDEF(GL_SAMPLE_COVERAGE_INVERT),

/* GetTextureParameter */
/*      GL_TEXTURE_MAG_FILTER */
/*      GL_TEXTURE_MIN_FILTER */
/*      GL_TEXTURE_WRAP_S */
/*      GL_TEXTURE_WRAP_T */

   EDEF(GL_IMPLEMENTATION_COLOR_READ_TYPE_OES),
   EDEF(GL_IMPLEMENTATION_COLOR_READ_FORMAT_OES),
   EDEF(GL_NUM_COMPRESSED_TEXTURE_FORMATS),
   EDEF(GL_COMPRESSED_TEXTURE_FORMATS),

/* HintMode */
   EDEF(GL_DONT_CARE),
   EDEF(GL_FASTEST),
   EDEF(GL_NICEST),

/* HintTarget */
   EDEF(GL_PERSPECTIVE_CORRECTION_HINT),
   EDEF(GL_POINT_SMOOTH_HINT),
   EDEF(GL_LINE_SMOOTH_HINT),
   EDEF(GL_FOG_HINT),
   EDEF(GL_GENERATE_MIPMAP_HINT),

/* LightModelParameter */
   EDEF(GL_LIGHT_MODEL_AMBIENT),
   EDEF(GL_LIGHT_MODEL_TWO_SIDE),

/* LightParameter */
   EDEF(GL_AMBIENT),
   EDEF(GL_DIFFUSE),
   EDEF(GL_SPECULAR),
   EDEF(GL_POSITION),
   EDEF(GL_SPOT_DIRECTION),
   EDEF(GL_SPOT_EXPONENT),
   EDEF(GL_SPOT_CUTOFF),
   EDEF(GL_CONSTANT_ATTENUATION),
   EDEF(GL_LINEAR_ATTENUATION),
   EDEF(GL_QUADRATIC_ATTENUATION),

/* DataType */
   EDEF(GL_BYTE),
   EDEF(GL_UNSIGNED_BYTE),
   EDEF(GL_SHORT),
   EDEF(GL_UNSIGNED_SHORT),
   EDEF(GL_FLOAT),
   EDEF(GL_FIXED),

/* LogicOp */
   EDEF(GL_CLEAR),
   EDEF(GL_AND),
   EDEF(GL_AND_REVERSE),
   EDEF(GL_COPY),
   EDEF(GL_AND_INVERTED),
   EDEF(GL_NOOP),
   EDEF(GL_XOR),
   EDEF(GL_OR),
   EDEF(GL_NOR),
   EDEF(GL_EQUIV),
   EDEF(GL_INVERT),
   EDEF(GL_OR_REVERSE),
   EDEF(GL_COPY_INVERTED),
   EDEF(GL_OR_INVERTED),
   EDEF(GL_NAND),
   EDEF(GL_SET),

/* MaterialFace */
/*      GL_FRONT_AND_BACK */

/* MaterialParameter */
   EDEF(GL_EMISSION),
   EDEF(GL_SHININESS),
   EDEF(GL_AMBIENT_AND_DIFFUSE),
/*      GL_AMBIENT */
/*      GL_DIFFUSE */
/*      GL_SPECULAR */

/* MatrixMode */
   EDEF(GL_MODELVIEW),
   EDEF(GL_PROJECTION),
   EDEF(GL_TEXTURE),

/* NormalPointerType */
/*      GL_BYTE */
/*      GL_SHORT */
/*      GL_FLOAT */
/*      GL_FIXED */

/* PixelFormat */
   EDEF(GL_ALPHA),
   EDEF(GL_RGB),
   EDEF(GL_RGBA),
   EDEF(GL_LUMINANCE),
   EDEF(GL_LUMINANCE_ALPHA),

/* PixelStoreParameter */
   EDEF(GL_UNPACK_ALIGNMENT),
   EDEF(GL_PACK_ALIGNMENT),

/* PixelType */
/*      GL_UNSIGNED_BYTE */
   EDEF(GL_UNSIGNED_SHORT_4_4_4_4),
   EDEF(GL_UNSIGNED_SHORT_5_5_5_1),
   EDEF(GL_UNSIGNED_SHORT_5_6_5),

/* ShadingModel */
   EDEF(GL_FLAT),
   EDEF(GL_SMOOTH),

/* StencilFunction */
/*      GL_NEVER */
/*      GL_LESS */
/*      GL_EQUAL */
/*      GL_LEQUAL */
/*      GL_GREATER */
/*      GL_NOTEQUAL */
/*      GL_GEQUAL */
/*      GL_ALWAYS */

/* StencilOp */
/*      GL_ZERO */
   EDEF(GL_KEEP),
   EDEF(GL_REPLACE),
   EDEF(GL_INCR),
   EDEF(GL_DECR),
/*      GL_INVERT */

/* StringName */
   EDEF(GL_VENDOR),
   EDEF(GL_RENDERER),
   EDEF(GL_VERSION),
   EDEF(GL_EXTENSIONS),

/* TexCoordPointerType */
/*      GL_SHORT */
/*      GL_FLOAT */
/*      GL_FIXED */
/*      GL_BYTE */

/* TextureEnvMode */
   EDEF(GL_MODULATE),
   EDEF(GL_DECAL),
/*      GL_BLEND */
   EDEF(GL_ADD),
/*      GL_REPLACE */

/* TextureEnvParameter */
   EDEF(GL_TEXTURE_ENV_MODE),
   EDEF(GL_TEXTURE_ENV_COLOR),

/* TextureEnvTarget */
   EDEF(GL_TEXTURE_ENV),

/* TextureMagFilter */
   EDEF(GL_NEAREST),
   EDEF(GL_LINEAR),

/* TextureMinFilter */
/*      GL_NEAREST */
/*      GL_LINEAR */
   EDEF(GL_NEAREST_MIPMAP_NEAREST),
   EDEF(GL_LINEAR_MIPMAP_NEAREST),
   EDEF(GL_NEAREST_MIPMAP_LINEAR),
   EDEF(GL_LINEAR_MIPMAP_LINEAR),

/* TextureParameterName */
   EDEF(GL_TEXTURE_MAG_FILTER),
   EDEF(GL_TEXTURE_MIN_FILTER),
   EDEF(GL_TEXTURE_WRAP_S),
   EDEF(GL_TEXTURE_WRAP_T),
   EDEF(GL_GENERATE_MIPMAP),

/* TextureTarget */
/*      GL_TEXTURE_2D */

/* TextureUnit */
   EDEF(GL_TEXTURE0),
   EDEF(GL_TEXTURE1),
   EDEF(GL_TEXTURE2),
   EDEF(GL_TEXTURE3),
   EDEF(GL_TEXTURE4),
   EDEF(GL_TEXTURE5),
   EDEF(GL_TEXTURE6),
   EDEF(GL_TEXTURE7),
   EDEF(GL_TEXTURE8),
   EDEF(GL_TEXTURE9),
   EDEF(GL_TEXTURE10),
   EDEF(GL_TEXTURE11),
   EDEF(GL_TEXTURE12),
   EDEF(GL_TEXTURE13),
   EDEF(GL_TEXTURE14),
   EDEF(GL_TEXTURE15),
   EDEF(GL_TEXTURE16),
   EDEF(GL_TEXTURE17),
   EDEF(GL_TEXTURE18),
   EDEF(GL_TEXTURE19),
   EDEF(GL_TEXTURE20),
   EDEF(GL_TEXTURE21),
   EDEF(GL_TEXTURE22),
   EDEF(GL_TEXTURE23),
   EDEF(GL_TEXTURE24),
   EDEF(GL_TEXTURE25),
   EDEF(GL_TEXTURE26),
   EDEF(GL_TEXTURE27),
   EDEF(GL_TEXTURE28),
   EDEF(GL_TEXTURE29),
   EDEF(GL_TEXTURE30),
   EDEF(GL_TEXTURE31),
   EDEF(GL_ACTIVE_TEXTURE),
   EDEF(GL_CLIENT_ACTIVE_TEXTURE),

/* TextureWrapMode */
   EDEF(GL_REPEAT),
   EDEF(GL_CLAMP_TO_EDGE),

/* PixelInternalFormat */
   EDEF(GL_PALETTE4_RGB8_OES),
   EDEF(GL_PALETTE4_RGBA8_OES),
   EDEF(GL_PALETTE4_R5_G6_B5_OES),
   EDEF(GL_PALETTE4_RGBA4_OES),
   EDEF(GL_PALETTE4_RGB5_A1_OES),
   EDEF(GL_PALETTE8_RGB8_OES),
   EDEF(GL_PALETTE8_RGBA8_OES),
   EDEF(GL_PALETTE8_R5_G6_B5_OES),
   EDEF(GL_PALETTE8_RGBA4_OES),
   EDEF(GL_PALETTE8_RGB5_A1_OES),

/* VertexPointerType */
/*      GL_SHORT */
/*      GL_FLOAT */
/*      GL_FIXED */
/*      GL_BYTE */

/* LightName */
   EDEF(GL_LIGHT0),
   EDEF(GL_LIGHT1),
   EDEF(GL_LIGHT2),
   EDEF(GL_LIGHT3),
   EDEF(GL_LIGHT4),
   EDEF(GL_LIGHT5),
   EDEF(GL_LIGHT6),
   EDEF(GL_LIGHT7),

/* Buffer Objects */
   EDEF(GL_ARRAY_BUFFER),
   EDEF(GL_ELEMENT_ARRAY_BUFFER),

   EDEF(GL_ARRAY_BUFFER_BINDING),
   EDEF(GL_ELEMENT_ARRAY_BUFFER_BINDING),
   EDEF(GL_VERTEX_ARRAY_BUFFER_BINDING),
   EDEF(GL_NORMAL_ARRAY_BUFFER_BINDING),
   EDEF(GL_COLOR_ARRAY_BUFFER_BINDING),
   EDEF(GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING),

   EDEF(GL_STATIC_DRAW),
   EDEF(GL_DYNAMIC_DRAW),


   EDEF(GL_BUFFER_SIZE),
   EDEF(GL_BUFFER_USAGE),


/* Texture combine + dot3 */
   EDEF(GL_SUBTRACT),
   EDEF(GL_COMBINE),
   EDEF(GL_COMBINE_RGB),
   EDEF(GL_COMBINE_ALPHA),
   EDEF(GL_RGB_SCALE),
   EDEF(GL_ADD_SIGNED),
   EDEF(GL_INTERPOLATE),
   EDEF(GL_CONSTANT),
   EDEF(GL_PRIMARY_COLOR),
   EDEF(GL_PREVIOUS),
   EDEF(GL_OPERAND0_RGB),
   EDEF(GL_OPERAND1_RGB),
   EDEF(GL_OPERAND2_RGB),
   EDEF(GL_OPERAND0_ALPHA),
   EDEF(GL_OPERAND1_ALPHA),
   EDEF(GL_OPERAND2_ALPHA),

   EDEF(GL_ALPHA_SCALE),

   EDEF(GL_SRC0_RGB),
   EDEF(GL_SRC1_RGB),
   EDEF(GL_SRC2_RGB),
    EDEF(GL_SRC0_ALPHA),
    EDEF(GL_SRC1_ALPHA),
    EDEF(GL_SRC2_ALPHA),

    EDEF(GL_DOT3_RGB),
    EDEF(GL_DOT3_RGBA),

/*****************************************************************************************/
/*                                 OES extension functions                               */
/*****************************************************************************************/

/* OES_draw_texture */

/* OES_matrix_palette */

/* OES_point_size_array */
    EDEF(GL_POINT_SIZE_ARRAY_OES),
    EDEF(GL_POINT_SIZE_ARRAY_TYPE_OES),
    EDEF(GL_POINT_SIZE_ARRAY_STRIDE_OES),
    EDEF(GL_POINT_SIZE_ARRAY_POINTER_OES),
    EDEF(GL_POINT_SIZE_ARRAY_BUFFER_BINDING_OES),

/* OES_point_sprite */
    EDEF(GL_POINT_SPRITE_OES),
    EDEF(GL_COORD_REPLACE_OES),
};

#define N_ENUMS (sizeof(enums) / sizeof(EnumLiteral))

#define BOOL_STRING(b) (b ? "true" : "false")

static void print_enum_name( FILE* OUT, GLenum e )
{
    int i, found= 0;
    for( i= 0; i < N_ENUMS; ++i )
    {
        if( enums[i].e == e )
        {
            if( found )
                fprintf( OUT, "/" );
            found= 1;
            fprintf( OUT, "%s", enums[i].name );
        }
    }
    if( ! found )
        fprintf( OUT, "*UNKNOWN* [%04x]", (int)e );
    fprintf( OUT, "\n" );
}

#define VAR_ENUM(VAR, OUT)                           \
    {                                           \
        GLint e= 0;                             \
        glGetIntegerv(VAR,&e);             \
        fprintf( OUT, "%s: ", #VAR );           \
        print_enum_name( OUT, (GLenum) e );     \
    }

#define VAR_FLOAT4(VAR, OUT)                             \
    {                                               \
        GLfloat f[4];                               \
        f[0]= f[1]= f[2]= f[3]= 0.0;                \
        glGetFloatv(VAR,f);                    \
        fprintf( OUT, "%s: [%f %f %f %f]\n",        \
                 #VAR, f[0], f[1], f[2], f[3] );    \
    }

#define VAR_MAT_FLOAT4(VAR, OUT)                         \
    {                                               \
        GLfloat f[4];                               \
        f[0]= f[1]= f[2]= f[3]= 0.0;                \
        glGetMaterialfv(GL_FRONT,VAR,f);       \
        fprintf( OUT, "FRONT_%s: [%f %f %f %f]\n",  \
                 #VAR, f[0], f[1], f[2], f[3] );    \
        glGetMaterialfv(GL_BACK,VAR,f);        \
        fprintf( OUT, " BACK_%s: [%f %f %f %f]\n",  \
                 #VAR, f[0], f[1], f[2], f[3] );    \
    }

#define VAR_LIGHT_FLOAT4(LIGHT,VAR,OUT)                    \
    {                                                   \
        GLfloat f[4];                                   \
        f[0]= f[1]= f[2]= f[3]= 0.0;                    \
        glGetLightfv(GL_LIGHT0+LIGHT,VAR,f);       \
        fprintf( OUT, "LIGHT%d.%s: [%f %f %f %f]\n",    \
                 LIGHT, #VAR, f[0], f[1], f[2], f[3] ); \
    }

#define VAR_LIGHT_FLOAT3(LIGHT,VAR,OUT)                \
    {                                               \
        GLfloat f[3];                               \
        f[0]= f[1]= f[2]= 0.0;                      \
        glGetLightfv(GL_LIGHT0+LIGHT,VAR,f);   \
        fprintf( OUT, "LIGHT%d.%s: [%f %f %f]\n",   \
                 LIGHT, #VAR, f[0], f[1], f[2] );   \
    }

#define VAR_FLOAT3(VAR, OUT)                         \
    {                                           \
        GLfloat f[3];                           \
        f[0]= f[1]= f[2]= 0.0;                  \
        glGetFloatv(VAR,f) ;               \
        fprintf( OUT, "%s: [%f %f %f]\n",       \
                 #VAR, f[0], f[1], f[2] );      \
    }
#define VAR_FLOAT2(VAR, OUT)                         \
    {                                           \
        GLfloat f[2];                           \
        f[0]= f[1]= 0.0;                        \
        glGetFloatv(VAR,f);                \
        fprintf( OUT, "%s: [%f %f]\n",          \
                 #VAR, f[0], f[1] );            \
    }

#define VAR_COLOR(VAR, OUT)    VAR_FLOAT4(VAR, OUT)
#define VAR_TEXCOORD(VAR, OUT) VAR_FLOAT4(VAR, OUT)
#define VAR_NORMAL(VAR, OUT)   VAR_FLOAT3(VAR, OUT)

#define VAR_MAT_COLOR(VAR, OUT)         VAR_MAT_FLOAT4(VAR, OUT)
#define VAR_LIGHT_COLOR(LIGHT,VAR, OUT) VAR_LIGHT_FLOAT4(LIGHT,VAR,OUT)

#define VAR_FLOAT(VAR, OUT)                          \
    {                                           \
        GLfloat f= 0.0;                         \
        glGetFloatv(VAR,&f);               \
        fprintf( OUT, "%s: %f\n", #VAR, f );    \
    }

#define VAR_MAT_FLOAT(VAR, OUT)                          \
    {                                               \
        GLfloat f= 0.0;                             \
        glGetMaterialfv(GL_FRONT,VAR,&f);      \
        fprintf( OUT, "FRONT_%s: %f\n", #VAR, f );  \
        glGetMaterialfv(GL_BACK,VAR,&f);       \
        fprintf( OUT, " BACK_%s: %f\n", #VAR, f );  \
    }

#define VAR_LIGHT_FLOAT(LIGHT,VAR,OUT)                 \
    {                                               \
        GLfloat f= 0.0;                             \
        glGetLightfv(GL_LIGHT0+LIGHT,VAR,&f);  \
        fprintf( OUT, "LIGHT%d.%s: %f\n",           \
                 LIGHT, #VAR, f );                  \
    }

#define VAR_INT(VAR, OUT)                                \
    {                                               \
        GLint i= 0;                                 \
        glGetIntegerv(VAR,&i);                 \
        fprintf( OUT, "%s: %d\n", #VAR, (int)i );   \
    }
#define VAR_INTEGER(VAR, OUT) VAR_INT(VAR, OUT)
#define VAR_INDEX(VAR, OUT)   VAR_INT(VAR, OUT)
#define VAR_HEXINT(VAR, OUT)                                 \
    {                                                   \
        GLint i= 0;                                     \
        glGetIntegerv(VAR,&i);                     \
        fprintf( OUT, "%s: 0x%04x\n", #VAR, (int)i );   \
    }
#define VAR_INT2(VAR, OUT)                                              \
    {                                                                   \
        GLint i[2];                                                     \
        i[0]= i[1]=  0;                                      \
        glGetIntegerv(VAR,i);                                      \
        fprintf( OUT, "%s: [%d %d]\n",                            \
                 #VAR, (int)i[0], (int)i[1]);    \
    }

#define VAR_INT4(VAR, OUT)                                                   \
    {                                                                   \
        GLint i[4];                                                     \
        i[0]= i[1]= i[2]= i[3]= 0;                                      \
        glGetIntegerv(VAR,i);                                      \
        fprintf( OUT, "%s: [%d %d %d %d]\n",                            \
                 #VAR, (int)i[0], (int)i[1], (int)i[2], (int)i[3] );    \
    }
#define VAR_BOOL(VAR, OUT)                                       \
    {                                                       \
        GLboolean b= 0;                                     \
        glGetBooleanv(VAR,&b);                         \
        fprintf( OUT, "%s: %s\n", #VAR, BOOL_STRING(b) );   \
    }
#define VAR_BOOL4(VAR, OUT)                          \
    {                                           \
        GLboolean b[4];                         \
        b[0]= b[1]= b[2]= b[3]= 0;              \
        glGetBooleanv(VAR,b);              \
        fprintf( OUT, "%s: [%s %s %s %s]\n",    \
                 #VAR,                          \
                 BOOL_STRING(b[0]),             \
                 BOOL_STRING(b[1]),             \
                 BOOL_STRING(b[2]),             \
                 BOOL_STRING(b[3]) );           \
    }
#define VAR_PTR(VAR, OUT)                            \
    {                                           \
        GLvoid* p= 0;                           \
        glGetPointerv(VAR,&p);             \
        fprintf( OUT, "%s: %p\n", #VAR, p );    \
    }
#define VAR_MATRIX(VAR, OUT)                                         \
    {                                                           \
    GLfloat m[16];                                              \
    int i;                                                      \
    for( i= 0; i < 16; ++i ) m[i]= 0.0;                         \
    glGetFloatv(VAR,m);                                    \
    fprintf( OUT,                                               \
        "%s:\n\t[%+.6f %+.6f %+.6f %+.6f]\n\t[%+.6f %+.6f %+.6f \
%+.6f]\n\t[%+.6f %+.6f %+.6f %+.6f]\n\t[%+.6f %+.6f %+.6f %+.6f]\n", \
                 #VAR,                                     \
                 m[0+0*4], m[0+1*4], m[0+2*4], m[0+3*4],   \
                 m[1+0*4], m[1+1*4], m[1+2*4], m[1+3*4],   \
                 m[2+0*4], m[2+1*4], m[2+2*4], m[2+3*4],   \
                 m[3+0*4], m[3+1*4], m[3+2*4], m[3+3*4] ); \
        }

namespace OpenGlEsStateDumping {

void dumpState(const char* section) {
    printf("-------------------------------------------------\n");
    printf("OPENGL ES DUMP FOR: %s\n", section);
    printf("-------------------------------------------------\n");
    
    
    GLint n_lights= 0;

    glGetIntegerv( GL_MAX_LIGHTS, &n_lights );

    
    VAR_COLOR(GL_CURRENT_COLOR, stdout);
    VAR_TEXCOORD(GL_CURRENT_TEXTURE_COORDS, stdout);
    VAR_NORMAL(GL_CURRENT_NORMAL, stdout);

    VAR_BOOL   (GL_VERTEX_ARRAY, stdout);
    VAR_INTEGER(GL_VERTEX_ARRAY_SIZE, stdout);
    VAR_ENUM   (GL_VERTEX_ARRAY_TYPE, stdout);
    VAR_INTEGER(GL_VERTEX_ARRAY_STRIDE, stdout);
    VAR_PTR    (GL_VERTEX_ARRAY_POINTER, stdout);


    VAR_BOOL   (GL_NORMAL_ARRAY, stdout);
    VAR_ENUM   (GL_NORMAL_ARRAY_TYPE, stdout);
    VAR_INTEGER(GL_NORMAL_ARRAY_STRIDE, stdout);
    VAR_PTR    (GL_NORMAL_ARRAY_POINTER, stdout);

    VAR_BOOL   (GL_COLOR_ARRAY, stdout);
    VAR_INTEGER(GL_COLOR_ARRAY_SIZE, stdout);
    VAR_ENUM   (GL_COLOR_ARRAY_TYPE, stdout);
    VAR_INTEGER(GL_COLOR_ARRAY_STRIDE, stdout);
    VAR_PTR    (GL_COLOR_ARRAY_POINTER, stdout);

    VAR_BOOL   (GL_TEXTURE_COORD_ARRAY, stdout);
    VAR_INTEGER(GL_TEXTURE_COORD_ARRAY_SIZE, stdout);
    VAR_ENUM   (GL_TEXTURE_COORD_ARRAY_TYPE, stdout);
    VAR_INTEGER(GL_TEXTURE_COORD_ARRAY_STRIDE, stdout);
    VAR_PTR    (GL_TEXTURE_COORD_ARRAY_POINTER, stdout);

    VAR_MATRIX(GL_MODELVIEW_MATRIX, stdout);
    VAR_MATRIX(GL_PROJECTION_MATRIX, stdout);
    VAR_MATRIX(GL_TEXTURE_MATRIX, stdout);
    VAR_INT4(GL_VIEWPORT, stdout);
    VAR_FLOAT2(GL_DEPTH_RANGE, stdout);
    VAR_INT(GL_MODELVIEW_STACK_DEPTH, stdout);
    VAR_INT(GL_PROJECTION_STACK_DEPTH, stdout);
    VAR_INT(GL_TEXTURE_STACK_DEPTH, stdout);
    VAR_ENUM(GL_MATRIX_MODE, stdout);
    VAR_BOOL(GL_NORMALIZE, stdout);
    VAR_BOOL(GL_CLIP_PLANE0, stdout);
    VAR_BOOL(GL_CLIP_PLANE1, stdout);
    VAR_BOOL(GL_CLIP_PLANE2, stdout);
    VAR_BOOL(GL_CLIP_PLANE3, stdout);
    VAR_BOOL(GL_CLIP_PLANE4, stdout);
    VAR_BOOL(GL_CLIP_PLANE5, stdout);
    VAR_INT4(GL_VIEWPORT, stdout);
    VAR_FLOAT2(GL_DEPTH_RANGE, stdout);
    VAR_INT(GL_MODELVIEW_STACK_DEPTH, stdout);
    VAR_INT(GL_PROJECTION_STACK_DEPTH, stdout);
    VAR_INT(GL_TEXTURE_STACK_DEPTH, stdout);
    VAR_ENUM(GL_MATRIX_MODE, stdout);
    VAR_BOOL(GL_NORMALIZE, stdout);
    VAR_BOOL(GL_CLIP_PLANE0, stdout);
    VAR_BOOL(GL_CLIP_PLANE1, stdout);
    VAR_BOOL(GL_CLIP_PLANE2, stdout);
    VAR_BOOL(GL_CLIP_PLANE3, stdout);
    VAR_BOOL(GL_CLIP_PLANE4, stdout);
    VAR_BOOL(GL_CLIP_PLANE5, stdout);

    VAR_COLOR(GL_FOG_COLOR, stdout);
    VAR_FLOAT(GL_FOG_DENSITY, stdout);
    VAR_FLOAT(GL_FOG_START, stdout);
    VAR_FLOAT(GL_FOG_END, stdout);
    VAR_ENUM(GL_FOG_MODE, stdout);
    VAR_BOOL(GL_FOG, stdout);
    VAR_ENUM(GL_SHADE_MODEL, stdout);

    VAR_BOOL(GL_LIGHTING, stdout);
    VAR_BOOL(GL_COLOR_MATERIAL, stdout);

    VAR_MAT_COLOR(GL_AMBIENT, stdout);
    VAR_MAT_COLOR(GL_DIFFUSE, stdout);
    VAR_MAT_COLOR(GL_SPECULAR, stdout);
    VAR_MAT_COLOR(GL_EMISSION, stdout);
    VAR_MAT_FLOAT(GL_SHININESS, stdout);

    VAR_COLOR(GL_LIGHT_MODEL_AMBIENT, stdout);
    /*  VAR_ENUM(GL_LIGHT_MODEL_COLOR_CONTROL, stdout)*/;

    for( int i= 0; i < n_lights; ++i)
    {
        GLboolean b= 0;

        glGetBooleanv( GL_LIGHT0 + i, &b);
        fprintf( stdout, "LIGHT%d: %s\n", i, BOOL_STRING(b) );

        if( ! b)
            continue;

        VAR_LIGHT_COLOR(i,GL_AMBIENT, stdout);
        VAR_LIGHT_COLOR(i,GL_DIFFUSE, stdout);
        VAR_LIGHT_COLOR(i,GL_SPECULAR, stdout);
        VAR_LIGHT_FLOAT4(i,GL_POSITION, stdout);
        VAR_LIGHT_FLOAT(i,GL_CONSTANT_ATTENUATION, stdout);
        VAR_LIGHT_FLOAT(i,GL_LINEAR_ATTENUATION, stdout);
        VAR_LIGHT_FLOAT(i,GL_QUADRATIC_ATTENUATION, stdout);
        VAR_LIGHT_FLOAT3(i,GL_SPOT_DIRECTION, stdout);
        VAR_LIGHT_FLOAT(i,GL_SPOT_EXPONENT, stdout);
        VAR_LIGHT_FLOAT(i,GL_SPOT_CUTOFF, stdout);
        /* COLOR_INDEXES */
    }

    VAR_FLOAT(GL_POINT_SIZE, stdout);
    VAR_BOOL(GL_POINT_SMOOTH, stdout);
    VAR_FLOAT(GL_LINE_WIDTH, stdout);
    VAR_BOOL(GL_LINE_SMOOTH, stdout);
    VAR_BOOL(GL_CULL_FACE, stdout);
    VAR_ENUM(GL_CULL_FACE_MODE, stdout);
    VAR_ENUM(GL_FRONT_FACE, stdout);
    VAR_FLOAT(GL_POLYGON_OFFSET_FACTOR, stdout);
    VAR_FLOAT(GL_POLYGON_OFFSET_UNITS, stdout);
    /* GetPolygonStipple */
    
    VAR_BOOL(GL_TEXTURE_2D, stdout);
    /*  VAR_BOOL(GL_TEXTURE_3D, stdout)*/;
    
    VAR_INT(GL_TEXTURE_BINDING_2D, stdout);
    /*  VAR_INT(GL_TEXTURE_BINDING_3D, stdout)*/;

    /* GetTexImage(, stdout) */;
    /* GetTexLevelParameter(, stdout) */;
    /* GetTexEnv(, stdout) */;

    /* GetTexGen(, stdout) */;

    VAR_BOOL(GL_SCISSOR_TEST, stdout);
    VAR_INT4(GL_SCISSOR_BOX, stdout);
    VAR_BOOL(GL_ALPHA_TEST, stdout);
    VAR_ENUM(GL_ALPHA_TEST_FUNC, stdout);
    VAR_FLOAT(GL_ALPHA_TEST_REF, stdout);
    VAR_BOOL(GL_STENCIL_TEST, stdout);
    VAR_ENUM(GL_STENCIL_FUNC, stdout);
    VAR_HEXINT(GL_STENCIL_VALUE_MASK, stdout);
    VAR_INT(GL_STENCIL_REF, stdout);
    VAR_ENUM(GL_STENCIL_FAIL, stdout);
    VAR_ENUM(GL_STENCIL_PASS_DEPTH_FAIL, stdout);
    VAR_ENUM(GL_STENCIL_PASS_DEPTH_PASS, stdout);
    VAR_BOOL(GL_DEPTH_TEST, stdout);
    VAR_ENUM(GL_DEPTH_FUNC, stdout);
    VAR_BOOL(GL_BLEND, stdout);
    VAR_ENUM(GL_BLEND_SRC, stdout);
    VAR_ENUM(GL_BLEND_DST, stdout);

    VAR_BOOL(GL_DITHER, stdout);
    VAR_BOOL(GL_COLOR_LOGIC_OP, stdout);
    
    VAR_BOOL4(GL_COLOR_WRITEMASK, stdout);
    VAR_BOOL(GL_DEPTH_WRITEMASK, stdout);
    VAR_HEXINT(GL_STENCIL_WRITEMASK, stdout);
    VAR_COLOR(GL_COLOR_CLEAR_VALUE, stdout);
    VAR_FLOAT(GL_DEPTH_CLEAR_VALUE, stdout);
    VAR_INT(GL_STENCIL_CLEAR_VALUE, stdout);
    
    VAR_FLOAT(GL_ALPHA_SCALE, stdout);
    
    VAR_ENUM(GL_PERSPECTIVE_CORRECTION_HINT, stdout);
    VAR_ENUM(GL_POINT_SMOOTH_HINT, stdout);
    VAR_ENUM(GL_LINE_SMOOTH_HINT, stdout);
    VAR_ENUM(GL_FOG_HINT, stdout);

    VAR_INT(GL_MAX_LIGHTS, stdout);
    VAR_INT(GL_MAX_CLIP_PLANES, stdout);
    VAR_INT(GL_MAX_MODELVIEW_STACK_DEPTH, stdout);
    VAR_INT(GL_MAX_PROJECTION_STACK_DEPTH, stdout);
    VAR_INT(GL_MAX_TEXTURE_STACK_DEPTH, stdout);
    VAR_INT(GL_SUBPIXEL_BITS, stdout);
    VAR_INT(GL_MAX_TEXTURE_SIZE, stdout);
    VAR_FLOAT2(GL_ALIASED_POINT_SIZE_RANGE, stdout);
    VAR_INT2(GL_MAX_VIEWPORT_DIMS, stdout);

    VAR_FLOAT2(GL_ALIASED_LINE_WIDTH_RANGE, stdout);

    VAR_FLOAT2(GL_SMOOTH_LINE_WIDTH_RANGE, stdout) /* SMOOTH_LINE_WIDTH_RANGE */;
    VAR_INT(GL_RED_BITS, stdout);
    VAR_INT(GL_GREEN_BITS, stdout);
    VAR_INT(GL_BLUE_BITS, stdout);
    VAR_INT(GL_ALPHA_BITS, stdout);
    VAR_INT(GL_DEPTH_BITS, stdout);
    VAR_INT(GL_STENCIL_BITS, stdout);
    printf("-------------------------------------------------\n");
}

void printBlend()
{
    VAR_BOOL(GL_BLEND, stdout);
    VAR_ENUM(GL_BLEND_SRC, stdout);
    VAR_ENUM(GL_BLEND_DST, stdout);    
}

} // End of namespace OpenGLStateDumping

