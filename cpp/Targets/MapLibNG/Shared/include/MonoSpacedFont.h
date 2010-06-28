/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef MONOSPACEDFONT_H
#define MONOSPACEDFONT_H

#include "BitMapTypes.h"
#include "Vector3d.h"

class MonoSpacedFont {
public:
   MonoSpacedFont();
   
   const RGBA32BitMap& getTexture();

   enum Constant { MAX_STRING_LENGTH = 64,
                   NUM_CHARS = 256,
                   CHAR_HEIGHT = 16,
                   CHAR_WIDTH = 8 };


   const vbuf3x& getVertexCoordinates();
   
   void fillTextureCoords(const char* c, vbuf3x& texCoords);

   int getCharHeight() const;

   int getCharWidth() const;

   bool supportsPlatform() const;
private:
   /**
    * Our vertex and texture coord sources. The vertex coords are
    * always the same since we only draw a single line. We do not
    * need to explicitly specify the world positions either since
    * we translate the text to the correct position.
    */ 
   vbuf3x m_vertCoords;
   vbuf3x m_texCoords;

   void fillVertCoords();

   void fillTexCoordLookup();
   
   RGBA32BitMap m_bitMap;
};

#endif // MONOSPACEDFONT_H
