/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#define CORE_LOGGING_MODULE_NAME "ExternalRenderer"
// #define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE

#include "CoreDebugPrint.h"
#include "MC2BoundingBox.h"
#include "ExternalRenderer.h"
#include "MC2Logging.h"
#include "VertexBlockManager.h"
#include "ImageSpecInternal.h"
#include "TileMapContainer.h"
#include "TileMapFormatDesc.h"
#include "TileMap.h"
#include "GfxConstants.h"
#include "TileMapHandler.h"
#include "TileMapUtil.h"
#include "TileMapTextHandler.h"
#include "BitMapTextLoader.h"
#include "TextureBlockManager.h" 
#include "TileMapUtil.h"
#include "Utility.h"
#include "MC2SimpleString.h"
#include "SwapListener.h"
#include "TextCachingBlock.h"
#include "DebugDrawing.h"
#include "HorizonImage.h"
#include "Camera.h"
#include "PreDrawListener.h"
#include "HollowImageSpec.h"
#include "PALGraphics.h"

#ifdef __SYMBIAN__
#include <e32math.h>
#endif



// #define DRAW_TILE_BOUNDS

// static const char* DBG = "[EXT_RENDERER]: ";
static const unsigned int GRADIENT_IMAGE_HEIGHT = 64;
static const unsigned int GRADIENT_IMAGE_WIDTH = 1;
static const unsigned int GRADIENT_MAX_HEIGHT = 10;
static const unsigned int HORIZON_IMAGE_SETS = 4;

struct ExternalRenderer::Impl {
   MapProjection* projection;
   float prevScale;
   int width, height;
   unsigned int backgroundColor;
   bool scaleChecking;
   bool enableWorldDrawing;
   bool forcePerspectiveSetup;
   SwapListener* swapListener;
   PreDrawListener* preDrawListener;
   TileMapHandler* mapHandler;
   BitMapTextLoader* textLoader;
   NativeTextInterface* textInterface;
   TextureBlockManager* texManager;
   VertexBlockManager* vertManager;
   MC2SimpleString additionalData;
   RGBA32BitMap horizonBitMap;
   TextureBlock* horizonTexBlock;
   RGBA32BitMap gradientBitMap;
   TextureBlock* gradientTexBlock;
   bool drawBlocks;
};

// Helper function to load the horizon image from the HorizonImage.h file.
RGBA32BitMap loadHorizonImage(const char* data){
   RGBA32BitMap bmp;
   bmp.imgHeight = horiz_height;
   bmp.imgWidth = horiz_width;
   bmp.memHeight = bmp.imgHeight;
   bmp.memWidth = bmp.imgWidth;
   bmp.data.reserve(bmp.memHeight*bmp.memWidth);
   for (unsigned int i = 0; i <= horiz_width*horiz_height; i++){
      char pix[3];
      HEADER_PIXEL(data,pix);
      unsigned int col = 0xff000000;
      col |= (pix[0] & 0xff) ;
      col |= (pix[1] & 0xff) << 8;
      col |= (pix[2] & 0xff) << 16;
      bmp.data.push_back(col);
   }
   return bmp;
}

// Helper function to create the gradient image at the bottom of the horizon.
RGBA32BitMap createHorizonGradient(const RGBA32BitMap& horizonBitMap){

   RGBA32BitMap gradient;
   gradient.imgWidth = GRADIENT_IMAGE_WIDTH;
   gradient.imgHeight = GRADIENT_IMAGE_HEIGHT;
   gradient.memWidth = gradient.imgWidth;
   gradient.memHeight = gradient.imgHeight;

   // We pick the color at the bottom left corner of the horizon image,
   // assuming that the bottom color of the horizon image is uniform.
   unsigned int topColor = horizonBitMap.data[(horizonBitMap.imgHeight - 1) *
                                              horizonBitMap.memWidth];
   // We are only interested in the color of the pixel,
   // the alpha bits will change along the gradient.
   topColor &= 0x00ffffff;
   
   for (unsigned int row = 0; row < gradient.imgHeight; row++){

      // The current transparency factor.
      float curAlpha = (gradient.imgHeight - row) /
         static_cast<float>(gradient.imgHeight);

      // We want the transparency to change quadratically instead of linearly,
      // so we square the alpha factor.
      curAlpha *= curAlpha;
      
      // The actual alpha value to set to the color.
      unsigned int curAlphaVal = static_cast<unsigned int>(curAlpha * 0xff);

      // We bitwise AND the curAlphaVal before shifting it, just to be sure.
      unsigned int curCol = topColor | ((curAlphaVal & 0xff) << 24);

      // We add this value to every column of the current row.
      for (unsigned int col = 0; col < gradient.imgWidth; col++){
         gradient.data.push_back(curCol);
      }
   }
   
   return gradient;
}

