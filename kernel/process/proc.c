#include "debug.h"
#include "kernel.h"
#include "macro.h"
#include "common.h"

// cpu struct array
struct cpu cpu[NCPU];
// proc struct array
struct proc proc[NPROC];
// root proc pointer
struct proc *root_proc;

extern char trampoline[];

extern void context_exchange(struct context *old, struct context *new);

void test() {
  Log("TEST");
  user_resume();
}

// process id for next alloc proc
static int next_pid = 1;

/**
 * alloc a pid for a proc
 *
 * @return int pid for a proc
 */
static int alloc_pid() {
  int pid = next_pid;
  next_pid++;
  return pid;
}

/**
 * init proc struct infomation
 *
 * @return void no return
 */
void proc_init() {
  Log("Initializing process...");

  for (int i = 0; i < NPROC; ++i) {
    proc[i].state = UNUSED;
    proc[i].proc_kernel_stack = PROC_KERNEL_STACK(i);
  }
}

static struct proc *find_unused_proc() {
  for (int i = 0; i < NPROC; ++i) {
    if (proc[i].state == UNUSED) {
      return proc + i;
    }
  }

  return NULL;
}

static bool alloc_proc_trapframe(struct proc *alloc_proc) {
  alloc_proc->user_trapframe = (struct trapframe *)alloc_physic_page();
  return true;
}

static bool alloc_proc_pagetable(struct proc *alloc_proc) {
  pagetable_t proc_pagetable = alloc_physic_page();

  va_map_pa(proc_pagetable, TRAMPOLINE, (addr_t)trampoline, MPTE_FLAG(R) | MPTE_FLAG(X));
  va_map_pa(proc_pagetable, TRAPFRAME, (addr_t)alloc_proc->user_trapframe, MPTE_FLAG(R) | MPTE_FLAG(W));

  alloc_proc->user_pagetable = proc_pagetable;

  return true;
}

static struct proc *alloc_proc() {
  struct proc *available_proc = find_unused_proc();

  if (available_proc == NULL) {
    return NULL;
  }

  available_proc->pid = alloc_pid();
  available_proc->state = USED;

  alloc_proc_trapframe(available_proc);
  alloc_proc_pagetable(available_proc);

  memset(&available_proc->user_context, 0, sizeof(available_proc->user_context));
  available_proc->user_context.ra = (uint64_t)test;
  available_proc->user_context.sp = available_proc->proc_kernel_stack + PAGE_SIZE;

  return available_proc;
}

void user_proc_init() {
  root_proc = alloc_proc();

  root_proc->user_trapframe->user_pc = 0;
  root_proc->user_trapframe->proc_kernel_sp = PAGE_SIZE;

  root_proc->state = RUNABLE;
}

void switch2user() {
  struct cpu *current_cpu = &cpu[READ_GRR(tp)];

  current_cpu->user_proc_running = NULL;
  while (true) {
    for (int i = 0; i < NPROC; ++i) {
      if (proc[i].state == RUNABLE) {
        proc[i].state = RUNNING;
        current_cpu->user_proc_running = &proc[i];
        context_exchange(&current_cpu->cpu_context, &proc[i].user_context);
        current_cpu->user_proc_running = NULL;
      }
    }
  }
}
