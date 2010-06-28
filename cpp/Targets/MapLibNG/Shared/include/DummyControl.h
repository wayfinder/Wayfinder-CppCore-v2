#ifndef DUMMYCONTROL_H
#define DUMMYCONTROL_H

#include "WindowEventListener.h"

class DummyControl : public WindowEventListener {
public:
   virtual void onWindowClosed() {}
   virtual void onKeyDown(const KeyEvent& event) {}
   virtual void onKeyUp(const KeyEvent& event) {}
   virtual void onPointerDown(const PointerEvent& event) {}
   virtual void onPointerMove(const PointerEvent& event) {}
   virtual void onPointerUp(const PointerEvent& event) {}
   virtual void onWindowResize(int newWidth, int newHeight) {}
};

#endif /* DUMMYCONTROL_H */
