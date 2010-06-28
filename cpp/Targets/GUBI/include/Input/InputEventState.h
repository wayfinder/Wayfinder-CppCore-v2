
#ifndef _INPUT_EVENT_STATE_H_
#define _INPUT_EVENT_STATE_H_

namespace gubi {

enum InputEventState
{
   InputEventState_Invalid,

   InputEventState_Down,
   InputEventState_Move,
   InputEventState_Up,

   InputEventState_Click
};

}

#endif // _INPUT_EVENT_STATE_H_
