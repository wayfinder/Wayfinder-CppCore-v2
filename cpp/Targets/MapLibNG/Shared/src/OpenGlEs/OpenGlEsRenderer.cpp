/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#define CORE_LOGGING_MODULE_NAME "OGLRenderer"
#define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE

#include "CoreDebugPrint.h"
#include "config.h"
#include "OpenGlEsRenderer.h"
#include "MC2Logging.h"
#include "OpenGlEsUtil.h"
#include "OpenGlEs.h"
#include "Vector3d.h"
#include "OpenGlDebugging.h"
#include "Vector3d.h"
#include "PNGImageLoader.h"
#include "OpenGlEsTextureBlockManager.h"
#include "OpenGlEsTextureBlock.h"
#include "OpenGlEsVertexBlockManager.h"
#include "OpenGlEsVertexBlock.h"
#include "GfxConstants.h"
#include "Camera.h"
#include "Utility.h"
#include "Vector3dMath.h"

#include "OpenGlEsStateDumping.h"

#define DBG "[GLES_RENDERER]: "
static bool spriteCoordsLoaded = false;

OpenGlEsRenderer::OpenGlEsRenderer( BitMapTextLoader* textLoader,
                                    NativeTextInterface* textInterface,
                                    OpenGlEsTextureBlockManager* texManager,
                                    OpenGlEsVertexBlockManager* vertManager,
                                    int resolutionWidth,
                                    int resolutionHeight ) :
   ExternalRenderer( textLoader,
                     textInterface,
                     texManager,
                     vertManager,
                     resolutionWidth,
                     resolutionHeight )
{
   glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
   
   m_spriteVertHandle = NULL;
   m_spriteTexHandle = NULL;
   m_fontSprite = NULL;
   m_cachedData.prevTexHandle = MAX_UINT32;
   glDisable (GL_DEPTH_TEST);
   glDisable (GL_ALPHA_TEST);
   glDisable (GL_BLEND);

   createSpriteBuffers();
   loadFontTexture();
}

void OpenGlEsRenderer::loadFontTexture()
{
   delete m_fontSprite;

   if(m_font.supportsPlatform()) {
      const RGBA32BitMap bmp = m_font.getTexture();
      m_fontSprite = getTextureBlockManager()->allocateFromBitMap(bmp);
      m_fontVertCoords = m_font.getVertexCoordinates();      
   }
}


void OpenGlEsRenderer::createSpriteBuffers()
{
   vbuf3x verts;

   VertexBlockManager* vertManager = getVertexBlockManager();

   /**
    * Setup the vertices in the following order: BL, BR, TL, TR
    */
   verts.push_back(
      vec3x( fixedp( 0.0f), fixedp( 0.0f ) ) );
   verts.push_back(
      vec3x( fixedp( 1.0f), fixedp( 0.0f ) ) );
   verts.push_back(
      vec3x( fixedp( 0.0f), fixedp( 1.0f ) ) );
   verts.push_back(
      vec3x( fixedp( 1.0f), fixedp( 1.0f ) ) );

   m_spriteVertHandle = vertManager->allocateVertexBlock( verts );

   vbuf3x texVerts;

   texVerts.push_back(
      vec3x( fixedp( 0.0f), fixedp( 0.0f ) ) );
   texVerts.push_back(
      vec3x( fixedp( 1.0f), fixedp( 0.0f ) ) );
   texVerts.push_back(
      vec3x( fixedp( 0.0f), fixedp( 1.0f ) ) );
   texVerts.push_back(
      vec3x( fixedp( 1.0f), fixedp( 1.0f ) ) );

   m_spriteTexHandle = vertManager->allocateVertexBlock( texVerts );

   vbuf3x boxVerts;

   boxVerts.push_back(
      vec3x( fixedp( 0.0f), fixedp( 0.0f ) ) );
   boxVerts.push_back(
      vec3x( fixedp( 1.0f), fixedp( 0.0f ) ) );
   boxVerts.push_back(
      vec3x( fixedp( 1.0f), fixedp( 1.0f ) ) );
   boxVerts.push_back(
      vec3x( fixedp( 0.0f), fixedp( 1.0f ) ) );
   boxVerts.push_back(
      vec3x( fixedp( 0.0f), fixedp( 0.0f ) ) );

   m_boxVertHandle = vertManager->allocateVertexBlock( boxVerts );
}

OpenGlEsRenderer::~OpenGlEsRenderer()
{
   delete m_fontSprite;
   delete m_spriteVertHandle;
   delete m_spriteTexHandle;
   delete m_boxVertHandle;
}

static bool drawFlag = false;

void OpenGlEsRenderer::clearScreen()
{
   glClear (GL_COLOR_BUFFER_BIT);
}

