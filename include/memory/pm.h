#ifndef _PM_H
#define _PM_H

/// 页表大小
#define PAGE_SIZE 0x1000
/// 获得页表的起始地址
#define PAGE_START(x) ((x) & ~0xfff)
/// 获得下一个页表的起始地址
#define PAGE_UP(x) PAGE_START(x + PAGE_SIZE)
/// 获取上一个页表的起始地址
#define PAGE_DOWN(x) PAGE_START(x - PAGE_SIZE)

/// 可用物理地址的最大值
#define PHYSIC_MEM_TOP 0x88000000l

/// 系统内核起始位置
#define KERNEL_BASE 0x80200000l

// kinit
void physic_memory_init();
// kalloc
void* alloc_physic_page();
// kfree
void free_physic_page(void *addr);

#endif
