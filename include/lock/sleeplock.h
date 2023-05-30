#ifndef _SLEEPLOCK_H
#define _SLEEPLOCK_H

#include "lock/spinlock.h"

struct sleeplock {
  bool is_locked;
  struct spinlock *spinlock;
};

#endif
