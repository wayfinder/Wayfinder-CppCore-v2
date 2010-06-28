
#ifndef _WIDGET_MANAGER_H_
#define _WIDGET_MANAGER_H_

#include <list>
#include "Common/SmartPointer.h"
#include "Input/IInputListener.h"
#include "Widget/IWidgetDestructor.h"
#include "Widget/WidgetFactory.h"

namespace gubi {

class IRenderSystem;
class IWidgetInternal;

class WidgetManager : public gubi::IWidgetDestructor,
                      public gubi::IInputListener
{
public:
   WidgetManager(
      gubi::IRenderSystem* pRenderSystem );

   ~WidgetManager(
      void );

   bool RegisterWidget(
      const gubi::Identifier& eWidgetId,
      WidgetConfig pWidgetConfig,
      WidgetCreator pWidgetCreator );

   bool UnregisterWidget(
      const gubi::Identifier& eWidgetId );

   bool CreateWidget(
      const gubi::Identifier& eWidgetId,
      const gubi::IPropertyBag* pProperties,
      IWidget** ppWidget );

   void OnWidgetDestroy(
      const gubi::IWidgetInternal* pWidget );

   void OnInputEvent(
      const gubi::InputEventState& eState,
      int x,
      int y );

private:
   gubi::WidgetFactory                                   m_widgetFactory;
   std::list<gubi::SmartPointer<gubi::IWidgetInternal> > m_lWidget;
};

}

#endif // _WIDGET_MANAGER_H_
