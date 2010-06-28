#ifndef APPWINDOW_H
#define APPWINDOW_H

class WindowEventListener;
class ExternalRenderer;
class MapLibKeyHandler;

namespace WFAPI {
class MapOperationInterface;
class DrawingContext;
} // End of namespace WFAPI


/**
 *  Platform indpendent super class for window classes used for map viewing.
 *  Represents the view in the model view control paradigm.
 *  The window creates the DrawingContext used for map rendering but has
 *  no knowledge of MapLib. Input from the window is supplied to the
 *  WindowEventListener which should be implemented by the controller class
 *  who also translates this into actions. 
 */
class AppWindow {
public:
   /**
    *  Constructor.
    */
   AppWindow();
   
   /**
    *  Set the window event listener, i.e. the interface to 
    *  the controller which determines actions for events 
    *  supplied by the window.
    * 
    *  @param listener The listener to set.
    */
   void setListener(WindowEventListener* listener);

   /**
    *  Requests that the window should be redrawn.
    */ 
   virtual void requestDraw() = 0;

   virtual WFAPI::DrawingContext* getDrawingContext() = 0;

   /**
    *  Returns the width of the widget in pixels.
    *
    *  @return width of widget in pixels.
    */ 
   virtual unsigned int getWidthPixels() = 0;

   /**
    *  Returns the height of the widget in pixels.
    *
    *  @return width of widget in pixels.
    */    
   virtual unsigned int getHeightPixels() = 0;
   
   /**
    *  Destructor.
    */
   virtual ~AppWindow() {}

   WindowEventListener* getListener();
protected:
   
   /**
    *  The interface to the controller.
    */
   WindowEventListener* m_listener;
};

#endif /* APPWINDOW_H */
