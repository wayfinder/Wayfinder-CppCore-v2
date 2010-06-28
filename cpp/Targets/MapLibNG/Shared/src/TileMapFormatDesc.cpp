/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "TileMapConfig.h"
#include "TileMapFormatDesc.h"
#include "TileMapParams.h"
#include "MC2BoundingBox.h"
#include "MapProjection/MapProjection.h"
#include "TileImportanceTable.h"
#include "GfxConstants.h"
#include "BitBuffer.h"
#include "TileMapContainer.h"
#include "TileMapUtil.h"
#include <stdio.h>
#include "MapLibSettingsNotifier.h"
#include "Utility.h"


using namespace std;

#define MAKE_RGB(r,g,b) ( (r << 16) | (g << 8) | (b) )

#ifdef MC2_SYSTEM
LangTypes::language_t
TileMapFormatDesc::
getLanguageFromParamString( const MC2SimpleString& paramString )
{
   char* minus = strchr( paramString.c_str(), '-' );
   if ( minus != NULL ) {
      char* nextMinus = strchr( minus+1, '-');
      if ( nextMinus == NULL ) {
         return LangTypes::invalidLanguage;
      }
      char langStr[64];
      int curPos = 0;
      for ( char* i = minus+1; i != nextMinus; ++i ) {
         langStr[curPos++] = *i;
      }
      langStr[curPos++] = 0;
      return LangTypes::getStringAsLanguage( langStr, false );
   } else {
      // Other format, the old one with pluses
      char* lastPlus = strrchr(paramString.c_str(), '+');
      if ( lastPlus != NULL ) {
         return LangTypes::getStringAsLanguage( lastPlus+1, false );
      } else {
         return LangTypes::invalidLanguage;
      }
   }
}

#endif

MC2SimpleString
TileMapFormatDesc::createParamString( LangTypes::language_t lang,
                                      const char* clientTypeString,
                                      const char* randChars,
                                      bool nightMode )
{
   char* tmpDesc = new char[1024];
   const char* langString = LangTypes::getLanguageAsString( lang );
   const char* lastMinus = randChars ? "-" : "";
   randChars = randChars ? randChars : "";

   if( nightMode ){
      sprintf( tmpDesc, "dX-%s-%s%s%s",
               langString,
               clientTypeString,
               lastMinus,
               randChars );
   }
   else{
      sprintf( tmpDesc, "DX-%s-%s%s%s",
               langString,
               clientTypeString,
               lastMinus,
               randChars );
   }
   // MC2SimpleStringNoCopy will delete the tmpDesc when deleted
   return MC2SimpleStringNoCopy( tmpDesc );   
}

TileMapFormatDesc::TileMapFormatDesc()
{
   // The real prefix is set when loading or in ServerTileMapFormatDesc.
   m_serverPrefix = 0xffff;
   m_primitiveDefaultMap            = NULL;
   m_primitiveDefaultMapSize        = 0;
   m_argsPerTypeArray               = NULL;
   m_argsPerTypeArraySize           = 0;
   m_argsByTileFeatureTypeArray     = NULL;
   m_argsByTileFeatureTypeArraySize = 0;
   m_categoryPoiTypes               = NULL;
   m_settingsNotifier               = NULL;
   m_unNotifiedChanges              = false;
   m_lang                           = LangTypes::invalidLanguage;
   m_crc                            = MAX_UINT32;
   m_timeStamp                      = 0; // Very old
   m_copyrightString                = NULL;
   m_copyrightHolder                = NULL;
}


TileMapFormatDesc::~TileMapFormatDesc()
{
   // Remove primitive stuff.
   {
      const int nbrPrimitiveDefaults = m_primitiveDefaultMapSize;

      for( int i = 0; i < nbrPrimitiveDefaults; ++i ) {         
         for ( primitiveVector_t::iterator jt =
                  m_primitiveDefaultMap[i].begin();
               jt != m_primitiveDefaultMap[i].end();
               ++jt ) {
            delete *jt;
         }
      }
      delete [] m_primitiveDefaultMap;
      m_primitiveDefaultMap = NULL;
   }


   // Delete the importance tables.
   for ( uint32 i = 0; i < m_importanceTables.size(); ++i ) {
      delete m_importanceTables[ i ];
   }
   
   // Delete the elements in m_tileScaleByLayer.
   {
      for ( uint32 i = 0; i < m_tileScaleByLayer.size(); ++i ) {
         delete m_tileScaleByLayer[ i ];
      }
   }
   delete [] m_argsPerTypeArray;
   
   if ( m_argsByTileFeatureTypeArray ) {
      for ( int i=0; i < m_argsByTileFeatureTypeArraySize; ++i ) {
         vector<TileFeatureArg*>& args = m_argsByTileFeatureTypeArray[i];
         for ( vector<TileFeatureArg*>::iterator it = args.begin();
               it != args.end();
               ++it ) {
            delete *it;
         }
      }                                                                    
      delete [] m_argsByTileFeatureTypeArray;
      m_argsByTileFeatureTypeArray = NULL;
   }

   delete [] m_categoryPoiTypes;
   delete [] m_copyrightString;
   delete m_copyrightHolder;
}

const TileMapLayerInfoVector&
TileMapFormatDesc::getLayerIDsAndDescriptions() const
{
   return m_layerIDsAndDesc;
}


void
TileMapFormatDesc::load( const char* /*filename*/ )
{

}

void
TileMapFormatDesc::readCategories( SharedBuffer& buf,
                                   const TileMapFormatDesc* previosDesc )
{
   // Poi types to enable and disable
   uint32 nbrPoiCategories = buf.readNextBALong();
   int totalNbrPoiTypes = buf.readNextBALong();
   // Allocate the poi-type-array and the category vector.
   m_categories.clear();
   m_categories.reserve( nbrPoiCategories );
 
   delete [] m_categoryPoiTypes;
   m_categoryPoiTypes = new TileFeature::tileFeature_t[totalNbrPoiTypes];
   
   // Read the stuff.
   int currentPoiTypeIdx = 0;
   for ( uint32 i = 0; i < nbrPoiCategories; ++i ) {
      // Save some pointers
      const TileFeature::tileFeature_t* firstCat =
         m_categoryPoiTypes + currentPoiTypeIdx;
      
      const char* curCatName = buf.readNextString();      
      int categoryID         = buf.readNextBAShort();
      int categoryEnabled    = buf.readNextBAByte();
      // Read in the poi-types of the category.
      int nbrPoiTypes = buf.readNextBALong();
      for ( int j = 0; j < nbrPoiTypes; ++j ) {
         m_categoryPoiTypes[currentPoiTypeIdx++] =
            TileFeature::tileFeature_t( buf.readNextBALong() );
      }
      m_categories.push_back( TileMapCategoryNotice(curCatName,
                                                    categoryID,
                                                    categoryEnabled,
                                                    nbrPoiTypes,
                                                    firstCat) );
   }
   // Update the client categories
   {

      m_clientCategories.clear();
      m_clientCategories.reserve( m_categories.size() );
   
      for ( uint32 i = 0; i < m_categories.size(); ++i ) {
         TileMapCategoryNotice* curNotice = &m_categories[i];
         bool found = false;
         if ( previosDesc ) {            
            // Disable the previously disabled categories.

            for( uint32 j = 0;
                 j < previosDesc->m_clientCategories.size();
                 ++j ) {
               if ( previosDesc->m_clientCategories[j]->getID() ==
                    curNotice->getID() ) {
                  curNotice->setVisible(
                     previosDesc->m_clientCategories[j]->isVisible() );
                  found = true;
                  break;
               }
            }

         }
         m_clientCategories.push_back( curNotice );
         // If the category was not found, there was a difference between
         // the category sets and these changes should be notified
         if (!found) {
            m_unNotifiedChanges = true;
         }

      }
   }

   if (m_settingsNotifier != NULL && m_unNotifiedChanges){
      m_settingsNotifier->notifyUpdatedPOICategories();
      m_unNotifiedChanges = false;
   }
   //dumpCategories();
}


