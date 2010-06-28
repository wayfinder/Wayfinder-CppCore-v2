
#ifndef _INPUT_MANAGER_H_
#define _INPUT_MANAGER_H_

#include <map>
#include <vector>
#include "Input/InputEvent.h"
#include "Input/InputIndexState.h"

namespace gubi {

class IInputListener;
class ITime;

class InputManager
{
public:
   InputManager(
      gubi::IInputListener* pListener,
      gubi::ITime* pTime );

   ~InputManager(
      void );

   void OnPointerDown(
      unsigned int iIndex,
      int x,
      int y );

   void OnPointerMove(
      unsigned int iIndex,
      int x,
      int y );

   void OnPointerUp(
      unsigned int iIndex,
      int x,
      int y );

   void Update(
      void );

private:
   gubi::InputIndexState* GetIndexState(
      unsigned int iIndex );

   std::vector<gubi::InputEvent>                m_vEvents;
   gubi::ITime*                                 m_pTime;
   gubi::IInputListener*                        m_pListener;
   std::map<unsigned int,gubi::InputIndexState> m_mapState;
};

}

#endif // _INPUT_MANAGER_H_
