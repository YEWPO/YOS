#ifndef _BLOCK_DEVICE_H
#define _BLOCK_DEVICE_H

#include <stdint.h>
#include "cache/buffer.h"

#define BLOCK_DEVICE 1

#define VIRTIO_BLK_T_IN           0
#define VIRTIO_BLK_T_OUT          1
#define VIRTIO_BLK_T_FLUSH        4
#define VIRTIO_BLK_T_GET_ID       8
#define VIRTIO_BLK_T_GET_LIFETIME 10
#define VIRTIO_BLK_T_DISCARD      11
#define VIRTIO_BLK_T_WRITE_ZEROES 13
#define VIRTIO_BLK_T_SECURE_ERASE 14

/// 块读取写入请求体
struct virtio_blk_req {
  uint32_t type;
  uint32_t reserved;
  uint64_t sector;
} __attribute__((packed));

/// read mmio resgister macro
#define VIRTIO_MMIO_REG(name) ((volatile uint32_t *)(VIRTIO_MMIO + (name)))

void device_init();
void virtio_interrupt_handler();
void device_read(struct buffer_block *buffer);
void device_write(struct buffer_block *buffer);

#endif