void
TileMapFormatDesc::load( BitBuffer& buf, TileMapFormatDesc* previosDesc )
{
   // Load the general tile information.
   buf.alignToByte();
   
   // Read and copy the string.
   const char* tmpStr = buf.readNextString();

   // This should not be a string, but the format must be changed then.
   m_serverPrefix = atoi(tmpStr);

   // Nbr layers.
   uint32 nbrLayers = buf.readNextBAByte();
  
   m_layerIDsAndDescForComp.resize( nbrLayers );
   m_importanceTables.resize( nbrLayers );
   m_tileScaleByLayer.resize( nbrLayers, NULL );
   uint32 i = 0; 
   for ( i = 0; i < nbrLayers; ++i ) {
      // Tile data for the layer.
      int meters = buf.readNextBAShort();
      int pixels = buf.readNextBAShort();
      int dpi = buf.readNextBAShort();
      float64 zoomFactor = buf.readNextBALong() / 1000.0;
      float64 exchangeTileFactor = buf.readNextBALong() / 1000.0;
      int detailLevels = buf.readNextBAByte();
     
      // Initialize the tiles.
      initTileSizesForLayer( i,
                             meters,
                             pixels,
                             dpi,
                             zoomFactor,
                             exchangeTileFactor,
                             detailLevels );

      uint32 layerID = buf.readNextBAByte();
      m_layerNbrByID[ layerID ] = i;
      m_layerIDsAndDescForComp[ i ] =
         make_pair(layerID,
                   MC2SimpleString( buf.readNextString() ) );
      mc2dbg << "Layer " << layerID << " contains "
             << m_layerIDsAndDescForComp[ i ].second
             << endl;

      // Create and load the importance table.
      m_importanceTables[ i ] = new TileImportanceTable();
      m_importanceTables[ i ]->load( buf );
   }

   {
      // Copy the information into the new vector
      // The rest is loaded at the end.
      int nbrLayers = m_layerIDsAndDescForComp.size();
      m_layerIDsAndDesc.resize( nbrLayers );
      for ( int i = 0; i < nbrLayers; ++i ) {
         m_layerIDsAndDesc[i].setIDAndName(
            m_layerIDsAndDescForComp[i].first,
            m_layerIDsAndDescForComp[i].second);

         // Fix for old tmfd:
#if 1
         bool visible = false;
         uint32 id = m_layerIDsAndDescForComp[i].first;
         if ( id == TileMapTypes::c_mapLayer ||
              id == TileMapTypes::c_routeLayer ||
              id == 2 ) {
            visible = true;
         }
         m_layerIDsAndDesc[i].setVisible( visible );
         m_layerIDsAndDesc[i].setPresent( visible );
#endif         
      }
   }

   // Load m_argsByTileFeatureType.
//   typedef map<featureInt, 
//      vector<TileFeatureArg*> > argsByType_t;

   // Size of the map.
   uint32 size = buf.readNextBAShort();

   // Allocate the arrays
   delete [] m_argsByTileFeatureTypeArray;
   m_argsByTileFeatureTypeArraySize = size;
   m_argsByTileFeatureTypeArray =
      new vector<TileFeatureArg*>[ m_argsByTileFeatureTypeArraySize ];
   
   for ( i = 0; i < size; ++i ) {
      // Key.
      featureInt type = 
         featureInt( int16( buf.readNextBAShort() ) );
      MC2_ASSERT( type == int(i) );
      type = type;
      // Value. vector<TileFeatureArg*>.
      // Size of vector.
      uint32 vecSize = buf.readNextBAByte();
      // Use already allocated vector in array
      vector<TileFeatureArg*>& vec = m_argsByTileFeatureTypeArray[i];
      vec.resize( vecSize );
      // The args of the vector.
      for ( uint32 j = 0; j < vecSize; ++j ) {
         // Read the arg.
         vec[ j ] = TileFeatureArg::loadFullArg( buf );
      }
      
      // Align 
      buf.alignToByte();
   }

   // Load m_scaleIndexByLevel.
   uint32 nbrScaleIndex = buf.readNextBAByte();
   int index = 0;
   {for ( uint32 i = 0; i < nbrScaleIndex; ++i ) {
      m_scaleIndexByLevel[ buf.readNextBALong() ] = index;
      ++index;
   }}
   
   // Load the default arguments for the primitives.
   m_defaultArgs.load( buf );
  
   buf.alignToByte();

   // Load m_primitiveDefaultMap.
   // m_primitiveDefaultMap contains the default arguments for the
   // primitives, keyed by the (complex) feature type.
   
   // typedef map<featureInt, primitiveVector_t>
   //    primitiveDefaultMap_t;

   // Size of the map.
   size = buf.readNextBAShort();
   mc2dbg8 << "[TMFD] load : m_primitiveDefaultMap.size() "
           << size << endl;

   // Temporary map needed since all the vectors aren't sent.
   map<featureInt, primitiveVector_t> primMap;

   for ( i = 0; i < size; ++i ) {
      // Key. TileFeature type (complex)
      featureInt type = 
         featureInt( int16( buf.readNextBAShort() ) );
      // Value. 
      // vector<TileFeature*>
      
      // Size of vector. Note the size should always be 1.
      // There's a 1:1 relationship between the TileFeature and
      // TilePrimitiveFeature.
      uint32 vecSize = buf.readNextBAByte();
      // And resize it.
      primitiveVector_t vec ( vecSize );

      // Read the (one and only) TilePrimitiveFeature
      for ( uint32 j = 0; j < vecSize; ++j ) {
         // Feature type for primitive.
         featureInt primType = 
            featureInt( int16( buf.readNextBAShort() ) );
         WritableTileFeature* primFeature = 
            new WritableTileFeature( primType );
         uint32 argSize = buf.readNextBAByte();
         vector<TileFeatureArg*> argVec;
         argVec.resize( argSize );

         for ( uint32 k = 0; k < argSize; ++k ) {            
            //argVec[ k ] = TileFeatureArg::loadFullArg( buf ); 
            argVec[ k ] = m_defaultArgs.getArg( buf.readNextBAShort() ); 
         }
         primFeature->setArgs( argVec );
         vec[ j ] = primFeature;
         buf.alignToByte();
      }
      // Insert it into the temporary map.
      primMap.insert( make_pair( type, vec ) );
   }

   int lastType = 1;
   if ( ! primMap.empty() ) {
      lastType = (*primMap.rbegin()).first;
   }

   // Move the primitive defaults to the vector.
   delete [] m_primitiveDefaultMap;
   m_primitiveDefaultMapSize = lastType + 1;
   m_primitiveDefaultMap =
      new primitiveDefaultMap_t[m_primitiveDefaultMapSize];
   for ( i = 0; int(i) < m_primitiveDefaultMapSize; ++i ) {
      if ( primMap.find( i ) != primMap.end() ) {
         m_primitiveDefaultMap[i].swap( primMap[i] );
      }
   }

#ifdef __unix
   if ( m_primitiveDefaultMapSize != int(primMap.size()) ) {
      mc2dbg << "[TMFD]: m_primitiveDefaultMapSize = "
             << m_primitiveDefaultMapSize << ", primMap.size() = "
             << primMap.size() << endl;
   }
#endif

   primMap.clear();
   
   // Load m_argsPerType.
   // typedef map<featureInt,
   //    argTransferMap_t*> argTransferPerType_t;

   // Size of the map.
   size = buf.readNextBAShort();
   mc2dbg8 << "[TMFD] load : m_argsPerType.size() "
           << size << endl;

   // Allocate the vector of transfer maps.
   delete [] m_argsPerTypeArray;

   m_argsPerTypeArraySize = size;
   m_argsPerTypeArray = new argTransferMap_t[ m_argsPerTypeArraySize ];
   
   for ( i = 0; i < size; ++i ) {
      // Key. Complex feature type.
      featureInt type = featureInt( int16( buf.readNextBAShort() ) );
      type = type; // For the assert
      MC2_ASSERT( int(type) == int(i) );      
      // Value.
      //   typedef multimap<MC2SimpleString, pair<int, MC2SimpleString> >
      //      argTransferMap_t;
      
      // Size of the multimap.
      uint32 mapSize = buf.readNextBAShort();

      // Put a new map in the vector      
      argTransferMap_t* argTransferMap = &m_argsPerTypeArray[i];
      
      for ( uint32 j = 0; j < mapSize; ++j ) {
         // Key, argument type.
         TileArgNames::tileArgName_t keyName =
            TileArgNames::tileArgName_t( buf.readNextBAByte() );
         // Value, pair<int, MC2SimpleString>.
         // int.
         int pos = buf.readNextBAShort();
         // The name of the value arg.
         TileArgNames::tileArgName_t valueName =
            TileArgNames::tileArgName_t( buf.readNextBAByte() );
         argTransferMap->insert( make_pair( keyName, 
                                            make_pair( pos, valueName ) ) );
      }
   }

   // Background color.
   m_backgroundColor = buf.readNextBALong();

   // First addition. The detail level for the reserve maps.
   if ( buf.getNbrBytesLeft() < 8 ) {
      m_reserveDetailLevel   = 6;
      m_extraTilesForReserve = 3;
      return;
   } else {
      m_reserveDetailLevel   = buf.readNextBALong();
      m_extraTilesForReserve = buf.readNextBALong();
      mc2dbg << "[TMFD]: Reserve detail level from server = "
             << m_reserveDetailLevel << " and extra tiles = "
             << m_extraTilesForReserve << endl;
   }

   // Second addition. The language and categories.
   
   // Check for more stuff coming in the buffer.
   if ( buf.getNbrBytesLeft() < 12 ) {
      return;
   }

   // Language!!!
   buf.readNextBALong( m_lang );

   // Categories. Latin-1 to work around bug in S60.
   readCategories( buf, previosDesc );
   
   // Check if there is room for the CRC.
   // Check if there is a timestamp
   if ( buf.getNbrBytesLeft() < 8 ) {
      return;
   }   
   m_crc = buf.readNextBALong(); // CRC
   m_timeStamp = buf.readNextBALong(); 
   mc2log << "[TMFD]: load crc = " << m_crc << endl;

   // Check if there is room for new tile settings.
   if ( buf.getNbrBytesLeft() > 0 ) {
      mc2log << "[TMFD]: Loading correct tile sizes." << endl;
      for ( uint32 i = 0; i < nbrLayers; ++i ) {
         mc2log << "[TMFD]: Layer nbr " << i << endl;
         tileScale_t& tileScale = *m_tileScaleByLayer[ i ];
         tileScale.clear(); // Clear the vector.
         // Nbr detail levels.
         int nbrDetails = buf.readNextBAByte();
         tileScale.reserve( nbrDetails );
         for ( int i = 0; i < nbrDetails; ++i ) {
            int mc2UnitsForTile = buf.readNextBALong();
            uint16 scale = buf.readNextBAShort();
            tileScale.push_back( make_pair( mc2UnitsForTile, scale ) );
            mc2log << "[TMFD]: Detaillevel " << i << ", mc2UnitsForTile "
                   << mc2UnitsForTile << ", scale " << scale << endl;
         }
      }
   }

   // Check if there is room for the copyright string.
   if ( buf.getNbrBytesLeft() > 0 ) {
      m_copyrightString = TileMapUtil::newStrDup( buf.readNextString() );
   }

   if ( m_copyrightString == NULL ) {
      // Some hardcoded stuff.
      m_copyrightString =
         TileMapUtil::newStrDup( "©2005 Wayfinder, Tele Atlas" );
   }

   // Check for categories that are sent in utf-8 (old Series 60
   // had problems with the utf-8
   if ( buf.getNbrBytesLeft() < 8 ) {
      return;
   }
   // Read categories again. This time in utf-8 as God intended.
   readCategories( buf, previosDesc );

   // Check for more layer information.
   if ( buf.getNbrBytesLeft() < 8 ) {
      return;
   }

   // Load more info about the layers.
   m_layerIDsAndDesc.load( buf );

   m_textColor = MAKE_RGB( 23, 23, 23 );
   m_horizonTopColor = MAKE_RGB( 178, 173, 244 );
   // Read the text color and horizon color.
   if ( buf.getNbrBytesLeft() < 8 ) {
      return;
   }
   m_textColor = buf.readNextBALong();
   m_horizonTopColor = buf.readNextBALong();

   m_horizonBottomColor = MAKE_RGB( 255, 255, 255 );
   if ( buf.getNbrBytesLeft() < 4 ) {
      return;
   }
   m_horizonBottomColor = buf.readNextBALong();

   // Check if there is any more to read.
   if ( buf.getNbrBytesLeft() < 4 ) {
      return;
   }
   
   // Load the copyright holder.
   delete m_copyrightHolder;
   m_copyrightHolder = new CopyrightHolder();
   m_copyrightHolder->load( buf );

}

