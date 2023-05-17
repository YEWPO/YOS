#ifndef _SPINLOCK_H
#define _SPINLOCK_H

#include <stdbool.h>

struct spinlock {
  bool is_locked;
};

void push_lock();
void pop_lock();
void acquire_lock();
void release_lock();

#endif
