/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef TEXTCACHINGBLOCK_H
#define TEXTCACHINGBLOCK_H

class TextureBlockManager;
class TextureBlock;

#include "WFString.h"
#include "MapPlotterTypes.h"

namespace isab {
class MapPlotter;
}

/**
 * We create a cached string representation to use when drawing
 * the item, since we will surely draw it more often that it is updated
 * by the user.
 */
class TextCachingBlock {
public:
   
   TextCachingBlock();

   ~TextCachingBlock();

   WFAPI::WFString source;

   /// The string rectangle to draw in.
   isab::Rectangle sourceRect;
   
   struct  {      
      /// The allocator used to create the cached strings.
      isab::MapPlotter* stringAllocator;
   
      /// A string containing the name of the item.
      STRING* nameString;
   } plotter;

   struct {
      /// The allocator for the texture block, used w/ external renderers
      TextureBlockManager* allocator;

      /// The texture block that is a rasterized version of the string
      TextureBlock* texBlock;
   } external;

   /**
    * Release data for the text caching block.
    */
   void releaseData();
};

#endif // TEXTCACHINGBLOCK_H