bool
TileMapFormatDesc::updateLayers( TileMapLayerInfoVector& clientVector )
{
   set<int> existingLayers;
   for ( map<uint32,uint32>::const_iterator it = m_layerNbrByID.begin();
         it != m_layerNbrByID.end();
         ++it ) {
      existingLayers.insert( it->second );
   }
   return m_layerIDsAndDesc.updateLayers( clientVector, existingLayers );
}

bool
TileMapFormatDesc::updateLayersToDisplay( set<int>& layers, bool acpModeEnabled ) const
{
   return m_layerIDsAndDesc.updateLayersToDisplay( layers, acpModeEnabled );
}

#ifdef __unix__
void
TileMapFormatDesc::dumpCategories() const
{
   int nbrPoiCategories = m_categories.size();
   mc2dbg << "[TMFD]: nbrPoiCategories = " << nbrPoiCategories << endl;
   for ( int i = 0; i < nbrPoiCategories; ++i ) {
      mc2dbg << "[TMFD]: Category " << MC2CITE(m_categories[i].getName().c_str())
             << " id = " << m_categories[i].getID()
             << " has " << m_categories[i].getNbrTypes() << " types : "
             << endl;
      for ( int j = 0; j < m_categories[i].getNbrTypes(); ++j ) {
         mc2dbg << "[TMFD]:  type[" << j << "] = "
                << int(m_categories[i].getType(j)) << endl;
      }

   }
}
#endif

