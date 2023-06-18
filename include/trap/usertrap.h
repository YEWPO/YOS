#ifndef _USERTRAP_H
#define _USERTRAP_H

#include <stdint.h>

#define CALL_FROM_U 0x8

void user_trap_handler();
void user_resume();

#endif
