#include "macro.h"
#include "kernel.h"
#include "riscv64.h"
#include "debug.h"

extern char trampoline[];
extern void uservec();
extern void userret(uint64_t satp);
extern void kernelvec();

/**
 * handle user trap
 * 
 * @return void no return
 */
void user_trap_handler() {
  Assert(GET_BIT(READ_CSR(s, status), SSTATUS_SPP) == 0x0,
         "Trap not from user!");

  WRITE_CSR(s, tvec, (uint64_t)kernelvec);

  struct proc *current_proc = cpu[READ_GRR(tp)].user_proc_running;

  current_proc->user_trapframe->user_pc = READ_CSR(s, epc);

  uint64_t scause = READ_CSR(s, cause);

  if (GET_INTERRUPT(scause)) {
    // an interrupt

    if (GET_EXCEPTION(scause) == STI) {
      timer_handler();
    }
  } else {
    // not an interrupt
    Assert(0, "user trap need to implement!");
  }
}

/**
 * resume user space
 *
 * @return void no return
 */
void user_resume() {
  struct proc *current_proc = cpu[READ_GRR(tp)].user_proc_running;

  CLEAR_SSTATUS_SIE;

  addr_t trampoline_uservec = TRAMPOLINE + ((addr_t)uservec - (addr_t)trampoline);
  WRITE_CSR(s, tvec, trampoline_uservec);

  current_proc->user_trapframe->kernel_satp = READ_CSR(s, atp);
  current_proc->user_trapframe->proc_kernel_sp = current_proc->proc_kernel_stack + PAGE_SIZE;
  current_proc->user_trapframe->kernle_user_trap_handler = (uint64_t)user_trap_handler;

  uint64_t sstatus = READ_CSR(s, status);

  sstatus = SET_BIT(sstatus, SSTATUS_SPIE);
  sstatus = CLEAR_BIT(sstatus, SSTATUS_SPP);

  WRITE_CSR(s, status, sstatus);
  WRITE_CSR(s, epc, current_proc->user_trapframe->user_pc);

  uint64_t user_satp = SET_SATP((uint64_t)current_proc->user_pagetable);

  addr_t trampoline_userret = TRAMPOLINE + ((addr_t)userret - (addr_t)trampoline);
  ((void (*)(uint64_t))trampoline_userret)(user_satp);
}