const vector<const WFAPI::POICategory*>&
TileMapFormatDesc::getCategories() const
{
   return m_clientCategories;
}

bool
TileMapFormatDesc::setCategoryVisible( WFAPI::wf_uint32 id, bool visible )
{
   const int catSize = m_categories.size();
   for( int i=0; i < catSize; ++i ) {
      if ( m_categories[i].getID() == id ) {
         m_categories[i].setVisible( visible );
         return true;
      }
   }
   return false;
}

void
TileMapFormatDesc::
getFeaturesDisabledByCategories(
   set<int32>& features) const
{
   const int catSize = m_categories.size();
   for( int i=0; i < catSize; ++i ) {
      if ( ! m_categories[i].isVisible() ) {
         m_categories[i].putInSet( features );
      }
   }
}


void
TileMapFormatDesc::getTileIndex( uint32 layerNbr,
                                 int detailLevel, 
                                 int32 lat, int32 lon, 
                                 int& tileLatIdx, 
                                 int& tileLonIdx ) const
{
   const tileScale_t& tileScale = *m_tileScaleByLayer[ layerNbr ];
   int mc2UnitsPerTile = tileScale[ detailLevel ].first;

   tileLatIdx = lat / mc2UnitsPerTile;
   // Trunc to the nearest lower integer.
   if ( lat < 0 ) {
      --tileLatIdx;
   }
   // Trunc to the nearest lower integer.
   tileLonIdx = lon / mc2UnitsPerTile;
   if ( lon < 0 ) {
      --tileLonIdx;
   }
}

inline void
TileMapFormatDesc::getTileIndex( const MapProjection& mapProj,
                                 uint32 layerNbr, int detailLevel,
                                 int& startTileLatIdx,
                                 int& startTileLonIdx,
                                 int& endTileLatIdx,
                                 int& endTileLonIdx ) const
{
 //   static const double mc2scaletometer = 6378137.0*2.0*
//       3.14159265358979323846 / 4294967296.0;
//    static const float meterstomc2scale = 1.0 / mc2scaletometer;

   
   {
      MC2BoundingBox bbox( mapProj.getBoundingBox() );
      
      // Get the lat indeces from the bbox
      getTileIndex( layerNbr, detailLevel,
                    bbox.getMinLat(),
                    bbox.getMinLon(),
                    startTileLatIdx, startTileLonIdx );
      getTileIndex( layerNbr, detailLevel,
                    bbox.getMaxLat(),
                    bbox.getMaxLon(),
                    endTileLatIdx, endTileLonIdx );
   }

   // Now fix the lon indeces
   {
      MC2BoundingBox bbox( mapProj.getBoundingBox() );
      
      const tileScale_t& tileScale = *m_tileScaleByLayer[ layerNbr ];
      int mc2UnitsPerTile = tileScale[ detailLevel ].first;
      
      float minLon = bbox.getMinLon();
      float maxLon = bbox.getMaxLon();

      static const float MC2_PERIOD = (float)MAX_INT32*2.0f;
      
      if(minLon > maxLon) {
         maxLon += MC2_PERIOD;
      }

      float width = maxLon - minLon;
      float centerLon = mapProj.getCenter().lon;
      minLon = centerLon - width / 2.0;
      maxLon = minLon + width;
      
      startTileLonIdx = int32( minLon / mc2UnitsPerTile );
      if ( minLon < 0 ) {
         --startTileLonIdx;
      }
      endTileLonIdx   = int32( maxLon / mc2UnitsPerTile );
      if ( maxLon < 0 ) {
         --endTileLonIdx;
      }
   }
}

