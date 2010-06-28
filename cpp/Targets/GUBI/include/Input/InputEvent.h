
#ifndef _INPUT_EVENT_H_
#define _INPUT_EVENT_H_

#include "Input/InputEventState.h"

namespace gubi {

class InputEvent
{
public:
   InputEvent(
      void )
   {
      m_eState = gubi::InputEventState_Invalid;
      m_iIndex = 0;
      m_x      = 0;
      m_y      = 0;
      m_iTime  = 0;
   }

   InputEvent(
      const gubi::InputEventState& eState,
      unsigned int iIndex,
      int x,
      int y,
      unsigned int iTime )
   {
      m_eState = eState;
      m_iIndex = iIndex;
      m_x      = x;
      m_y      = y;
      m_iTime  = iTime;
   }

   gubi::InputEventState getState(
      void )
   {
      return m_eState;
   }

   unsigned int getIndex(
      void )
   {
      return m_iIndex;
   }

   int getX(
      void )
   {
      return m_x;
   }

   int getY(
      void )
   {
      return m_y;
   }

   unsigned int getTime(
      void )
   {
      return m_iTime;
   }

private:
   gubi::InputEventState m_eState;
   unsigned int          m_iIndex;
   int                   m_x;
   int                   m_y;
   unsigned int          m_iTime;
};

}

#endif // _INPUT_EVENT_H_
