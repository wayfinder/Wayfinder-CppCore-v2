#ifndef MAINTHREADSCHEDULER_H
#define MAINTHREADSCHEDULER_H

typedef void(*schedFunc_t)(void*);

class MainThreadScheduler {
public:
   virtual void scheduleOnMainThread(schedFunc_t func, void* data) = 0;
};

#endif /* MAINTHREADSCHEDULER_H */
