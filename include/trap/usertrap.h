#ifndef _USERTRAP_H
#define _USERTRAP_H

#include <stdint.h>

void user_trap_handler();

extern char trampoline[];
extern void uservec();
extern void userret(uint64_t satp);

#endif
