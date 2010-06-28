
#ifndef _WIDGET_FACTORY_H_
#define _WIDGET_FACTORY_H_

#include "Common/Identifier.h"
#include "Widget/WidgetConfiguration.h"
#include <map>

namespace gubi {

// Prototypes
class IWidgetInternal;
class IWidgetDestructor;
class IRenderFunction;
class IRenderSystem;
class IPropertyBag;

typedef gubi::IWidgetInternal*(*WidgetCreator)(
   gubi::IWidgetDestructor*,
   gubi::IRenderFunction*,
   const gubi::IPropertyBag*);

typedef const gubi::WidgetConfiguration&(*WidgetConfig)(
   void);

class WidgetFactory
{
   typedef std::map<gubi::Identifier,
      std::pair<WidgetConfig,WidgetCreator> > widget_map;
public:
   WidgetFactory(
      gubi::IRenderSystem* pRenderSystem );

   ~WidgetFactory(
      void );

   bool RegisterWidget(
      const gubi::Identifier& eWidgetId,
      WidgetConfig pWidgetConfig,
      WidgetCreator pWidgetCreator );

   bool UnregisterWidget(
      const gubi::Identifier& eWidgetId );

   bool CreateWidget(
      gubi::IWidgetDestructor* pDestructor,
      const gubi::Identifier& eWidgetId,
      const gubi::IPropertyBag* pProperties,
      gubi::IWidgetInternal** ppWidget );

private:   
   widget_map           m_mapWidget;
   gubi::IRenderSystem* m_pRenderSystem;
};

}

#endif // _WIDGET_FACTORY_H_
