/*
  Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
  * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef WFAPI_MAPOBJECTINFO_H
#define WFAPI_MAPOBJECTINFO_H

#include "PALMachineTypes.h"
#include "WFString.h"

namespace WFAPI {

// Forward declarations
class WFString;

/**
 * Class which contains information about an object on the map.
 * The MapObjectInfo may either be added to an OverlayItem
 * onto the map, or it may represent a map object, integrated into
 * the map (such as a street).
 */
class MapObjectInfo {
public:
   /**
    * The type of map object info.
    *
    * A set of predefined constants exists, but it is also
    * allowed to add more types if needed.
    */
   typedef wf_int32 map_object_info_t;   

   /// Map object info type for integrated map features.
   static const map_object_info_t INVALID_FEATURE = WF_MAX_INT32;

   /// Map object info type for integrated map features.
   static const map_object_info_t INTEGRATED_MAP_FEATURE = WF_MAX_INT32 - 1;
   
   /// Map object info type for contacts.
   static const map_object_info_t CONTACT = 0;
   
   /// Map object info type for images.
   static const map_object_info_t IMAGE = 1;
   
   /// Map object info type for videos.
   static const map_object_info_t VIDEO = 2;
   
   /// Map object info type for user generated POIs.
   static const map_object_info_t USER_GENERATED_POI = 3;

   /// Map object info type for other types.
   static const map_object_info_t OTHER_TYPE = 4;

   /**
    * The type of ID that is used, integer key, or string.
    * Either may be used, depending on what is most appropriate.
    * ID_KEY is recommended to use if possible to avoid extra
    * allocations.
    */
   enum idType {
      /// An integer based id-type
      ID_KEY,
      /// A string based id-type
      ID_STRING
   };

   /**
    * Construct the MapObjectInfo with supplied parameters.
    *
    * @param name The descriptive name in utf8. The name will be copied.
    * @param type The type of map object info.
    * @param idKey The identifier of this feature, stored as an int.
    * @param genericData Generic data to be associated with this object.
    *                    Defaulted to NULL so user don't have to bother
    *                    if not needed.
    */
   MapObjectInfo(const WFString& name,
                 map_object_info_t type,
                 wf_int32 idKey,
                 void* genericData = NULL);

   /**
    * Construct the MapObjectInfo with supplied parameters.
    *
    * @param name The descriptive name in utf8. The name will be copied.
    * @param type The type of map object info.
    * @param idString The identifier of this feature, stored as a string.
    *                 The string will be copied.
    * @param genericData Generic data to be associated with this object.
    *                    Defaulted to NULL so user don't have to bother
    *                    if not needed.
    */
   MapObjectInfo(const WFString& name,
                 map_object_info_t type,
                 const WFString& idString,
                 void* genericData = NULL);
   
   /**
    * Get the name (utf8) of the map object info.
    *
    * @return The name of the map object info in utf8.
    */
   const WFString& getName() const;

   /**
    * Get the type of map object info.
    *
    * @return The type of map object info.
    */
   map_object_info_t getType() const;

   /**
    * Get which type of ID that is used, ID_KEY or ID_STRING.
    *
    * @return The type of ID that is used.
    */
   idType getIDType() const;

   /**
    * Get the id key. Valid if getIDType() returns ID_KEY.
    *
    * @return The id key.
    */
   wf_int32 getIDKey() const;

   /**
    * Get the id string. Valid if getIDType() returns ID_STRING.
    *
    * @return The id string.
    */
   const WFString& getIDString() const;

   /**
    * To set the generic data for this object.
    *
    * @param genericData A pointer to the generic data.
    */
   void setGenericData(void* genericData);
   
   /**
    * Get the generic data for this object.
    *
    * @return The generic data.
    */
   void* getGenericData() const;
   
private:
   /// The name of the map object
   const WFString m_name;
   
   /// The type of the object
   const map_object_info_t m_type;
   
   /// The type of ID we are using, string or integer
   idType m_idType;

   /// The id of the object, if id type is key
   const wf_int32 m_idKey;

   /// The id of the object, if id type is string
   const WFString m_idString;

   /// Generic data to be associated with this object.
   void* m_genericData;
};

} // End of namespace WFAPI

#endif // WFAPI_MAPOBJECTINFO_H
