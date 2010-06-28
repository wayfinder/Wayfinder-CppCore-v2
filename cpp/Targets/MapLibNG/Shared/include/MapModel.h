#ifndef MAPMODEL_H
#define MAPMODEL_H

#include "Util/NotCopyable.h"

class MapManipulator;
class MapOperationHandler;
class MapLibKeyHandler;
class MapEventListener;
class MapProjection;
class BufferRequesterSettings;
class ProjectionInterface;

namespace WFAPI {
class DBufConnection;
class DrawingContext;
class MapDrawingCallback;
} // End of namespace WFAPI

/**
 *  The name is derived from the model concept in the
 *  MVC paradigm, this class basically acts as an initialized
 *  library instance with the necessary objects.
 *
 *  It is responsible for keeping the data necessary to draw
 *  and manipulate maps.
 */ 
class MapModel : public NotCopyable {
public:
   /**
    *  Creates a new map model.
    *
    *  @param connection The buffer connection to ultimately
    *                    request tiles from when not available
    *                    in cache.
    *  @param drawContext The drawing context that is used to
    *                     render the maps.
    */ 
   MapModel(const BufferRequesterSettings& modelSettings,
            WFAPI::DBufConnection* connection,
            WFAPI::DrawingContext* drawContext);

   virtual ~MapModel();   

   /**
    *  Specifies which map backend to use.  
    */  
   enum BackendType { WF_MAPS = 0,     // Use wayfinder bitmaps
                      OSM_MAPS,        // Use OpenStreetMap bitmaps
                      NUM_BACKENDS};   

   /**
    *  Changes the current map backend in use.
    */ 
   void setMapBackend(BackendType type);

   /**
    *  Sets the listener that should be notified about map events.
    */ 
   void setMapEventListener(MapEventListener* listener);


   void setMapDrawingCallback(WFAPI::MapDrawingCallback* callback);
   
   /**
    * XXX: These three interfaces should be merged into a common
    * ActionInterface or similar.
    */ 
   MapManipulator& getMapManipulator();
   MapOperationHandler& getMapOperationHandler();
   MapLibKeyHandler& getKeyHandler();

   /**
    *  Used to project coordinates into screen space and vice versa.
    *
    *  @return a projection interface that can be used to project stuff.
    */ 
   ProjectionInterface& getProjectionInterface();
   
   /**
    * XXX: Refactor to drawInterface or similar?
    */ 
   void renderMap();
   
   /**
    * XXX: Should this be exposed?
    */ 
   MapProjection& getMapProjection();
private:
   struct Impl;
   Impl* m_impl;
};

#endif /* MAPMODEL_H */
