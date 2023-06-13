#include "lib/string.h"

/**
 * 内存复制，复制长度为n
 *
 * 目标块不可以和源块重叠
 * 如果重叠，请使用memmove
 *
 * @param dest 目标内存的地址
 * @param src 源内存地址
 *
 * @return void* 目标内存地址
 */
void *memcpy(void *dest, const void* src, size_t n) {
  size_t i = 0;

  const char *s = src;
  char *d = dest;

  while (i < n) {
    d[i] = s[i];
    i++;
  }

  return d;
}

/**
 * 内存复制，复制长度为n
 *
 * @param dest 目标内存的地址
 * @param src 源内存地址
 *
 * @return void* 目标内存地址
 */
void *memmove(void *dest, const void* src, size_t n) {
  const char *s = src;
  char *d = dest;

  size_t i;

  if (dest <= src) {
    i = 0;
    while (i < n) {
     d[i] = s[i];
     i++;
    }
  } else {
    i = n;
    while (i > 0) {
      d[i] = s[i];
      i--;
    }
  }

  return d;
}

/**
 * 初始化内存值
 *
 * @param addr the address of the area
 * @param c the byte value need to set
 * @param n the size of the area need to set value
 *
 * @return void* the address of the area
 */
void *memset(void *addr, int c, size_t n) {
  char *ptr = (char*)addr;

  for (int i = 0; i < n; ++i) {
    ptr[i] = c;
  }

  return ptr;
}

/**
 * 获取字符串的长度
 *
 * @param s the address of the string
 *
 * @return size_t the length of input string
 */
size_t strlen(const char *s) {
  size_t length = 0;

  while (s[length] != '\0') {
    length++;
  }

  return length;
}

/**
 * 复制字符串
 *
 * @param dest the destination string
 * @param src the source string
 *
 * @return char* the destination string address
 */
char *strcpy(char *dest, const char *src) {
  size_t i = 0;

  while (src[i] != '\0') {
    dest[i] = src[i];
    i++;
  }

  dest[i] = '\0';

  return dest;
}

/**
 * 复制长度为n的字符串
 *
 * @param dest the destination string
 * @param src the source string
 * @param n the size of string needed to copy
 *
 * @return char* the destination string address
 */
char *strncpy(char *dest, const char *src, size_t n) {
  size_t i = 0;

  while (i < n && src[i] != '\0') {
    dest[i] = src[i];
    i++;
  }

  if (i != n) {
    dest[i] = '\0';
  }

  return dest;
}

/**
 * 字符串比较
 *
 * @param s1 first string
 * @param s2 second string
 *
 * @return int an integer less than,
 * equal to, or greater than zero if s1 (or the first n
 * bytes thereof) is found, respectively, to
 * be less than, to match, or be greater than s2.
 */
int strcmp(const char *s1, const char *s2) {
  size_t i = 0;

  while (s1[i] != '\0' && s2[i] != '\0' && s1[i] == s2[i]) {
    i++;
  }

  return s1[i] - s2[i];
}

/**
 * 比较长度为n的字符串
 *
 * @param s1 first string
 * @param s2 second string
 * @param n the size of string needed to compare
 *
 * @return int an integer less than,
 * equal to, or greater than zero if s1 (or the first n
 * bytes thereof) is found, respectively, to
 * be less than, to match, or be greater than s2.
 */
int strncmp(const char *s1, const char *s2, size_t n) {
  size_t i = 0;

  while (i < n && s1[i] != '\0' && s2[i] != '\0' && s1[i] == s2[i]) {
    i++;
  }

  return i == n ? 0 : s1[i] - s2[i];
}

/**
 * 拼接字符串
 *
 * @param dst 目标串的起始地址
 * @param src 源串的起始地址
 *
 * @return 返回拼接后的字符串的起始地址
 */
char *strcat(char *dst, const char *src) {
  size_t i = 0;
  while (dst[i] != '\0') {
    i++;
  }

  size_t j = 0;

  while (src[j] != '\0') {
    dst[i] = src[j];
    i++;
    j++;
  }

  dst[i] = '\0';

  return dst;
}

/**
 * 比较两个内存块的内容
 *
 * @param s1 第一个内存块的起始地址
 * @param s2 第二个内存块的起始地址
 * @param n 需要比较的的块的大小
 *
 * @return 如果第一个内存块的内容小于第二内存块，则返回小于0的数
 * 相等返回0
 * 大于返回大于0的数
 */
int memcmp(const void *s1, const void *s2, size_t n) {
  const char *p1 = s1;
  const char *p2 = s2;
  size_t i = 0;

  while (i < n && p1[i] != '\0' && p2[i] != '\0' && p1[i] == p2[i]) {
    i++;
  }

  return i == n ? 0 : p1[i] - p2[i];
}
