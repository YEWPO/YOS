#include "kernel.h"
#include "common.h"
#include "debug.h"

/// 内核页表
static pagetable_t kernel_pagetable;
/// 内核代码结束位置
extern char endtext[];
/// 内核只读数据结束位置
extern char endrodata[];
/// trampoline的地址
extern char trampoline[];

void satp_init() {
  Log("Set satp for main hart");
  FENCE_VMA;
  WRITE_CSR(s, atp, SET_SATP((uint64_t)kernel_pagetable));
  FENCE_VMA;
  Log("Finish set satp");
}

/**
 * 分配页表
 *
 * @return 分配成功返回页表地址，失败返回0
 */
static pagetable_t alloc_pagetable() {
  return alloc_physic_page();
}

/**
 * 建立虚拟地址到物理地址的映射关系
 *
 * @param pagetable 页表地址
 * @param va 虚拟地址, 地址必须页对齐
 * @param pa 物理地址, 地址必须页对齐
 * @param flags 标志位, 低9位有效
 *
 * @return bool 成功返回1，失败返回0（包括页表不存在）
 */
bool va_map_pa(pagetable_t pagetable, addr_t va, addr_t pa, word_t flags) {
  Assert(va == PAGE_START(va), "va not page align");
  Assert(pa == PAGE_START(pa), "pa not page align");

  for (int i = 2; i > 0; --i) {
    pte_t *pte = &pagetable[VA_VPN(va, i)];

    if (!GPTE_FLAG(*pte, V)) {
      // PTE无效
      pagetable = alloc_pagetable();
      
      if (pagetable == NULL) {
        Log("va_map_pa alloc pagetable failed!");
        return false;
      }

      *pte = SPTE_FLAG(PA2PTE((addr_t)pagetable), V);
    } else {
      // PTE有效
      pagetable = (pagetable_t)PTE2PA(*pte);
    }
  }

  pagetable[VA_VPN(va, 0)] = SPTE_FLAGS(PA2PTE(pa), flags | MPTE_FLAG(V));

  return true;
}

/**
 * 解除虚拟地址到物理地址的映射关系
 * 不会释放物理空间
 *
 * @param pagetable 页表地址
 * @param va 虚拟地址, 地址必须页对齐
 *
 * @return bool 成功返回1, 失败返回0
 */
bool va_unmap_pa(pagetable_t pagetable, addr_t va) {
  Assert(va == PAGE_START(va), "va not page align");

  for (int i = 2; i > 0; --i) {
    pte_t *pte = &pagetable[VA_VPN(va, i)];

    if (!GPTE_FLAG(*pte, V)) {
      Log("va_unmap_pa va not map to pa");
      return 0;
    }

    pagetable = (pagetable_t)PTE2PA(*pte);
  }

  pagetable[VA_VPN(va, 0)] = 0;

  return true;
}

/**
 * 初始化内核页表
 *
 * @return 分配成功且正常建立映射关系后正常返回
 */
void kernel_pagetable_init() {
  Log("Initializing kernel pagetable");

  kernel_pagetable = alloc_physic_page();
  Assert(kernel_pagetable != NULL, "alloc kernel pagetable failed!");

  // map text part
  addr_t kaddr = KERNEL_BASE;
  for (; kaddr < (addr_t)endtext; kaddr += PAGE_SIZE) {
    va_map_pa(kernel_pagetable, kaddr, kaddr, MPTE_FLAG(R) | MPTE_FLAG(X));
  }

  // map rodata part
  for (; kaddr < (addr_t)endrodata; kaddr += PAGE_SIZE) {
    va_map_pa(kernel_pagetable, kaddr, kaddr, MPTE_FLAG(R));
  }

  // map data part
  for (; kaddr < PHYSIC_MEM_TOP; kaddr += PAGE_SIZE) {
    va_map_pa(kernel_pagetable, kaddr, kaddr, MPTE_FLAG(R) | MPTE_FLAG(W));
  }

  // map kernel stack
  for (int i = 0; i < NPROC; ++i) {
    addr_t proc_kernel_stack_pa = (addr_t)alloc_physic_page();
    Assert(proc_kernel_stack_pa != 0, "alloc proc kernel stack failed!");

    addr_t proc_kernel_stack_va = PROC_KERNEL_STACK(i);

    va_map_pa(kernel_pagetable, proc_kernel_stack_va, proc_kernel_stack_pa, MPTE_FLAG(R) | MPTE_FLAG(W));
  }

  // map trampoline part
  va_map_pa(kernel_pagetable, TRAMPOLINE, (addr_t)trampoline, MPTE_FLAG(R) | MPTE_FLAG(X));

  // map virtio
  va_map_pa(kernel_pagetable, VIRTIO_MMIO, VIRTIO_MMIO, MPTE_FLAG(R) | MPTE_FLAG(X));

  Log("Initialized kernel pagetable");
}

/**
 * 释放页表操作
 * 如果下一级是一个有效页表，则去遍历这个页表进行释放
 *
 * @param pagetable_t pagetable 页表起始地址
 *
 * @return 无返回
 */
void free_pagetable(pagetable_t pagetable) {
  Assert((addr_t)pagetable == PAGE_START((addr_t)pagetable), "free_pagetable: not a pagetable!");

   for (int i = 0; i < NR_PTE; ++i) {
     pte_t pte = pagetable[i];

     if (IS_LEAF(pte)) {
       // is a leaf pte and will get a not pagetable block
       continue;
     }
    
     if (GPTE_FLAG(pte, V)) {
       // not a leaf but is valid
       free_pagetable((pagetable_t)PTE2PA(pte));
       pagetable[i] = 0;
     }
   }

   free_physic_page(pagetable);
}

/**
 * 将虚拟地址转换为物理地址
 *
 * @param pagetable 页表
 * @param va 虚拟地址
 *
 * @return addr_t 返回物理地址，失败返回-1
 */
addr_t va2pa(pagetable_t pagetable, addr_t va) {
  for (int i = 2; i > 0; --i) {
    pte_t pte = pagetable[VA_VPN(va, i)];

    if (!GPTE_FLAG(pte, V)) {
      // PTE无效
      return -1;
    } else {
      // PTE有效
      pagetable = (pagetable_t)PTE2PA(pte);
    }
  }

  pte_t pte = pagetable[VA_VPN(va, 0)];

  if (!GPTE_FLAG(pte, V)) {
    return -1;
  }

  return PTE2PA(pte) | VA_PO(va);
}

/**
 * 将内核地址空间的内容复制到用户地址空间上
 *
 * @param pagetable 用户页表
 * @param user_va 用户虚拟地址
 * @param kernel_pa 内核物理地址
 * @param size 复制内容的大小
 *
 * @return bool 成功返回true，失败返回false
 */
bool mem_kernel2user(pagetable_t pagetable, addr_t user_va, addr_t kernel_pa, size_t size) {
  addr_t user_page_start;
  addr_t user_pa;

  while (size > 0) {
    user_page_start = PAGE_START(user_va);
    user_pa = va2pa(pagetable, user_va);

    if (user_pa == -1) {
      return false;
    }

    size_t copy_size = PAGE_SIZE - (user_va - user_page_start);

    if (copy_size > size) {
      copy_size = size;
    }

    memmove((void *)user_pa, (void *)kernel_pa, copy_size);

    user_va += copy_size;
    kernel_pa += copy_size;
    size -= copy_size;
  }

  return true;
}