ExternalRenderer::ExternalRenderer( BitMapTextLoader* textLoader,
                                    NativeTextInterface* textInterface,
                                    TextureBlockManager* texManager,
                                    VertexBlockManager* vertManager,
                                    int width, int height ) 
{
   m_impl = new Impl;
   m_impl->drawBlocks = false;
   m_impl->projection = NULL;
   m_impl->textLoader = textLoader;
   m_impl->texManager = texManager;
   m_impl->vertManager = vertManager;
   m_impl->textInterface = textInterface;
   m_impl->backgroundColor = 0;
   m_impl->prevScale = 0;
   m_impl->scaleChecking = true;
   m_impl->preDrawListener = NULL;
   m_impl->swapListener = NULL;
   m_impl->width = width;
   m_impl->height = height;
   m_impl->enableWorldDrawing = true;
   m_impl->mapHandler = NULL;
   m_impl->forcePerspectiveSetup = false;
   m_impl->horizonBitMap = loadHorizonImage(horiz_day_header_data);
   m_impl->horizonTexBlock =
      texManager->allocateFromBitMap(m_impl->horizonBitMap);
   m_impl->gradientBitMap = createHorizonGradient(m_impl->horizonBitMap);
   m_impl->gradientTexBlock = 
      m_impl->texManager->allocateFromBitMap(m_impl->gradientBitMap);

   m_numTriangles = 0;
   m_numDrawPolygonCalls = 0;
   m_numDrawPolylineCalls = 0;

   m_camera = NULL;
   
   uint32 currTime = TileMapUtil::currentTimeMillis();

   m_timingCycle = 10;
   m_lastFrameTime = currTime;
   m_lastExternalTime = currTime;
   m_sumFrameTime = 0;
   m_sumExternalTime = 0;
   m_numFrames = 0;
   m_stringTextureRepresentation = NULL;
   
   featCache.reserve( 1000 );
}

ExternalRenderer::~ExternalRenderer()
{
   delete m_impl->horizonTexBlock;
   delete m_impl->gradientTexBlock;
   delete m_stringTextureRepresentation;
   delete m_impl->texManager;
   delete m_impl->vertManager;
   delete m_impl;
}

void ExternalRenderer::drawLineBlock(TriStorageBlock& block,
                                     unsigned int pass,
                                     vec4x* triangleData,
                                     const MC2BoundingBox& box,
                                     TileMapContainer& tileMapCont,
                                     TileMapFormatDesc& formatDesc,
                                     bool skipOutlines)
{
   if(pass == 0 && skipOutlines) {
      return;
   } 
   if (pass == 1){
      drawPolyLine(triangleData + block.begin,
                   block.size,
                   0,
                   0,
                   pass);
   } else {
      for(std::vector<TriSection>::iterator sectIt = block.primitives.begin();
          sectIt != block.primitives.end(); sectIt++)
      {
         TriSection& section = *sectIt;
         TilePrimitiveFeature& feat = *section.src;

         const CoordsArg* coords = static_cast<const CoordsArg*>(
            feat.getArg(TileArgNames::coords));   

         if ( coords && box.overlaps(coords->getBBox() ) ) {
         
            drawPolyLine(triangleData + section.begin,
                         section.size,
                         section.color1,
                         section.color2,
                         pass);
            feat.setDrawn(true);
         }      
      }         
   }

}

void ExternalRenderer::drawPolygonBlock(TriStorageBlock& block,
                                        unsigned int pass,
                                        vec4x* triangleData,
                                        const MC2BoundingBox& box,
                                        TileMapContainer& tileMapCont,
                                        TileMapFormatDesc& formatDesc,
                                        bool skipOutlines)
{
   if(pass == 1) {
      return;
   }

   // for(std::vector<TriSection>::iterator sectIt = block.primitives.begin();
   //     sectIt != block.primitives.end(); sectIt++){
   //    TriSection& section = *sectIt;
   //    TilePrimitiveFeature& feat = *section.src;

   //    const CoordsArg* coords = static_cast<const CoordsArg*>(
   //       feat.getArg(TileArgNames::coords));   

   //    if ( coords && box.overlaps(coords->getBBox() ) ) {
         
   //       drawPolygon(triangleData + section.begin,
   //                   section.size,
   //                   section.color1);
   //       feat.setDrawn(true);
   //    }      
   // }

   drawPolygon(triangleData + block.begin,
               block.size,0);

}

