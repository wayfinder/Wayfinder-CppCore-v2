/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "Util/StringCache.h"
#include "TextPlacement/TextPlacementDelegator.h"
#include "TileMapContainer.h"
#include "TileMapHandler.h"
#include "TileFeature.h" // For VectorProxy, which should be moved.
#include "MapProjectionAdapter.h"
#include "TileMapTextSettings.h"
#include "TextPlacement/TextResult.h"
#include "TextPlacement/TextPlacementData.h"
#include "TextPlacement/OldLinePlacementPolicy.h"

#define CORE_LOGGING_MODULE_NAME "TextPlacementDelegator"
// #define CORE_LOGGING_MODULE_LEVEL CL_ALL_FILE

#include "CoreDebugPrint.h"


struct TextPlacementDelegator::Impl {
   Impl(TileMapHandler& mapHandler,
        NativeTextInterface* textInterface,
        ViewPortInterface* viewPort,
        const MapProjection& mapProjection,
        const MapsToDrawIt& endIt) :
      mapHandler(mapHandler),
      textInterface(textInterface),
      viewPort(viewPort),
      mapProjection(mapProjection),
      mapItBegin(endIt),
      mapItEnd(endIt),
      mapItCur(endIt),
      textIndexCur(),
      textSettings(),
      stringCache(textInterface),
      curPolicyIndex(),
      processingTimeMs()
   {
      policies.push_back(
         new OldLinePlacementPolicy(textInterface,
                                    viewPort,
                                    textSettings,
                                    mapProjection,
                                    stringCache,
                                    overlapDetector));
      curPolicy = *policies.begin();
   }

   ~Impl() {
      for(PolicyVec::iterator itr = policies.begin();
          itr != policies.end(); itr++)
      {
         delete *itr;
      }

      policies.clear();
   }
   
   /// Used to get screen coordinates for features etc
   TileMapHandler& mapHandler;

   /// Used to create platform specific string representations
   NativeTextInterface* textInterface;

   /// Used to query viewport dimensions. Useful for clipping polygon max x & y
   ViewPortInterface* viewPort;

   /// Used to transform single points, query angles etc
   const MapProjection& mapProjection;

   /// Iterator which points to the first map to place text for
   MapsToDrawIt mapItBegin;

   /// Iterator which points to the end of the tile map placement range
   MapsToDrawIt mapItEnd;

   /// Iterator which points to the current map we're placing text for
   MapsToDrawIt mapItCur;

   /// Cache of placed texts, ensures that texts don't get placed multiple times
   std::set<MC2SimpleString> placedTexts;

   /// Index which points to the current text block inside the currrent map
   int textIndexCur;

   /// Stores which fonts to use for polygons, polylines etc
   TileMapTextSettings textSettings;

   /// Stores the current text placement results
   TextResultVec textResults;

   /// The string cache for common strings
   StringCache stringCache;

   /// The overlap detection mechanism, assures that no texts overlap
   OverlapDetector<PixelBox> overlapDetector;

   PolicyVec policies;
   
   /// The policy used to place text horizontally
   TextPlacementPolicy* curPolicy;

   /// The index of the current policy
   unsigned int curPolicyIndex;

   /// The time it took for the complete sweep to finish
   unsigned int processingTimeMs;
};

TextPlacementDelegator::TextPlacementDelegator(TileMapHandler& mapHandler,
                                               NativeTextInterface* textInterface,
                                               ViewPortInterface* viewPort,
                                               const MapProjection& mapProjection,
                                               const MapsToDrawIt& endIt)
{
   m_impl = new Impl(mapHandler, textInterface, viewPort, mapProjection,
                     endIt);
}

TextPlacementDelegator::~TextPlacementDelegator()
{
   delete m_impl;
}

void
TextPlacementDelegator::updateMaps(const MapsToDrawIt& beginIt,
                                   const MapsToDrawIt& endIt)
{
   m_impl->mapItBegin = beginIt;
   m_impl->mapItEnd = endIt;
   
   resetState();
}


void TextPlacementDelegator::resetState()
{
   // The first call to moveToNextString will have to initialize
   // m_impl->currentTextIndexInMap to 0 in order to process the
   // first text item, and -1 + 1 == 0.
   
   m_impl->textIndexCur = -1;
   m_impl->mapItCur = m_impl->mapItBegin;
   deleteAndClear(m_impl->textResults);
   m_impl->curPolicy->resetViewport();
   m_impl->overlapDetector.clear();
   m_impl->placedTexts.clear();
}

void TextPlacementDelegator::doFullRun()
{
   uint32 startTimeMs = TileMapUtil::currentTimeMillis();
   
   resetState();
   
   while(placeNextText() >= 0) {
      
   }

   m_impl->processingTimeMs = TileMapUtil::currentTimeMillis() - startTimeMs;
}

