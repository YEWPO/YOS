#include "lib/stdlib.h"

/**
 * 取32位整形数的绝对值
 *
 * @param x 32位整形数
 *
 * @return int 32位绝对值整形数
 */
int abs(int x) {
  if (x < 0) {
    x = -x;
  }

  return x;
}

/**
 * 返回两个数的最小值
 *
 * @param a 第一个数
 * @param b 第二个数
 *
 * @return uint64_t 最小值
 */
uint64_t min(uint64_t a, uint64_t b) {
  return a < b ? a : b;
}

/**
 * 返回两个数的最大值
 *
 * @param a 第一个数
 * @param b 第二个数
 *
 * @return uint64_t 最大值
 */
uint64_t max(uint64_t a, uint64_t b) {
  return a > b ? a : b;
}
