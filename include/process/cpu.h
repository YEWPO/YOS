#ifndef _CPU_H
#define _CPU_H

#include "lock/spinlock.h"
#include "process/proc.h"
#include "riscv64.h"

#define NCPU 1

struct cpu {
  struct proc *user_proc_running;
  struct context cpu_context;

  int lock_num;
  bool prev_lock_status;
};

extern struct cpu cpu[NCPU];

#define CPU_ID READ_GRR(tp)

static inline struct proc *current_cpu_proc() {
  push_lock();
  struct proc *ret_proc = cpu[CPU_ID].user_proc_running;
  pop_lock();

  return ret_proc;
}

#endif
