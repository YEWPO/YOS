#ifndef _KERNELTRAP_H
#define _KERNELTRAP_H

#include "common.h"

/// supervisor software interrupt
#define SSI 0x1
/// supervisor timer interrupt
#define STI 0x5
/// supervisor external interrupt
#define SEI 0x9

/// get scause interrupt bit
#define GET_INTERRUPT(x) GET_BIT(x, 63)
/// get scause exception code
#define GET_EXCEPTION(x) BITS(x, 62, 0) 

void kernelvec();
void kernel_trap_init();
void timer_handler();

#endif
