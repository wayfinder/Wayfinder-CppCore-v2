#ifndef PALRUNLOOPLINUX_H
#define PALRUNLOOPLINUX_H

#include <gtk/gtk.h>
#include <map>

namespace pal {

/**
 * Sync class in order to protect our own data structures
 * from access from different threads.
 */
class GSync {
public:
   GSync() {
      g_static_mutex_lock(&s_timerMutex);
   }
   ~GSync() {
      g_static_mutex_unlock(&s_timerMutex);
   }
private:
   static GStaticMutex s_timerMutex;
};

/**
 *  Holds the necessary data so that the callback
 *  function can be properly invoked when the timer
 *  is triggered.
 */ 
struct CallbackData {

   // The function to call
   pal_run_loop_func func;

   // The associated id of the timer event
   int id;

   // The user-supplied data
   void* data;

   // The context which initiated the callback
   RunLoopContext* context;
};

/**
 *  Holds information necessary for cancelling requested
 *  timer events.
 */ 
struct TimerData {
   GSource* source;
   CallbackData* info;
};

typedef std::map<unsigned int, TimerData> TimerMap;

/**
 *  Represents a run loop context which has information about
 *  outstanding timers and other state data.
 */
struct RunLoopContext {
   // The variable used to increment unique ID:s for each request
   unsigned int timer_count;

   // The collection of outstanding timers that are not yet fired
   TimerMap timers;
};

}

#endif /* PALRUNLOOPLINUX_H */