void ExternalRenderer::drawBitmapBlock(TriStorageBlock& block,
                                       unsigned int pass,
                                       vec4x* triangleData,
                                       const MC2BoundingBox& box,
                                       OverlapDetector<MC2BoundingBox>& detector,
                                       TileMapContainer& tileMapCont,
                                       TileMapFormatDesc& formatDesc,
                                       bool skipOutlines)
{
   if(pass == 0) {
      return;
   }
   
   for(std::vector<TriSection>::iterator sectIt = block.primitives.begin();
       sectIt != block.primitives.end(); sectIt++){
      TriSection& section = *sectIt;
      TilePrimitiveFeature& feat = *section.src;


      const TileFeatureArg* arg = feat.getArg( TileArgNames::coord );
      const CoordArg* coordArg = static_cast<const CoordArg*>( arg );
      MC2Coordinate coord = coordArg->getCoord();
      
      if( box.contains( coord ) ) {
         const StringArg* bitMapName =
            static_cast<const StringArg*> (
               feat.getArg(TileArgNames::image_name) );
         
         if ( bitMapName == NULL ) {
            continue;
         }
         
         TextureBlock* texture = 
            static_cast<TextureBlock*>(
               m_impl->mapHandler->getOrRequestBitMap(
                  bitMapName->getValue() ) );
         
         if ( texture != NULL ) {
            MC2Point position = worldToScreen(coord);
            
            /**
             *   We subtract the anchor position, to position the texture
             *   on the anchor instead of the top left corner. */
            PixelBox overlapBox = texture->boundingBox;
            MC2Point topLeft = overlapBox.getTopLeft();
            MC2Point bottomRight = overlapBox.getBottomRight();
            
            overlapBox.set( topLeft, bottomRight );

            overlapBox.move(
               MC2Point( position.getX(), position.getY() ) );

            bool notOverlapping = true;

            notOverlapping = detector.addIfNotOverlapping( overlapBox );

            if (notOverlapping) {
               drawSprite( texture,
                           position,
                           0.0f,
                           texture->centerOffset,
                           1.0f, 1.0f);
               
               feat.setDrawn( true );
            }
         }
      }
   }
}

void
ExternalRenderer::blockDrawingFunction( const MC2BoundingBox& box,
                                        TileMapContainer& tileMapCont,
                                        TileMapFormatDesc& formatDesc,
                                        bool skipOutlines)
{
   typedef std::vector<TriStorageBlock> BlockVec;
      
   MapsToDrawIt endIt = tileMapCont.endMapsToDraw();
   OverlapDetector<MC2BoundingBox> overlapDetector;

   int numTileMapsDrawn = 0;

   for( int pass = 0; pass < 2; ++pass ) {
      
      for( int level = formatDesc.getMinLevel();
           level <= formatDesc.getMaxLevel();
           ++level ) {

         for( MapsToDrawIt mIt = tileMapCont.beginMapsToDraw();
              mIt != endIt; ++mIt ) {
            TileMap* tileMap = *mIt;

            if ( tileMap == NULL ) {
               continue;
            }
            
            if (m_impl->mapHandler && !m_impl->mapHandler->isTileMapVisible(tileMap)){
               // Consult tilemaphandler to see if this tileMap should be drawn
               // or not.
               continue;
            }

            numTileMapsDrawn++;
            
            bool inPerspective = false;
            TriStorage& tris = tileMap->getGeometryData(level);
            
            for (BlockVec::iterator bIt = tris.blocks.begin();
                 bIt != tris.blocks.end(); bIt++)
            {
               TriStorageBlock& block = *bIt;

               switch(block.type) {
               case TilePrimitiveFeature::line:
                  if (!inPerspective){
                     setupPerspectiveProjection();
                     offsetPerspective(tileMap->getReferenceCoord());
                     inPerspective = true;
                  }
                  drawLineBlock(block, pass, tileMap->getTriangleData(),
                                box, tileMapCont, formatDesc, skipOutlines);
                  break;
               case TilePrimitiveFeature::polygon:
                  if (!inPerspective){
                     setupPerspectiveProjection();
                     offsetPerspective(tileMap->getReferenceCoord());
                     inPerspective = true;
                  }
                  
                  drawPolygonBlock(block, pass, tileMap->getTriangleData(), 
                                   box, tileMapCont, formatDesc, skipOutlines);
                  break;
               case TilePrimitiveFeature::bitmap:
                  if (inPerspective){
                     resetOffsetPerspective();
                     inPerspective = false;
                  }

                  setupOrthographicProjection();
                  drawBitmapBlock(block, pass, tileMap->getTriangleData(),
                                  box, overlapDetector, tileMapCont,
                                  formatDesc, skipOutlines);
                  break;
               }
            }
            if (inPerspective){
               resetOffsetPerspective();
            }
#ifdef DRAW_TILE_BOUNDS
            setupPerspectiveProjection();
            MC2BoundingBox tBox;
            tileMap->getBoundingBox(tBox, formatDesc);
            drawBBox(tBox);
#endif

         }
      }

   }

   // std::cout << "Num tile-maps drawn: " << numTileMapsDrawn << std::endl;
   
   // setupPerspectiveProjection();

   // The following call to drawBBox draws the bounding box supplied from
   // TileMapHandler::plotWhatWeGotUsingExternal. Use this when working on
   // horizon image etc. in 3d mode.
   // drawBBox( box );

   // drawBBox( m_impl->projection->getCenterBox() );
   
   // drawBBox( projection.getOldSchoolBoundingBox() );
   // drawBBox( newBox );
   // 
   // std::cout << "oldBox: " << box << std::endl;
   // std::cout << "newWidth: " <<  newBox.getWidth()
   //           << " newHeight: " << newBox.getHeight() << std::endl;
   // std::cout << "oldWidth: " << box.getWidth()
   //           << " oldHeight: " << box.getHeight() << std::endl;
   // std::pair<float,float> wah = projection.getWidthAndHeight();
   // std::cout << "proj.width: " << static_cast<int>( wah.first )
   //           << " proj.height: " << static_cast<int>( wah.second ) <<
   // std::endl;

   setupOrthographicProjection();
}

