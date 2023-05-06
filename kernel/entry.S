.section entry_section, "ax"
.global _entry
.type _entry, @function
_entry:
  la sp, boot_stack
  lui t0, 0x1
  add sp, sp, t0
  call main
spin:
  j spin
