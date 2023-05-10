#ifndef _CPU_H
#define _CPU_H

#include "process/proc.h"

#define NCPU 1

struct cpu {
  struct proc *user_proc_running;
  struct context cpu_context;
};

#endif
