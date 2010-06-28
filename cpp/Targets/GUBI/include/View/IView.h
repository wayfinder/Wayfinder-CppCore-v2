
#ifndef _IVIEW_H_
#define _IVIEW_H_

#include "Core/IObject.h"
#include "Signal/ISignalCollection.h"
#include "View/ViewShowFlag.h"
#include "View/ViewSpace.h"

namespace gubi {

class IWidget;
class IWidgetArray;

class IView : public gubi::IObject,
              public gubi::ISignalCollection
{
public:
   virtual bool Show(
      const gubi::ViewShowFlag& eFlag ) = 0;

   virtual void Hide(
      void ) = 0;

   virtual bool AddWidget(
      gubi::IWidget* pWidget,
      const gubi::ViewSpace& eViewSpace ) = 0;

   virtual bool AddWidgetArray(
      gubi::IWidgetArray* pWidgetArray,
      const gubi::ViewSpace& eViewSpace ) = 0;
};

}

#endif // _IVIEW_H_
