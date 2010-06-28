#ifndef MAPPROJECTIONPROXY_H
#define MAPPROJECTIONPROXY_H

#include "MapOperationInterface.h"
#include "PALMachineTypes.h"

class MapProjection;
class ExternalRenderer;

/**
 *  The purpose of this class is to match API calls against
 *  our internal map projection. It needs access to an
 *  ExternalRenderer in order to perform 3D projection
 *  related operations.
 */ 
class MapProjectionProxy : public WFAPI::MapOperationInterface {
public:
   MapProjectionProxy(MapProjection& projection,
                      ExternalRenderer* extRenderer);
   
   virtual ~MapProjectionProxy(){};

   /**
    *  @see MapOperationInterface::
    */ 
   virtual WFAPI::WGS84Coordinate getCenter() const;
   /**
    *  @see MapOperationInterface::
    */ 

   virtual void worldToScreen(WFAPI::ScreenPoint& point,
                              const WFAPI::WGS84Coordinate& coord) const;
   
   /**
    *  @see MapOperationInterface::
    */ 
   virtual void screenToWorld(WFAPI::WGS84Coordinate& coord,
                              const WFAPI::ScreenPoint& point) const;
   
   /**
    *  @see MapOperationInterface::
    */ 
   virtual void setCenter(const WFAPI::WGS84Coordinate& newCenter);
   
   virtual void setPoint(const WFAPI::WGS84Coordinate& newCoord,
                         const WFAPI::ScreenPoint& screenPoint );

   virtual void setPoint(const WFAPI::WGS84Coordinate& newCoord,
                         const WFAPI::ScreenPoint& screenPoint, 
                         double angleDegrees );

   virtual void move(WFAPI::wf_int32 deltaX, WFAPI::wf_int32 deltaY);
   
   virtual void setAngle(double angleDegrees);

   virtual void setAngle(double angleDegrees,
                         const WFAPI::ScreenPoint& rotationPoint);

   virtual void rotateLeftDeg(WFAPI::wf_int32 nbrDeg);
   virtual double getAngle() const;

   virtual double setZoomLevel(double zoomLevel);

   virtual double getZoomLevel() const;

   virtual double zoom(double factor);
   
   virtual double zoom(double factor, 
                       const WFAPI::WGS84Coordinate& zoomCoord,
                       const WFAPI::ScreenPoint& zoomPoint);

   virtual void setScreenBox(const WFAPI::ScreenPoint& oneCorner,
                             const WFAPI::ScreenPoint& otherCorner);


   virtual void setWorldBox(const WFAPI::WGS84Coordinate& oneCorner,
                            const WFAPI::WGS84Coordinate& otherCorner) ;

   virtual void setMovementMode(bool moving);
private:
   ExternalRenderer* m_renderer;
   MapProjection& m_projection;
};

#endif /* MAPPROJECTIONPROXY_H */
