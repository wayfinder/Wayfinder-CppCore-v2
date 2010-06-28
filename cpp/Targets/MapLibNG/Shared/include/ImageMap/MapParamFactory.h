#ifndef MAPPARAMFACTORY_H
#define MAPPARAMFACTORY_H

#include "ImageMap/MapParam.h"

class MC2SimpleString;
class BoundingBox;

/**
 *  The purpose of this class is to abstract different ways of requesting maps
 *  from a server. 
 */ 
class MapParamFactory {
public:
   /**
    *  Destructor. 
    */ 
   virtual ~MapParamFactory() {}
   
   /**
    *  Given a request string the implementing class will return a suitable
    *  parameter that represents a downloaded map.
    *
    *  @param descr The parameter description of the map.
    *  @return a parameter object representing the map.
    */ 
   virtual MapParam getParamFromString(const MC2SimpleString& descr) = 0;

   /**
    *  Given a bounding box and a corresponding scale, the implementing
    *  class will return a vector of request parameters that represents
    *  the box and scale.
    *
    *  @param bbox The bounding box which covers the area in world coordinates.
    *  @param scale The world scale.
    *  @return a vector of parameters.
    */ 
   virtual void getParamsFromWorldBox(std::vector<MapParam>& ret,
                                      const BoundingBox& bbox,
                                      double worldScale) = 0;
   
   /**
    *  Converts the supplied world scale into a discrete zoom index.
    *
    *  @param worldScale The world scale to use.
    *  @return The discrete zoom index corresponding to the worldScale
    *  argument.
    */ 
   virtual int worldScaleToZoomIndex(double worldScale) = 0;

   /**
    *  @ return a test server that this parameter factory can work with.
    */
   virtual const char* getTestHostAndPort() = 0;
};

#endif /* MAPPARAMFACTORY_H */
