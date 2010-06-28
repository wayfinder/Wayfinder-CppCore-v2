#ifndef OSMMAPPARAMFACTORY_H
#define OSMMAPPARAMFACTORY_H

#include "ImageMap/MapParamFactory.h"
#include "Util/NotCopyable.h"
#include "ImageMap/GridBox.h"

class ImageMapGrid;

/**
 *  This class provides a concrete implementation of the
 *  protocol used to request image map tiles from an OpenStreetMap server.
 */
class OSMMapParamFactory : public MapParamFactory,
                           public NotCopyable {
public:
   friend class OSMMapFactoryTest;

   /**
    *  Creates a new OSM map parameter factory.
    */

   OSMMapParamFactory();

   /**
    *  Destructor.
    */
   virtual ~OSMMapParamFactory();

   /**
    *   @see MapParamFactory::getParamFromString
    */
   MapParam getParamFromString(const MC2SimpleString& string);

   /**
    *   @see MapParamFactory::getParamsFromWorldBox
    */
   void getParamsFromWorldBox(std::vector<MapParam>& ret,
                              const BoundingBox& bbox,
                              double worldScale);

   /**
    *   @see MapParamFactory::worldScaleToZoomIndex
    */
   int worldScaleToZoomIndex(double worldScale);

   /**
    *   @see MapParamFactory::getTestHostAndPort
    */ 
   const char* getTestHostAndPort();
private:

   /**
    *  Like getParamsFromWorldBox, but operates in IG coordinate space instead.
    */ 
   void getParamsFromGridBox(std::vector<MapParam>& out,
                             const GridBox& box);
   
   /**
    *  Maps directly to @see ImageMapGridConverter::worldToGrid
    */
   GridBox getGridBox(const BoundingBox& bbox, double worldScale);

   /**
    *  Since the OSM maps are defined using only positive screen
    *  coordinates (the upper right quadrant of the pixel coordinate
    *  system) we need to shift the previously scaled bounding box
    *  into this quadrant. We do this by looking up the world extent
    *  corresponding to the zoom index.
    *
    *
    *            |                                 |
    *            |                                 +-----+
    *         +--+--+               /----->        |     |
    *     ----+--+--+-----    -----/          -----+-----+---
    *         +--+--+                              |
    *            |                                 |
    *            |                                 |
    *
    *             
    */
   void offsetGridBox(BoundingBox& bbox, int zoomIndex);

   /**
    *  Like the previous method, but in the other direction (the inverse).
    */ 
   void invOffsetGridBox(BoundingBox& bbox, int zoomIndex);
   
   /**
    *  In order for this to function correctly, this method should
    *  be called after offsetGridBox. Since OSM maps are
    *  defined using an inverted Y-axis (compared to our map projection),
    *  we need to compensate for this. The algorithm works like this:
    *
    *  ^                       ^   
    *  |- - - - - - -          |- - - - +----+
    *  |            |          |        |    |
    *  |            |          |        +----+
    *  |       +----+   --->   |             |
    *  |       |    |          |             |
    *  +-------+----+->        +------------->
    */
   void invertGridBoxYAxis(GridBox& gb);

   /**
    *  Like the previous method, but in the other direction (the inverse).
    */ 
   void invInvertGridBoxYAxis(GridBox& gb);

   /**
    *  Given the components of a request param (x, y, and zoomIndex)
    *  this method calculates the corresponding bounding box expressed
    *  in world coordinates.
    *
    *  @param x The x index of the tile
    *  @param y The y index of the tile
    *  @param zoomIndex The zoom index of the tile
    *  @return The bounding box in world coordinates.
    */ 
   BoundingBox getWorldBoxForParam(int x, int y, int zoomIndex);

   /**
    *  Helper method for calculating the translation offsets, since the
    *  same equations are used both in the normal method and the inverse.
    */ 
   UPoint getTranslationOffsets(int zoomIndex);
         
   struct Impl;
   Impl* m_impl;
};

#endif /* OSMMAPPARAMFACTORY_H */