void ExternalRenderer::drawMap( const MC2BoundingBox& box,
                                TileMapContainer& tileMapCont,
                                TileMapFormatDesc& formatDesc,
                                bool skipOutlines)
{
   m_numTriangles = 0;
   m_numPolylines = 0;

   // Profiler::EntryHandle secHandle =
   //    profiler.startTiming( "drawMap" );

   // Profiler::EntryHandle camHandle =
   //    profiler.startTiming("cameraSetup");
      
   if( !m_impl->enableWorldDrawing ) {
      return;
   }

   // profiler.endTiming( camHandle );

   // Profiler::EntryHandle drawHandle =
   //    profiler.startTiming("DRAW_FUNC_3");
   
   // if(m_impl->drawBlocks) {
   //    Prof(ExR_dfDraw);
   //    defaultDrawingFunction( box, tileMapCont, formatDesc, skip

   {
      blockDrawingFunction( box, tileMapCont, formatDesc, skipOutlines);
   }

   // profiler.endTiming( drawHandle );
   
   // profiler.endTiming( secHandle );
   
   mc2dbg << "Num rendered triangles: " << getNumRenderedTriangles()
             << endl;
   mc2dbg << "Num polylines: " << m_numPolylines << endl;
}

void ExternalRenderer::getMapSizePixels( isab::Rectangle& rect ) const
{
   rect = isab::Rectangle( 0, 0, m_impl->width, m_impl->height );
}

void ExternalRenderer::toggleScaleChecking()
{
   m_impl->scaleChecking = !m_impl->scaleChecking;
}

bool ExternalRenderer::scaleCheckingOn()
{
   return m_impl->scaleChecking;
}

void ExternalRenderer::setMapHandler(TileMapHandler* mapHandler )
{
   m_impl->mapHandler = mapHandler;
}

void ExternalRenderer::setEnableWorldDrawing( bool enableWorldDrawing )
{
   m_impl->enableWorldDrawing = enableWorldDrawing;
}

void ExternalRenderer::forcePerspectiveSetup()
{
   m_impl->forcePerspectiveSetup = true;
}

void ExternalRenderer::setScreenSize( int width, int height )
{
   m_impl->width = width;
   m_impl->height = height;
   screenSizeChanged();
}