void OpenGlEsRenderer::initializeDrawing()
{
   drawFlag = true;
   m_cachedData.prevTexHandle = MAX_UINT32;
   m_numDrawPolylineCalls = 0;
   m_numDrawPolygonCalls = 0;
   spriteCoordsLoaded = false;
}

struct RgbColor {
   fixedp r;
   fixedp g;
   fixedp b;
};

RgbColor getFixedRgbColor( unsigned int rgb24 ) {
   int r = ( rgb24 >> 16) & 0xff;
   int g = ( rgb24 >>  8) & 0xff;
   int b = ( rgb24 >>  0) & 0xff;

   float fr = static_cast<float>(r) / 255.0f;
   float fg = static_cast<float>(g) / 255.0f;
   float fb = static_cast<float>(b) / 255.0f;

   RgbColor ret = { fixedp( fr ), fixedp( fg ), fixedp( fb )};
   
   return ret;
}

void setupColor( unsigned int rgb24 ) {
   RgbColor color = getFixedRgbColor( rgb24 );

   glColor4x( color.r.fx, color.g.fx, color.b.fx, fx::FX_ONE.fx );
}

template<typename BufferType>
void printBuffer( const BufferType& buf ) {
   mc2dbg << "Printing vertex buffer of size: " << buf.size() << endl;

   for( typename BufferType::const_iterator bI = buf.begin(); bI != buf.end(); bI++ ) {
      mc2dbg << *bI << endl;
   }

   mc2dbg << "--------------------------" << endl;
}

void compareMatrixWithInternal( const char* header, float m[16], int matrixStack ) {
#ifdef GL_OES_VERSION_1_1
   float fetchedMatrix[16];
   glGetFloatv ( matrixStack, fetchedMatrix );
   printf( "%s%s\n", DBG, header );
   printf( "               _OPEN_GL_              "
           "               _USER_\n");
   
   OpenGlEsUtil::glPrintMatrices( fetchedMatrix, m );
#endif
}

static unsigned char s_indices[4];

void OpenGlEsRenderer::enableImageTileState() {
   glEnableClientState(GL_VERTEX_ARRAY);
   
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   OpenGlEsUtil::assertNoErrors();
   glEnable(GL_TEXTURE_2D);
   OpenGlEsUtil::assertNoErrors();
   glDisable(GL_BLEND);
   OpenGlEsUtil::assertNoErrors();

   static vec3x tCoords[4];
   tCoords[0] = vec3x(0.0, 0.0);
   tCoords[1] = vec3x(1.0, 0.0);
   tCoords[2] = vec3x(0.0, 1.0);
   tCoords[3] = vec3x(1.0, 1.0);

   unsigned char *i = &s_indices[0];
   (*i++) = 0, (*i++) = 1, (*i++) = 2, (*i++) = 3;
   
   glTexCoordPointer(3, GL_FIXED, 0, tCoords);
   OpenGlEsUtil::assertNoErrors();
}

void OpenGlEsRenderer::setImageTileTexture(TextureBlock* block) {
   const OpenGlEsTextureBlock& texBlock(
      *static_cast<OpenGlEsTextureBlock*>(block ));
   
   glBindTexture(GL_TEXTURE_2D, texBlock.texHandle);
}

void OpenGlEsRenderer::drawImageTile(MC2BoundingBox bbox)
{
   // OpenGlEsStateDumping::dumpState("drawImageTile");   
   // compareMatrixWithInternal("drawImageTile", m_mvMatrix, GL_MODELVIEW);

   vec3x vCoords[4];
   // Upper left.
   vCoords[0] = vec3x(fixedp(fixedp::RAW,bbox.getMinLon()),
                      fixedp(fixedp::RAW,bbox.getMaxLat()));
   // Upper right
   vCoords[1] = vec3x(fixedp(fixedp::RAW,bbox.getMaxLon()),
                      fixedp(fixedp::RAW,bbox.getMaxLat()));
   // Lower left
   vCoords[2] = vec3x(fixedp(fixedp::RAW,bbox.getMinLon()),
                      fixedp(fixedp::RAW,bbox.getMinLat()));
   // Lower right
   vCoords[3] = vec3x(fixedp(fixedp::RAW,bbox.getMaxLon()),
                      fixedp(fixedp::RAW,bbox.getMinLat()));


   glVertexPointer(3, GL_FIXED, 0, vCoords);
   OpenGlEsUtil::assertNoErrors();

   OpenGlEsUtil::assertNoErrors();
   glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_BYTE, s_indices);
   
   OpenGlEsUtil::assertNoErrors();
}

void OpenGlEsRenderer::disableImageTileState() {

}

