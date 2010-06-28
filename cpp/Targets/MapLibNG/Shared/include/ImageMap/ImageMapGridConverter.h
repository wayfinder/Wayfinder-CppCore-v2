#ifndef IMAGEMAPGRIDCONVERTER_H
#define IMAGEMAPGRIDCONVERTER_H

#include "ImageMap/ImageMapGrid.h"
#include "ImageMap/GridBox.h"
#include "Util/NotCopyable.h"

/**
 * Class responsible for mapping between internal map coordinate space 
 * (such as mercator) and image grid coordinate space.
 *
 * Note that this is in fact a very light-weight projection system.
 * It could very well be using matrices instead, but in the general
 * interest of simplicity it does not.
 */
class ImageMapGridConverter : public NotCopyable {
public:
   friend class MercatorMathTests;
  
   /**
    *  Constructor.
    *
    *  @param range the ImageMapGrid.
    */
   ImageMapGridConverter(const ImageMapGrid& range);

   /**
    *  Destructor.
    */
   ~ImageMapGridConverter();
   
   /**
    *  This method performs a mapping from mercator coordinate space
    *  to image grid coordinate space for a certain bounding box and 
    *  scale level.
    *
    *  This method converts the bounding box worldBox from the world projection
    *  space (MIN_INT to MAX_INT):
    *
    *
    *                  |
    *    +-------------+---------------+
    *    |             |               |
    *    |             |               |
    *    |             |               |
    *    |             |               |
    *    |             |               |
    *  --+-------------+---------------+--
    *    |             |               |
    *    |             |               |
    *    |             |               |
    *    |             |               |
    *    |             |               |
    *    +-------------+---------------+
    *                  |
    *
    *   (Fixed point world coordinate space)
    *
    *  To the corresponding image grid bounding box by doing scale. Which scale
    *  factor to use depends upon the world scale supplied, since there are
    *  different extents on different levels.
    *
    *                  |
    *                  |
    *                  |
    *                  |
    *             +----+----+
    *             |    |    |
    *    ---------+----+----+----------
    *             |    |    |
    *             +----+----+
    *                  |
    *                  |
    *                  |
    *                  |
    *
    *     (Screen pixel coordinate space)
    *
    *  In short, the algorithm begins by retrieveing the correct scaling
    *  factor by looking up a precomputed table of factors. Then, it
    *  scales the corners of the bounding box by using this factor.
    *    
    *  @param worldBox The box in mercator coordinate space.
    *  @param scale The scale level, as from the map projection.
    *  @return The corresponding zoom level and box in image grid 
    *          coordinate space.
    */
   GridBox worldToGrid(const BoundingBox& worldBox, double scale) const;

   /**
    *  This method performs a mapping from image grid coordinate space to 
    *  mercator coordinate space.
    *
    *  @param res The imagegrid box (zoom level + box)
    *  @return The corresponding bounding box in mercator coordinate space.
    */
   BoundingBox gridToWorld(GridBox res);

   /**
    *  Get the corresponding image grid discrete zoom level for the
    *  map projection scale.
    *
    *  @param scale The map projection scale.
    *  @return The corresponding discrete image grid zoom level 
    *          (NB! starts at 1, not 0 as may be expected)
    */
   int worldScaleToZoomIndex(double scale) const;
private:

   /**
    * Translation details for an image grid zoom level.
    */
   struct ConversionDetails {
      /// The image grid zoom level.
      int zoomIndex;

      /**
       *  The factor between a image grid coordinate for this
       *  zoom level index and for a mercator coordinate.
       */
      double scalingFactor;
   };

   /**
    *  Get translation details for the supplied image grid zoom level index.
    * 
    *  @param index The image grid zoom level index. Starts at 1.
    *  @return The ConversionDetails.
    */
   ConversionDetails getConversionDetails(int index) const;
   
   struct Impl;
   Impl* m_impl;
};

#endif /* IMAGEMAPGRIDCONVERTER_H */
