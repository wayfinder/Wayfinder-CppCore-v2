#ifndef WFMAPPARAMFACTORY_H
#define WFMAPPARAMFACTORY_H

#include "ImageMap/MapParamFactory.h"
#include "Util/NotCopyable.h"

class ImageMapGrid;

/**
 *  This class provides a concrete implementation of the
 *  protocol used to request image map tiles from a WF server.
 */ 
class WFMapParamFactory : public MapParamFactory,
                          public NotCopyable {
public:
   /**
    *  Creates a new WF map parameter factory.
    */ 
   WFMapParamFactory();

   /***
    *  Destructor.
    */  
   virtual ~WFMapParamFactory();
   
   /**
    *   @see MapParamFactory::getParamFromString
    */ 
   MapParam getParamFromString(const MC2SimpleString& string);

   /**
    *   @see MapParamFactory::getParamsFromWorldBox
    */ 
   void getParamsFromWorldBox(std::vector<MapParam>& out,
                              const BoundingBox& bbox,
                              double worldScale);

   /**
    *   @see MapParamFactory::worldScaleToZoomIndex
    */ 
   int worldScaleToZoomIndex(double worldScale);

   /**
    *   @see MapParam::getTestHostAndPort
    */ 
   const char* getTestHostAndPort();
private:
   /**
    *  Given a tile index (x, y) and a corresponding zoom index,
    *  this method will return a bounding box in world coordinates.
    */ 
   BoundingBox getWorldBoxForParam(int x, int y, int zoomIndex);
      
   struct Impl;
   Impl* m_impl;
};

#endif /* WFMAPPARAMFACTORY_H */
