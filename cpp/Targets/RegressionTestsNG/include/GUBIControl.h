#ifndef GUBICONTROL_H
#define GUBICONTROL_H

#include "WindowEventListener.h"
#include "Util/NotCopyable.h"
#include "Core/IDrawCallback.h"

#include "AppEventListener.h"

namespace gubi {
   class IPropertyBag;
}

class AppWindow;

class GUBIControl : public WindowEventListener,
                    public NotCopyable,
                    public gubi::IDrawCallback
{

public:

   GUBIControl(AppWindow* window,
               AppEventListener* appListener);

   virtual ~GUBIControl();
   
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
    *  @see gubi::IDrawCallback::gubiShouldBeDrawn
    *  
    */ 
   void gubiShouldBeDrawn();

   void onEffectUpdate(gubi::IPropertyBag *bag);

private:
   struct Impl;
   Impl* m_impl;
};

#endif /* GUBICONTROL_H */
