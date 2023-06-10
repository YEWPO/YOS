#include "kernel.h"
#include "debug.h"
#include "common.h"

/**
 * 檢查該鎖是否被鎖上
 *
 * @return bool 鎖上返回true
 * 未被鎖上返回false
 */
bool sleeplock_is_locked(struct sleeplock *lock) {
  bool ret;

  acquire_lock(&lock->spinlock);
  ret = lock->is_locked;
  release_lock(&lock->spinlock);

  return ret;
}

/**
 * 獲得一個睡眠鎖
 *
 * @param lock 一個睡眠鎖的地址
 *
 * @return void 獲得到鎖後無返回
 */
void acquire_sleeplock(struct sleeplock *lock) {
  acquire_lock(&lock->spinlock);

  while (lock->is_locked) {
    sleep(lock, &lock->spinlock);
  }

  lock->is_locked = true;

  release_lock(&lock->spinlock);
}

/**
 * 釋放一個睡眠鎖
 *
 * @param lock 一個睡眠鎖的地址
 *
 * @return void 釋放鎖之後無返回
 */
void release_sleeplock(struct sleeplock *lock) {
  acquire_lock(&lock->spinlock);

  lock->is_locked = false;

  wakeup(lock);

  release_lock(&lock->spinlock);
}