bool
TileMapFormatDesc::
innerCreateParams(const MapProjection* mapProj,
                  map<uint32, ParamsNotice>& paramsByLayerNbr,
                  ParamsNotice& reserveParamsNotice,
                  vector<TileMapParams>& paramVectorOut,
                  vector<TileMapParams>& reserveParams,
                  const MC2BoundingBox& projBBox,
                  const MC2Coordinate& center,
                  uint32 inScale,
                  int useGzip,
                  LangTypes::language_t language,
                  const set<int>& layers,
                  const RouteID* routeID,
                  int inDetailLevel )
{
   // Add as parameter
   //bool stillMoving = false;
   uint16 scale   = uint16( inScale );
   //scale = 5000;

   // Set the (maybe rotated) bounding box)
   int32 minLat    = projBBox.getMinLat();
   int32 minLon    = projBBox.getMinLon();

   int32 maxLat    = projBBox.getMaxLat();
   int32 maxLon    = projBBox.getMaxLon();

   map<uint32, ParamsNotice> paramsNotices;

   int detailLevel = inDetailLevel;
   
   uint32 nbrParams = 0;
   int startTileLatIdx, startTileLonIdx, endTileLatIdx, endTileLonIdx;
   int nbrImportance;
   for ( set<int>::const_iterator lt = layers.begin();
         lt != layers.end(); ++lt )  {

      if ( ! hasLayerID( *lt ) ) {
         mc2dbg << "[TMFD]: Skipping unknown layer" << endl;
         continue;
      }
            
      uint32 layerID = *lt;
      uint32 layerNbr = getLayerNbrFromID( layerID );
      if ( ( layerID == TileMapTypes::c_routeLayer ) &&
           ( ( routeID == NULL ) || ( ! routeID->isValid() ) ) ) {
         // Skip the route, if the routeID is invalid.
         continue;
      }
      
      if ( inDetailLevel == MAX_INT32 ) { 
         detailLevel = getDetailLevel( layerNbr, scale );
      }
      if ( mapProj == NULL ) {
         getTileIndex( layerNbr, detailLevel, minLat, minLon, 
                       startTileLatIdx, startTileLonIdx );
         getTileIndex( layerNbr, detailLevel, maxLat, maxLon, 
                       endTileLatIdx, endTileLonIdx );
         
         if ( startTileLonIdx > endTileLonIdx ) {
            int maxTileIndexLon, foo;
            getTileIndex( layerNbr, detailLevel, 0, MIN_INT32, 
                          foo, maxTileIndexLon );
            endTileLonIdx += (-maxTileIndexLon);        
         }
      } else {
         getTileIndex( *mapProj, layerNbr, detailLevel,
                       startTileLatIdx, startTileLonIdx,
                       endTileLatIdx, endTileLonIdx );
      }

      mc2dbg8 << "[TMFD]: startTileLonIdx = " << startTileLonIdx
             << ", endTileLonIdx = " << endTileLonIdx << endl;
      
      nbrImportance = 
         m_importanceTables[ layerNbr ]->getNbrImportanceNbrs( 
                                                      scale, detailLevel );
      // WARNING: Debug!
      //nbrImportance = MIN( 1, nbrImportance );

      ParamsNotice paramsNotice;
      paramsNotice.m_startLatIdx = startTileLatIdx;
      paramsNotice.m_endLatIdx = endTileLatIdx;
      paramsNotice.m_startLonIdx = startTileLonIdx;
      paramsNotice.m_endLonIdx = endTileLonIdx;
      paramsNotice.m_detailLevel = detailLevel;      
      paramsNotice.m_nbrImportances = nbrImportance;
      paramsNotice.m_layerID = layerID;

      // Calculate nbr params. * 2 for the strings.
      nbrParams += ((endTileLatIdx - startTileLatIdx + 1) * 
                    (endTileLonIdx - startTileLonIdx + 1)) *
                   nbrImportance << 1;
      
      paramsNotices[ layerNbr ] = paramsNotice; 
   }
   
   if ( paramsByLayerNbr == paramsNotices ) {
      // Same params as before! No need to create new ones.
      return false;
   } else {
      // Update paramsByLayerNbr.
      paramsByLayerNbr = paramsNotices;
   }
    
   paramVectorOut.resize( nbrParams );
   uint32 index = 0;
   
   for ( map<uint32, ParamsNotice>::const_iterator it = 
            paramsByLayerNbr.begin(); it != paramsByLayerNbr.end(); 
         ++it ) {
      const ParamsNotice& paramsNotice = it->second;
      startTileLatIdx = paramsNotice.m_startLatIdx;
      endTileLatIdx = paramsNotice.m_endLatIdx;
      startTileLonIdx = paramsNotice.m_startLonIdx;
      endTileLonIdx = paramsNotice.m_endLonIdx;
      detailLevel = paramsNotice.m_detailLevel;      
      nbrImportance = paramsNotice.m_nbrImportances;
      uint32 layerID = paramsNotice.m_layerID;
      const RouteID* currRouteID = NULL;
      if ( layerID == TileMapTypes::c_routeLayer ) {
         currRouteID = routeID;
      }
      // TODO: Improvment, fully covered tiles should be first. 
      mc2dbg4 << "[TMFD] createParams: lat: " 
              << minLat << " lon: " << minLon 
              << " scale: " << scale
              << ", nbr importance: " << nbrImportance << endl;
      for ( int i = 0; i < nbrImportance; ++i ) {
         for ( int j = startTileLatIdx; j <= endTileLatIdx; ++j ) {
            for ( int k = startTileLonIdx; k <= endTileLonIdx; ++k ) {
               mc2dbg4 << "[TMFD] createParams: latidx: " << j 
                       << " lonidx: " << k 
                       << " detail: " << detailLevel << endl;
               MC2_ASSERT( index < paramVectorOut.size() );
               paramVectorOut[ index++ ].setParams(
                                    m_serverPrefix,
                                    useGzip,
                                    layerID,
                                    TileMapTypes::tileMapData,
                                    i,
                                    LangTypes::swedish,
                                    j, k, detailLevel,
                                    currRouteID );
            }
         }
      }
   }     
   {for ( map<uint32, ParamsNotice>::const_iterator it = 
            paramsByLayerNbr.begin(); it != paramsByLayerNbr.end(); 
         ++it ) {
      const ParamsNotice& paramsNotice = it->second;
      startTileLatIdx = paramsNotice.m_startLatIdx;
      endTileLatIdx = paramsNotice.m_endLatIdx;
      startTileLonIdx = paramsNotice.m_startLonIdx;
      endTileLonIdx = paramsNotice.m_endLonIdx;
      detailLevel = paramsNotice.m_detailLevel;      
      nbrImportance = paramsNotice.m_nbrImportances;
      uint32 layerID = paramsNotice.m_layerID;
      const RouteID* currRouteID = NULL;
      if ( layerID == TileMapTypes::c_routeLayer ) {
         currRouteID = routeID;
      }

      // Wait with requesting the strings for now.  

      for ( int p = 0; p < nbrImportance; ++p ) {
         for ( int q = startTileLatIdx; q <= endTileLatIdx; ++q ) {
            for ( int r = startTileLonIdx; r <= endTileLonIdx; ++r ) {
               MC2_ASSERT( index < paramVectorOut.size() );
               paramVectorOut[ index++ ].setParams(
                                    m_serverPrefix,
                                    useGzip,
                                    layerID,
                                    TileMapTypes::tileMapStrings,
                                    p,
                                    language, // Language is imortant here.
                                    q, r, detailLevel,
                                    currRouteID );
            }
         }
      }
   }}
   
   MC2_ASSERT( index == paramVectorOut.size() );
   
//   for (vector<TileMapParams>::iterator it = paramVectorOut.begin();
//         it != paramVectorOut.end(); ++it ) {
//      mc2dbg << "TMFD: " << it->getAsString() << endl;
//   }


   // No reserve maps for series60v1.
#ifdef NAV2_CLIENT_SERIES60_V1
   return true;
#endif

   // Now add the reserve params.
   int reserveLatIdx, reserveLonIdx;
   // Hardcoded:
   uint32 reserveLayerNbr = getLayerNbrFromID( TileMapTypes::c_mapLayer );

   getTileIndex( reserveLayerNbr, m_reserveDetailLevel, center.lat, center.lon, 
         reserveLatIdx, reserveLonIdx );
   ParamsNotice reserveNotice;
   reserveNotice.m_startLatIdx = reserveLatIdx - m_extraTilesForReserve;
   reserveNotice.m_endLatIdx = reserveLatIdx + m_extraTilesForReserve;
   reserveNotice.m_startLonIdx = reserveLonIdx - m_extraTilesForReserve;
   reserveNotice.m_endLonIdx = reserveLonIdx + m_extraTilesForReserve;
   reserveNotice.m_detailLevel = m_reserveDetailLevel;
   reserveNotice.m_nbrImportances = 1;
   reserveNotice.m_layerID = TileMapTypes::c_mapLayer;
 
   // Only update the reserveparams if they changed since last time.
   if ( reserveParamsNotice != reserveNotice ) {
   
      reserveParams.resize( ( 2 * m_extraTilesForReserve + 1) *
                            ( 2 * m_extraTilesForReserve + 1) );
      index = 0;   
      for ( int i = reserveNotice.m_startLatIdx; 
            i <= reserveNotice.m_endLatIdx; ++i ) {
         for ( int j = reserveNotice.m_startLonIdx; 
               j <= reserveNotice.m_endLonIdx; ++j ) {
            MC2_ASSERT( index < reserveParams.size() );
            reserveParams[ index++ ].setParams(
                  m_serverPrefix,
                  useGzip,
                  TileMapTypes::c_mapLayer,
                  TileMapTypes::tileMapData,
                  0, // Importance 0 is the country.
                  LangTypes::swedish,
                  i, j, m_reserveDetailLevel,
                  NULL );
         }
      }
      MC2_ASSERT( index == reserveParams.size() );
      reserveParamsNotice = reserveNotice;
   }

   return true;
}

