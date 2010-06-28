
#ifndef _WBUTTON_H_
#define _WBUTTON_H_

#include "Widget/WidgetImplementation.h"

namespace gubi {

class WButton : public gubi::WidgetImplementation
{
public:
   static const gubi::WidgetConfiguration& Config(
      void );

   static gubi::IWidgetInternal* Create(
      gubi::IWidgetDestructor* pDestructor,
      gubi::IRenderFunction* pRenderFunction,
      const gubi::IPropertyBag* pProperties );

   WButton(
      gubi::IWidgetDestructor* pDestructor,
      gubi::IRenderFunction* pRenderFunction );

   ~WButton(
      void );

   bool OnInputEvent(
      const gubi::InputEventState& eState,
      int x,
      int y );

private:
   static WidgetConfiguration _config;
};

}

#endif // _WBUTTON_H_
