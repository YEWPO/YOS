#ifndef _VM_H
#define _VM_H

#include "macro.h"
#include "common.h"

// virtual address
/// 获取虚拟地址对应级别的虚拟页面号
#define VA_VPN(x, lev) BITS(x, 20 + lev * 9, 12 + lev * 9)
/// 获取虚拟地址的偏移量
#define VA_PO(x) BITS(x, 11, 0)

// physical address
/// 获取物理地址的物理页面的起始地址
#define PA_PPN(x) BITS(x, 55, 12)
/// 获取物理地址的偏移量
#define PA_PO(x) BITS(x, 11, 0)

/// 将物理地址转化为PTE的PPN
#define PA2PTE(x) (PA_PPN(x) << 10)

// PTE
/// 获取PTE中的物理页面起始地址
#define PTE_PPN(x) BITS(x, 53, 10)
/// The number of PTE in a page
#define NR_PTE 512

#define _F 9
#define _C 8
#define _D 7
#define _A 6
#define _G 5
#define _U 4
#define _X 3
#define _W 2
#define _R 1
#define _V 0
/// 获取PTE的标志位
#define GPTE_FLAG(x, tag) GET_BIT(x, _ ## tag)
/// 获取PTE的标志位
#define GPTE_FLAGS(x) BITS(x, 9, 0)
/// 设置PTE的标志位
#define SPTE_FLAG(x, tag) SET_BIT(x, _ ## tag)
/// 创建PTE的标志位
#define MPTE_FLAG(tag) SPTE_FLAG(0, tag)
/// 设置PTE的标志位
#define SPTE_FLAGS(x, flags) (x | GPTE_FLAGS(flags))

/// 截取PTE中的PPN并转化为PA
#define PTE2PA(x) (PTE_PPN(x) << 12)

#define IS_LEAF(x) (BITS(x, 9, 1) != 0 && GPTE_FLAG(x, V))

typedef uint64_t *pagetable_t;
typedef uint64_t pte_t;

void satp_init();
word_t va_map_pa(pagetable_t pagetable, addr_t va, addr_t pa, addr_t flags);
word_t va_unmap_pa(pagetable_t pagetable, addr_t va);
void kernel_pagetable_init();
void free_pagetable(pagetable_t pagetable);
addr_t va2pa(pagetable_t pagetable, addr_t va);
bool mem_kernel2user(pagetable_t pagetable, addr_t user_Va, addr_t kernel_pa, size_t size);

#endif
