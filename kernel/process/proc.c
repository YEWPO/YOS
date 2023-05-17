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

/**
 * 找到一个没有被使用的进程块
 *
 * @return struct proc* 返回找到的进程块地址
 * 如果没有找到，返回空指针
 */
static struct proc *find_unused_proc() {
  for (int i = 0; i < NPROC; ++i) {
    if (proc[i].state == UNUSED) {
      return proc + i;
    }
  }

  return NULL;
}

/**
 * 向分配的新的进程块分配trapframe空间
 *
 * @return bool 成功返回true
 */
static bool alloc_proc_trapframe(struct proc *alloc_proc) {
  alloc_proc->user_trapframe = (struct trapframe *)alloc_physic_page();
  return true;
}

/**
 * 向分配的新的进程块分配用户页表
 *
 * @return true 分配成功返回true
 */
static bool alloc_proc_pagetable(struct proc *alloc_proc) {
  pagetable_t proc_pagetable = alloc_physic_page();

  va_map_pa(proc_pagetable, TRAMPOLINE, (addr_t)trampoline, MPTE_FLAG(R) | MPTE_FLAG(X));
  va_map_pa(proc_pagetable, TRAPFRAME, (addr_t)alloc_proc->user_trapframe, MPTE_FLAG(R) | MPTE_FLAG(W));

  alloc_proc->user_pagetable = proc_pagetable;

  return true;
}

/**
 * 分配一个进程块给用户程序
 *
 * @return struct proc* 分配成功则返回进程块的地址
 * 分配失败返回空指针
 */
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

/**
 * 初始化第一各用户进程root
 *
 * @return void 无返回
 */
void user_proc_init() {
  root_proc = alloc_proc();

  root_proc->user_trapframe->user_pc = 0;
  root_proc->user_trapframe->proc_kernel_sp = PAGE_SIZE;

  root_proc->state = RUNABLE;
}

/**
 * 从内核态进程切换到用户进程
 *
 * @return void 无返回
 */
void switch2user() {
  struct cpu *current_cpu = &cpu[CPU_ID];

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

/**
 * 从用户进程切换到内核进程
 *
 * @return void 无返回
 */
void switch2kernel() {
}
