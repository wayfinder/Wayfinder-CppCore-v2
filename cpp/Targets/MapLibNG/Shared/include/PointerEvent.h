#ifndef POINTEREVENT_H
#define POINTEREVENT_H

#include <vector>
#include "PALMachineTypes.h"
#include "ScreenPoint.h"

/**
 *  The purpose of this class is to translate platform specific
 *  pointer implementations into a generic structure. This
 *  enables us to use a cross-platform controller in our
 *  various clients.
 */ 
struct PointerEvent {

   /**
    *  Specifies the generating entity of the
    *  pointer event.
    */ 
   enum Code {
      INVALID_POINTER_TYPE=0,
      LEFT_MOUSE_BUTTON,
      FINGER
   };

   /**
    *  Specifies the type of the event
    */ 
   enum Type {
      INVALID_EVENT_TYPE = 0,
      POINTER_DOWN,
      /**
       *  For systems with more than one input sources (fingers
       *  on touch device for instance), movement updates should
       *  be generated when one or more sources have moved. This
       *  means that in some cases, one of the sources have not
       *  generated a new position when the movement listener
       *  function is called..
       *
       *  If that is the case, POINTER_IDLE is set as the type.
       */ 
      POINTER_STATIONARY,
      POINTER_MOVE,
      POINTER_UP
   };

   /**
    *  Structure 
    */ 
   struct State {
      int id;
      Code code;
      Type type;
      int tapCount;
      WFAPI::ScreenPoint location;
      unsigned int timeStampMillis;
   };
   
   std::vector<State> modStates;
   std::vector<State> allStates;

   PointerEvent() {
      
   }
};

#ifdef WFAPI_IOSTREAM_AVAILABLE
/**
 * Print on ostream.
 *
 * @param stream The stream to print on.
 * @param coord  The coordinate to print.
 * @return The stream.
 */
std::ostream& operator<< (std::ostream& stream,
                          const PointerEvent& event);
#endif //WFAPI_IOSTREAM_AVAILABLE

#endif /* POINTEREVENT_H */
