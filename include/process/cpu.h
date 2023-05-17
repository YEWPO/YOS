#ifndef _CPU_H
#define _CPU_H

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

#endif
