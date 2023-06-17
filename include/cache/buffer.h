#ifndef _BUFFER_H
#define _BUFFER_H

#include "driver/block_device.h"
#include <stdbool.h>

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

extern struct buffer_block buffer[NBUFFER];

#endif
