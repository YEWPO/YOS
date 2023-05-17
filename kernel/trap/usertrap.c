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

  // set stvec to kernelvec
  WRITE_CSR(s, tvec, (uint64_t)kernelvec);

  struct proc *current_proc = current_cpu_proc();

  // save user pc to trapframe
  current_proc->user_trapframe->user_pc = READ_CSR(s, epc);

  uint64_t scause = READ_CSR(s, cause);

  if (GET_INTERRUPT(scause)) {
    // an interrupt

    if (GET_EXCEPTION(scause) == STI) {
      timer_handler();

      // yield
      yield();
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
  struct proc *current_proc = current_cpu_proc();

  CLEAR_SSTATUS_SIE;

  // set user trap vector in virtural addresss
  addr_t trampoline_uservec = TRAMPOLINE + ((addr_t)uservec - (addr_t)trampoline);
  WRITE_CSR(s, tvec, trampoline_uservec);

  // save some kernel information
  current_proc->user_trapframe->kernel_satp = READ_CSR(s, atp);
  current_proc->user_trapframe->proc_kernel_sp = current_proc->proc_kernel_stack + PAGE_SIZE;
  current_proc->user_trapframe->kernle_user_trap_handler = (uint64_t)user_trap_handler;

  /*
   * set spie bit in sstatus
   * and clear spp bit in sstatus for user mode
   * when return to user mode
   * the sie will set to the value of spie bit
   */
  uint64_t sstatus = READ_CSR(s, status);
  sstatus = SET_BIT(sstatus, SSTATUS_SPIE);
  sstatus = CLEAR_BIT(sstatus, SSTATUS_SPP);
  WRITE_CSR(s, status, sstatus);

  // write sepc to user pc
  WRITE_CSR(s, epc, current_proc->user_trapframe->user_pc);

  // write user satp
  uint64_t user_satp = SET_SATP((uint64_t)current_proc->user_pagetable);

  // jump to userret function in vitural memory
  addr_t trampoline_userret = TRAMPOLINE + ((addr_t)userret - (addr_t)trampoline);
  ((void (*)(uint64_t))trampoline_userret)(user_satp);
}
