#ifndef MAPPARAM_H
#define MAPPARAM_H

#include "Geometry/BoundingBox.h"
#include "MC2SimpleString.h"

class MapParam {
public:
   /**
    *  Creates a new map parameter. Should not be
    *  used directly but rather through a parameter
    *  factory.
    */ 
   MapParam(int zoomIndex,
            BoundingBox worldBox,
            const MC2SimpleString& string,
            int x,
            int y);

   /**
    *  Default constructor, resulting parameter will be invalid.
    */ 
   MapParam();

   /**
    *   @return The zoom index of the map this parameter represents.
    */ 
   int getZoomIndex() const;

   /**
    *   @return The bounding box that the map represented
    *           by this parameter covers.
    */
   BoundingBox getWorldBox() const;

   /**
    *   @return The string that should be used when requesting the map
    *           that this parameter represents.
    */ 
   const MC2SimpleString& getString() const;

   /**
    *  Returns true if this parameters is considered valid, i.e.
    *  it contains a correct request string.
    */ 
   bool isValid() const;
   
   /**
    *   Comparison operator. Operates solely on the string member.
    *
    *   @return true if str compares to less than internal string.
    */ 
   bool operator<(const char* str) const;
   bool operator<(const MapParam& rhs) const;

   /**
    *   Comparison operator. Operates solely on the string member.
    *
    *   @return true if str compares equal with internal string.
    */ 
   bool operator==(const char* str) const;
   bool operator==(const MapParam& rhs) const;
private:
   int m_x;
   int m_y;
   int m_zoomIndex;
   BoundingBox m_worldBox;
   MC2SimpleString m_string;
};

#endif /* MAPPARAM_H */
