/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "TextPlacement/TextPlacementPolicy.h"
#include "TextPlacement/TextPlacementData.h"
#include "TextPlacement/TextResult.h"
#include "Util/StringCache.h"
#include "ViewPortInterface.h"
#include "NativeTextInterface.h"
#include "OverlapDetector.h"
#include "MapProjection/MapProjection.h"
#include "TileFeature.h"
#include "TileMapTextSettings.h"
#include "InsideUtil.h"

TextPlacementDelegator::TextPlacementPolicy::TextPlacementPolicy(
   NativeTextInterface* textInterface,
   ViewPortInterface* viewPort,
   TileMapTextSettings& textSettings,
   const MapProjection& mapProjection,
   StringCache& stringCache,
   OverlapDetector<PixelBox>& overlapDetector) :
   
   m_textInterface(textInterface), 
   m_viewPort(viewPort),
   m_textSettings(textSettings),
   m_mapProjection(mapProjection),
   m_stringCache(stringCache),
   m_overlapDetector(overlapDetector)
{
   resetViewport();   
}

void TextPlacementDelegator::TextPlacementPolicy::resetViewport()
{
   isab::Rectangle r;
   
   if(m_viewPort) {
      m_viewPort->getMapSizeDrawingUnits( r );
      m_screenCoords = r;      
   } else {
      m_screenCoords = PixelBox( MC2Point(0, 0),
                                 MC2Point(0, 0) );
   } 
}

void
TextPlacementDelegator::TextPlacementPolicy::setTextInterface(
   NativeTextInterface* textInterface)
{
   m_textInterface = textInterface;
}

void
TextPlacementDelegator::TextPlacementPolicy::setViewPortInterface(
   ViewPortInterface* viewPort)
{
   m_viewPort = viewPort;
   resetViewport();
}

MC2Coordinate&
TextPlacementDelegator::TextPlacementPolicy::toWorld(
   MC2Coordinate& dest, const MC2Point& src)
{
   m_mapProjection.inverseTranformCosLatSupplied( dest,
                                                  src.getX(),
                                                  src.getY(),
                                                  m_mapProjection.getCosLat(
                                                     m_mapProjection.getCenter().lat ) );
   return dest;
}

int
TextPlacementDelegator::TextPlacementPolicy::placeHorizontal(
   TextPlacementData& data, TextResultVec& output)
{
   // For city centers use the first point in the feature.
   MC2Point posOfString ( data.pointsInFeature.front().getX(), 
                          data.pointsInFeature.front().getY() + 
                          c_heightDisplacmentOftextOverCityCenter );
   
   TextResult* tr = new TextResult();
   
   const STRING* font = m_stringCache.getOrCreateString(
      m_textSettings.getHorizontalFont().first );

   uint32 textColor = m_textSettings.getTextColor();
   
   tr->setString( data.nameOfFeature );
   tr->setFontName( font );
   tr->setFontColor( (textColor >> 16 ) & 0xff,
                     (textColor >> 8 ) & 0xff,
                     (textColor >> 0 ) & 0xff );
   tr->setTypeOfString( TileMapNameSettings::horizontal );
   tr->setFontSize( m_textSettings.getHorizontalFont().second );
   m_textInterface->setFont( *font, tr->getFontSize() );

   isab::Rectangle stringRectangle( m_textInterface->getStringAsRectangle( 
                                       (*data.nameOfFeature), posOfString));

   MC2Coordinate worldCoord;
   
   TextPlacementNotice tpn( posOfString,
                            toWorld(worldCoord, posOfString),
                            0, -1, 0,
                            MC2Coordinate(),
                            MC2Coordinate() );
   
   // Add the TextPlacementNotice to the TextResult.
   tr->addTPN( tpn );
   
   if( PixelBox( stringRectangle ).inside( m_screenCoords ) && 
       m_overlapDetector.addIfNotOverlapping( stringRectangle ) )
   {
      output.push_back( tr );
      return 1;
   } else {
      delete( tr );
      return 0;
   }
}

