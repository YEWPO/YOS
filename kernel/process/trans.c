#include "common.h"
#include "debug.h"
#include "kernel.h"

extern struct proc proc[NPROC];

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

void wakeup(void *sleeplock) {
  for (int i = 0; i < NPROC; ++i) {
    if (&proc[i] != current_cpu_proc()) {
      acquire_lock(&proc[i].proc_lock);

      if (proc[i].state == SLEEPING && proc[i].sleep_lock == sleeplock) {
        proc[i].state = RUNABLE;
      }

      release_lock(&proc[i].proc_lock);
    }
  }
}
