/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef _EXTERNALDRAWER_H_
#define _EXTERNALDRAWER_H_

#include "OverlapDetector.h"
#include "ViewPortInterface.h"
#include "TileMap.h"
#include "OverlayDrawer.h"
#include "TileMapHandler.h"

class PreDrawListener;
class NativeTextInterface;
class BitMapTextLoader;
class Camera;
class ImageLoader;
class TextCachingBlock;
class TextureBlock;
class TileMapHandler;
class TileMapContainer;
class TileMapFormatDesc;
class TilePrimitiveFeature;
class TileMap;
class Profiler;
class VertexBlockManager;
class TextureBlockManager;
class PixelBox;
class SwapListener;
class BitMapLoader;
class TextResult;
class WFString;

namespace WFAPI {
class FontSpec;
}


namespace isab {
class Rectangle;
class BitMap;
}

namespace WFAPI {
class ImageSpec;
class WFString;
}

/**
 *     ExternalRenderer
 *
 *     An abstract base class for external renderers. An external renderer is
 *     a renderer which can draw FeatureShape-structures, which are generated from
 *     TilePrimitiveFeatures. These structures contain triangulated polygons
 *     or textured bitmaps, suitable for drawing by a 3D API such as OpenGL ES
 *     or Direct3d Mobile.
 *
 *
 *     For example, an OpenGL ES based renderer would inherit from this class
 *     and implement all the pure virtual functions as documented. 
 *
 *
 */ 

