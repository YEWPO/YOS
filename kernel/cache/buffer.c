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
  return 0;
}

/**
 * 更缓存块中的数据到设备中
 *
 * @param buffer 缓存块的地址
 *
 * @return void 无返回
 */
void buffer_update(struct buffer_block *buffer) {
}

/**
 * 释放缓存块
 *
 * @param buffer 缓存块的地址
 *
 * @return void 无返回
 */
void buffer_release(struct buffer_block *buffer) {
}
