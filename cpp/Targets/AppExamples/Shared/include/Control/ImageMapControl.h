#ifndef IMAGEMAPCONTROL_H
#define IMAGEMAPCONTROL_H

#include "MapDrawingCallback.h"
#include "WindowEventListener.h"
#include "Util/NotCopyable.h"
#include "Core/IDrawCallback.h"

namespace WFAPI {
class DBufConnection;
class DrawingContext;
} // End of namespace WFAPI

namespace gubi {
   class IPropertyBag;
} // namespace gubi

class AppWindow;
class BufferRequesterSettings;
class AppEventListener;

/**
 *  This is a sample control which uses our model.
 *
 *  The purpose of the class is to receive input from
 *  an input class and delegate appropriate actions to
 *  the model.
 *
 *  The input from the view is supplied through the
 *  WindowEventListener interface that this class implements.
 */ 
class ImageMapControl : public WindowEventListener,
                        public NotCopyable,
                        public WFAPI::MapDrawingCallback,
                        public gubi::IDrawCallback {
public:
  /**
   *  Constructor of ImageMapControl.
   *
   *  @param window The window to use for the control
   *  @param rootPath The filesystem path of the resources for the control
   *  @param modelSettings The settings class   
   *  @param connection The DBufConnection to fetch maps from.
   *  @param drawContext The drawing context.
   *  @param appListener The listener that will be notified about
   *                     application-wide events.
   */ 
   ImageMapControl(AppWindow* window,
                   const std::string& resourcePath,
                   const BufferRequesterSettings& modelSettings,
                   WFAPI::DBufConnection* connection,
                   AppEventListener* appListener);

  /**
   *  Destructor.
   */
   virtual ~ImageMapControl();

   /**
    *  @see WindowEventListener::onKeyDown
    */
   void onKeyDown(const KeyEvent& event);

   /**
    *  @see WindowEventListener::onKeyUp
    */
   void onKeyUp(const KeyEvent& event);
   
   /**
    *  @see WindowEventListener::onWindowClosed
    */
   void onWindowClosed();

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
    *  @see WindowEventListener::onWindowResize
    */
   void onWindowResize(int newWidth, int newHeight);

   /**
    *  @see WindowEventListener::onRedraw
    */
   void onRedraw();

   /**
    *  @see MapDrawingCallback::mapIsReadyToBeDrawn
    */
   virtual void mapIsReadyToBeDrawn();

   /**
    *  @see gubi::IDrawCallback::gubiShouldBeDrawn
    *  
    */ 
   void gubiShouldBeDrawn();

   void onEffectUpdate(gubi::IPropertyBag *bag);
   
private:
   static void timerCallback(unsigned int id, void* data);

   /**
    *  Updates the logic of the components.
    */
   void updateLogic();
   
   struct Impl;
   Impl* m_impl;
};


#endif /* IMAGEMAPCONTROL_H */