void ExternalRenderer::drawString(const char* utf8String,
                                  const MC2Point& stringPos,
                                  uint32 stringColor ) {

   STRING* textInterfaceString=NULL;
   
   if(m_prevString != utf8String) {
      //generate a new texture for the string, since it's a different one now
      unsigned int r = ( stringColor >> 16) & 0xff;
      unsigned int g = ( stringColor >>  8) & 0xff;
      unsigned int b = ( stringColor >>  0) & 0xff;
   
      textInterfaceString = m_impl->textInterface->createString(utf8String);
      m_impl->textLoader->setStringToDraw(textInterfaceString, 0,
                                          m_impl->textInterface->getStringLength(*textInterfaceString),
                                          r, g, b);
      delete m_stringTextureRepresentation;
      m_stringTextureRepresentation = NULL;
      RGBA32BitMap bmp;
      if(m_impl->textLoader->load( bmp ) == 0) {
         TextureBlockManager* texManager = getTextureBlockManager();
         m_stringTextureRepresentation = texManager->allocateFromBitMap( bmp );
      } else {
         //unable to load the bmp
         m_prevString = NULL;
         if(textInterfaceString != NULL) {
            m_impl->textInterface->deleteString(textInterfaceString);
         }
         return;
      }
   }
   MC2Point centerOffset(0,0); //nothing fancy needed, this will be a straight line
   drawSprite( m_stringTextureRepresentation,
               stringPos,
               0.0,
               centerOffset,
               1.0f, 1.0f );
   if(textInterfaceString != NULL) {
      m_impl->textInterface->deleteString(textInterfaceString);
   }
}

MC2Point ExternalRenderer::worldToScreen(const MC2Coordinate& coord)
{
   MC2Point ret;
   float x, y, z;
   transformf(coord.lon, coord.lat, &x, &y, &z);

   ret.getX() = static_cast<int>(x);
   ret.getY() = static_cast<int>(y);

   return ret;
}

void ExternalRenderer::drawTexts( const std::vector<TextResult*>& textResults )
{
   if( !m_impl->textLoader ) {
      return;
   }

   for( int i = 0; i < (int)textResults.size(); i++ ) {
      const TextResult* tr = textResults[i];
      const isab::Rectangle rect = tr->getStringAsRect();
      
      if ( tr->getTypeOfString() == TileMapNameSettings::on_roundrect ) {
         // Add rectangle background here (blue)
      }

      //    m_plotter->setFont( tr->getFontName(), tr->getFontSize() );
      
      //    unsigned int r;
      //    unsigned int g;
      //    unsigned int b;
      //    tr->getFontColor( r, g, b );
      //    m_plotter->setPenColor( r, g, b );

      for( int j = 0; j < (int)tr->getTPN().size(); j++ ) {
         const TextPlacementNotice& tpn = tr->getTPN()[j];
         MC2Point position = worldToScreen(tpn.m_worldCoord);

         unsigned int r;
         unsigned int g;
         unsigned int b;
         tr->getFontColor( r, g, b );
         
         if(tpn.m_texRepresentation == NULL) {
            if ( tr->getTypeOfString() == TileMapNameSettings::on_roundrect ) {
               m_impl->textLoader->setStringToDraw( &tr->getString(),
                                                    tpn.m_startIndex,
                                                    tpn.m_endIndex,
                                                    r, g, b , true);
               
            } else {
               m_impl->textLoader->setStringToDraw( &tr->getString(),
                                                    tpn.m_startIndex,
                                                    tpn.m_endIndex,
                                                    r, g, b );
               
            }

            RGBA32BitMap bmp;
            
            if(m_impl->textLoader->load( bmp ) == 0) {
         
               TextureBlockManager* texManager = getTextureBlockManager();
               tpn.m_texRepresentation = texManager->allocateFromBitMap( bmp );               
            } else {
               continue;
            }
         }
         
         PixelBox overlapBox = tpn.m_texRepresentation->boundingBox;

         overlapBox.move( MC2Point( position.getX(),
                                    position.getY() ) );


         float angle = 0.0f;
         
         if(tpn.m_lineBeginCoord.isValid()) {
            MC2Point topLeft = worldToScreen(tpn.m_lineBeginCoord);
            MC2Point topRight = worldToScreen(tpn.m_lineEndCoord);
            
            double xDiff = topLeft.getX() - topRight.getX();
            double yDiff = topLeft.getY() - topRight.getY();
         #ifdef __SYMBIAN__
            TReal atanRes;
            Math::ATan( atanRes, yDiff, xDiff );
         #else
            double atanRes = atan2( yDiff, xDiff );
          #endif
            angle = (int16)( ( atanRes / M_PI ) * 180 );
         } else {
            angle = -tpn.m_angle;            
         }
         
         drawSprite( tpn.m_texRepresentation,
                     position,
                     angle,
                     tpn.m_texRepresentation->centerOffset,
                     1.0f, 1.0f );
            
         // drawPixelBox( overlapBox,
         //               0.0f, 0.0f, 0.0f,
         //               tpn.m_texRepresentation->centerOffset,
         //               1.0f,
         //               -tpn.m_angle );

         //    m_plotter->drawText( tr->getString(), tpn.m_point,
         //                         tpn.m_startIndex, tpn.m_endIndex, tpn.m_angle );
      }
   }
}

