/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef TEXTPLACEMENTDATA_H
#define TEXTPLACEMENTDATA_H

#include "MapPlotterConfig.h"
#include "TileMapCoord.h"

template<class T> class VectorProxy;
class TileFeature;
class TilePrimitiveFeature;
class CoordsArg;
class MC2SimpleString;

/**
 *  This structure holds the data necessary to delegate
 *  placement and actually place text for a given feature.
 */
struct TextPlacementDelegator::TextPlacementData {

   /**
    *  @param type The type of the primitive to place text for.
    *  @param nameOfFeature Platform specific string representation of
    *                       feature.
    *  @param feature   The parent feature.
    *  @param primitive The primitive feature.
    *  @param pointsInFeature Contains the screen space coordinates for
    *                         the feature shape.
    *  @param coords Contains the world space coordinates for the feature
    *                shape.   
    *
    */ 
   TextPlacementData(uint16 type,
                     STRING* nameOfFeature,
                     MC2SimpleString& mc2String,
                     TileFeature* feature,
                     TilePrimitiveFeature* primitive,
                     const VectorProxy<MC2Point>& pointsInFeature,
                     TileMapCoords::const_iterator coordsBegin) :
      type(type),
      nameOfFeature(nameOfFeature),
      mc2String(mc2String),
      feature(feature),
      primitive(primitive),
      pointsInFeature(pointsInFeature),
      coordsBegin(coordsBegin),
      polyLineWidth(0)
      {}

   /// @see TextPlacementData()
   uint16 type;

   /// @see TextPlacementData()
   STRING* nameOfFeature;

   /// @see TextPlacementData()
   MC2SimpleString& mc2String;

   /// @see TextPlacementData()
   TileFeature* feature;

   /// @see TextPlacementData()
   TilePrimitiveFeature* primitive;

   /// @see TextPlacementData()
   const VectorProxy<MC2Point>& pointsInFeature;

   /// @see TextPlacementData()
   TileMapCoords::const_iterator coordsBegin;
      
   /// This should be set only for polylines. Defaults to 0.
   uint32 polyLineWidth;
};


#endif /* TEXTPLACEMENTDATA_H */
