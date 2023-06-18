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

// process id for next alloc proc
static int next_pid = 1;
static struct spinlock pid_lock = {0};

/**
 * alloc a pid for a proc
 *
 * @return int pid for a proc
 */
static int alloc_pid() {
  int pid;

  acquire_lock(&pid_lock);
  pid = next_pid;
  next_pid++;
  release_lock(&pid_lock);
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
    proc[i].proc_lock.is_locked = false;
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
    acquire_lock(&proc[i].proc_lock);
    if (proc[i].state == UNUSED) {
      Log("get proc %d unused and alloc it", i);
      return proc + i;
    } else {
      release_lock(&proc[i].proc_lock);
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
    // not proc available
    return NULL;
  }

  available_proc->pid = alloc_pid();

  alloc_proc_trapframe(available_proc);
  alloc_proc_pagetable(available_proc);

  // init ra and sp
  memset(&available_proc->user_context, 0, sizeof(available_proc->user_context));
  available_proc->user_context.ra = (uint64_t)user_env_init;
  available_proc->user_context.sp = available_proc->proc_kernel_stack + PAGE_SIZE;

  // set state
  available_proc->state = USED;

  return available_proc;
}

extern char user_start[];

/**
 * 初始化第一各用户进程root
 *
 * @return void 无返回
 */
void root_proc_init() {
  root_proc = alloc_proc();

  root_proc->user_trapframe->user_pc = 0;
  root_proc->user_trapframe->x2 = PAGE_SIZE;

  // 为用户进程分配空间并且分配标志位
  void *user_code_page = alloc_physic_page();
  va_map_pa(root_proc->user_pagetable, 0, (addr_t)user_code_page, MPTE_FLAG(X) | MPTE_FLAG(W) | MPTE_FLAG(R) | MPTE_FLAG(U));
  // 将用户代码写入用户虚拟内存
  mem_kernel2user(root_proc->user_pagetable, 0, (addr_t)user_start, PAGE_SIZE);

  root_proc->state = RUNABLE;

  release_lock(&root_proc->proc_lock);
}

/**
 * 初始化用户进程的系统环境
 *
 * @return void 无返回
 */
void user_env_init() {
  release_lock(&current_cpu_proc()->proc_lock);

  user_resume();
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
    // make device can interrupt
    SET_SSTATUS_SIE;

    for (int i = 0; i < NPROC; ++i) {
      acquire_lock(&proc[i].proc_lock);
      if (proc[i].state == RUNABLE) {
        Log("get proc %d to run", i);

        proc[i].state = RUNNING;
        current_cpu->user_proc_running = &proc[i];

        // switch context
        context_exchange(&current_cpu->cpu_context, &proc[i].user_context);

        // finish runing and set back
        current_cpu->user_proc_running = NULL;
      }
      release_lock(&proc[i].proc_lock);
    }
  }
}

/**
 * 从用户进程切换到内核进程
 *
 * @return void 无返回
 */
void switch2kernel() {
  struct proc *current_proc = current_cpu_proc();

  Assert(spinlock_is_locked(&current_proc->proc_lock), "The proc lock isn't hold during switch to kernel");
  Assert(cpu[CPU_ID].lock_num == 1, "The cpu hold more than 1 lock during switch to kernel");
  Assert(current_proc->state != RUNNING, "The proc is running during switch to kernel");
  Assert(GET_SSTATUS_SIE == 0, "The interrupt is on during switch to kernel");

  // switch to kernel
  bool prev_cpu_locked_status = cpu[CPU_ID].prev_lock_status;
  context_exchange(&current_proc->user_context, &cpu[CPU_ID].cpu_context);
  cpu[CPU_ID].prev_lock_status = prev_cpu_locked_status;
}

/**
 * 时间片用完，切换进程
 *
 * @return void 无返回
 */
void yield() {
  struct proc *current_proc = current_cpu_proc();
  acquire_lock(&current_proc->proc_lock);
  current_proc->state = RUNABLE;
  switch2kernel();
  release_lock(&current_proc->proc_lock);
}
