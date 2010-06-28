
#ifndef _IWIDGET_INTERNAL_H_
#define _IWIDGET_INTERNAL_H_

#include "Widget/IWidget.h"
#include "Input/InputEventState.h"

namespace gubi {

class IWidgetInternal : public gubi::IWidget
{
public:
   virtual bool OnInputEvent(
      const gubi::InputEventState& eState,
      int x,
      int y ) = 0;
};

}

#endif // _IWIDGET_INTERNAL_H_
