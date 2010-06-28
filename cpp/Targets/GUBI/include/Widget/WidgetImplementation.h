
#ifndef _WIDGET_IMPLEMENTATION_H_
#define _WIDGET_IMPLEMENTATION_H_

#include "Signal/SignalCollection.h"
#include "Widget/IWidgetInternal.h"
#include "Widget/WidgetConfiguration.h"

namespace gubi {

class IWidgetDestructor;

class WidgetImplementation : public gubi::IWidgetInternal,
                             public gubi::SignalCollection
{
public:
   WidgetImplementation(
      gubi::IWidgetDestructor* pDestructor,
      gubi::IRenderFunction* pRenderFunction );

   virtual ~WidgetImplementation(
      void );

   bool GetSignal(
      const gubi::Identifier& eSignalId,
      gubi::ISignalHub** ppSignalHub );

   void render(
      void );

   void setSize(
      fmath::fixed width,
      fmath::fixed height );

   void getSize(
      fmath::fixed& width,
      fmath::fixed& height );

   virtual bool OnInputEvent(
      const gubi::InputEventState& eState,
      int x,
      int y );

private:
   gubi::IWidgetDestructor* m_pDestructor;
   gubi::IRenderFunction*   m_pRenderFunction;
};

}

#endif // _WIDGET_IMPLEMENTATION_H_