#ifdef MC2_SYSTEM
void
TileMapFormatDesc::getAllParams( set<MC2SimpleString>& allParams,
                                 const MC2BoundingBox& bbox,
                                 const set<int>& layers,
                                 int useGzip,
                                 LangTypes::language_t language,
                                 uint32 minScale )
{
   // Divide the bounding box into many (slow)
   //mc2log << "[TMFD]: Bounding box area = " << bbox.getArea() << endl;

   int64 maxArea = int64(704074)*int64(16);
   maxArea *= maxArea;
   
   if ( bbox.getArea() > maxArea ) {
      // Divide the bounding box into 4 new ones
      vector<MC2BoundingBox> bboxVec;
      bboxVec.resize( 4, bbox );
      int32 halfLat = bbox.getMinLat() + bbox.getHeight() / 2;
      int32 halfLon = bbox.getMinLon() + bbox.getLonDiff() / 2;
      bboxVec[0].setMinLat(halfLat);
      bboxVec[1].setMinLat(halfLat);
      bboxVec[2].setMaxLat(halfLat);
      bboxVec[3].setMaxLat(halfLat);
      bboxVec[0].setMaxLon(halfLon);
      bboxVec[1].setMinLon(halfLon);
      bboxVec[2].setMinLon(halfLon);
      bboxVec[3].setMaxLon(halfLon);
      for ( int i = 0; i < 4; ++i ) {
         getAllParams(allParams, bboxVec[i], layers, useGzip, language,
                      minScale );
      }
      return;
   }
   
   for ( set<int>::const_iterator it = layers.begin();
         it != layers.end();
         ++it ) {
      set<int> tmpLayers;
      tmpLayers.insert(*it);
      uint32 layerNbr = getLayerNbrFromID( *it );

      // Get the interesting scales.
      vector<uint32> interestingScales;
      interestingScales.push_back( minScale );
      int prevScale = 0;
      for ( uint32 i = 0; i < m_tileScaleByLayer[ layerNbr ]->size(); 
            ++i ) {
         interestingScales.push_back( prevScale );
         prevScale = (*m_tileScaleByLayer[ layerNbr ])[ i ].second + 1;
      }

      for ( vector<uint32>::iterator scaleIt = interestingScales.begin();
            scaleIt != interestingScales.end();
            ++scaleIt ) {
         if ( *scaleIt < minScale ) {
            continue;
         }                                        
         //mc2log << "[TMFD]: getAllParams - scale = " << *scaleIt << endl;
         map<uint32, ParamsNotice> paramsByLayerNbr;
         ParamsNotice reserveParamsNotice;
         vector<TileMapParams> paramVectorOut;
         vector<TileMapParams> reserveParams;
         MC2Coordinate center ( bbox.getCenter() );
         innerCreateParams( (const MapProjection*)NULL, // No mapprojv ailable.
                            paramsByLayerNbr,
                            reserveParamsNotice,
                            paramVectorOut,
                            reserveParams,
                            bbox,
                            center,                        
                            *scaleIt,
                            useGzip,
                            language,
                            tmpLayers,
                            (const RouteID*)NULL, // No need to cache RouteID
                            MAX_INT32 );
         allParams.insert( paramVectorOut.begin(), paramVectorOut.end() );
         allParams.insert( reserveParams.begin(), reserveParams.end() );
      }
   }
   // Test that we do not crash
   for( set<MC2SimpleString>::iterator xt = allParams.begin();
        xt != allParams.end();
        ++xt ) {
      getImportanceNbr( TileMapParams( *xt ) );
   }
}

#endif

bool
TileMapFormatDesc::createParams( map<uint32, ParamsNotice>& paramsByLayerNbr,
                                 ParamsNotice& reserveParamsNotice,
                                 vector<TileMapParams>& paramVectorOut,
                                 vector<TileMapParams>& reserveParams,
                                 const MapProjection& projection,
                                 int useGzip,
                                 LangTypes::language_t language,
                                 const set<int>& layers,
                                 const RouteID* routeID,
                                 int detailLevel )
{   
   return innerCreateParams( &projection,
                             paramsByLayerNbr,
                             reserveParamsNotice,
                             paramVectorOut,
                             reserveParams,
                             projection.getBoundingBox(),
                             projection.getCenter(),
                             uint32(projection.getDPICorrectedScale()),
                             useGzip,
                             language,
                             layers,
                             routeID,
                             detailLevel );
}


pair<uint16,uint16> 
TileMapFormatDesc::getScaleRange( const TileMapParams& param ) const
{
   const tileScale_t& tileScale = 
      *m_tileScaleByLayer[ getLayerNbrFromID( param.getLayer() ) ];

   pair<uint16, uint16> scaleRange; // first is min, second max.
  
   if ( param.getDetailLevel() == 0 ) {
      scaleRange.first = 0;
   } else {
      scaleRange.first = tileScale[ param.getDetailLevel() ].second;
   }
   const TileImportanceNotice* importance = getImportanceNbr( param );
   scaleRange.second = 
      MIN( importance->getMaxScale(), 
           tileScale[ param.getDetailLevel() + 1 ].second );

   return scaleRange; 
}
   
float64
TileMapFormatDesc::getPixelScale( float64 meters, int pixels ) 
{
   return meters / pixels;
}

void
TileMapFormatDesc::getBBoxFromTileIndex( uint32 layerID,
                                         MC2BoundingBox& bbox,
                                         int detailLevel,
                                         int tileLatIdx,
                                         int tileLonIdx ) const 
{   
   bbox.reset();
   // Fix for old corrupt clients.
   if ( ( layerID == TileMapTypes::c_routeLayer ) &&
        ( detailLevel > 6 ) ) {
      mc2dbg2 << "[TMFD]: getBBoxFromTileIndex - "
              << "Corrupt client asked for too high detaillevel (" 
              << detailLevel << ") for route. "
              << "Correcting detaillevel to 6 instead." << endl;
      // For old clients there only exists two tiles above detaillevel 6,
      // west and east of Greenwhich.
      if ( tileLonIdx < 0 ) {
         tileLonIdx = -1;
      } else {
         tileLonIdx = 0;
      }

      int mc2UnitsPerTile = MAX_INT32;
      bbox.setMinLat( MIN_INT32 );
      bbox.setMinLon( mc2UnitsPerTile * tileLonIdx );
      bbox.setMaxLat( MAX_INT32 );
      bbox.setMaxLon( bbox.getMinLon() + mc2UnitsPerTile );

   } else {
      const tileScale_t& tileScale = 
         *m_tileScaleByLayer[ getLayerNbrFromID( layerID ) ];
      
      int mc2UnitsPerTile = tileScale[ detailLevel ].first;
      
      bbox.setMinLat( mc2UnitsPerTile * tileLatIdx );
      bbox.setMinLon( mc2UnitsPerTile * tileLonIdx );
      bbox.setMaxLat( bbox.getMinLat() + mc2UnitsPerTile );
      bbox.setMaxLon( bbox.getMinLon() + mc2UnitsPerTile );
   }
   




   bbox.updateCosLat();

}
   
