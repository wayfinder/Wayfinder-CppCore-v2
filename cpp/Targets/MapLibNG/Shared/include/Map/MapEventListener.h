#ifndef MAPEVENTLISTENER_H
#define MAPEVENTLISTENER_H

/**
 *   Used by the different map modules to notify when map related
 *   events have occured.
 */ 
class MapEventListener {
public:
   /**
    *   Callback function that is called whenever
    *   the module has completely downloaded the
    *   current map area.
    */ 
   virtual void mapCompletelyLoaded() = 0;

   /**
    *  Destructor.
    */
   virtual ~MapEventListener() {}
};

#endif /* MAPEVENTLISTENER_H */
