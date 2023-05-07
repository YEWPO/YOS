#ifndef _DEGUG_H
#define _DEGUG_H

#include "lib/stdio.h"
#include "marco.h"

/// 打印日志信息
#define Log(format, ...) \
  printf(ANSI_FMT("[%s:%d %s] " format, ANSI_FG_BLUE) "\n", \
    __FILE__, __LINE__, __func__, ## __VA_ARGS__)

/// Assert的变种实现
#define Assert(cond, format, ...) do { \
  if (!(cond)) { \
    printf(ANSI_FMT("Failed on [%s:%d %s], reason: " format, ANSI_FG_RED) "\n", \
    __FILE__, __LINE__, __func__, ## __VA_ARGS__); \
    printf(ANSI_FMT("panic", ANSI_FG_GREEN ANSI_BG_RED) "\n"); \
    for (;;); \
  } \
} while (0)

#endif
