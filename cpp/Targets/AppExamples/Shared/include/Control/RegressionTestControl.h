#ifndef REGRESSIONTESTCONTROL_H
#define REGRESSIONTESTCONTROL_H

#include "WindowEventAdapter.h"
#include "Util/NotCopyable.h"
#include <string>

class AppWindow;
class AppEventListener;

class RegressionTestControl : public WindowEventAdapter,
                              public NotCopyable {
public:
   RegressionTestControl(AppWindow* window,
                         AppEventListener* appListener,
                         const std::string suitePath);

   virtual ~RegressionTestControl();
private:
   /**
    *  @see WindowEventListener::onKeyDown
    */
   void onKeyDown(const KeyEvent& event);

   /**
    *  @see WindowEventListener::onWindowClosed
    */
   void onWindowClosed();
   
   /**
    *  @see WindowEventListener::onPointerUp
    */
   void onPointerUp(const PointerEvent& event);
private:
   struct Impl;
   Impl* m_impl;
};

#endif /* REGRESSIONTESTCONTROL_H */