void OpenGlEsRenderer::drawPolyLine(const vec4x* buffer,
                                    unsigned int size,
                                    unsigned int color,
                                    unsigned int borderColor,
                                    int pass)
{
   glEnableClientState(GL_VERTEX_ARRAY);

   if( pass == 1 ) {
      glVertexPointer(3, GL_FIXED, sizeof(vec4x), &buffer[0].coord);

#ifdef VEC_3D_USE_COLOR_BUFFER
      glEnableClientState(GL_COLOR_ARRAY);
      glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(vec4x), &buffer[0].color);
#else
      unsigned int a = color >> 24 & 0xFF;
      unsigned int b = color >> 16 & 0xFF;
      unsigned int g = color >> 8 & 0xFF;
      unsigned int r = color >> 0 & 0xFF;
   
      static const fixedp fac(1/255.0f);

      glColor4x((fac * fixedp(r)).fx,
                (fac * fixedp(g)).fx,
                (fac * fixedp(b)).fx,
                (fac * fixedp(a)).fx);

      // setupColor( polyline.lineColor );
#endif

      m_numDrawPolylineCalls++;


      // loadVertexCoordinates( block, 0, 0);

      glDrawArrays( GL_TRIANGLE_STRIP, 0, size );   
   
      OpenGlEsUtil::assertNoErrors();

      m_numPolylines++;
      glDisableClientState(GL_COLOR_ARRAY);
   } else if(borderColor != 0) {

      glLineWidth(2.0f);     

      unsigned int a = borderColor >> 24 & 0xFF;
      unsigned int b = borderColor >> 16 & 0xFF;
      unsigned int g = borderColor >> 8 & 0xFF;
      unsigned int r = borderColor >> 0 & 0xFF;

      
      static const fixedp fac(1/255.0f);

      glColor4x((fac * fixedp(r)).fx,
                (fac * fixedp(g)).fx,
                (fac * fixedp(b)).fx,
                (fac * fixedp(a)).fx);

      // setupColor( polyline.outlineColor );
      glVertexPointer(3, GL_FIXED, sizeof(vec4x)*2, &buffer[0].coord);
      
      glDrawArrays( GL_LINE_STRIP, 0, size / 2 );

      glVertexPointer(3, GL_FIXED, sizeof(vec4x)*2, &buffer[1].coord);

      glDrawArrays( GL_LINE_STRIP, 0, size / 2 );

   }
}


void OpenGlEsRenderer::drawPolygon(const vec4x* buffer,
                                   unsigned int size,
                                   unsigned int color)
{
   // if( !polygon.handle ) {
   //    return;
   // }

   
   glEnableClientState(GL_VERTEX_ARRAY);
   
   glVertexPointer(3, GL_FIXED, sizeof(vec4x), &buffer[0].coord);

#ifdef VEC_3D_USE_COLOR_BUFFER
   glEnableClientState(GL_COLOR_ARRAY);
   glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(vec4x), &buffer[0].color);
#else
   unsigned int a = color >> 24 & 0xFF;
   unsigned int b = color >> 16 & 0xFF;
   unsigned int g = color >> 8 & 0xFF;
   unsigned int r = color >> 0 & 0xFF;
   
   static const fixedp fac(1/255.0f);

   glColor4x((fac * fixedp(r)).fx,
             (fac * fixedp(g)).fx,
             (fac * fixedp(b)).fx,
             (fac * fixedp(a)).fx);
   
   // setupColor( color );
#endif

   m_numDrawPolygonCalls++;

   glDrawArrays (GL_TRIANGLES, 0, size);

   // m_numTriangles += polygon.size / 3;

   OpenGlEsUtil::assertNoErrors();    

   glDisableClientState(GL_COLOR_ARRAY);

   // OpenGlEsVertexBlock* block =
   //    OpenGlEsVertexBlockManager::getBufferBlock( polygon.handle );

   // loadVertexCoordinates( block, 0, 0);

   // setupColor( polygon.color );

   // m_numDrawPolygonCalls++;

   // glDrawArrays (GL_TRIANGLES, 0, polygon.size );

   // m_numTriangles += polygon.size / 3;

   OpenGlEsUtil::assertNoErrors();
}

void OpenGlEsRenderer::loadTextureCoordinates( OpenGlEsVertexBlock* block )
{
#ifdef USE_VBO
   if( block->location == OpenGlEsVertexBlock::VIDEO_MEMORY ) {
      glBindBuffer( GL_ARRAY_BUFFER, block->data.bufferHandle );
      glTexCoordPointer( vec3x::NUM_ELEMENTS, GL_FIXED, 0, 0 );
   } else {
      glTexCoordPointer( vec3x::NUM_ELEMENTS, GL_FIXED, 0, block->data.bufferVector );
   }
#else
   glTexCoordPointer( vec3x::NUM_ELEMENTS, GL_FIXED, 0, block->data.bufferVector );
#endif
}

