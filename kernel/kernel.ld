OUTPUT_ARCH(riscv)
ENTRY(_entry)
BASE_ADDRESS = 0x80200000;

SECTIONS
{
  . = BASE_ADDRESS;
  .text : {
    *(entry_section)
    *(.text .text.*)
    . = ALIGN(0x1000);
    *(trap_section)
  }
  . = ALIGN(0x1000);
  PROVIDE(endtext = .);

  .rodata : {
    . = ALIGN(16);
    *(.srodata .srodata.*)
    . = ALIGN(16);
    *(.rodata .rodata.*)
  }
  . = ALIGN(0x1000);
  PROVIDE(endrodata = .);

  .data : {
    . = ALIGN(16);
    *(.data .data.*)
    . = ALIGN(16);
    *(.sdata .sdata.*)
  }

  .bss : {
    . = ALIGN(16);
    *(.bss .bss.*)
    . = ALIGN(16);
    *(.sbss .sbss.*)
  }

  . = ALIGN(0X1000);
  PROVIDE(end = .);
}
