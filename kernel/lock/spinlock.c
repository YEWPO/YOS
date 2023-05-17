#include "debug.h"
#include "kernel.h"
#include "common.h"
#include "macro.h"

static inline bool is_locked(struct spinlock *lock) {
  return lock->is_locked;
}

/**
 * 将一个锁压入栈
 *
 * @return void 无返回
 */
void push_lock() {
  bool prev_locked_status = GET_SSTATUS_SIE;

  // clear interrupt bit
  CLEAR_SSTATUS_SIE;
  if (cpu[CPU_ID].lock_num == 0) {
    // save prev locked status;
    cpu[CPU_ID].prev_lock_status = prev_locked_status;
  }
  // push stack
  cpu[CPU_ID].lock_num++;
}

/**
 * 将一个锁弹出栈
 *
 * @return void 无返回
 */
void pop_lock() {
  Assert(GET_SSTATUS_SIE == 0, "The interrupt is enable when pop_lock!");
  Assert(cpu[CPU_ID].lock_num >= 1, "The deep of lock stack is incorrect!");

  // pop stack
  cpu[CPU_ID].lock_num--;
  if (cpu[CPU_ID].lock_num == 0 && cpu[CPU_ID].prev_lock_status == true) {
    // reset prev locked status
    SET_SSTATUS_SIE;
  }
}