void OpenGlEsRenderer::loadVertexCoordinates(OpenGlEsVertexBlock* block,
                                             unsigned int stride,
                                             unsigned int offset)
{
#ifdef USE_VBO
  if( block->location == OpenGlEsVertexBlock::VIDEO_MEMORY ) {
     glBindBuffer( GL_ARRAY_BUFFER, block->data.bufferHandle );
     glVertexPointer( vec3x::NUM_ELEMENTS,
                      GL_FIXED, stride,
                      reinterpret_cast<GLvoid*>(sizeof(vec3x)*offset));
   } else {
     glVertexPointer( vec3x::NUM_ELEMENTS, GL_FIXED, stride,
                      block->data.bufferVector + offset );
   }
#else
  glVertexPointer( vec3x::NUM_ELEMENTS, GL_FIXED, stride,
                   block->data.bufferVector + offset );
#endif
}

/**
 *  res = res * right
 */

void leftMultiplyAndAssign( float res[16], float right[16] ) {
   float tempMatrix[16];
   OpenGlEsUtil::glMultMatrixf( right, res, tempMatrix );
   std::copy( &tempMatrix[0], &tempMatrix[16], &res[0] );
}


float OpenGlEsRenderer::getAspectRatio() const {
   isab::Rectangle size;

   getMapSizePixels( size );

   return static_cast<float>( size.getWidth() ) /
      static_cast<float>( size.getHeight() );
}

void OpenGlEsRenderer::setupViewPortVector(GLint viewPort[4]) {
   isab::Rectangle size;
   getMapSizePixels( size );
   
   viewPort[0] = 0;
   viewPort[1] = 0;
   viewPort[2] = size.getWidth();
   viewPort[3] = size.getHeight();   
}

void OpenGlEsRenderer::setupProjectionMatrix(const CameraParameters& c,
                                             GLfloat projMatrix[16]) {
   /**
    * Initialize the projection stack to match the correct parameters
    */
   
   OpenGlEsUtil::glLoadIdentityf( projMatrix );
   
   float tempMatrix[16];
   
   OpenGlEsUtil::gluPerspective( c.proj.FOVDegrees,
                                 c.proj.aspectRatio,
                                 c.proj.zNear,
                                 c.proj.zFar, tempMatrix );
   
   leftMultiplyAndAssign( projMatrix, tempMatrix );
}

void OpenGlEsRenderer::setupModelViewMatrix(const CameraParameters& c,
                                            GLfloat mvMatrix[16],
                                            bool offsetWorld)
{   
   float tempMatrix[16];

   /**   
    * Now, we setup the camera projetion stack.
    */
   
   // Reset the old model view matrix
   OpenGlEsUtil::glLoadIdentityf(tempMatrix);
   OpenGlEsUtil::glLoadIdentityf(mvMatrix);
   
   /**
    * Note that the comments for the below calls have been omitted.
    * This is because this method is identical to setupPerspectiveProjection,
    * except that it operates on system memory matrices. If they
    * don't match, something is wrong.
    */
   OpenGlEsUtil::glTranslatef(0.0f, 0.0f, c.mv.translationZ, tempMatrix);
   leftMultiplyAndAssign(mvMatrix, tempMatrix);
   OpenGlEsUtil::glRotateXf(c.mv.rotationXDeg, tempMatrix);
   leftMultiplyAndAssign(mvMatrix, tempMatrix);
   OpenGlEsUtil::glTranslatef( 0.0f,
                               c.mv.translationY,
                               0.0f,
                               tempMatrix );
   leftMultiplyAndAssign(mvMatrix, tempMatrix);
   OpenGlEsUtil::glRotateZf(c.mv.rotationZDeg, tempMatrix);
   leftMultiplyAndAssign(mvMatrix, tempMatrix);
   OpenGlEsUtil::glScalef(c.mv.scalingX, 1.0f, 1.0f, tempMatrix);
   leftMultiplyAndAssign(mvMatrix, tempMatrix);

   /**
    * Since we always project in absolute world coordinates, we also
    * need to translate the modelview camera to the correct position.
    */
   
   if(offsetWorld) {
      OpenGlEsUtil::glTranslatef( (float)c.mv.worldTranslationX,
                                  (float)c.mv.worldTranslationY,
                                  -0.0f,
                                  tempMatrix );

      leftMultiplyAndAssign( mvMatrix, tempMatrix );      
   }
}


void OpenGlEsRenderer::setupMatrices()
{
   CameraParameters c = m_camera->getCameraParams();
   
   setupProjectionMatrix(c, m_projMatrix);
   setupModelViewMatrix(c, m_mvMatrix, true);
   setupViewPortVector(m_viewPort);
}