TextureBlockManager* ExternalRenderer::getTextureBlockManager()
{
   return m_impl->texManager;
}

VertexBlockManager* ExternalRenderer::getVertexBlockManager()
{
   return m_impl->vertManager;
}

void ExternalRenderer::beginFrame()
{
   MC2Point p = m_impl->projection->getScreenSize();

   if(p.getX() != m_impl->width || p.getY() != m_impl->height) {
      setScreenSize(p.getX(), p.getY());
   }
   
   uint32 currTime = TileMapUtil::currentTimeMillis();

   if(m_impl->preDrawListener) {
      m_impl->preDrawListener->initializeDrawing();      
   }

   /** END ROUNDTRIP TIMER **/
   m_sumExternalTime += ( currTime - m_lastExternalTime );
   /** START FRAME TIMER **/
   m_lastFrameTime = currTime;
   
   initializeDrawing();
   clearScreen();
   // setClippingRectangle(0, 0, 200, 200);
}

void ExternalRenderer::endFrame()
{   
   doLastMinuteStuff();
}

unsigned int ExternalRenderer::getBackgroundColor()
{
   return m_impl->backgroundColor;
}

void ExternalRenderer::setBackgroundColor( unsigned int rgbColor )
{
   m_impl->backgroundColor = rgbColor;
   backgroundColorChanged();
}


bool ExternalRenderer::atEndOfType( TileMap::primVect_t::iterator curIt,
                                    TileMap::primVect_t::iterator theEnd,
                                    int type)
{
   return ( curIt == theEnd || (*curIt)->getType() != type );
}

void
ExternalRenderer::setAdditionalData( MC2SimpleString& additionalData ) {
   m_impl->additionalData = additionalData;
}

void ExternalRenderer::setSwapListener( SwapListener* swapListener )
{
   m_impl->swapListener = swapListener; 
}

void ExternalRenderer::setPreDrawListener( PreDrawListener* preDrawListener )
{
   m_impl->preDrawListener = preDrawListener; 
}

void ExternalRenderer::drawHorizon( float height )
{
#if defined (_WIN32_WCE)
   return;
#endif
   
   // If the height of the horizon is zero, nothing should be drawn.
   if(height == 0.0){
      return;
   }
   
   const RGBA32BitMap& horizBmp = m_impl->horizonBitMap;

   // The amount of horizon images needed to cover the screen.
   unsigned int amountToCoverScreen =
      static_cast<unsigned int>(
         ceil(static_cast<double>(m_impl->width) / horizBmp.imgWidth));

   // The actual width of the amount needed to cover the screen.
   unsigned int setWidth = amountToCoverScreen * horizBmp.imgWidth;

   // The angle used to offset the horizon images
   double angle = m_camera->getWorldAngle();

   // The current height offset that the horizon images should be positioned at.
   int heightOffs =
      static_cast<int>(height * m_impl->height - horizBmp.imgHeight);

   // The pixel offset that corresponds to the current angle offset.
   int angleOffs =
      static_cast<int>(-angle * setWidth * HORIZON_IMAGE_SETS / 360.0);
   // To wrap around, the angleOffset
   // is modified by the modulus of the setWidth.
   angleOffs = angleOffs % setWidth;
   
   // PRINT_VAL(angleOffs);
   // PRINT_VAL(amountToCoverScreen);
   // PRINT_VAL(horizBmp.imgWidth);
   // Iterate through the amount of image sets.

   for (unsigned int i = 0; i < HORIZON_IMAGE_SETS; i++){
      // For every set, iterate through the amount of images needed
      // to cover the screen.
      for (unsigned int j = 0; j < amountToCoverScreen; j++){

         // The x offset for the current image is a composite of the
         // current image set position (setWidth * i), the current image
         // position relative the set position (horizBmp.imgWidth * j)
         // and finally the angleOffset. 
         // The height will be the same for all images (heightOffs).
         MC2Point pos(-setWidth * i +
                      horizBmp.imgWidth * j +
                      angleOffs,
                      heightOffs );

         drawSprite(m_impl->horizonTexBlock,
                    pos,
                    0.0f,
                    MC2Point(0, 0));
      }
   }

   // To avoid the sharp edge of the horizon image, we add a small gradient
   // below the horizon line.

   MC2Point gradientPosition =
      MC2Point(0, heightOffs + m_impl->horizonBitMap.imgHeight);

   // We need to scale the gradient image to cover the whole
   // width of the screen.
   float xScale = m_impl->width / static_cast<float>(GRADIENT_IMAGE_WIDTH);
   
   // We also want the height of the gradient to scale according to the visible
   // height of the horizon. It should also be clamped to be smaller or equal 
   // to GRADIENT_MAX_HEIGHT.
   float yScale = std::min(m_impl->height * height,
                           static_cast<float>(GRADIENT_MAX_HEIGHT))
      / GRADIENT_IMAGE_HEIGHT;

   // Finally we draw the image with the correct scaling factors.
   drawSprite(m_impl->gradientTexBlock,
              gradientPosition,
              0.0f,
              MC2Point( 0, 0),
              xScale, yScale);
   
}

