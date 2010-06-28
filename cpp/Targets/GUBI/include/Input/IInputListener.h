
#ifndef _IINPUT_LISTENER_H_
#define _IINPUT_LISTENER_H_

#include "Input/InputEventState.h"

namespace gubi {

class IInputListener
{
public:
   virtual void OnInputEvent(
      const gubi::InputEventState& eState,
      int x,
      int y ) = 0;
};

}

#endif // _IINPUT_LISTENER_H_
