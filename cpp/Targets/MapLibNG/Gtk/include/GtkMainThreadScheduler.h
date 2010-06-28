#ifndef GTKMAINTHREADSCHEDULER_H
#define GTKMAINTHREADSCHEDULER_H

#include "MainThreadScheduler.h"

class GtkMainThreadScheduler : public MainThreadScheduler {
public:
   virtual void scheduleOnMainThread(schedFunc_t func, void* data);
};

#endif /* GTKMAINTHREADSCHEDULER_H */