void ExternalRenderer::drawImageSpec(const WFAPI::ImageSpec* imgSpec,
                                     const MC2Point& point)
{  
   const ImageSpecInternal* spec = static_cast<const ImageSpecInternal*>(imgSpec);
   if(spec->m_texBlock == NULL) {
      coreprintln("No tex block, loading");
      RGBA32BitMap bmp;
      spec->load(bmp);
      spec->m_texBlock = m_impl->texManager->allocateFromBitMap(bmp);

      if(!spec->m_texBlock) {
         coreprintln("Failed to load texBlock, not drawing image.");
         return;
      }
   } 

   PixelBox overlapBox = spec->m_texBlock->boundingBox;

   overlapBox.move(point);
   
   drawSprite(spec->m_texBlock,
              point,
              0.0f,
              MC2Point(0, 0));      
   // drawPixelBox(overlapBox,
   //              0.0f, 0.0f, 0.0f,
   //              MC2Point(0, 0), 1.0f, 0.0f);
}

void ExternalRenderer::setNightMode(bool enabled)
{
   delete m_impl->horizonTexBlock;
   m_impl->horizonTexBlock = NULL;
   delete m_impl->gradientTexBlock;
   m_impl->gradientTexBlock = NULL;

   if (enabled){
      m_impl->horizonBitMap = loadHorizonImage(horiz_night_header_data);
      m_impl->horizonTexBlock =
         m_impl->texManager->allocateFromBitMap(m_impl->horizonBitMap);
      m_impl->gradientBitMap = createHorizonGradient(m_impl->horizonBitMap);
      m_impl->gradientTexBlock =
         m_impl->texManager->allocateFromBitMap(m_impl->gradientBitMap);
   } else {
      m_impl->horizonBitMap = loadHorizonImage(horiz_day_header_data);
      m_impl->horizonTexBlock =
         m_impl->texManager->allocateFromBitMap(m_impl->horizonBitMap);
      m_impl->gradientBitMap = createHorizonGradient(m_impl->horizonBitMap);
      m_impl->gradientTexBlock =
         m_impl->texManager->allocateFromBitMap(m_impl->gradientBitMap);
      
   }
}

void ExternalRenderer::setCamera(const Camera* camera)
{
   m_camera = camera;
}

void ExternalRenderer::cropString(STRING& text,
                                  unsigned int maxWidth)
{
   
}