void OpenGlEsRenderer::setupPerspectiveProjection()
{
   isab::Rectangle size;
   getMapSizePixels( size );
   
   /**
    * Setup client states and properties which are associated with the
    * perspective projection.
    */
 
   glDisable (GL_DEPTH_TEST);
   glEnableClientState(GL_VERTEX_ARRAY);

   glViewport( 0, 0, size.getWidth(), size.getHeight() );

   glEnableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   glDisable(GL_TEXTURE_2D);
   glDisable(GL_BLEND);

   /**
    * Then, initialize the projection stack to match the correct parameters
    */
 
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   CameraParameters c = m_camera->getCameraParams();
   OpenGlEsUtil::gluPerspective( c.proj.FOVDegrees,
                                 c.proj.aspectRatio,
                                 c.proj.zNear,
                                 c.proj.zFar );
   
   /**
    * Now, we setup the camera projection stack.
    */

   glMatrixMode(GL_MODELVIEW);

   glLoadIdentity();
   
   glTranslatex( fx::FX_ZERO.fx,
                 fx::FX_ZERO.fx,
                 fixedp(c.mv.translationZ ).fx);
   
   glRotatex(fixedp(c.mv.rotationXDeg).fx,
             fx::FX_ONE.fx, fx::FX_ZERO.fx, fx::FX_ZERO.fx);
   
   glTranslatex(fx::FX_ZERO.fx, fixedp(c.mv.translationY).fx, fx::FX_ZERO.fx);
   
   glRotatex( fixedp(c.mv.rotationZDeg).fx,
              fx::FX_ZERO.fx, fx::FX_ZERO.fx, fx::FX_ONE.fx );
   
   glScalex(fixedp(c.mv.scalingX).fx, fx::FX_ONE.fx, fx::FX_ONE.fx);
   
   OpenGlEsUtil::assertNoErrors();
}

void OpenGlEsRenderer::setupOrthographicProjection()
{
   isab::Rectangle size;
   getMapSizePixels(size);
   
   unsigned int screenWidth = size.getWidth();
   unsigned int screenHeight = size.getHeight();

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   glEnable(GL_TEXTURE_2D);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   glEnableClientState(GL_VERTEX_ARRAY);

   /**
    *   Setup the orthographic projection to have 0,0 in the
    *   bottom left corner and screenWidth, screenHeight in
    *   the upper right.
    */
   glOrthox( fx::FX_ZERO.fx,
             fixedp(screenWidth).fx, fixedp(screenHeight).fx,
             fx::FX_ZERO.fx, -fx::FX_ONE.fx, fx::FX_ONE.fx );

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   OpenGlEsUtil::assertNoErrors();
}

int OpenGlEsRenderer::getNumRenderedTriangles()
{
   return m_numTriangles;
}

void OpenGlEsRenderer::screenSizeChanged()
{
   forcePerspectiveSetup();
}


void gluMultMatrixVecf(const GLfloat matrix[16], const GLfloat in[4], GLfloat out[4])
{
   int i;

   for (i=0; i<4; i++) {
      out[i] =
         in[0] * matrix[0*4+i] +
         in[1] * matrix[1*4+i] +
         in[2] * matrix[2*4+i] +
         in[3] * matrix[3*4+i];
   }
}


void OpenGlEsRenderer::drawSprite( TextureBlock* block,
                                   MC2Point position,
                                   float angle,
                                   MC2Point rotationAnchor,
                                   float scalingFactorX,
                                   float scalingFactorY)
{
   if(block == NULL) {
      return;
   }

   glLoadIdentity();  

   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   glEnable(GL_TEXTURE_2D);
   glEnable(GL_BLEND);
   
   const OpenGlEsTextureBlock& texBlock(
      *static_cast<OpenGlEsTextureBlock*>(block ));
   
   OpenGlEsVertexBlock* spriteVertCoords =
      OpenGlEsVertexBlockManager::getBufferBlock(m_spriteVertHandle);
   OpenGlEsVertexBlock* spriteTexCoords =
      OpenGlEsVertexBlockManager::getBufferBlock(m_spriteTexHandle);


   // TODO: no need to load these in every call?
   loadTextureCoordinates(spriteVertCoords);
   loadVertexCoordinates(spriteTexCoords, 0, 0);
   
#ifdef GL_OES_VERSION_1_1
   // assert(glIsTexture( texBlock.texHandle ));
#endif

   if(m_cachedData.prevTexHandle != texBlock.texHandle) {
      
      glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
      glBindTexture(GL_TEXTURE_2D, texBlock.texHandle);      
   }
   
   m_cachedData.prevTexHandle = texBlock.texHandle;
   
   fixedp translateX(position.getX());
   fixedp translateY(position.getY());
   
   fixedp scaleX(texBlock.width * scalingFactorX);
   fixedp scaleY(texBlock.height * scalingFactorY);

   glTranslatex(translateX.fx, translateY.fx, fx::FX_ZERO.fx);

   // TODO: no need to rotate if angle == 0.0f?
   glRotatex(fixedp(angle).fx,
             fx::FX_ZERO.fx, fx::FX_ZERO.fx, fx::FX_ONE.fx );      

   // TODO: no need to multiply with scalingfactors if they are 1?
   glTranslatex(fixedp(-rotationAnchor.getX() * scalingFactorX).fx,
                fixedp(-rotationAnchor.getY() * scalingFactorY).fx,
                fx::FX_ZERO.fx );

   glScalex(scaleX.fx, scaleY.fx, fx::FX_ZERO.fx);      
    
   glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

   OpenGlEsUtil::assertNoErrors();
}

