.altmacro

.equ XLEN, 8

.macro SAVE_REG reg, offset
  sd \reg, \offset * XLEN(sp)
.endm

.macro SAVE_N n
  SAVE_REG x\n, \n
.endm

.macro LOAD_REG reg, offset
  ld \reg, \offset * XLEN(sp)
.endm

.macro LOAD_N n
  LOAD_REG x\n, \n
.endm

.section .text
.global kernelvec
.align 4
kernelvec:
  addi sp, sp, 32 * XLEN

  .set n, 1
  .rept 31
    SAVE_N %n
    .set n, n + 1
  .endr

  call kernel_trap_handler

  SAVE_N 1
  SAVE_N 2
  SAVE_N 3

  # not tp (contains hartid), in case we moved CPUs

  .set n, 5
  .rept 27
    LOAD_N %n
    .set n, n + 1
  .endr

  addi sp, sp, 32 * XLEN

  sret
