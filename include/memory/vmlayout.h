#ifndef _VMLAYOUT_H
#define _VMLAYOUT_H

#include "pm.h"
#include "marco.h"

/// 虚拟地址的最大值
#define  VA_MAX SET_BIT(0, 38)

/// trampoline的虚拟地址
#define TRAMPOLINE (VA_MAX - PAGE_SIZE)

/// trapframe的虚拟地址
#define TRAPFRAME (TRAMPOLINE - PAGE_SIZE)

/// 用户进程内核栈的栈底地址
#define PROC_KERNEL_STACK(index) (TRAMPOLINE - (index + 1) * 2 * PAGE_SIZE)

#endif