void OpenGlEsRenderer::drawPixelBox( const PixelBox& bbox,
                                     float r, float g, float b,
                                     MC2Point rotationAnchor,
                                     float correctionFactor,
                                     float angle )
{
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   glDisable(GL_BLEND);
   glDisable(GL_TEXTURE_2D);
   glLoadIdentity();

   OpenGlEsVertexBlock* spriteVertCoords =
      OpenGlEsVertexBlockManager::getBufferBlock(m_boxVertHandle);

   loadVertexCoordinates(spriteVertCoords, 0, 0);
   
   glColor4f(r, g, b, 1.0f);
   
   fixedp translateX(bbox.getTopLeft().getX());

   fixedp translateY(bbox.getTopLeft().getY());

   glTranslatex(translateX.fx, translateY.fx, fx::FX_ZERO.fx);

   glRotatex(fixedp(angle).fx,
             fx::FX_ZERO.fx, fx::FX_ZERO.fx, fx::FX_ONE.fx );

   glTranslatex(fixedp( -rotationAnchor.getX() * correctionFactor).fx,
                fixedp(-rotationAnchor.getY() * correctionFactor).fx,
                fx::FX_ZERO.fx );

   glScalex(fixedp(bbox.getWidth()).fx, fixedp(bbox.getHeight()).fx,
            fx::FX_ZERO.fx);

   glPointSizex(fixedp(1).fx);
   glLineWidthx(fixedp(1).fx);

   glDrawArrays(GL_LINE_STRIP, 0, 5);
   glDrawArrays(GL_POINTS, 0, 4);

   glEnable(GL_TEXTURE_2D);
   glEnable(GL_BLEND);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);

   OpenGlEsUtil::assertNoErrors();
}

void OpenGlEsRenderer::backgroundColorChanged()
{
   RgbColor color = getFixedRgbColor(getBackgroundColor());

   glClearColorx( color.r.fx, color.g.fx, color.b.fx, fx::FX_ONE.fx );

   OpenGlEsUtil::assertNoErrors();
}

void OpenGlEsRenderer::drawBBox(const MC2BoundingBox& bbox)
{
   glPushMatrix();

   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   glDisable(GL_TEXTURE_2D);
   glDisable(GL_BLEND);
   
   vbuf3x draw;

   int32 minLat, maxLat;
   int32 minLon, maxLon;

   minLat = bbox.getMinLat();
   maxLat = bbox.getMaxLat();
   minLon = bbox.getMinLon();
   maxLon = bbox.getMaxLon();

   // resetCamera();
   setupPerspectiveProjection();
   offsetPerspective(MC2Coordinate(0, 0));

   draw.push_back(vec3x( fixedp(fixedp::RAW,  minLon),
                          fixedp(fixedp::RAW, minLat ) ));

   draw.push_back(vec3x( fixedp(fixedp::RAW,  maxLon),
                          fixedp(fixedp::RAW, minLat ) ));

   draw.push_back(vec3x( fixedp(fixedp::RAW,  maxLon),
                          fixedp(fixedp::RAW, maxLat ) ));

   draw.push_back(vec3x( fixedp(fixedp::RAW,  minLon),
                          fixedp(fixedp::RAW, maxLat ) ));

   draw.push_back(draw.front());

   glPointSizex(fixedp(10).fx);

   glLineWidthx(fixedp(3).fx);

   glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

   glEnableClientState(GL_VERTEX_ARRAY);
   
   VertexBlockManager* vertManager = getVertexBlockManager();
   
   VertexBlock* vertBlock = vertManager->allocateVertexBlock(draw);
   OpenGlEsVertexBlock* glesVertBlock =
      OpenGlEsVertexBlockManager::getBufferBlock(vertBlock);
   
   loadVertexCoordinates(glesVertBlock, 0, 0);

   glDrawArrays(GL_LINE_LOOP, 0, draw.size());
   glDrawArrays(GL_POINTS, 0, draw.size());

   OpenGlEsUtil::assertNoErrors();
   
   delete vertBlock;
   
   glPopMatrix();

   glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

   resetOffsetPerspective();
}

