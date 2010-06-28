#ifndef IMAGEMAPGRIDBOX_H
#define IMAGEMAPGRIDBOX_H

#include "Geometry/BoundingBox.h"

/**
 *  This represents a bounding box inside a certain
 *  level in image grid coordinate space for a zoom range.
 */ 
struct GridBox {
   /**
    * The bounding box in the image grid coordinate space.
    */
   BoundingBox box;

   /**
    * The zoom index in the image grid coordinate space.
    */
   int zoomIndex;

#ifndef _MSC_VER
/**
 * Print on ostream.
 *
 * @param stream The stream to print on.
 * @param bbox   The bbox to print.
 * @return The stream.
 */
   friend ostream& operator<< ( ostream& stream, const GridBox& bbox );
#endif
};


#endif /* IMAGEMAPGRIDBOX_H */