void ExternalRenderer::drawOverlayText(
   const WFAPI::WFString& text,
   TextCachingBlock& cacheBlock,
   const WFAPI::OverlayItemVisualSpec::TextAlignment alignment,
   WFAPI::FontSpec* font,
   const isab::Rectangle& stringRect,
   const WFAPI::OverlayItemVisualSpec::RGB& textColor)
{
   // Update the block if necessary
   if(cacheBlock.external.texBlock && text != cacheBlock.source)
   {
      delete cacheBlock.external.texBlock;
      cacheBlock.external.allocator = NULL;
      cacheBlock.external.texBlock = NULL;
   }
   
   if(!cacheBlock.external.texBlock) {
      cacheBlock.source = text;
      STRING* tmp = m_impl->textInterface->createString(text.c_str());
      
      m_impl->textLoader->setStringToDraw( tmp,
                                           0,
                                           strlen(text.c_str()),
                                           0, 0, 0 );

      RGBA32BitMap bmp;
            
      m_impl->textLoader->load( bmp );
         
      TextureBlockManager* texManager = getTextureBlockManager();

      cacheBlock.external.texBlock = texManager->allocateFromBitMap( bmp );
      cacheBlock.external.allocator = texManager;
      
      m_impl->textInterface->deleteString(tmp);
   }

   // PixelBox overlapBox = textSprite.texture->boundingBox;

   // overlapBox.move( MC2Point( textSprite.position.getX(),
   //                            textSprite.position.getY() ) );
   

   // overlapBox.move( MC2Point( textSprite.position.getX(),
   //                            textSprite.position.getY() ) );
   
   // textSprite.position = MC2Point(0, 0);

   // drawPixelBox( overlapBox,
   //               0.0f, 0.0f, 0.0f,
   //               MC2Point(0, 0), 1.0f, 0.0f );
   
   drawSprite( cacheBlock.external.texBlock,
               MC2Point(0, 0), //Fix this later.
               0.0f,
               cacheBlock.external.texBlock->centerOffset,
               1.0f );
}

void ExternalRenderer::setProjection(MapProjection* projection)
{
   m_impl->projection = projection; 
}

void ExternalRenderer::calcOffsetCenterCoord(const MC2Point& lockPoint,
                                             const MC2Coordinate& lockCoord,
                                             MC2Coordinate& coord)
{
   MC2Coordinate lockPointCoord;
   if(!inverseTransform(lockPointCoord, lockPoint)) {
      // std::cout << "InverseTransform FAILED" << std::endl;
   }
   
   int latDiff = lockCoord.lat -lockPointCoord.lat;
   int lonDiff = lockCoord.lon -lockPointCoord.lon;
   
   MC2Coordinate center = m_camera->getFocalCoordinate();

   coord.lat = center.lat + latDiff;

   double newCosLat = m_impl->projection->getCosLat(coord.lat);
   double oldCosLat = m_impl->projection->getCosLat();
   
   
   coord.lon = center.lon + static_cast<int>(lonDiff * (oldCosLat / newCosLat));
}

void ExternalRenderer::getBitMapAsRectangle(isab::Rectangle& size,
                                            const MC2Coordinate& center,
                                            const isab::BitMap* bmp)
{
   const TextureBlock* texture =
      static_cast<const TextureBlock*>(bmp);
   
   MC2Point position = worldToScreen(center);

   /**
    *   We subtract the anchor position, to position the texture
    *   on the anchor instead of the top left corner. */

   PixelBox overlapBox = texture->boundingBox;
   MC2Point topLeft = overlapBox.getTopLeft();
            
   topLeft.getX() *= m_camera->getDPICorrectionFactor();
   topLeft.getY() *= m_camera->getDPICorrectionFactor();

   MC2Point bottomRight = overlapBox.getBottomRight();
   bottomRight.getX() *= m_camera->getDPICorrectionFactor();
   bottomRight.getY() *= m_camera->getDPICorrectionFactor();

   overlapBox.set( topLeft, bottomRight );
   overlapBox.move( MC2Point( position.getX(), position.getY() ) );
   
   overlapBox.move(MC2Point(0, 0) - texture->centerOffset);
   
   size = overlapBox;
}

void ExternalRenderer::toggleDrawingFunction()
{
   m_impl->drawBlocks = !m_impl->drawBlocks;
}

void ExternalRenderer::triggerApplicationRedraw()
{
   if( !m_impl->swapListener ) {
      return;
   }
   
   uint32 currTime = TileMapUtil::currentTimeMillis();

   /** END FRAME TIMER **/
   m_sumFrameTime += ( currTime - m_lastFrameTime );

   m_numFrames++;
   
   /** START ROUNDTRIP TIMER **/
   m_lastExternalTime = currTime;
   
   m_impl->swapListener->doSwap();
   
}

WFAPI::ImageSpec* ExternalRenderer::loadImageSpecFromRGBA(const pal::wf_uint8* buffer,
                                                          pal::wf_uint32 widthPixels,
                                                          pal::wf_uint32 heightPixels)
{
   HollowImageSpec* spec =
      new HollowImageSpec(widthPixels, heightPixels);
   
   spec->m_texBlock = m_impl->texManager->allocateFromRawData(
      buffer,
      widthPixels,
      heightPixels);
   
   return spec;
}