void OpenGlEsRenderer::setClippingRectangle(int x, int y,
                                            int width, int height)
{
   isab::Rectangle rect;
   getMapSizePixels(rect);
   
   glEnable(GL_SCISSOR_TEST);

   y = rect.getHeight() - y - height;
   
   // -height since glScissor works on lower-left point as defining point
   glScissor(x, y, width, height);
}

void OpenGlEsRenderer::resetClippingRectangle()
{
   isab::Rectangle rect;
   getMapSizePixels(rect);
   
   glScissor(0, 0, rect.getWidth(), rect.getHeight());
   glDisable(GL_SCISSOR_TEST);
}

void OpenGlEsRenderer::userClearOrthoDebug()
{
   glPushMatrix();

   glDisable(GL_TEXTURE_2D);
   vbuf3x draw;

   int32 minY, maxY;
   int32 minX, maxX;

   minY = fixedp(fixedp::RAW, (int)MIN_INT32);
   maxY = fixedp(fixedp::RAW, (int)MAX_INT32);
   minX = fixedp(fixedp::RAW, (int)MIN_INT32);
   maxX = fixedp(fixedp::RAW, (int)MAX_INT32);

   // resetCamera();
   setupPerspectiveProjection();

   draw.push_back(vec3x( fixedp(fixedp::RAW,  minX),
                           fixedp(fixedp::RAW, minY ) ));

   draw.push_back(vec3x( fixedp(fixedp::RAW,  minX),
                           fixedp(fixedp::RAW, maxY ) ));
   
   draw.push_back(vec3x( fixedp(fixedp::RAW,  maxX),
                           fixedp(fixedp::RAW, minY ) ));

   draw.push_back(vec3x( fixedp(fixedp::RAW,  maxX),
                           fixedp(fixedp::RAW, maxY ) ));

   draw.push_back(draw.front());

   glPointSizex(fixedp(10).fx);

   glLineWidthx(fixedp(3).fx);

   glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
   glEnableClientState(GL_VERTEX_ARRAY);

   glVertexPointer(3, /*3 components per vertex (x,y)*/
                   GL_FIXED, 0, &draw[0]);

   glDrawArrays(GL_TRIANGLE_STRIP, 0, draw.size());

   OpenGlEsUtil::assertNoErrors();

   glPopMatrix();
   glEnable(GL_TEXTURE_2D);
}

void OpenGlEsRenderer::offsetPerspective(const MC2Coordinate& corner)
{
   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();

   MC2Coordinate focalCoord = m_camera->getFocalCoordinate();
   
   int diffLat = focalCoord.lat - corner.lat;
   int diffLon = focalCoord.lon - corner.lon;
   
   vec3x pos(fixedp(fixedp::RAW,  diffLon ),
                fixedp(fixedp::RAW,  diffLat),
                fx::FX_ZERO);

   glTranslatex(-pos.x.fx, -pos.y.fx, -pos.z.fx);
}

void OpenGlEsRenderer::resetOffsetPerspective()
{
   glPopMatrix();

   OpenGlEsUtil::assertNoErrors(); 
}

void OpenGlEsRenderer::cameraUpdated()
{
   setupPerspectiveProjection();
   setupMatrices();
}

void OpenGlEsRenderer::transformf(int x, int y,
                                  float* outX, float* outY, float* outZ)
{
   float fx = (float)(fixedp(fixedp::RAW,  x ));
   float fy = (float)(fixedp(fixedp::RAW,  y ));
   
   projectCoord(m_mvMatrix,
                m_projMatrix,
                fx, fy,
                outX, outY, outZ);
}

void OpenGlEsRenderer::projectCoord( const float mvMatrix[16],
                                     const float projMatrix[16],
                                     float x,
                                     float y,
                                     float* outX,
                                     float* outY,
                                     float* outZ)
{
   isab::Rectangle size;

   getMapSizePixels(size);

   float in[4];
   float out[4];

   in[0] = x;
   in[1] = y;
   in[2] = 0.0f;
   in[3] = 1.0f;

   gluMultMatrixVecf(mvMatrix, in, out);
   gluMultMatrixVecf(projMatrix, out, in);

   in[0] /= in[3];
   in[1] /= in[3];
   in[2] /= in[3];

   in[0] = in[0] * 0.5f + 0.5f;
   in[1] = in[1] * 0.5f + 0.5f;
   in[2] = in[2] * 0.5f + 0.5f;

   x = in[0];
   y = in[1];
   
   *outX = x * size.getWidth();
   *outY = size.getHeight() - y * size.getHeight();
   *outZ = in[2]; //TODO: What to multiply this by?
}

