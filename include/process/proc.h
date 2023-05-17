#ifndef _PROC_H
#define _PROC_H

#include "common.h"
#include "memory/vm.h"
#include "lock/spinlock.h"

#define NPROC 64

struct context {
  uint64_t ra;
  uint64_t sp;

  uint64_t s0;
  uint64_t s1;
  uint64_t s2;
  uint64_t s3;
  uint64_t s4;
  uint64_t s5;
  uint64_t s6;
  uint64_t s7;
  uint64_t s8;
  uint64_t s9;
  uint64_t s10;
  uint64_t s11;
};

struct trapframe {
  uint64_t x0;
  uint64_t x1;
  uint64_t x2;
  uint64_t x3;
  uint64_t x4;
  uint64_t x5;
  uint64_t x6;
  uint64_t x7;
  uint64_t x8;
  uint64_t x9;
  uint64_t x10;
  uint64_t x11;
  uint64_t x12;
  uint64_t x13;
  uint64_t x14;
  uint64_t x15;
  uint64_t x16;
  uint64_t x17;
  uint64_t x18;
  uint64_t x19;
  uint64_t x20;
  uint64_t x21;
  uint64_t x22;
  uint64_t x23;
  uint64_t x24;
  uint64_t x25;
  uint64_t x26;
  uint64_t x27;
  uint64_t x28;
  uint64_t x29;
  uint64_t x30;
  uint64_t x31;

  uint64_t kernel_satp;           // 32
  uint64_t proc_kernel_sp;        // 33
  uint64_t user_pc;               // 34
  uint64_t kernle_user_trap_handler; //35
};

enum proc_state {
  UNUSED,
  USED,
  SLEEPING,
  RUNABLE,
  RUNNING
};

struct proc {
  struct spinlock proc_lock;

  enum proc_state state;
  int pid;

  addr_t proc_kernel_stack;
  pagetable_t user_pagetable;
  struct trapframe *user_trapframe;
  struct context user_context;
};

void proc_init();
void root_proc_init();
void switch2user();
void switch2kernel();

#endif
