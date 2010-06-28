#ifndef PROJECTIONINTERFACE_H
#define PROJECTIONINTERFACE_H

class ProjectionCoordinate;
class MapProjection;
class ExternalRenderer;

class ProjectionInterface {
public:
   ProjectionInterface(MapProjection& projection,
                       ExternalRenderer& renderer);
   
   /**
    *  Returns an optimized coordinate that can be used together with
    *  this projection interface.
    *
    *  @param wgs84lat The latitude of the coordinate
    *  @param wgs84lon The longitude of the coordinate
    *  @param outCoord The output coordinate
    */ 
   void getProjectionCoordinate(float wgs84lat,
                                float wgs84lon,
                                ProjectionCoordinate* outCoord);
   
   /**
    *  Projects the world position of coord to screen space.
    *    
    *  @param coord The coordinate that should be projected to screen space
    *  @param x The float out parameter for the x pixel coordinate
    *  @param y The float out parameter for the y pixel coordinate
    *  @param z The float out parameter for the z pixel coordinate
    */
   void projectf(const ProjectionCoordinate& coord,
                 float* x,
                 float* y,
                 float* z);
private:
   MapProjection& m_projection;
   ExternalRenderer& m_renderer;
};

#endif /* PROJECTIONINTERFACE_H */