bool
OpenGlEsRenderer::inverseTransform(MC2Coordinate& coord,
                                   const MC2Point& point)
{
   isab::Rectangle size;
   getMapSizePixels(size);

   int h = size.getHeight();
   
   MC2Coordinate ret;
   
   CameraParameters c = m_camera->getCameraParams();
   
   float p1[4] = { point.getX(), h - point.getY(), c.proj.zNear, 1.0f };
   float p2[4] = { point.getX(), h - point.getY(), c.proj.zFar, 1.0f };

   float c1[4], c2[4];

   float mvMatrix[16];
   setupModelViewMatrix(c, mvMatrix, false);
   
   OpenGlEsUtil::gluUnProject( p1, mvMatrix, m_projMatrix, m_viewPort, c1);
   OpenGlEsUtil::gluUnProject( p2, mvMatrix, m_projMatrix, m_viewPort, c2);

   vec3f v(c2[0] - c1[0], c2[1] - c1[1], c2[2] - c1[2]);
   
   vec3f startPoint(0, 0, c.proj.zFar);
   
   float modelViewInv[16];

   if (!OpenGlEsUtil::glInvertMatrix(mvMatrix, modelViewInv)){
      return false;
   }
   
   float inPos[4] = {0.0f, 0.0f, 0.0f, 1.0f};

   float camPos[4];

   OpenGlEsUtil::glMultMatrixVecf(modelViewInv, inPos, camPos);

   vmath::normalize(v);
   
   double t = -camPos[2]/v.z;
   
   vec3f intersection;

   MC2Coordinate centerCoord = m_camera->getFocalCoordinate();
   
   intersection.x = camPos[0] + t * v.x;
   intersection.y = camPos[1] + t * v.y;
   intersection.z = camPos[2] + t * v.z;

   coord.lon = static_cast<int>(intersection.x / FixedPointHelpers::FXFACTOR);
   coord.lat = static_cast<int>(intersection.y / FixedPointHelpers::FXFACTOR);
   coord.lon += centerCoord.lon;
   coord.lat += centerCoord.lat;

   return true;
}

vec3x
VecAroundZ( const vec3x& v, double angle )
{
   return vec3x( v.x * cos( angle ) - v.y * sin( angle ),
                 v.x * sin( angle ) + v.y * cos( angle ) );
}

void OpenGlEsRenderer::doLastMinuteStuff()
{
   // std::cout << "length: " << getTextWidth(str) << std::endl;

   // drawText(0, 0, 1.0f, 0.0f, 0.0, 1.0f, str);
   // 
   // glFinish();
   

      
#ifdef Prof_ENABLED   
//   #error "HA"
   g_renderer = this;

   Prof_update(PROF_accumulate);
      
   Prof_draw_gl(0.0f, 40.0f, 
                320.0f, 240.0f,
                static_cast<float>(m_font.getCharHeight()), 1,
                drawTextProxy,
                getTextWidthProxy);

   g_renderer = NULL;
   g_renderer = NULL;
#endif

   // glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

   // glLineWidth(4.0f);
   
   // vec3f testArray[]= { vec3f(0.0f,   400.0f),
   //                      vec3f(100.0f, 400.0f),
   //                      vec3f(100.0f, 400.0f),
                        
   //                      vec3f(200.0f, 400.0f),
   //                      vec3f(200.0f, 400.0f),
   //                      vec3f(300.0f, 400.0f),

   // };

   // glEnableClientState(GL_VERTEX_ARRAY);
   // glVertexPointer(3, GL_FLOAT, 0, &testArray);

   // glDrawArrays(GL_LINE_STRIP, 0, 5);
   
   return;
}

void OpenGlEsRenderer::drawText(float x, float y,
                                float r, float g, float b, float a,
                                char* str)
{
   glLoadIdentity();

#ifdef GL_OES_VERSION_1_1
   glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif

   m_textCoords.clear();
   
   m_font.fillTextureCoords(str, m_textCoords);
   if (m_fontSprite == NULL){
      return;
   }
   const OpenGlEsTextureBlock& texBlock(
      *static_cast<OpenGlEsTextureBlock*>(m_fontSprite));

   glTexCoordPointer(3, GL_FIXED, 0, &m_textCoords[0]);
   
   glVertexPointer(3, GL_FIXED, 0, &m_fontVertCoords[0]);
   
   glTranslatef(x, y, 0.f);
   
   glColor4f(r, g, b, a);

   glBindTexture(GL_TEXTURE_2D, texBlock.texHandle);
   unsigned int size = m_textCoords.size();
   glDrawArrays(GL_TRIANGLES, 0, size);    
}

float OpenGlEsRenderer::getTextWidth(char* str) {
   float lengthChars = strlen(str);
   return lengthChars * m_font.getCharWidth();
}


