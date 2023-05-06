#ifndef _COMMON_H
#define _COMMON_H

// 颜色属性
#define ANSI_FG_BLACK   "\e[1;30m"
#define ANSI_FG_RED     "\e[1;31m"
#define ANSI_FG_GREEN   "\e[1;32m"
#define ANSI_FG_YELLOW  "\e[1;33m"
#define ANSI_FG_BLUE    "\e[1;34m"
#define ANSI_FG_MAGENTA "\e[1;35m"
#define ANSI_FG_CYAN    "\e[1;36m"
#define ANSI_FG_WHITE   "\e[1;37m"
#define ANSI_BG_BLACK   "\e[1;40m"
#define ANSI_BG_RED     "\e[1;41m"
#define ANSI_BG_GREEN   "\e[1;42m"
#define ANSI_BG_YELLOW  "\e[1;43m"
#define ANSI_BG_BLUE    "\e[1;44m"
#define ANSI_BG_MAGENTA "\e[1;35m"
#define ANSI_BG_CYAN    "\e[1;46m"
#define ANSI_BG_WHITE   "\e[1;47m"
#define ANSI_NONE       "\e[0m"

/// 以一种颜色组合输出
#define ANSI_FMT(str, fmt) fmt str ANSI_NONE

/// 获取一个数组的长度，包括结构体数组
#define ARRLEN(arr) ( sizeof(arr) / sizeof(arr[0]) )

/// 生成低len位的掩码
#define LOW_MASK(len) ((1ull << (len)) - 1)

/// 位获取，获取一个数[hi, lo]之间的比特位
#define BITS(x, hi, lo) ((x >> (lo)) & LOW_MASK((hi) - (lo) + 1))
/// 位获取，获取下标为p的比特
#define GET_BIT(x, p) BITS(x, p, p)
/// 在下标为p的位置上置1
#define SET_BIT(x, p) (x | (1ull << p))
/// 在下标为p的位置上置0
#define CLEAR_BIT(x, p) (x & ~(1ull << p))

#endif
