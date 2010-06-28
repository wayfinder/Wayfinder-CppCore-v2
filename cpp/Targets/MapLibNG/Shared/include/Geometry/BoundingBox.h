#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include "MC2Point.h"

// Unitless point
typedef MC2Point UPoint;

class MC2BoundingBox;

#define LESS_THAN( a,b ) ( ( ( (a) - (b) ) <= 0 ) )
#define IN_RANGE(a,b,c) ( LESS_THAN(a,b) && LESS_THAN(b,c) )

/**
 *  Unit less bounding box.
 *  The context of when this class determines how to treat the contents.
 */
class BoundingBox {
public:
   
   /**
    *  Empty constructor.
    */
   BoundingBox() : topLeft(0, 0), bottomRight(0, 0)
   {}

   /**
    *  Constructor with MC2BoundingBox.
    *
    *  @param bbox The MC2BoundingBox.
    */
   BoundingBox(const MC2BoundingBox& bbox);


   BoundingBox(const UPoint& topLeft, const UPoint& bottomRight) :
      topLeft(topLeft),
      bottomRight(bottomRight)
   {
      
   }

   bool 
   overlaps( const BoundingBox& b ) const {
      return (b.topLeft.getY() >= bottomRight.getY()) && (
         topLeft.getY() >= b.bottomRight.getY()) &&
         ( IN_RANGE( topLeft.getX(), b.topLeft.getX(), bottomRight.getX() ) ||
           IN_RANGE( topLeft.getX(), b.bottomRight.getX(), bottomRight.getX() ) ||
           IN_RANGE( b.topLeft.getX(), topLeft.getX(), b.bottomRight.getX() ) ||
           IN_RANGE( b.topLeft.getX(), bottomRight.getX(), b.bottomRight.getX() ) );
   }
   
   /**
    *  Returns true if rhs is geometrically equal to this object.
    */ 
   bool operator==(const BoundingBox& rhs) const {
      return topLeft == rhs.topLeft && bottomRight == rhs.bottomRight;
   }
   
   /// The top left point.
   UPoint topLeft;

   /// The bottom right point.
   UPoint bottomRight;
};

#undef IN_RANGE
#undef LESS_THAN

#ifndef _MSC_VER
/**
 * Print on ostream.
 *
 * @param stream The stream to print on.
 * @param bbox   The bbox to print.
 * @return The stream.
 */
ostream& operator<< ( ostream& stream, const BoundingBox& bbox );
#endif

#endif /* BOUNDINGBOX_H */
