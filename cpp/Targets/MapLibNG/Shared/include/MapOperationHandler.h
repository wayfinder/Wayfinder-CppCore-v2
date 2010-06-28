#ifndef _MAPOPERATIONHANDLER_H_
#define _MAPOPERATIONHANDLER_H_

#include "PALMachineTypes.h"
#include "MapOperationInterface.h"
#include "WGS84Coordinate.h"

class MapProjection;
class ExternalRenderer;
class TileMapHandler;

class MapOperationHandler : public WFAPI::MapOperationInterface {
public:
   MapOperationHandler(MapProjection* projection,
                       ExternalRenderer* extRenderer);

   /**
    * Virtual destructor.
    */
   virtual ~MapOperationHandler();
      
   /**
    * @see MapOperationInterface::getCenter
    */
   virtual WFAPI::WGS84Coordinate getCenter() const;

   /**
    * @see MapOperationInterface::worldToScreen
    */
   virtual void worldToScreen(WFAPI::ScreenPoint& point,
                              const WFAPI::WGS84Coordinate& coord) const;
   
   /**
    * @see MapOperationInterface::screenToWorld
    */
   virtual void screenToWorld(WFAPI::WGS84Coordinate& coord,
                              const WFAPI::ScreenPoint& point) const;
   
   /**
    * @see MapOperationInterface::setCenter
    */
   virtual void setCenter(const WFAPI::WGS84Coordinate& newCenter);
   
   /**
    * @see MapOperationInterface::setPoint
    */
   virtual void setPoint(const WFAPI::WGS84Coordinate& newCoord,
                         const WFAPI::ScreenPoint& screenPoint );

   /**
    * @see MapOperationInterface::setPoint
    */
   virtual void setPoint(const WFAPI::WGS84Coordinate& newCoord,
                         const WFAPI::ScreenPoint& screenPoint, 
                         double angleDegrees );

   /**
    * @see MapOperationInterface::move
    */
   virtual void move(WFAPI::wf_int32 deltaX, WFAPI::wf_int32 deltaY);
   
   /**
    * @see MapOperationInterface::setAngle
    */
   virtual void setAngle(double angleDegrees);

   /**
    * @see MapOperationInterface::setAngle
    */
   virtual void setAngle(double angleDegrees,
                         const WFAPI::ScreenPoint& rotationPoint);

   /**
    * @see MapOperationInterface::rotateLeftDeg
    */
   virtual void rotateLeftDeg(WFAPI::wf_int32 nbrDeg);
   
   /**
    * @see MapOperationInterface::getAngle
    */
   virtual double getAngle() const;

   /**
    * @see MapOperationInterface::setZoomLevel
    */
   virtual double setZoomLevel(double zoomLevel);

   /**
    * @see MapOperationInterface::getZoomLevel
    */
   virtual double getZoomLevel() const;

   /**
    * @see MapOperationInterface::zoom
    */
   virtual double zoom(double factor);
   
   /**
    * @see MapOperationInterface::zoom
    */
   virtual double zoom(double factor, 
                       const WFAPI::WGS84Coordinate& zoomCoord,
                       const WFAPI::ScreenPoint& zoomPoint);

   /**
    * @see MapOperationInterface::setScreenBox
    */
   virtual void setScreenBox(const WFAPI::ScreenPoint& oneCorner,
                             const WFAPI::ScreenPoint& otherCorner);

   /**
    * @see MapOperationInterface::setWorldBox
    */
   virtual void setWorldBox(const WFAPI::WGS84Coordinate& oneCorner,
                            const WFAPI::WGS84Coordinate& otherCorner) ;

   /**
    * @see MapOperationInterface::setMovementMode
    */
   virtual void setMovementMode(bool moving);


   /**
    *  @see MapOperationInterface::set3dMode
    */
   void set3dMode(bool enabled);

   /**
    *  @see MapOperationInterface::get3dMode
    */
   bool get3dMode() const;

   /**
    *  @see MapOperationInterface::setVariable3dMode
    */  
   void setVariable3dMode(float step);
   
private:
   MapProjection* m_projection;
   ExternalRenderer* m_extRenderer;
};

#endif /* _MAPOPERATIONHANDLER_H_ */
