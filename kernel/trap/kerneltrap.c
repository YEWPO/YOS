#include "macro.h"
#include "kernel.h"
#include "debug.h"
#include "lib/sbi.h"

extern void kernelvec();

/**
 * initialize kernel_trap
 *
 * @return void no return
 */
void kernel_trap_init() {
  Log("Set stvec");
  WRITE_CSR(s, tvec, (uint64_t)kernelvec);
  Log("Finish set stvec");
  
  Log("Set timer interrupt");
  set_next_timer();
  Log("Finish timer interrupt");
}

/**
 * handle the kernel trap
 *
 * @return void no return
 */
void kernel_trap_handler() {
  Assert(GET_BIT(READ_CSR(s, status), SSTATUS_SPP) == 0x1, "Trap not from kernel!");
  Assert(!GET_SSTATUS_SIE, "kernel trap interrupt is enabled!");

  uint64_t scause = READ_CSR(s, cause);
  uint64_t sstatus = READ_CSR(s, status);
  uint64_t sepc = READ_CSR(s, epc);

  if (GET_INTERRUPT(scause)) {
    // an interrupt

    uint64_t exception = GET_EXCEPTION(scause);

    if (exception == STI) {
      // a timer interrupt
      timer_handler();

      // yield
      if (current_cpu_proc() != NULL && current_cpu_proc()->state == RUNNING) {
        yield();
      }
    } else if (exception == SEI) {
      // a device interrupt
      int irq = GET_CLAIM_REG(SUPERVISOR_CONTEXT);

      Log("the interrupt of device is: %d", irq);

      if (irq == VIRTIO_IRQ) {
        virtio_interrupt_handler();
      } else {
        // Unknown device
        Assert(0, "Unknown device!");
      }

      SET_COMPLETION_REG(SUPERVISOR_CONTEXT, irq);
    } else {
      // an interrupt not implement
      Assert(0, "kernel trap need to implement!");
    }
  } else {
    // not an interrupt
    Assert(0, "kernel trap need to implement!");
  }

  WRITE_CSR(s, status, sstatus);
  WRITE_CSR(s, epc, sepc);
}
