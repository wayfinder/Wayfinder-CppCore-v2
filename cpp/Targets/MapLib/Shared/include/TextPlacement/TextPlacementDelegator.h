/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef TEXTPLACEMENTDELEGATOR_H
#define TEXTPLACEMENTDELEGATOR_H

//TODO: Remove dependency upon TMH
class TileMapHandler;
class MapsToDrawIt;
class NativeTextInterface;
class ViewPortInterface;
class MapProjection;
class TileMapTextSettings;
class TextResult;
class StringCache;

#include <vector>

class TextPlacementDelegator {
public:
   typedef std::vector<TextResult*> TextResultVec;
   struct TextPlacementData;
   
   TextPlacementDelegator(TileMapHandler& mapHandler,
                          NativeTextInterface* textInterface,
                          ViewPortInterface* viewPort,
                          const MapProjection& mapProjection,
                          const MapsToDrawIt& endIt);   

   ~TextPlacementDelegator();
   
   /**
    *  The set of maps to place text for has  changed. The maps to place
    *  text are the maps in the range from beginIt to endIt.
    *
    *    @param   beginIt  Iterator pointing to first map to place text for.
    *    @param   endIt    Iterator pointing to end of range.
    */
   void updateMaps(const MapsToDrawIt& beginIt, const MapsToDrawIt& endIt);

   /**
    *  If the map projection changes, i.e. the user pans & zooms, this
    *  method is called. The class can then add delta values to the existing
    *  text results instead of re-placing them.
    *
    *    @param   beginIt  Iterator pointing to first map to place text for.
    *    @param   endIt    Iterator pointing to end of range.
    *    @param   trackingMode Whether or not we are actively moving.
    */ 
   void mapProjectionChanged(const MapsToDrawIt& beginIt,
                             const MapsToDrawIt& endIt,
                             int trackingMode);

   /**
    *  Returns true if there is still text to place.
    */ 
   bool needsToRun() const;
   
   /**
    *  Clears all generated text results and does a complete
    *  text placement pass. After this method completes, all
    *  text will have been placed for the current view.
    */
   void doFullRun();

   /**
    *  Performs a single iteration of text placement. Will need
    *  to be run multiple times before needsToRun will return false.
    */ 
   void doIteration();
   
   /**
    *    Sets new text settings. These settings dictate which fonts
    *    will be used for polygons, lines etc.
    */
   void setTextSettings( const TileMapTextSettings& settings );

   /**
    *   @return The text settings.
    */
   TileMapTextSettings& getTextSettings();

   /**
    *   Returns the current output of the text placer.
    */ 
   const TextResultVec& getTextResults();
   
   /**
    *  Changes the native text interface to use for font dimensions etc.
    */
   void setTextInterface( NativeTextInterface* textInterface );
   
   /**
    *  Changes the viewport interface to use.
    */
   void setViewPortInterface( ViewPortInterface* viewPortInterface );
   
   /** 
    *  @return the string cache for this delegator. Commonly used
    *            by the policies etc.
    */ 
   StringCache* getStringCache() const;
      
   /**
    *  Resets the iterators and cache so that the next call to
    *  moveToNextString will point to the first placeable text.
    */ 
   void resetState();

   /**
    *   @return the name of the current text placing algorithm.
    */ 
   const char* getAlgorithmName();

   /**
    *   Cycles the algorithm currently in use. Will reposition
    *   all text.
    */ 
   void cycleAlgorithm();

   /**
    *   Returns the last measured time for the call to
    *   doFullRun.
    */ 
   unsigned int getProcessingTimeMs() const;
private:
   struct Impl;

   /**
    *   Deletes all contents of the vector and clears it.
    */
   void deleteAndClear( TextResultVec& vect );


   /**
    *  Moves to the next placeable string. Will increment internal
    *  iterators.
    */ 
   bool moveToNextString();

   /**
    *  Place the next valid text portion.
    *
    *  @return -1 If there are no more texts to place.
    *           0 If the current text or feature was missing.
    *           1 If text was successfully placed.
    */ 
   int placeNextText();

   /**
    *  Delegates a placement load to the policy class.
    *
    *  @param data The parameters for the placement.
    */ 
   int delegatePlacement(TextPlacementData& data);
private:
   class TextPlacementPolicy;
   class OldLinePlacementPolicy;
   typedef std::vector<TextPlacementPolicy*> PolicyVec;
   
   /**
    * Hinder copying.
    */ 
   TextPlacementDelegator(const TextPlacementDelegator& rhs);

   /**
    * Hinder copying.
    */ 
   TextPlacementDelegator& operator=(const TextPlacementDelegator& rhs);
   

   Impl* m_impl;
};

#endif /* TEXTPLACEMENTDELEGATOR_H */
