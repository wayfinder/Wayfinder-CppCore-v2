#ifndef INPUTCONTROL_H
#define INPUTCONTROL_H 

#include "Util/NotCopyable.h"
#include "Control/Input/GestureListener.h"

class MapModel;
class AppEventListener;
class KeyEvent;
class PointerEvent;

/**
 *  This is the controller that handles user input and maps
 *  to appropriate actions.
 */ 
class InputControl : public NotCopyable,
                     public GestureListener {
public:
  /**
   *  Constructor of InputControl.
   *
   *  @param model The map model.
   *  @param appListener The listener that will be notified about
   *                     application-wide events.
   */ 
   InputControl(MapModel* model, AppEventListener* appListener);

   /**
    *  Destructor.
    */
   virtual ~InputControl();

   /**
    *  @see WindowEventListener::onKeyDown
    */
   void onKeyDown(const KeyEvent& event);

   /**
    *  @see WindowEventListener::onKeyUp
    */
   void onKeyUp(const KeyEvent& event);
   
   /**
    *  @see WindowEventListener::onPointerDown
    */
   void onPointerDown(const PointerEvent& event);

   /**
    *  @see WindowEventListener::onPointerMove
    */
   void onPointerMove(const PointerEvent& event);
   
   /**
    *  @see WindowEventListener::onPointerUp
    */
   void onPointerUp(const PointerEvent& event);

   /**
    *  @see GestureListener::onFling
    */
   virtual bool onFling(const PointerEvent& event, 
                        float velocityX,
                        float velocityY);   
   
private:

   /**
    *  Initialize the mapping of actions for supplied KeyCodes.
    */
   void initKeyMap();
   
   struct Impl;
   Impl* m_impl;
};


#endif /* INPUTCONTROL_H */
