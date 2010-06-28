#ifndef MAPMODULE_H
#define MAPMODULE_H

#include "Util/NotCopyable.h"

class MapEventListener;

/**
 *  Class that contains shared implementation details for
 *  the different map modules.
 */  
class MapModule : public NotCopyable {
public:
   /**
    *  Constructor.
    */
   MapModule();

   /**
    *  Destructor.
    */
   virtual ~MapModule();
   
   /**
    * @param listener MapEventListener to add.
    */
   void addListener(MapEventListener* listener);

   /**
    * @param listener MapEventListener to remove.
    */
   void removeListener(MapEventListener* listener);

protected:

   /**
    *  Notify all listeners that the map is fully loaded.
    */
   void notifyMapCompletelyLoaded();
private:
   struct Impl;
   Impl* m_impl;
};

#endif /* MAPMODULE_H */
