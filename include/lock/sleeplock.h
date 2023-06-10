#ifndef _SLEEPLOCK_H
#define _SLEEPLOCK_H

#include "lock/spinlock.h"

struct sleeplock {
  bool is_locked;
  struct spinlock spinlock;
};

bool sleeplock_is_locked(struct sleeplock *lock);
void acquire_sleeplock(struct sleeplock *lock);
void release_sleeplock(struct sleeplock *lock);

#endif
