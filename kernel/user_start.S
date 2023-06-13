.section user_start_section, "ax"
.global user_start
user_start:
  li a0, 0
spin:
  j spin
