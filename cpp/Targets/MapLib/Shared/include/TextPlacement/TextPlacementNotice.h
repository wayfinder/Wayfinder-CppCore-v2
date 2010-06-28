/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef TEXTPLACEMENTNOTICE_H
#define TEXTPLACEMENTNOTICE_H

class MC2Point;
class TextureBlock;
class MC2Coordinate;

#include "config.h"
#include "MC2Coordinate.h"
#include "MC2Point.h"

/**
 *    A container that holds information needed for rendering a 
 *    string.
 */
struct TextPlacementNotice {

   TextPlacementNotice( const MC2Point& point,
                        const MC2Coordinate& worldCoordinate,
                        int startIndex, 
                        int endIndex, 
                        uint16 angle,
                        const MC2Coordinate& lineBeginCoord,
                        const MC2Coordinate& lineEndCoord);

   ~TextPlacementNotice();
   
   /// The position on the screen.
   MC2Point m_point;

   /// The angle.
   uint16 m_angle;

   /// Start index in string.
   int m_startIndex;

   /// End index in string.
   int m_endIndex;

   /// The world coordinate - used to update when screen moved
   MC2Coordinate m_worldCoord;

   MC2Coordinate m_lineBeginCoord;
   MC2Coordinate m_lineEndCoord;
   
   // The texture representation is an external entity
   // which does not affect the internal state of the
   // object. It can be seen as a caching mechanism.
   // To be able to handle this object in different
   // contexts, we make it mutable.
   
   mutable TextureBlock* m_texRepresentation;
};

#endif /* TEXTPLACEMENTNOTICE_H */
