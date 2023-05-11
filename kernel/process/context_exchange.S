.equ XLEN, 8

.section .text
.global context_exchange
context_exchange:
  sd ra, 0 * XLEN(a0)
  sd sp, 1 * XLEN(a0) 
  sd s0, 2 * XLEN(a0) 
  sd s1, 3 * XLEN(a0)
  sd s2, 4 * XLEN(a0)
  sd s3, 5 * XLEN(a0)
  sd s4, 6 * XLEN(a0)
  sd s5, 7 * XLEN(a0)
  sd s6, 8 * XLEN(a0)
  sd s7, 9 * XLEN(a0)
  sd s8, 10 * XLEN(a0)
  sd s9, 11 * XLEN(a0)
  sd s10, 12 * XLEN(a0)
  sd s11, 13 * XLEN(a0)

  ld ra, 0 * XLEN(a1)
  ld sp, 1 * XLEN(a1) 
  ld s0, 2 * XLEN(a1) 
  ld s1, 3 * XLEN(a1)
  ld s2, 4 * XLEN(a1)
  ld s3, 5 * XLEN(a1)
  ld s4, 6 * XLEN(a1)
  ld s5, 7 * XLEN(a1)
  ld s6, 8 * XLEN(a1)
  ld s7, 9 * XLEN(a1)
  ld s8, 10 * XLEN(a1)
  ld s9, 11 * XLEN(a1)
  ld s10, 12 * XLEN(a1)
  ld s11, 13 * XLEN(a1)

  ret
