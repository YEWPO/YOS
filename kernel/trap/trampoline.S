.altmacro

.section trap_section
.global trampoline
trampoline:
.align 4
.global uservec
uservec:
  nop
.global userret
userret:
  nop
