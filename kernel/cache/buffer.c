#include "kernel.h"
#include "debug.h"
#include "common.h"
#include "macro.h"

struct buffer_block buffers[NBUFFER];
struct spinlock buffers_lock;

/**
 * 从缓存区中获取一个缓存块，
 * 该块为设备号为device_id，且
 * 扇区号为sector_id持有
 *
 * @param device_id 设备号
 * @param sector_id 扇区号
 *
 * return struct buffer_block * 成功获取则返回获取的缓存块
 */
struct buffer_block *buffer_acquire(uint32_t device_id, uint32_t sector_id) {
  Log("Acquire a buffer for device %d sector %d", device_id, sector_id);

  acquire_lock(&buffers_lock);

  for (int i = 0; i < NBUFFER; ++i) {
    if (buffers[i].valid && buffers[i].device_id == device_id && buffers[i].sector_id == sector_id) {
      // hit cache
      buffers[i].reference++;
      release_lock(&buffers_lock);

      acquire_sleeplock(&buffers[i].buffer_lock);
      return &buffers[i];
    }
  }

  // not hit cache
  for (int i = 0; i < NBUFFER; ++i) {
    if (!buffers[i].valid) {
      // update infomation to this new data
      buffers[i].valid = true;
      buffers[i].device_id = device_id;
      buffers[i].sector_id = sector_id;
      buffers[i].reference = 1;
      release_lock(&buffers_lock);

      acquire_sleeplock(&buffers[i].buffer_lock);
      device_read(&buffers[i]);
      return &buffers[i];
    }
  }
  
  Assert(0, "Don't have available buffer!");

  return NULL;
}

/**
 * 更缓存块中的数据到设备中
 *
 * @param buffer 缓存块的地址
 *
 * @return void 无返回
 */
void buffer_update(struct buffer_block *buffer) {
  Log("Update buffer for device %d sector %d", buffer->device_id, buffer->sector_id);

  if (buffer->dirty) {
    device_write(buffer);
  }
}

/**
 * 释放缓存块
 *
 * @param buffer 缓存块的地址
 *
 * @return void 无返回
 */
void buffer_release(struct buffer_block *buffer) {
  Log("Release buffer for device %d sector %d", buffer->device_id, buffer->sector_id);

  Assert(sleeplock_is_locked(&buffer->buffer_lock), "Buffer lock isn't hold!");
  buffer_update(buffer);
  release_sleeplock(&buffer->buffer_lock);

  acquire_lock(&buffers_lock);
  buffer->reference--;
  if (buffer->reference == 0) {
    // not use any
    buffer->valid = false;
  }
  release_lock(&buffers_lock);
}