uint16
TileMapFormatDesc::getCoordAndScaleForTile( uint32 layerID,
                                            int detailLevel,
                                            int tileLatIdx,
                                            int tileLonIdx,
                                            TileMapCoord& coord ) const
{
   // Fix for old corrupt clients.
   if ( ( layerID == TileMapTypes::c_routeLayer ) &&
        ( detailLevel > 6 ) ) {
      mc2dbg2 << "[TMFD]: getBBoxFromTileIndex - "
              << "Corrupt client asked for too high detaillevel ("
              << detailLevel << ") for route. "
              << "Correcting detaillevel to 6 instead." << endl;
      // This what the corrupt clients will use for mc2UnitsPerTile.
      // So then we set it to that value here too.
      int mc2UnitsPerTile = MIN_INT32;
      coord.setCoord( mc2UnitsPerTile * tileLatIdx,
                      mc2UnitsPerTile * tileLonIdx );
      // This what the corrupt clients will do. We do the same here.
      return getScaleForDetailLevel( 
            getLayerNbrFromID( TileMapTypes::c_mapLayer ), detailLevel );
   } else {
                                              
      uint32 layerNbr = getLayerNbrFromID( layerID );
      const tileScale_t& tileScale = *m_tileScaleByLayer[ layerNbr ];
      int mc2UnitsPerTile = tileScale[ detailLevel ].first;
      coord.setCoord( mc2UnitsPerTile * tileLatIdx,
                      mc2UnitsPerTile * tileLonIdx );

      return getScaleForDetailLevel( layerNbr, detailLevel );
   }       
}

void
TileMapFormatDesc::initTileSizesForLayer( 
                                  uint32 layerNbr,
                                  int meters,
                                  int pixels,
                                  int /*dpi*/,
                                  float64 zoomFactor, 
                                  float64 exchangeTileFactor,
                                  int detailLevels )
{
   delete m_tileScaleByLayer[ layerNbr ];
   m_tileScaleByLayer[ layerNbr ] = new tileScale_t();
   tileScale_t& tileScale = *m_tileScaleByLayer[ layerNbr ];
   
   float64 prevScale = getPixelScale( (float64) meters, pixels );
   int prevMc2UnitsForTile = 
      int(rint(meters * GfxConstants::METER_TO_MC2SCALE));
   for ( int i = 0; i < detailLevels; ++i ) {
      // The scale has the unit: map meters / pixel
      float64 scale = prevScale * zoomFactor;
      float64 mc2UnitsForTileFloat = rint(prevMc2UnitsForTile * zoomFactor);
      if ( mc2UnitsForTileFloat < MIN_INT32 || 
           mc2UnitsForTileFloat > MAX_INT32 ) {
         // Casting too big / small float to int will result in
         // MIN_INT32 in linux (server). Therefore we must do the same.
         mc2UnitsForTileFloat = MIN_INT32;
      }

      int mc2UnitsForTile = int( mc2UnitsForTileFloat );
      
      mc2dbg4 << "[TMFD] initTileSizes: detail " << i << ", mc2 " 
             << mc2UnitsForTile << " scale " << scale << endl; 
      tileScale.push_back( 
            make_pair( mc2UnitsForTile, 
                       uint16( prevScale + 
                       (scale - prevScale) * exchangeTileFactor ) ) );
      prevScale = scale;
      prevMc2UnitsForTile = mc2UnitsForTile;
   }
}

int
TileMapFormatDesc::getDetailLevel( uint32 layerNbr, uint16 scale ) const
{
   const tileScale_t& tileScale = *m_tileScaleByLayer[ layerNbr ];
   int detailLevel = tileScale.size() - 1; 
   
   vector< pair<int, uint16> >::const_reverse_iterator rIt = 
      tileScale.rbegin();
   bool found = false;

   while ( ( rIt != tileScale.rend() ) && ( ! found ) ) {
      if ( scale > (*rIt).second ) {
         found = true;
      } else {
         ++rIt;
         --detailLevel;
      }
   }

   if ( ! found ) {
      ++detailLevel;
   }

   return detailLevel;
}

int
TileMapFormatDesc::getNbrDetailLevelsForLayerID( uint32 layerID )
{
   return m_tileScaleByLayer[getLayerNbrFromID( layerID )]->size();
}

int 
TileMapFormatDesc::getDetailLevelFromLayerID( uint32 layerID, 
                                              uint16 scale ) const
{
   uint32 layerNbr = getLayerNbrFromID( layerID );
   return getDetailLevel( layerNbr, scale );
}
   
uint16
TileMapFormatDesc::getScaleForDetailLevel( uint32 layerNbr, 
                                           int detailLevel ) const
{
   const tileScale_t& tileScale = 
      *m_tileScaleByLayer[ layerNbr ];
   return tileScale[ detailLevel ].second;
}

// -- Feature stuff

const TileMapFormatDesc::argTransferMap_t&
TileMapFormatDesc::getArgMap(featureInt featureType) const
{
   if ( featureType < int(m_argsPerTypeArraySize) ) {
      return m_argsPerTypeArray[featureType];
   } else {
      return m_emptyTransferMap;
   }
}

void
TileMapFormatDesc::
countNbrFeaturesPerLevel( vector<int>& levelCounts,
                          const vector<TileFeature>& features ) const
{
   levelCounts.resize( getMaxLevel() - getMinLevel() + 1);
   int nbrfeatures = features.size();
   for ( int i = 0 ; i < nbrfeatures; ++i ) {
      const TileFeature * const curFeature = &features[i];
      const featureInt featureType = curFeature->getType();
      if ( featureType < m_primitiveDefaultMapSize ) {         
         // It is assumed that there is no level arg in the primitives
         // if there is one in the complex feature.
         const SimpleArg* levelArg =static_cast<const SimpleArg*>
            (curFeature->getArg( TileArgNames::level ) );
         if ( levelArg ) {
            const argTransferMap_t& argmap = getArgMap( featureType );
            pair<argTransferMap_t::const_iterator,
               argTransferMap_t::const_iterator> findits =
               argmap.equal_range(TileArgNames::level);            
            // The level is in the feature. Add it and continue
            for ( argTransferMap_t::const_iterator it = findits.first;
                  it != findits.second;
                  ++it ) {
               levelCounts[ levelArg->getValue() - getMinLevel() ]++;
            }
         }
         const primitiveVector_t& origVect =
            m_primitiveDefaultMap[featureType];
         
         for( primitiveVector_t::const_iterator it = origVect.begin();
              it != origVect.end();
              ++it ) {
            int level = 13; // Watch out! This is also hard coded in
                           // TileMap::createPrimitives.
            levelArg =static_cast<const SimpleArg*>
               ((*it)->getArg(TileArgNames::level));
            if ( levelArg != NULL ) {
               level = levelArg->getValue();               
            }
            ++levelCounts[level - getMinLevel() ];
         }
      }
   }
}

void
TileMapFormatDesc::
getFeaturePrimitivesDefault(vector<TilePrimitiveFeature*>& primitives,
                            featureInt featureType) const
{
   if ( featureType < m_primitiveDefaultMapSize ) {
      const primitiveVector_t& origVect = m_primitiveDefaultMap[featureType];
      
      for( primitiveVector_t::const_iterator it = origVect.begin();
           it != origVect.end();
           ++it ) {
         // Look here. The default parameters are not deep-copied.
         primitives.push_back(new TilePrimitiveFeature(*(*it)));
      }
   }
}

