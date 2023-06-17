.section user_start_section, "ax"
.global user_start
.type user_start, @function
user_start:
  addi a0,a1, 0
  ecall
spin:
  j spin