int
TextPlacementDelegator::TextPlacementPolicy::placeOnRoundRect(
   TextPlacementData& data, TextResultVec& output)
{
   MC2Point posOfString ( data.pointsInFeature.front() );
   
   if( posOfString.getX() == 0 && posOfString.getY() == 0 ){
      return 0;
   }
   // Collect and send the results to our textresults vector.
   const STRING* font =
      m_stringCache.getOrCreateString(
         m_textSettings.getRoundRectFont().first );
   
   TextResult* tr = new TextResult();

   tr->setString( data.nameOfFeature );
   tr->setFontName( font );
   tr->setFontColor( 255, 255, 255 );
   tr->setTypeOfString( TileMapNameSettings::on_roundrect );
   tr->setFontSize( m_textSettings.getRoundRectFont().second );  
   
   m_textInterface->setFont( *font, tr->getFontSize() );
   isab::Rectangle stringRectangle( 
      m_textInterface->getStringAsRectangle(
         (*data.nameOfFeature), posOfString));
   int border_size = 3;
   int x1 = stringRectangle.getX() - border_size;
   int x2 = stringRectangle.getWidth() + ( border_size * 2 );
   int y1 = stringRectangle.getY() - border_size;
   int y2 = stringRectangle.getHeight() + ( border_size * 2 );
   stringRectangle = isab::Rectangle( x1, y1, x2, y2 );
   tr->setStringAsRect ( stringRectangle );

   MC2Coordinate worldCoord;
   
   TextPlacementNotice tpn( posOfString,
                            // toWorld returns worldCoord after converting.
                            toWorld(worldCoord, posOfString),
                            0, -1, 0,
                            MC2Coordinate(), MC2Coordinate());

   // Add the TextPlacementNotice to the TextResult.
   tr->addTPN( tpn );

   if( PixelBox( stringRectangle ).inside( m_screenCoords ) &&
       m_overlapDetector.addIfNotOverlapping( stringRectangle ) )
   {
      output.push_back( tr );
      return 1;
   } else {
      delete tr;
      return 0;
   }
}

int
TextPlacementDelegator::TextPlacementPolicy::placeInsidePolygon(
   TextPlacementData& data, TextResultVec& output)
{
   int max_x = 0; 
   int max_y = 0;
   int min_x = m_screenCoords.getMaxLon();
   int min_y = m_screenCoords.getMaxLat();
   for( int i = 0; i < (int)data.pointsInFeature.size(); i++ ) {
      int x = data.pointsInFeature[i].getX();
      int y = data.pointsInFeature[i].getY();
      if( x > max_x ) { max_x = x; }
      if( y > max_y ) { max_y = y; }
      if( x < min_x ) { min_x = x; }
      if( y < min_y ) { min_y = y; }
   }
    
   if( max_x > m_screenCoords.getMaxLon() || 
       max_y > m_screenCoords.getMaxLat() ||  
       min_x < m_screenCoords.getMinLon() || 
       min_y < m_screenCoords.getMinLat() )
      return 0;
  
   MC2Point posOfString( ( min_x + max_x ) / 2,
                         ( min_y + max_y ) / 2 );
   
   if( !InsideUtil::inside(
          data.pointsInFeature.begin(),
          data.pointsInFeature.end(),
          posOfString ) ) {
      return 0;
   }
   
   if( posOfString.getX() == 0 && posOfString.getY() == 0 ){
      return 0;
   }
  
   // Collect and send the results to our textresults vector.
   TextResult* tr = new TextResult();
   tr->setString( data.nameOfFeature );
   const STRING* font =
      m_stringCache.getOrCreateString(m_textSettings.getInsidePolygonFont().first);

   uint32 textColor = m_textSettings.getTextColor();
   
   tr->setFontName( font );
   tr->setFontColor( (textColor >> 16 ) & 0xff,
                     (textColor >> 8 ) & 0xff,
                     (textColor >> 0 ) & 0xff );
   tr->setTypeOfString( TileMapNameSettings::inside_polygon );
   tr->setFontSize( m_textSettings.getInsidePolygonFont().second );
   
   m_textInterface->setFont( *font, tr->getFontSize() );
   
   PixelBox stringRectangle =
      m_textInterface->getStringAsRectangle( *data.nameOfFeature, posOfString);

   MC2Coordinate worldCoord;
   
   // Add the TextPlacementNotice to the TextResult.

   TextPlacementNotice tpn( posOfString,
                            // toWorld returns worldCoord
                            toWorld(worldCoord, posOfString), 
                            0, -1, 0,
                            MC2Coordinate(), MC2Coordinate());
   
   
   tr->addTPN( tpn );

   if( stringRectangle.inside( m_screenCoords ) &&
       m_overlapDetector.addIfNotOverlapping( stringRectangle ) )
   {
      output.push_back( tr );
      return 1;
   } else {
      delete tr;
      return 0;
   }
}



