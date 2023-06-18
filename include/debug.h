#ifndef _DEGUG_H
#define _DEGUG_H

#include "lib/stdio.h"
#include "macro.h"
#include "riscv64.h"
#include "common.h"
#include "memory/pm.h"

/**
 * 打印栈信息
 *
 * @return void 无返回
 */
static inline void backtrace() {
  printf(ANSI_FMT("Backtrace: \n", ANSI_FG_YELLOW));

  uint64_t fp = READ_GRR(fp);

  while (true) {
    printf("0x%p\n", *((uint64_t *)(fp - 8)));
    uint64_t prefp = *((uint64_t *)(fp - 16));

    if (PAGE_START(fp) != PAGE_START(prefp)) {
      break;
    }

    fp = prefp;
  }
}

/**
 * 打印panic的调试信息
 *
 * @return void 无返回
 */
static inline void panic() {
  printf(ANSI_FMT("sstatus: 0x%p\n", ANSI_FG_RED), READ_CSR(s, status));
  printf(ANSI_FMT("sepc: 0x%p\n", ANSI_FG_RED), READ_CSR(s, epc));
  printf(ANSI_FMT("scause: 0x%p\n", ANSI_FG_RED), READ_CSR(s, cause));
  backtrace();

  printf(ANSI_FMT("panic", ANSI_FG_GREEN ANSI_BG_RED) "\n");
  for (;;);
}

/// 打印日志信息
#ifdef DEBUG
#define Log(format, ...) \
  printf(ANSI_FMT("[%s:%d %s] " format, ANSI_FG_BLUE) "\n", \
    __FILE__, __LINE__, __func__, ## __VA_ARGS__)
#else
#define Log(format, ...) {}
#endif

/// Assert的变种实现
#define Assert(cond, format, ...) do { \
  if (!(cond)) { \
    printf(ANSI_FMT("Failed on [%s:%d %s], reason: " format, ANSI_FG_RED) "\n", \
    __FILE__, __LINE__, __func__, ## __VA_ARGS__); \
    panic(); \
  } \
} while (0)

#endif