class ExternalRenderer : public ViewPortInterface,
                         public OverlayDrawer
{
public:
   ExternalRenderer( BitMapTextLoader* loader,
                     NativeTextInterface* textInterface,
                     TextureBlockManager* texManager,
                     VertexBlockManager* vertManager,
                     int width,
                     int height);
   
   virtual ~ExternalRenderer();
      
   /**
    *   Called before any rendering (i.e. drawMap) takes place.
    *
    */
   
   void beginFrame();

   /**
    *   Called after the rendering (i.e. drawMap) has been performed.
    *
    */
   void endFrame();
   
   /**
    *   Clear the screen using the current background color.
    */
   virtual void clearScreen() = 0;
   
   /**
    *   Should be called whenever the resolution has changed.
    *
    */
   void setScreenSize( int width, int height );
   
   /**
    *  Sets up the current background color.
    *
    *  @param  rgbColor  Background color in rgb format.
    *
    */  
   
   void setBackgroundColor( unsigned int rgbColor );

   /**
    *   Sets up a swap listener. This listener will be called upon
    *   whenever the screen needs to be refreshed.
    */ 
   
   void setSwapListener( SwapListener* swapListener );

   /**
    *   Sets up a pre-draw listener. This listener will be called upon
    *   whenever the screen needs to be initialized before the drawing
    *   of a frame.
    */
   void setPreDrawListener( PreDrawListener* preDrawListener );


   /**
    *   Will cause the renderer to trigger the swap listener.
    */ 
   void triggerApplicationRedraw();
   
   /**
    *   Returns a TextureBlockManager pointer that can be used to
    *   load and unload bitMaps in a platform- and texture source-specific
    *   manner, compatible with deriving class. 
    */
   
   TextureBlockManager* getTextureBlockManager();

   /**
    *   Returns a VertexBlockManager pointer that can be used
    *   to store, update and release vertexBuffers in a platform
    *   specific manner, compatible with the deriving class.
    *   
    */
   
   VertexBlockManager* getVertexBlockManager();

   /** 
    *   Draws all of the features in tileMapCont. Will delegate calls to
    *   drawPrimitive, which in turn will delegate calls to the actual drawing
    *   methods (drawPolygon, drawPolyline, drawBitmap).
    *
    *   @param   tilemapCont   The container which holds the tile maps to draw
    *   @param   formatDesc    The format description which influences the drawing
    *   @param   skipOutlines  Determines if outlines should be drawn
    *   
    */   
   
   void drawMap( const MC2BoundingBox& box,
                 TileMapContainer& tileMapCont,
                 TileMapFormatDesc& formatDesc,
                 bool skipOutlines);

   void drawImageTileVector(std::vector<ImageTile*>& tiles);
   
   /**
    *   Draws the texts defined in textResults.
    *   
    *   @param   textResults   A vector containing placement information,
    *                          and strings for all texts.
    */
   
   void drawTexts( const std::vector<TextResult*>& textResults );

   /**
    * Draws the horizon.
    *
    * @param height The height of the horizon. In normalized unit from 0 to 1,
    *               where 1 is the screen height.
    * 
    */
   void drawHorizon( float height );

   /**
    * Draws a string on the map, typically used to draw the copyright text
    *
    * @param string The UTF-8 string to draw
    * @param stringPos The screen position to start drawing the string to
    * @param stringColor The color to use when drawing the string
    *
    */
   void drawString( const char* utf8String,
                    const MC2Point& stringPos, uint32 stringColor );
   
   /**
    * Draws an image specification for overlay.
    * 
    */ 
   virtual void drawImageSpec(const WFAPI::ImageSpec* imgSpec,
                              const MC2Point& point);

   /**
    *   Creates an image specification from a raw RGBA image. Note that
    *   this will only be valid as long as the current context is.
    *   If switching between drawers or when losing the display, the
    *   buffer will have to be reloaded.
    *
    *   @param image The raw image to base the textured specification from.
    */ 
   WFAPI::ImageSpec* loadImageSpecFromRGBA(const pal::wf_uint8* buffer,
                                           pal::wf_uint32 widthPixels,
                                           pal::wf_uint32 heightPixels);
   
   /**
    * Crops the string, modifies the string to fit within the
    * maxWidth with the last three chars as ...
    *
    * @param text The text to crop.
    * @param maxWidth the maximal width in pixels.
    */
   virtual void cropString(STRING& text, unsigned int maxWidth);
   
   /**
    * Draws a string of text as a texture block. The string
    * will be aligned according to alignment within the given
    * rectangle.
    */ 
   virtual void drawOverlayText(
      const WFAPI::WFString& text,
      TextCachingBlock& cacheBlock,
      const WFAPI::OverlayItemVisualSpec::TextAlignment alignment,
      WFAPI::FontSpec* font,
      const isab::Rectangle& stringRect,
      const WFAPI::OverlayItemVisualSpec::RGB& textColor);
   
   /**          
    *   Returns the screen size (in pixels) of the renderer.
    *
    *   @ret rect  The screen rectangle.
    *
    */
   void getMapSizePixels( isab::Rectangle& rect ) const;


   void setCamera(const Camera* camera);

   void setProjection(MapProjection* projection);
   
   void setNightMode(bool enabled);

   void getBitMapAsRectangle(isab::Rectangle& size,
                             const MC2Coordinate& center,
                             const isab::BitMap* bmp);

   /**
    *    Projects a coordinate from world space to screen space
    *
    *    using the renderers perspective projection.
    *
    *    @param    coord   The world coordinate.
    *
    *    @ret      point   The screen space coordinate.
    *
    */
   virtual void transform(MC2Point& point, const MC2Coordinate& coord ) = 0;

   virtual void transform(MC2Point& point, float x, float y ) = 0;

   virtual void transformf(MC2Pointf& point,  const MC2Coordinate& coord ) = 0;

   virtual bool inverseTransform(MC2Coordinate& coord, const MC2Point& point) = 0;

   void calcOffsetCenterCoord(const MC2Point& lockPoint,
                              const MC2Coordinate& lockCoord,
                              MC2Coordinate& coord);
   
   /**
    *   @see   ExternalRenderer::offsetPerspective
    */
   virtual void offsetPerspective(const MC2Coordinate& corner) = 0;

   /**
    *   @see   ExternalRenderer::resetOffsetPerspective
    */
   virtual void resetOffsetPerspective() = 0;

   virtual void cameraUpdated() = 0;

   
   /**************** DEBUGGING FUNCTIONS *******************/

   virtual void drawPixelBox( const PixelBox& box,
                              float r, float g, float b,
                              MC2Point rotationAnchor,
                              float correctionFactor, 
                              float angle ) = 0;
   
   void toggleScaleChecking();
   bool scaleCheckingOn();
   void setEnableWorldDrawing( bool enableWorldDrawing );
   void setMapHandler(TileMapHandler* mapHandler );
   void forcePerspectiveSetup();
   void setAdditionalData( MC2SimpleString& additionalData );

   void toggleDrawingFunction();
private:
   void drawLineBlock(TriStorageBlock& block,
                      unsigned int pass,
                      vec4x* triangleData,
                      const MC2BoundingBox& box,
                      TileMapContainer& tileMapCont,
                      TileMapFormatDesc& formatDesc,
                      bool skipOutlines);
   
   void drawPolygonBlock(TriStorageBlock& block,
                         unsigned int pass,
                         vec4x* triangleData,
                         const MC2BoundingBox& box,
                         TileMapContainer& tileMapCont,
                         TileMapFormatDesc& formatDesc,
                         bool skipOutlines);
   
   void drawBitmapBlock(TriStorageBlock& block,
                        unsigned int pass,
                        vec4x* triangleData,
                        const MC2BoundingBox& box,
                        OverlapDetector<MC2BoundingBox>& detector,
                        TileMapContainer& tileMapCont,
                        TileMapFormatDesc& formatDesc,
                        bool skipOutlines);
   
   
   virtual void doLastMinuteStuff() { }
   
   void blockDrawingFunction( const MC2BoundingBox& box,
                              TileMapContainer& tileMapCont,
                              TileMapFormatDesc& formatDesc,
                              bool skipOutlines);
   
   /**
    *   Returns how many triangles were rendered during a pass
    */
   
   virtual int getNumRenderedTriangles() = 0;

   /**
    *   Initialize frame specific members such as counters.
    */ 

   virtual void initializeDrawing() { }
   
   /**
    *   Perform state changes so that the rendering adapts to the
    *   new screen size.
    */

   virtual void screenSizeChanged( ) = 0;

   /**
    *   Adapt the background color setting so that clear works
    *   as expected.
    */ 
   
   virtual void backgroundColorChanged() = 0;

   /**
    *   Draws a single polygon based on its PolygonFeatureShape structure. This
    *   structure is tied to a TilePrimitiveFeature. This call should be
    *   performed between beginFrame and endFrame. These methods are made
    *   private since the drawing of the map should be orchestrated by
    *   the renderer itself.
    *
    *   @param polygon  The polygon to be drawn.
    */
   
   virtual void drawPolygon(const vec4x* buffer,
                            unsigned int size,
                            unsigned int color) = 0;
   
   /**
    *   Draws a polyline. For more info, see drawPolygon.
    *
    *   @param   polyline   The polyline structure to be drawn.
    *   @param   pass       The drawing pass. Polylines are drawn in two
    *                       passes, the first is the outline and the second
    *                       is the inner part of the road.
    *                       
    */          
   virtual void drawPolyLine(const vec4x* buffer,
                             unsigned int size,
                             unsigned int color,
                             unsigned int borderColor,
                             int pass) = 0;
   /**
    *   Draws a SpriteShape on screen. These are used for POI:s,
    *   texts and other features. Like the above drawing operations, these are
    *   performed on a FeatureShape object tied to a TilePrimitiveFeature.
    *
    *   @param    bitmap             The bitmap structure to be drawn.
    *   @param    rotationAnchor     The offset to the center of the texture.
    *   @param    correctionFactor   The correction factor for scaling. Some
    *                                displays use this to correct DPI factor.
    *
    */

   virtual void drawSprite( TextureBlock* block,
                            MC2Point position,
                            float angle,
                            MC2Point rotationAnchor,
                            float scalingFactorX = 1.0f,
                            float scalingFactorY = 1.0f ) = 0;
   
   /**
    * Sets up a perspective projection.
    */
   virtual void setupPerspectiveProjection() = 0;

   /**
    * Sets up an orthographic projection. All vertex coordinates will be
    * specified in screen space, with 0,0 located in the upper left corner.
    */ 
   virtual void setupOrthographicProjection() = 0;

   virtual void drawImageTile(TextureBlock* block,
                      MC2BoundingBox bbox) = 0;
   
   /* DEBUG */
   
   virtual void drawBBox( const MC2BoundingBox& bbox ) = 0;
   
   bool atEndOfType( TileMap::primVect_t::iterator curIt,
                      TileMap::primVect_t::iterator theEnd,
                      int type);
   
   uint32 m_timingCycle;
   uint32 m_lastFrameTime;
   uint32 m_lastExternalTime;
   uint32 m_sumFrameTime;
   uint32 m_sumExternalTime;
   uint32 m_numFrames;
   
   std::vector<TextureBlock*> m_timerResultTex;

   //the copyright texture to cache between runs
   TextureBlock* m_stringTextureRepresentation;
   //the string to compare changes to between runs
   WFAPI::WFString m_prevString;
   
   struct Impl;
   
   Impl* m_impl;
protected:
   unsigned int getBackgroundColor();
   typedef std::vector<std::pair<const TilePrimitiveFeature*, TileMap*> > FeatureCache;

   FeatureCache featCache;
   
   int m_numTriangles;
   int m_numPolylines;
   int m_numDrawPolygonCalls;
   int m_numDrawPolylineCalls;

   const Camera* m_camera;
};

#endif /* _EXTERNALDRAWER_H_ */