int
TileMapFormatDesc::countFeaturesForFeature( TileFeature* complexFeature )
{
   if ( complexFeature->getType() < m_primitiveDefaultMapSize ) {
      return m_primitiveDefaultMap[complexFeature->getType()].size();
   } else {
      return 0;
   }
}

void
TileMapFormatDesc::
getFeaturesForFeature(vector<TilePrimitiveFeature*>& primitives,
                      TileFeature* complexFeature) const
{
   mc2dbg8 << "[TMFD]: getFeaturesForFeature("
           << int(complexFeature->getType()) << ")" << endl;
   // Get the vector that tells us which new features we need.
   // These have the constant values filled in already.
   getFeaturePrimitivesDefault(primitives, complexFeature->getType());
   
   // Copy the pointers to the vector of coordinates and pixelbox
   for ( vector<TilePrimitiveFeature*>::iterator it = primitives.begin();
        it != primitives.end(); 
        ++it ) {
      (*it)->setFeatureNbr( complexFeature->getFeatureNbr() );
   }  

 
   // Get the map that maps the names into new params
   // First is the name in the complexFeature,
   // second.first is the index in the featureTypes-vector and
   // second.second is the name in that feature.
   const argTransferMap_t& argmap = getArgMap(complexFeature->getType());

   // For all args - check if they must be moved into primitives
   // or if the defaults can be kept.   
   {for( TileFeature::arg_iterator it = complexFeature->arg_begin();
        it != complexFeature->arg_end();
        ++it ) {
      TileArgNames::tileArgName_t curName = (*it)->getName();
      mc2dbg8 << "[TMH]: Argname = " << MC2CITE(curName) << endl;
      pair<argTransferMap_t::const_iterator,
         argTransferMap_t::const_iterator> findits =
         argmap.equal_range(curName);
      for ( argTransferMap_t::const_iterator findit = findits.first;
            findit != findits.second;
            ++findit ) {
         mc2dbg8 << "<" << findit->first << ", pair<" << findit->second.first
                << "," << findit->second.second << "> >" << endl;
         TileArgNames::tileArgName_t argName = findit->first;
         TileFeatureArg* origArg = complexFeature->getArg( argName );
         if ( origArg == NULL ) {
            mc2dbg8 << "[TMFD]: origArg == NULL " << endl;
         }
         primitives[findit->second.first]->addArg( origArg );
      }      
   }}
}

//

uint32
TileMapFormatDesc::getDefaultBackgroundColor() const
{
   return m_backgroundColor;
}

uint32 
TileMapFormatDesc::getTextColor() const
{
   return m_textColor;
}

uint32 
TileMapFormatDesc::getHorizonTopColor() const
{
   return m_horizonTopColor;
}

uint32
TileMapFormatDesc::getHorizonBottomColor() const
{
   return m_horizonBottomColor;
}

const TileImportanceNotice*
TileMapFormatDesc::getImportanceNbr( const TileMapParams& param ) const
{
   uint32 layerNbr = getLayerNbrFromID( param.getLayer() );
   return m_importanceTables[ layerNbr ]->getImportanceNbr( 
                                              param.getImportanceNbr(),
                                              param.getDetailLevel() );
}

uint32
TileMapFormatDesc::getCRC() const
{
   return m_crc;
}

uint32
TileMapFormatDesc::getTimeStamp() const
{
   return m_timeStamp;
}
   
MC2SimpleString 
TileMapFormatDesc::getStaticCopyrightString() const
{
   // We ignore the bbox.
   return m_copyrightString;
}

const TileMapLayerInfo*
TileMapFormatDesc::getLayerInfo( uint32 layerID ) const
{
   return m_layerIDsAndDesc.getLayerInfo( layerID );
}

int
TileMapFormatDesc::hasLayerID( uint32 layerID ) const
{
   map<uint32, uint32>::const_iterator it = m_layerNbrByID.find( layerID );
   return it != m_layerNbrByID.end();
}

int
TileMapFormatDesc::canExpire( const TileMapParams& params ) const
{
   const TileMapLayerInfo* layerInfo = getLayerInfo( params.getLayer() );
   MC2_ASSERT( layerInfo != NULL );
   return layerInfo->isTransient();
}

int
TileMapFormatDesc::allowedOnDisk( const TileMapParams& params ) const
{
   return ! canExpire( params );
}


uint32
TileMapFormatDesc::getUpdatePeriodMinutes( const TileMapParams& params ) const
{
   const TileMapLayerInfo* layerInfo = getLayerInfo( params.getLayer() );
   if ( layerInfo == NULL ) {
      return 0;
   }
   return layerInfo->getUpdatePeriodMinutes();
}

int
TileMapFormatDesc::checkFetching( const TileMapParams& params ) const
{
   const TileMapLayerInfo* layerInfo = getLayerInfo( params.getLayer() );
   if ( layerInfo == NULL ) {
      return 1;
   }
   if ( params.getTileMapType() == TileMapTypes::tileMapStrings ) {
      return layerInfo->alwaysFetchStrings();
   } else {
      return 1;
   }
}


// -- Class TileMapFormatDescCRC -----------------------------------

TileMapFormatDescCRC::TileMapFormatDescCRC( uint32 crc )
{
   m_crc = crc;
}

bool
TileMapFormatDescCRC::save( SharedBuffer& buf ) const
{
   if ( buf.getNbrBytesLeft() >= 5 ) {
      buf.writeNextByte( 0 );
      buf.writeNextBALong( m_crc );
      return true;
   } else {
      return false;
   }
}

bool
TileMapFormatDescCRC::load( SharedBuffer& buf )
{
   if ( buf.getNbrBytesLeft() >= 5 ) {
      buf.readNextByte();
      m_crc = buf.readNextBALong();
      return true;
   } else {
      return false;
   }
}

uint32
TileMapFormatDescCRC::getCRC() const
{
   return m_crc;
}

MC2SimpleString
TileMapFormatDescCRC::createParamString( LangTypes::language_t lang,
                                         const char* clientTypeString,
                                         const char* randChars,
                                         bool nightMode )
{
   MC2SimpleString formatDescStr =
      TileMapFormatDesc::createParamString( lang, clientTypeString,
                                            randChars, nightMode );
   // Make the first letter a C. Unfortunately we will have to copy
   // the string twice or more
   char* tmpStr = new char[formatDescStr.length() + 1 ];
   strcpy( tmpStr, formatDescStr.c_str() );
   if( nightMode ){
      tmpStr[0] = 'c';
   }
   else{
      tmpStr[0] = 'C';
   }
   return MC2SimpleStringNoCopy( tmpStr );
}

void TileMapFormatDesc::setNotifier(MapLibSettingsNotifier* notifier)
{
   m_settingsNotifier = notifier;
   if (m_settingsNotifier != NULL && m_unNotifiedChanges){
      m_settingsNotifier->notifyUpdatedPOICategories();
      m_unNotifiedChanges = false;
   }
}

