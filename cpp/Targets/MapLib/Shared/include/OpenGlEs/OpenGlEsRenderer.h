/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef OPENGLESRENDERER_H
#define OPENGLESRENDERER_H

#include "ExternalRenderer.h"
#include "Vector3d.h"
#include "VertexBlockManager.h"
#include "ImageLoader.h"
#include "TextureBlockManager.h"
#include "TextureBlock.h"
#include "OpenGlEs.h"
#include "MonoSpacedFont.h"

struct CameraParameters;
class Camera;
class PNGLoader;
class OpenGlEsTextureBlockManager;
class OpenGlEsVertexBlockManager;
class OpenGlEsVertexBlock;
class NativeTextInterface;


class OpenGlEsRenderer : public ExternalRenderer {
public:
   /**
    *   Creates an OpenGL ES-based renderer.
    *
    *   @param   eglParameters      Parameters that are used when
    *                               swapping buffers.
    *
    *   @param   textLoader         Platform specific bitmap text
    *                               loader for rasterization of
    *                               texts.
    *
    *   @param   texManager         Memory manager for textures.
    *
    *   @param   vertManager        Memory manager for vertices.
    *
    *   @param   resolutionWidth    The screen width.
    *
    *   @param   resolutionHeight   The screen height.
    *   
    */
   
   OpenGlEsRenderer(BitMapTextLoader* textLoader,
                    NativeTextInterface* textInterface,
                    OpenGlEsTextureBlockManager* texManager,
                    OpenGlEsVertexBlockManager* vertManager,
                    int resolutionWidth,
                    int resolutionHeight);
   
   virtual ~OpenGlEsRenderer();

   

   /**
    * Sets the clipping region of the drawer, which is the
    * region that stuff can be drawn to. Pixels outside this
    * region will not be affected. Default implementation
    * does nothing. If implementation supports clipping,
    * supportsClipping() will return true.
    */ 
   virtual void setClippingRectangle(int x, int y,
                                     int width, int height);

   /**
    * Resets the clipping region of the drawer to the entire screen.
    */ 
   virtual void resetClippingRectangle();

   /**
    * Drawers the support clipping will return true if this method
    * is called.
    */ 
   virtual bool supportsClipping() const { return true; }

   virtual void userClearOrthoDebug();
   
   /**
    * @see ExternalRenderer::setupPerspectiveProjection
    */
   virtual void setupPerspectiveProjection(); 

   /**
    *   @see   ExternalRenderer::offsetPerspective
    */
   virtual void offsetPerspective(const MC2Coordinate& corner);

   /**
    *   @see   ExternalRenderer::resetOffsetPerspective
    */
   virtual void resetOffsetPerspective();
   
   /**
    * @see ExternalRenderer::setupOrthographicProjection
    */ 
   virtual void setupOrthographicProjection();

   /**
    * @see ExternalRenderer::projectionUpdated
    */ 
   virtual void cameraUpdated();
   
   void setupMatrices();

   void setupViewPortVector(GLint viewPort[4]);

   void setupProjectionMatrix(const CameraParameters& c,
                              GLfloat projMatrix[16]);

   void setupModelViewMatrix(const CameraParameters& c,
                             GLfloat mvMatrix[16],
                             bool offsetWorld);

   float getTextWidth(char* str);
   void drawText(float x, float y,
                 float r, float g, float b, float a,
                 char* str);


private:
   
   /**
   *   @see   ExternalRenderer::getNumRenderedTriangles
   */

   virtual int getNumRenderedTriangles();
   
   /**
   *   @see   ExternalRenderer::perspectiveProjectToScreen
   */

   virtual void transform(MC2Point& point, const MC2Coordinate& coord );

   virtual void transform(MC2Point& point, float x, float y );

   virtual void transformf(MC2Pointf& point, const MC2Coordinate& coord );
   
   /**
   *   @see   ExternalRenderer::clearScreen
   */