int TextPlacementDelegator::placeNextText() {
   if(!moveToNextString()) {
      return -1;
   }

   /*
    *  Start by fetching the TileMap containing the strings
    *  and then get the TileMap containing the feature.
    */ 
   const TileMap* stringMap = m_impl->mapItCur.getStringMap();
   TileMap* dataMap         = *m_impl->mapItCur;
   STRING* nameOfFeature    = NULL;
   
   MC2_ASSERT( dataMap != NULL );
   MC2_ASSERT( stringMap != NULL );

   int featureIndex = stringMap->getFeatureIdxInTextOrder(
      m_impl->textIndexCur );

   /*
    *  Get the actual string to place from the string map.
    */ 
   const char* tmpString =
      stringMap->getStringForFeature( featureIndex );
   
   if (tmpString == NULL  || tmpString[0] == '\0') {
      coreprintln_info("Null/empty string");
      return 0;
   }

   /*
    *  Attempt to get the parent feature of the string.
    */ 
   TileFeature* feature = 
      const_cast<TileFeature*> (dataMap->getFeature( featureIndex ));
   
   if(feature == NULL) {
      coreprintln_info("No parent feature for string");
      return 0;
   }
   
   /*
    *  Attempt to get the primitive feature associated with the text
    *  from its parent feature.
    */ 
   TilePrimitiveFeature* prim = dataMap->getOnePrimitive( *feature );
   
   if ( prim == NULL || ( ! prim->isDrawn() ) ) {
      // Primitive NULL or was not drawn. 
      // Then it shouldn't get any text either.
      return 0;
   }
   
   /*
    *  Get the projected screen space coordinates for the feature
    */ 
   const VectorProxy<MC2Point>& pointsInFeature = 
      m_impl->mapHandler.getScreenCoords( *dataMap, *prim );

   const CoordsArg* coords =
      static_cast<const CoordsArg*>(prim->getArg(TileArgNames::coords));

   if (pointsInFeature.empty() ) {
      coreprintln_info("No points for feature - not placing any text.");
      return 0;
   }

   /*
    *  Allocate strings - one is needed for the platform specific
    *  drawing (nameOfFeature), and one is needed for book-keeping
    *  (tmpSimpleString)
    */ 

   MC2SimpleString tmpSimpleString( tmpString );

   nameOfFeature =
      m_impl->textInterface->createString( tmpSimpleString.c_str() );

   /*
    * If this string has already been placed properly, there
    * is no need to do it again.
    */
   
   if( m_impl->placedTexts.find( tmpSimpleString ) !=
       m_impl->placedTexts.end() )
   {
      coreprintln_info("Already placed %s - not placing again.",
                       tmpSimpleString.c_str());      
      return 0;
   }


   TileMapCoords::const_iterator coordsBegin = TileMapCoords::const_iterator();
   
   if(coords) {
      coordsBegin = coords->begin(*dataMap);
   }
   
   /** 
    *  Delegate the placement to the proper policy class.
    */   
   TextPlacementData data(feature->getType(),
                          nameOfFeature, tmpSimpleString,
                          feature, prim, pointsInFeature,
                          coordsBegin);
   
   int retVal = delegatePlacement(data);
   
   if( retVal == 1) {
      coreprintln_info("Placement successful, marking string in set");

      m_impl->placedTexts.insert(tmpSimpleString);
   }
   
   return retVal;
}

int TextPlacementDelegator::delegatePlacement(TextPlacementData& data) {


   coreprintln_info("Placing %s with %u screen points",
                    data.mc2String.c_str(),
                    data.pointsInFeature.size());
   
   int retVal = 0;

   switch(data.type) {
   case ( TileFeature::city_centre_2 ):
   case ( TileFeature::city_centre_4 ):
   case ( TileFeature::city_centre_5 ):
   case ( TileFeature::city_centre_7 ):
   case ( TileFeature::city_centre_8 ):
   case ( TileFeature::city_centre_10 ):
   case ( TileFeature::city_centre_11 ):
   case ( TileFeature::city_centre_12 ):
      retVal = m_impl->curPolicy->placeHorizontal(data, m_impl->textResults);
   break;

   /**
    *  These feature classes should perhaps be represented
    *  by a round rect instead of a regular text.
    */ 
   case ( TileFeature::street_class_0 ):
   case ( TileFeature::street_class_0_level_0 ): {
      bool roundRect = false;
      
      // For this to be possible, at least one of the characters
      // in the text string has to be a number
      for ( uint32 i = 0; i < data.mc2String.length(); ++i ) {
         if ( data.mc2String.c_str()[ i ] >= '0' &&
              data.mc2String.c_str()[ i ] <= '9' ) {
            roundRect = true;
            break;
         }
      }
      
      // A round rect cannot be used for strings that are too
      // long. Otherwise for instance "Vei Merket Mot E18" 
      // gets a roundrect.
      if ( data.mc2String.length() > 6 ) {
         roundRect = false;
      }
      
      if ( roundRect ) {   
         // Skip E-roads if the scalelevel is too high.
         if ( m_impl->mapHandler.getMapProjectionAdapter().getPixelScale()
              > 50.0 )
         {
            break;
         }
         retVal = m_impl->curPolicy->placeOnRoundRect(data, m_impl->textResults);
      } else {
         retVal = m_impl->curPolicy->placeOnLine(data, m_impl->textResults);
      }
   } break;
   case ( TileFeature::street_class_1 ):
   case ( TileFeature::street_class_2 ):
   case ( TileFeature::street_class_3 ):
   case ( TileFeature::street_class_4 ):
   case ( TileFeature::street_class_1_level_0 ):
   case ( TileFeature::street_class_2_level_0 ):
   case ( TileFeature::street_class_3_level_0 ):
   case ( TileFeature::street_class_4_level_0 ): {
      data.polyLineWidth =
         m_impl->mapHandler.getPolylineOuterPixelWidth( *data.primitive );
      
      retVal = m_impl->curPolicy->placeOnLine(data, m_impl->textResults);
      break;
   } 
   case ( TileFeature::water ):
   case ( TileFeature::park ):
      retVal = m_impl->curPolicy->placeInsidePolygon(data, m_impl->textResults);
   break;
   default:            
      break;
   }
   
   return retVal;
}

