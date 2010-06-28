/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef SEARCH_RECORD_H
#define SEARCH_RECORD_H

#include "arch.h"
#include "nav2util.h"
#include <string.h>

using namespace isab;

/**
 * Class that describes one search record for a combined search
 */
class SearchRecord 
{
 public:

   /**
    * Default constructor
    */
   SearchRecord( ) :
      m_regionId(0), 
      m_areaId(NULL),
      m_areaName(NULL), 
      m_searchString(NULL),
      m_categoryId(MAX_UINT32),
      m_origLat(MAX_INT32), 
      m_origLon(MAX_INT32),
      m_index(0),
      m_maxHits(25),
      m_heading(MAX_UINT32),
      m_round(MAX_UINT32)
      {}
   
   /**
    * Constructro creating an instance of
    * this class
    * @param regionId, The region id
    * @param areaId, The area id
    * @param areaName, Name of the area
    * @param searchString, The search string for this search
    * @param categoryId, The id of the category
    * @param origLat, Latitude for this search
    * @param origLon, Longtitude for this search
    */
   SearchRecord( unsigned int regionId,
                 const char* areaId,
                 const char* areaName,
                 const char* searchString,
                 uint32 categoryId,
                 int32 origLat = MAX_INT32,
                 int32 origLon = MAX_INT32,
                 uint32 index  = 0,
                 uint32 maxHits = 25,
                 uint32 heading = MAX_UINT32,
                 uint32 round = MAX_UINT32
) : 
      m_regionId( regionId ), 
      m_categoryId(categoryId),
      m_origLat( origLat ), 
      m_origLon( origLon ),
      m_index(index),
      m_maxHits(maxHits),
      m_heading(heading),
      m_round(round)
   {
      // Copy, no worry if NULL result will dest
      // will be null if source is NULL
      m_areaId = strdup_new(areaId);
      m_areaName = strdup_new(areaName);
      m_searchString = strdup_new(searchString);
   }

   /**
    * Copy constructor, creates a copy of sr.
    * @param sr, the SearchRecord to be copied.
    */
   SearchRecord( const SearchRecord& sr) :
      m_regionId(sr.m_regionId),
      m_categoryId(sr.m_categoryId),
      m_origLat(sr.m_origLat ), 
      m_origLon(sr.m_origLon ),
      m_index(sr.m_index),
      m_maxHits(sr.m_maxHits),
      m_heading(sr.m_heading),
      m_round(sr.m_round)
   {
      m_areaId = strdup_new(sr.m_areaId);
      m_areaName = strdup_new(sr.m_areaName);
      m_searchString = strdup_new(sr.m_searchString);
   }
   
   ~SearchRecord() {
      delete[] m_areaId;
      delete[] m_areaName;
      delete[] m_searchString;
   }
   
   /**
    * Simple set function. Sets the area id.
    * @param areaId, the new area id.
    */
   void setAreaId(const char* areaId);

   /**
    * Simple set function. Sets the area name.
    * @param areaName, the new area name.
    */
   void setAreaName(const char* areaName);

   /**
    * Simple set function. Sets the search string.
    * @param searchString, the new search string.
    */
   void setSearchString(const char* searchString);

   /**
    * Simple set function. Sets the category id.
    * @param categoryId, the new category id.
    */
   void setCategoryId(uint32 categoryId);

   /**
    * Simple set function. Sets the index.
    * @param index, the new index.
    */
   void setIndex(uint32 index);

   /**
    * Simple set function. Sets max hits.
    * @param maxHits, the new max hits.
    */
   void setMaxHits(uint32 maxHits);
   /**
    * Simple set function. Sets the heading.
    * @param heading, the heading .
    */
   void setHeading(uint32 heading);

   /**
    * Set the round to search in, by default this is set to
    * MAX_UINT32 which means that we will search in all possible
    * rounds.
    *
    * @param round The round to search in. If search should be done in all
    *              available rounds set it to MAX_UINT32-
    */
   void setRound(uint32 round);
   
   /**
    * Ovarloading of assignment operator that handles
    * self assignment.
    * @param sr The searchRecord that should be copied.
    * @return The copied searchRecord, this that now contains
    *         the copied values.
    */
   SearchRecord& operator= (const SearchRecord& sr);

   /// Region id for this search
   uint32 m_regionId;
   /// Area id for this search
   char* m_areaId;
   /// Area name of this search
   char* m_areaName;
   /// Search string for this search
   char* m_searchString;
   /// Category id for this search
   uint32 m_categoryId;
   /// Latitude for this search
   int32 m_origLat;
   /// Longtitude for this search
   int32 m_origLon;

   /// Starting index of the search query.
   uint32 m_index;

   /// Max number of hits.
   uint32 m_maxHits;

   /// The heading. MAX_UINT32 if all headings should be searched.
   uint32 m_heading;

   ///
   uint32 m_round;

};

inline void SearchRecord::setAreaId(const char* areaId)
{
   if (m_areaId)
      // Delete if existing
      delete[] m_areaId;

   // Copy new area id
   m_areaId = strdup_new(areaId);
}

inline void SearchRecord::setAreaName(const char* areaName)
{
   if (m_areaName)
      // Delete if existing
      delete[] m_areaName;

   // Copy new area id
   m_areaName = strdup_new(areaName);   
}

inline void SearchRecord::setSearchString(const char* searchString)
{
   if (m_searchString) {
      // Delete if no null
      delete[] m_searchString;
   }

   // Copy new searchString
   m_searchString = strdup_new(searchString);
}

inline void SearchRecord::setCategoryId(uint32 categoryId)
{
   m_categoryId = categoryId;
}

inline void SearchRecord::setIndex(uint32 index)
{
   m_index = index;
}

inline void SearchRecord::setMaxHits(uint32 maxHits)
{
   m_maxHits = maxHits;
}

inline void SearchRecord::setHeading(uint32 heading)
{
   m_heading = heading;
}

inline void SearchRecord::setRound(uint32 round)
{
   m_round = round;
}

inline SearchRecord& SearchRecord::operator= (const SearchRecord& sr) 
{
   if( this == &sr ) {
      // If same object, just return this
      return *this;
   }
   // Delete if exisiting
   if(m_areaId) 
      delete[] m_areaId;
   if(m_areaName) 
      delete[] m_areaName;
   if(m_searchString) 
      delete[] m_searchString;

   // Copy all records
   m_regionId = sr.m_regionId; 
   m_areaId = strdup_new(sr.m_areaId);
   m_areaName = strdup_new(sr.m_areaName);
   m_searchString = strdup_new(sr.m_searchString);

   m_categoryId = sr.m_categoryId;
   m_origLat = sr.m_origLat;
   m_origLon = sr.m_origLon;

   m_index = sr.m_index;
   m_maxHits = sr.m_maxHits;
   m_heading = sr.m_heading;
   m_round = sr.m_round;

   return *this;
}
#endif
