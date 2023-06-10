#include "common.h"
#include "debug.h"
#include "kernel.h"

extern struct proc proc[NPROC];

/**
 * 讓一個進程睡眠
 *
 * @param sleeplock 睡眠鎖
 * @param spinlock 睡眠锁的自旋锁
 *
 * @return void 无返回
 */
void sleep(void *condition, struct spinlock *spinlock) {
  struct proc *current_proc = current_cpu_proc();

  acquire_lock(&current_proc->proc_lock);
  release_lock(spinlock);

  current_proc->sleep_condition = condition;
  current_proc->state = SLEEPING;

  switch2kernel();

  current_proc->sleep_condition = 0;

  release_lock(&current_proc->proc_lock);
  acquire_lock(spinlock);
}

/**
 * 释放在睡眠锁上休眠的所有进程
 *
 * @param sleeplock 睡眠锁
 *
 * @return void 无返回
 */
void wakeup(void *condition) {
  for (int i = 0; i < NPROC; ++i) {
    if (&proc[i] != current_cpu_proc()) {
      acquire_lock(&proc[i].proc_lock);

      if (proc[i].state == SLEEPING && proc[i].sleep_condition == condition) {
        proc[i].state = RUNABLE;
      }

      release_lock(&proc[i].proc_lock);
    }
  }
}
