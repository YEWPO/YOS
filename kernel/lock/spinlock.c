#include "debug.h"
#include "kernel.h"
#include "common.h"
#include "macro.h"

static inline bool is_locked(struct spinlock *lock) {
  return lock->is_locked;
}
