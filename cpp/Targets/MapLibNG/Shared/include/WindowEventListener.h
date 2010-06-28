#ifndef WINDOWEVENTLISTENER_H
#define WINDOWEVENTLISTENER_H

struct KeyEvent;
struct PointerEvent;

/**
 *  This class is to be used as the communication layer between the
 *  view and the controller. The view class should keep an instance of
 *  this and forward events to it.
 *
 *  The controller class implements this interface and can delegate
 *  suitable actions to the model accordingly.
 */ 
class WindowEventListener {
public:
   /**
    *  Should be called whenever a window is closed, for instance
    *  when the user has pressed the X-button.
    */ 
   virtual void onWindowClosed() = 0;

   /**
    *  Should be called whenever the user has generated a key event.
    *
    *  @param code The KeyCode representing the press.
    */ 
   virtual void onKeyDown(const KeyEvent& event) = 0;

   /**
    *  Should be called whenever the user has generated a key event.
    *
    *  @param code The KeyCode representing the press.
    */ 
   virtual void onKeyUp(const KeyEvent& event) = 0;
   
   /**
    *  Should be called whenever the user has pressed down the pointing
    *  device (mouse, finger) to a location on the screen.
    *
    *  @param event The structure describing the class. @see PointerEvent
    */ 
   virtual void onPointerDown(const PointerEvent& event) = 0;

   /**
    *  Should be called whenever the pointing device has moved across
    *  the screen.
    *
    *  @param event The structure describing the class. @see PointerEvent
    */ 
   virtual void onPointerMove(const PointerEvent& event) = 0;

   /**
    *  Should be called whenever the pointing device has been lifted
    *  from the screen.
    *
    *  @param event The structure describing the class. @see PointerEvent
    */ 
   virtual void onPointerUp(const PointerEvent& event) = 0;

   /**
    *  Should be called whenever the window dimensions have
    *  changed.
    *
    *  @param newWidth  The new width of the window after the resize.
    *  @param newHeight The new height of the window after the resize.
    */ 
   virtual void onWindowResize(int newWidth, int newHeight) = 0;   

   /**
    *
    */ 
   virtual void onRedraw() = 0;
   
   /**
    *  Virtual destructor to ensure that implementing classes get their
    *  destructors invoked properly.
    */ 
   virtual ~WindowEventListener() {}
};

#endif /* WINDOWEVENTLISTENER_H */
