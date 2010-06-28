#ifndef NULLDRAWINGINTERFACE_H
#define NULLDRAWINGINTERFACE_H

#include "MapDrawingInterface.h"

namespace WFAPI {
class MapDrawingCallback;
}

/**
 *  Kludge used to transition architecture into a cleaner
 *  version.
 * 
 *  Implements the abstract MapDrawingInterface but does nothing.
 */ 
class NullDrawingInterface : public WFAPI::MapDrawingInterface {
public:

   /**
    *     Implements the abstract method but does nothing.
    */
   virtual ~NullDrawingInterface() {}
   
   /**
    *     Implements the abstract method but does nothing.
    */
   virtual void repaintNow() {}
   
   /**
    *     Implements the abstract method but does nothing.
    */
   virtual void requestRepaint() {}
   
   /**
    *     Implements the abstract method but does nothing.
    */
   virtual void setMapDrawingEnabled(bool mapDrawingEnabled) {}
   
   /**
    *     Implements the abstract method but does nothing.
    */
   virtual void setMapDrawingCallback(WFAPI::MapDrawingCallback* callback) {}
   
   /**
    *     Implements the abstract method but does nothing.
    */
   virtual void drawImageSpec(const WFAPI::ImageSpec* spec,
                              const WFAPI::ScreenPoint& point) {}
};

#endif /* NULLDRAWINGINTERFACE_H */
