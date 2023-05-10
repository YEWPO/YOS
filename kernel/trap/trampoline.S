.altmacro

.equ TRAPFRAME, 0x4000000000 - 0x2000

.equ XLEN, 8

.macro SAVE_REG reg, offset
  sd \reg, \offset * XLEN(a0)
.endm

.macro SAVE_N n
  SAVE_REG x\n, \n
.endm

.macro LOAD_REG reg, offset
  ld \reg, \offset * XLEN(a0)
.endm

.macro LOAD_N n
  LOAD_REG x\n, \n
.endm

.section trap_section
.global trampoline
trampoline:
.align 4

.global uservec
uservec:
  csrw sscratch, a0
  li a0, TRAPFRAME

  .set n, 1
  .rept 9
    SAVE_N %n
    .set n, n + 1
  .endr

  .set n, 11
  .rept 21
    SAVE_N %n
    .set n, n + 1
  .endr

  csrr t0, sscratch
  sd t0, 10 * XLEN(a0)

  ld t1, 32 * XLEN(a0)
  ld sp, 33 * XLEN(a0)

  sfence.vma zero, zero
  csrw satp, t1
  sfence.vma zero, zero

  j user_trap_handler

.global userret
userret:
  sfence.vma zero, zero
  csrw satp, a0
  sfence.vma zero, zero

  li a0, TRAPFRAME

  .set n, 1
  .rept 9
    LOAD_N %n
    .set n, n + 1
  .endr

  .set n, 11
  .rept 21
    LOAD_N %n
    .set n, n + 1
  .endr

  LOAD_N 10
  
  sret
