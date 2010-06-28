
#ifndef _INPUT_INDEX_STATE_H_
#define _INPUT_INDEX_STATE_H_

#include "Input/InputEventState.h"

namespace gubi {

class InputIndexState
{
public:
   InputIndexState(
      void );

   void PushEvent(
      const gubi::InputEventState& eState,
      int x,
      int y,
      unsigned int iTime );

   bool TranslateEvent(
      gubi::InputEventState& eState,
      int& x,
      int& y );

private:
   gubi::InputEventState m_eState;
   int                   m_x;
   int                   m_y;
   unsigned int          m_iDownTime;
};

}

#endif // _INPUT_INDEX_STATE_H_
