/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/



#include "config.h"
#include "WGS84Coordinate.h"
#include "MC2Coordinate.h"
#include "GfxConstants.h"
#include <math.h>

// -- Nav2Coordinate
Nav2Coordinate::Nav2Coordinate( const MC2Coordinate& realCoord ) {
   if ( realCoord.isValid() ) {
      nav2lat = 
         int32(realCoord.lat * GfxConstants::invRadianFactor * 100000000 );
      nav2lon = 
         int32(realCoord.lon * GfxConstants::invRadianFactor * 100000000 );
   } else {
      nav2lat = MAX_INT32;
      nav2lon = MAX_INT32;
   }
}

Nav2Coordinate::Nav2Coordinate( const WFAPI::WGS84Coordinate& coord )
{
   MC2Coordinate mc2Coord = MC2Coordinate(coord);
   Nav2Coordinate nav2Coord( mc2Coord);
   nav2lat = nav2Coord.nav2lat;
   nav2lon = nav2Coord.nav2lon;
}
      
Nav2Coordinate::operator WFAPI::WGS84Coordinate() const
{
   return WFAPI::WGS84Coordinate(MC2Coordinate(*this));
}


// -- MC2Coordinate

//static const MC2Coordinate
//MC2Coordinate::invalidCoordinate( MAX_INT32, MAX_INT32 );

#ifdef MC2_SYSTEM
const MC2Coordinate
MC2Coordinate::invalidCoordinate( MAX_INT32, MAX_INT32 );
#endif

MC2Coordinate::MC2Coordinate( const WFAPI::WGS84Coordinate& coord )
{
   MC2Coordinate mc2Coord;

   const double degreeFactor = 11930464.7111; // 2^32/360
   
   float64 addLat = ( (coord.latDeg < 0) ? -1: 1) * 0.5;
   float64 addLon = ( (coord.lonDeg < 0) ? -1: 1) * 0.5;
   
   float64 mc2LatFloat = coord.latDeg * degreeFactor + addLat;
   float64 mc2LonFloat = coord.lonDeg * degreeFactor + addLon;
  
   while ( mc2LonFloat > float( MAX_INT32 ) ) {
      mc2LonFloat -= MAX_UINT32;
   }
   while ( mc2LonFloat < float( MIN_INT32 ) ) {
      mc2LonFloat += MAX_UINT32;
   }
   
   lat = int( mc2LatFloat );
   lon = int( mc2LonFloat );
}

MC2Coordinate::MC2Coordinate( const Nav2Coordinate& nav2 ) {
   if ( nav2.nav2lat != MAX_INT32 && uint32(nav2.nav2lat) != MAX_UINT32 ) {
      lat = int32( nav2.nav2lat * GfxConstants::radianFactor / 100000000 );
      lon = int32( nav2.nav2lon * GfxConstants::radianFactor / 100000000 );
   } else {
      lat = MAX_INT32;
      lon = MAX_INT32;
   }
}

MC2Coordinate::operator WFAPI::WGS84Coordinate() const {
   
   const double invDegreeFactor = 1.0 / 11930464.7111;
   
   double latDeg = lat * invDegreeFactor;
   double lonDeg = lon * invDegreeFactor;

   return WFAPI::WGS84Coordinate( latDeg, lonDeg );
}

MC2Coordinate::MC2Coordinate( const MercatorCoord& merc ) 
{
   double latRad = double(merc.lat * GfxConstants::invRadianFactor);
   double lonRad = double(merc.lon * GfxConstants::invRadianFactor);
  
   double latRadMC2 = atan( sinh( double( latRad ) ) );
   double lonRadMC2 = double( lonRad );

   double latMC2 = double( latRadMC2 * GfxConstants::radianFactor );
   double lonMC2 = double( lonRadMC2 * GfxConstants::radianFactor );
   
   int32 latResult = int32( rint( latMC2 ) );
   int32 lonResult = int32( rint( lonMC2 ) );

   lat = latResult;
   lon = lonResult;

//   mc2dbg << "[MC2Coord]: Merc: << " << merc << " -> MC2: " << *this << endl;
//   mc2dbg << "[MC2Coord]: Double check:" << endl;
//   MercatorCoord tmp( *this );
//   tmp = tmp;
}

MercatorCoord::MercatorCoord() :
   lat(MAX_INT32), lon(MAX_INT32)
{

}

MercatorCoord::MercatorCoord( const MC2Coordinate& mc2Coord )
{
   // In radians
   double latRad = double(mc2Coord.lat * GfxConstants::invRadianFactor);
   double lonRad = double(mc2Coord.lon * GfxConstants::invRadianFactor);

   double maxLatRad = 85.0 / 180 * M_PI;
   
   // Truncate at maximum upper latitude.
   if ( latRad > maxLatRad ) {
      latRad = maxLatRad;
   }
  
  // Truncate at minimum lower latitude.
   if ( latRad < -maxLatRad ) {
      latRad = -maxLatRad;
   }
   
   
//   double maxY = log( tan( maxLatRad ) + double( 1.0 / cos( maxLatRad ) ) );
//   double maxX = M_PI; 

   double xRad = lonRad;
   double yRad = log( tan( latRad ) + double( 1.0 / cos( latRad ) ) );

   double xScaled = xRad * GfxConstants::radianFactor;
   double yScaled = yRad * GfxConstants::radianFactor;

   int32 xResult = int32( rint( xScaled ) );
   int32 yResult = int32( rint( yScaled ) );

//   mc2dbg << "[Merc]: MC2: " << mc2Coord << ", WGS84 Rad: " << latRad << ", " << lonRad
//          << ", Merc Rad: " << xRad << ", " << yRad
//          << ", Merc scaled: " << xResult << ", " << yResult << endl;
     
   lat = yResult;
   lon = xResult;
   
//   mc2dbg << "[MercCoord]: MC2: << " << mc2Coord << " -> Merc: " << *this << endl;
//   mc2dbg << "[MercCoord]: Double check:" << endl;
//   MC2Coordinate tmp( *this );
//   tmp = tmp;
}

