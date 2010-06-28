#ifndef WINDOWEVENTADAPTER_H
#define WINDOWEVENTADAPTER_H

#include "WindowEventListener.h"

struct KeyEvent;
struct PointerEvent;

/**
 *  Convenience class used for classes that do not need to
 *  implement all the functionality of the WindowEventListener.
 */ 
class WindowEventAdapter : public WindowEventListener {
public:
   /**
    * @see WindowEventListener::onWindowClosed
    */ 
   virtual void onWindowClosed() {}

   /**
    * @see WindowEventListener::onKeyDown
    */ 
   virtual void onKeyDown(const KeyEvent& event) {}

   /**
    * @see WindowEventListener::onKeyUp
    */ 
   virtual void onKeyUp(const KeyEvent& event) {}
   
   /**
    * @see WindowEventListener::onPointerDown
    */ 
   virtual void onPointerDown(const PointerEvent& event) {}

   /**
    * @see WindowEventListener::onPointerMove
    */ 
   virtual void onPointerMove(const PointerEvent& event) {}

   /**
    * @see WindowEventListener::onPointerUp
    */ 
   virtual void onPointerUp(const PointerEvent& event) {}

   /**
    * @see WindowEventListener::onWindowResize
    */ 
   virtual void onWindowResize(int newWidth, int newHeight) {}   

   /**
    * @see WindowEventListener::onRedraw
    */ 
   virtual void onRedraw() {}
   
   /**
    * Virtual destructor, so that base classes are deleted properly
    */ 
   virtual ~WindowEventAdapter() {}
};

#endif /* WINDOWEVENTADAPTER_H */
