#include "macro.h"
#include "kernel.h"
#include "riscv64.h"
#include "debug.h"

void user_trap_handler() {
  Assert(GET_BIT(READ_CSR(s, status), SSTATUS_SPP) == 0x0,
         "Trap not from user!");

  WRITE_CSR(s, tvec, (uint64_t)kernelvec);
}
