#ifndef IMAGEMAPGRID_H
#define IMAGEMAPGRID_H

#include "Geometry/BoundingBox.h"
#include "Util/NotCopyable.h"

/**
 *    Class defining the layout of the image map grid, i.e.
 *    the number of discrete zoom levels, the geospatial indexing etc.
 */
class ImageMapGrid {
public:
   /**
    *  ImageMapGrid constructor.
    *
    *  @param zoomInLevel The index of the zoom level when the grid is
    *                     completely zoomed in.
    *  @param zoomOutLevel The index of the zoom level when the grid is
    *                      completely zoomed out.
    *  @param minX   At the most zoomed out level, the minimum x value.
    *  @param minY   At the most zoomed out level, the minimum y value.
    *  @param maxX   At the most zoomed out level, the maximum x value.
    *  @param maxY   At the most zoomed out level, the maximum y value.
    *  @param gridSize The static x and y increment between the different
    *                  geospatial indeces.
    *  @param subfactor The factor between the coordinate space for two adjacent zoomlevels.
    *                   I.e. the width of the world at zoom level 2 is 
    *                   (maxX - minY)*subfactor.
    *  @param yPointsUp Whether the y-axis points upwards or downwards from origo.                 
    */ 
   ImageMapGrid(int zoomInLevel,
                int zoomOutLevel,
                int minX, int minY,
                int maxX, int maxY,
                int gridSize,
                int subfactor);

   /**
    *  Copy constructor
    */ 
   ImageMapGrid(const ImageMapGrid& rhs);
   
   /**
    *    Destructor.
    */
   ~ImageMapGrid();
   
   /**
    *  Get the bounding box extent (for the entire world) 
    *  for the specified zoom level.
    *  
    *  Zoomlevel 0 is the most zoomed out zoom level.
    *
    *  @param level  The zoom level.
    *  @return The corresponding bounding box for the zoom level.
    */
   BoundingBox getCompleteImageGridBox(int level) const;

   /**
    *  @return The number of unique levels in the zoom range.
    */ 
   int getNumLevels() const;

   /**
    *  @return The grid size that defines the zoom range
    */ 
   int getGridSize() const;

   /**
    *   @return Whether or not the Y-axis points upwards in this
    *           map grid projection. If so, the axis should be inverted
    *           when converting from and to mercator space and world space.
    */ 
   bool getYPointsUp() const;
   
   /**
    *   @return true if rhs is equal in all aspects to this.
    */ 
   bool operator==(const ImageMapGrid& rhs) const;
private:
   ImageMapGrid& operator = (const ImageMapGrid& );
   
   struct Impl;
   Impl* m_impl;
};

#endif /* IMAGEMAPGRID_H */
