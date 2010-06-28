
#ifndef _WIMAGE_H_
#define _WIMAGE_H_

#include "Widget/WidgetImplementation.h"

namespace gubi {

class WImage : public gubi::WidgetImplementation
{
public:
   static const gubi::WidgetConfiguration& Config(
      void );

   static gubi::IWidgetInternal* Create(
      gubi::IWidgetDestructor* pDestructor,
      gubi::IRenderFunction* pRenderFunction,
      const gubi::IPropertyBag* pProperties );

   WImage(
      gubi::IWidgetDestructor* pDestructor,
      gubi::IRenderFunction* pRenderFunction );

   ~WImage(
      void );

private:
   static WidgetConfiguration _config;
};

}
#endif // _WIMAGE_H_
