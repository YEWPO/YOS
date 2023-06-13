# Kill process (QEMU) on gdb exits
define hook-quit
  kill
end

# connect remote
target remote localhost:27000
file build/kernel.bin
break *0x80200000
continue
layout asm
focus cmd
set architecture riscv:rv64
set disassemble-next-line auto
set riscv use-compressed-breakpoints yes
