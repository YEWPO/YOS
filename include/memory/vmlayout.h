#ifndef _VMLAYOUT_H
#define _VMLAYOUT_H

#include "pm.h"

/// 虚拟地址的最大值
#define  VA_MAX 0x3fffffffl

/// trampoline的虚拟地址
#define TRAMPOLINE PAGE_START(VA_MAX)

/// trapframe的虚拟地址
#define TRAPFRAME (TRAMPOLINE - PAGE_SIZE)

#endif
