#ifndef _BUFFER_H
#define _BUFFER_H

#include <stdbool.h>
#include <stdint.h>

/// 每个扇区的大小
#define SECTOR_SIZE 512


/// 缓存块结构体
struct buffer_block {
  // 该缓存块是否有效
  bool valid;
  // 该缓存块数据是否已经被更新
  bool dirty;
  // 该缓存块所属的设备编号
  uint32_t device_id;
  // 该缓存块所属的扇区号
  uint32_t sector_id;
  // 缓存引用计数
  uint32_t reference;
  // 数据区
  uint8_t data[SECTOR_SIZE];
};

/// 缓存区的大小
#define NBUFFER 32

extern struct buffer_block buffers[NBUFFER];

struct buffer_block *buffer_acquire(uint32_t device_id, uint32_t sector_id);
void buffer_update(struct buffer_block *buffer);
void buffer_release(struct buffer_block *buffer);

#endif