bool TextPlacementDelegator::moveToNextString()
{
   // Move to next feature index.
   ++m_impl->textIndexCur;
   
   // Stop when we have reached the end iterator.
   while ( m_impl->mapItCur != m_impl->mapItEnd ) {
      // Get the current string map.
      TileMap* stringMap = m_impl->mapItCur.getStringMap();
      if ( *m_impl->mapItCur == NULL || 
           stringMap == NULL ) {
         // Either string or feature map is NULL. Move to next map.
         ++m_impl->mapItCur;
      } else if ( m_impl->textIndexCur >= 
                  stringMap->getNbrFeaturesWithText() ) {
         // Feature index is out of range. Move to next map.
         m_impl->textIndexCur = 0;
         ++m_impl->mapItCur;
      } else {
         // Valid string found.
         return true;
      }
   }
   
   // If we get here, no string has been found.
   return false; 
}

void TextPlacementDelegator::setTextSettings( const TileMapTextSettings& settings )
{
   m_impl->textSettings = settings;
}

TileMapTextSettings& TextPlacementDelegator::getTextSettings() 
{
   return m_impl->textSettings;
}

bool TextPlacementDelegator::needsToRun() const
{
   return m_impl->mapItCur != m_impl->mapItEnd;
}

const TextPlacementDelegator::TextResultVec&
TextPlacementDelegator::getTextResults()
{
   return m_impl->textResults;
}

void TextPlacementDelegator::setTextInterface( NativeTextInterface* textInterface )
{
   m_impl->textInterface = textInterface;
   m_impl->stringCache.setTextInterface(textInterface);
      
   for(PolicyVec::iterator itr = m_impl->policies.begin();
       itr != m_impl->policies.end(); itr++)
   {
      (*itr)->setTextInterface(textInterface);
   }
}

void TextPlacementDelegator::setViewPortInterface( ViewPortInterface* viewPortInterface )
{
   m_impl->viewPort = viewPortInterface;

   for(PolicyVec::iterator itr = m_impl->policies.begin();
       itr != m_impl->policies.end(); itr++)
   {
      (*itr)->setViewPortInterface(viewPortInterface);
   }
}

void TextPlacementDelegator::doIteration()
{
   if(!needsToRun()) {
      return;
   }

   // Run until we have successfully placed one text string
   while(placeNextText() == 0) {}
}

void TextPlacementDelegator::deleteAndClear( TextResultVec& vect )
{  
   for ( unsigned int i = 0; i < vect.size(); ++i ) {
      if(m_impl->textInterface) {
         m_impl->textInterface->deleteString ( vect[i]->m_string );
      }

      delete vect[i];
   }
   
   vect.clear(); 
}

void TextPlacementDelegator::mapProjectionChanged(const MapsToDrawIt& beginIt,
                                                  const MapsToDrawIt& endIt,
                                                  int trackingMode)
{
   m_impl->mapItBegin = beginIt;
   m_impl->mapItEnd = endIt;
   
   resetState();
}

void TextPlacementDelegator::cycleAlgorithm()
{
   m_impl->curPolicyIndex = (m_impl->curPolicyIndex+1)%m_impl->policies.size();
   m_impl->curPolicy = m_impl->policies[m_impl->curPolicyIndex];
   resetState();
   doFullRun();
}

const char* TextPlacementDelegator::getAlgorithmName()
{
   return m_impl->curPolicy->getName(); 
}

unsigned int TextPlacementDelegator::getProcessingTimeMs() const
{
   return m_impl->processingTimeMs;
}

