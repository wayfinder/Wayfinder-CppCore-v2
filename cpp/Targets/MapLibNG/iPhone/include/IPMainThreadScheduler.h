#ifndef IPMAINTHREADSCHEDULER_H
#define IPMAINTHREADSCHEDULER_H

#include "MainThreadScheduler.h"

class IPMainThreadScheduler : public MainThreadScheduler {
public:
   virtual void scheduleOnMainThread(schedFunc_t func, void* data);   
};

#endif /* IPMAINTHREADSCHEDULER_H */
