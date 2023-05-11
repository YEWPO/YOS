#ifndef _DEGUG_H
#define _DEGUG_H

#include "lib/stdio.h"
#include "macro.h"
#include "riscv64.h"

static inline void panic() {
  printf(ANSI_FMT("sstatus: 0x%p\n", ANSI_FG_RED), READ_CSR(s, status));
  printf(ANSI_FMT("sepc: 0x%p\n", ANSI_FG_RED), READ_CSR(s, epc));
  printf(ANSI_FMT("scause: 0x%p\n", ANSI_FG_RED), READ_CSR(s, cause));

  printf(ANSI_FMT("panic", ANSI_FG_GREEN ANSI_BG_RED) "\n");
  for (;;);
}

/// 打印日志信息
#define Log(format, ...) \
  printf(ANSI_FMT("[%s:%d %s] " format, ANSI_FG_BLUE) "\n", \
    __FILE__, __LINE__, __func__, ## __VA_ARGS__)

/// Assert的变种实现
#define Assert(cond, format, ...) do { \
  if (!(cond)) { \
    printf(ANSI_FMT("Failed on [%s:%d %s], reason: " format, ANSI_FG_RED) "\n", \
    __FILE__, __LINE__, __func__, ## __VA_ARGS__); \
    panic(); \
  } \
} while (0)

#endif
