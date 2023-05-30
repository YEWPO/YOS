#include "common.h"
#include "debug.h"
#include "kernel.h"

void sleep(void *sleeplock, struct spinlock *spinlock) {
  struct proc *current_proc = current_cpu_proc();

  acquire_lock(&current_proc->proc_lock);
  release_lock(spinlock);

  current_proc->sleep_lock = sleeplock;
  current_proc->state = SLEEPING;

  switch2kernel();

  current_proc->sleep_lock = 0;

  release_lock(&current_proc->proc_lock);
  acquire_lock(spinlock);
}
