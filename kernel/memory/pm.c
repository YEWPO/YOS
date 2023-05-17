#include "kernel.h"
#include "common.h"
#include "debug.h"

/// 可用物理地址的最小值
extern char end[];

/// 物理页结构
struct pPage {
  struct pPage *next;
};

static struct pPage *_free;
static struct spinlock free_list_lock = {0};

/**
 * 初始化可以使用的物理页面链表
 */
static void init_free_list() {
  void* satp = end;

  for (; (uint64_t)satp < PHYSIC_MEM_TOP; satp += PAGE_SIZE) {
    free_physic_page(satp);
  }
}

/**
 * 物理内存初始化
 */
void physic_memory_init(){
  Log("Initializing physical memory");

  init_free_list();

  Log("Initialized physical memory");
}

/**
 * 分配一个物理页面
 *
 * @return 分配成功返回一个void类型指针, 分配失败返回为0
 */
void* alloc_physic_page() {
  struct pPage *page;

  acquire_lock(&free_list_lock);
  page = _free;
  if (page != NULL) {
    _free = _free->next;
  }
  release_lock(&free_list_lock);

  if (page) {
    memset(page, 0, PAGE_SIZE);
  }

  return page;
}

/**
 * @param addr 需要释放页面的起始地址
 *
 * @return 释放成功后返回
 */
void free_physic_page(void* addr) {
  Assert(addr != NULL, "The page to be freed is null!");
  Assert(addr >= (void*)end && (uint64_t)addr < PHYSIC_MEM_TOP, "Invalid page address to be freed!");
  Assert((uint64_t)addr == PAGE_START((uint64_t)addr), "The page to be freed is not aligned!");

  memset(addr, 0xcd, PAGE_SIZE);
  struct pPage *page = addr;

  acquire_lock(&free_list_lock);
  page->next = _free;
  _free = page;
  release_lock(&free_list_lock);
}
