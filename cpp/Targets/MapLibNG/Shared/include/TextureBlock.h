/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef TEXTUREBLOCK_H
#define TEXTUREBLOCK_H

#include "config.h"
#include "MC2Point.h"
#include "PixelBox.h"
#include "BitMapTypes.h"

/**
 *    Base class for platform dependant texture information.
 *    This class stores the common attributes needed for positioning.
 *    Please note that in order to actually use these texture blocks
 *    with any kind of renderer, you will need to instantiate a derived
 *    class which is specific to the renderer you intend to use.
 *
 */

class TextureBlockManager;

class TextureBlock : public isab::BitMap {
public:
   virtual ~TextureBlock();
   
   /**
    *   Assuming the texture has a coordinate system with 0,0 in
    *   the top left corner, the anchor indicates the point which
    *   the image should be centered upon.
    */
   
   MC2Point centerOffset;

   /**
    *   The boundingBox member contains pixel coordinates for the
    *   bounding box surrounding the "collidable" area of the texture.
    *   The coordinates are defined relative to the anchor point.
    */
   
   PixelBox boundingBox;
protected:
   /**
    *   Protected constructor to ensure that we only instantiate
    *   derived classes directly.
    */
   
   TextureBlock( MC2Point centerOffset,
                 PixelBox boundingBox,
                 TextureBlockManager* parent)
      : centerOffset( centerOffset ),
        boundingBox( boundingBox ),
        m_parent( parent )
      {}

private:
   TextureBlockManager* m_parent;
};

#endif /* TEXTUREBLOCK_H */