   virtual void clearScreen();

   /**
   *   @see   ExternalRenderer::initializeDrawing
   */

   virtual void initializeDrawing();
   
   /**
   *   @see   ExternalRenderer::screenSizeChanged
   */

   virtual void screenSizeChanged();
   
   /**
   *   @see   ExternalRenderer::drawBBox
   */

   virtual void drawBBox( const MC2BoundingBox& bbox );

   /**
   *   @see   ExternalRenderer::drawPixelBox
   */

   virtual void drawPixelBox( const PixelBox& bbox,
                              float r, float g, float b,
                              MC2Point rotationAnchor,
                              float correctionFactor,
                              float angle );
   
   void drawImageTile(TextureBlock* block,
                      MC2BoundingBox bbox);
   
   /**
   *   @see   ExternalRenderer::drawPolygon
   */

   virtual void drawPolygon(const vec4x* buffer,
                            unsigned int size,
                            unsigned int color);
   
   /**
   *   @see   ExternalRenderer::drawPolyLine
   */

   virtual void drawPolyLine(const vec4x* buffer,
                             unsigned int size,
                             unsigned int color,
                             unsigned int borderColor,
                             int pass);
   /**
   *   @see   ExternalRenderer::backgroundColorChanged
   */

   virtual void backgroundColorChanged();

   /**
   *   @see   ExternalRenderer::drawSpriteShape
   */

   virtual void drawSprite( TextureBlock* block,
                            MC2Point position,
                            float angle,
                            MC2Point rotationAnchor,
                            float scalingFactorX = 1.0f,
                            float scalingFactorY = 1.0f);
   
   virtual bool inverseTransform(MC2Coordinate& coord, const MC2Point& point);

private:

   /**
    *   Creates the internal sprite buffers that are used for drawing.
    *   Sets up vertex blocks for textures, sprites and boxes.
    *
    */ 

   void createSpriteBuffers();


   /**
    *   Internal helper method for projecting points to screen. Uses
    *   a modelview matrix and a projection matrix to accomplish the
    *   projection.
    *
    *   @param   mvMatrix    The modelview matrix.
    *   @param   projMatrix  The projection matrix.
    *   @param   coord       The coordinate to base the projection on.
    *
    */ 

   MC2Pointf projectCoord( const float mvMatrix[16],
                           const float projMatrix[16],
                           float x, float y);

   MC2Pointf projectCoord( const float mvMatrix[16],
                           const float projMatrix[16],
                           const MC2Coordinate& coord);

   /**
    *   Interprets the vertices in block as texture coordinates and
    *   loads them into the OpenGL state machine so that it can be used
    *   by subsequent rendering calls.
    *
    *   @param    The vertex block containing the texture coordinates.
    *
    */ 
   
   void loadTextureCoordinates( OpenGlEsVertexBlock* block );

   /**
    *   Loads the vertices from block into the OpenGL state machine
    *   so that they can be used in subsequent rendering calls.
    *   
    *   @param   block   The vertex block.
    */
   
   void loadVertexCoordinates( OpenGlEsVertexBlock* block,
                               unsigned int stride,
                               unsigned int offset);
   

   void doLastMinuteStuff();

private:
   void loadFontTexture();
   float getAspectRatio() const;

   
   VertexBlock* m_spriteVertHandle;
   VertexBlock* m_boxVertHandle;
   VertexBlock* m_spriteTexHandle;

   TextureBlock* m_fontSprite;
   vbuf3x m_fontVertCoords;
   vbuf3x m_textCoords;
   
   int m_numDrawPolylineCalls;
   int m_numDrawPolygonCalls;
   
   float m_mvMatrix[16];
   float m_projMatrix[16];
   int m_viewPort[4];

   struct {
      unsigned int prevTexHandle;
   } m_cachedData;
   MonoSpacedFont m_font;
};


#endif /* OPENGLESRENDERER_H */
