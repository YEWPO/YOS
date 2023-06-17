#include "debug.h"
#include "kernel.h"
#include "macro.h"

struct virtq device_virtq;
struct spinlock block_device_lock;

// 设备正在处理的事件信息
struct {
  struct buffer_block *buffer;
  bool status;
  bool notify;
} device_event[VIRTQ_NUM];

// 已经处理过的事件编号
uint16_t handled_idx;

uint16_t free_descriptor_stack[VIRTQ_NUM];
int free_descriptor_stack_top;

struct virtio_blk_req operation_req[VIRTQ_NUM];

/**
 * 初始化设备参数
 *
 * @return void 无返回
 */
void device_init() {
  Log("Initalizing mmio device");

  Assert((*VIRTIO_MMIO_REG(MMIO_MAGIC_VALUE) == 0x74726976)
      && (*VIRTIO_MMIO_REG(MMIO_VERSION) == 0x2), "MMIO INFO ERROR!");

  Assert(*VIRTIO_MMIO_REG(MMIO_DEVICE_ID) != 0x0, "Invalid Device!");

  int status = 0;

  // set zero to reset the device
  *VIRTIO_MMIO_REG(MMIO_STATUS) = status;

  // set acknowledge bit
  status |= DEVICE_STATUS_ACKNOWLEDGE;
  *VIRTIO_MMIO_REG(MMIO_STATUS) = status;

  // set driver bit
  status |= DEVICE_STATUS_DRIVER;
  *VIRTIO_MMIO_REG(MMIO_STATUS) = status;

  // set device features
  uint64_t features = *VIRTIO_MMIO_REG(MMIO_DEVICE_FEATURES);
  features = CLEAR_BIT(features, VIRTIO_BLK_F_CONFIG_WCK);
  features = CLEAR_BIT(features, VIRTIO_F_INDIRECT_DESC);
  features = CLEAR_BIT(features, VIRTIO_F_EVENT_IDX);
  *VIRTIO_MMIO_REG(MMIO_DEVICE_FEATURES) = features;

  // set feature_ok bit
  status |= DEVICE_STATUS_FEATURES_OK;
  *VIRTIO_MMIO_REG(MMIO_STATUS) = status;

  status = *VIRTIO_MMIO_REG(MMIO_STATUS);
  Assert((status & DEVICE_STATUS_FEATURES_OK) == DEVICE_STATUS_FEATURES_OK,
      "Device feature write failed!");

  // Initalizing queue
  *VIRTIO_MMIO_REG(MMIO_QUEUE_SEL) = 0;
  Assert(*VIRTIO_MMIO_REG(MMIO_QUEUE_READY) == 0, "Initalizing device queue cannot ready!");

  int virtq_maxnum = *VIRTIO_MMIO_REG(MMIO_QUEUE_NUM_MAX);

  Assert(virtq_maxnum >= VIRTQ_NUM, "Virtio queue num error!");

  device_virtq.num = VIRTQ_NUM;
  *VIRTIO_MMIO_REG(MMIO_QUEUE_NUM) = device_virtq.num;

  device_virtq.desc = alloc_physic_page();
  device_virtq.avail = alloc_physic_page();
  device_virtq.used = alloc_physic_page();

  Assert(device_virtq.desc && device_virtq.avail && device_virtq.used, "device virtq alloc failed!");

  *VIRTIO_MMIO_REG(MMIO_QUEUE_DESC_LOW) = (uint64_t)device_virtq.desc;
  *VIRTIO_MMIO_REG(MMIO_QUEUE_DESC_HIGH) = (uint64_t)device_virtq.desc >> 32;
  *VIRTIO_MMIO_REG(MMIO_QUEUE_DRIVER_LOW) = (uint64_t)device_virtq.avail;
  *VIRTIO_MMIO_REG(MMIO_QUEUE_DRIVER_HIGH) = (uint64_t)device_virtq.avail >> 32;
  *VIRTIO_MMIO_REG(MMIO_QUEUE_DEVICE_LOW) = (uint64_t)device_virtq.used;
  *VIRTIO_MMIO_REG(MMIO_QUEUE_DEVICE_HIGH) = (uint64_t)device_virtq.used >> 32;

  // queue is ready
  *VIRTIO_MMIO_REG(MMIO_QUEUE_READY) = 0x1;

  // update device status for ready
  status |= DEVICE_STATUS_DRIVER_OK;
  *VIRTIO_MMIO_REG(MMIO_STATUS) = status;

  // init descriptor stack
  for (int i = 0; i < VIRTQ_NUM; ++i) {
    free_descriptor_stack[free_descriptor_stack_top++] = i;
  }

  Log("Initalized device!");
}

/**
 * 分配3个描述符用于一个读写事件
 *
 * @param descriptor_set 描述符集合的起始地址
 * 如果分配成功，会将分配的描述符下标写入到这个集合中
 *
 * @return bool 成功分配返回true，分配失败返回false
 */
bool alloc_descriptors(uint16_t *descriptor_set) {
  if (free_descriptor_stack_top < 3) {
    // no enough descriptor
    return false;
  }

  // have descriptors
  for (int i = 0; i < 3; ++i) {
    descriptor_set[i] = free_descriptor_stack[--free_descriptor_stack_top];
  }

  return true;
}

/**
 * 释放3个描述符
 *
 * @param descriptor_set 描述符集合的起始地址
 *
 * @return void 无返回
 */
void free_descriptors(uint16_t *descriptor_set) {
  for (int i = 0; i < 3; ++i) {
    memset(&device_virtq.desc[descriptor_set[i]], 0, sizeof(struct virtq_desc));
    free_descriptor_stack[free_descriptor_stack_top++] = descriptor_set[i];
  }
  wakeup(free_descriptor_stack);
}

/**
 * 设备读写操作
 *
 * @param buffer 缓冲块地址
 * @param write 操作类型
 *
 * @return void 无返回
 *
 */
void device_operation(struct buffer_block *buffer, bool write) {
  acquire_lock(&block_device_lock);

  // get descriptors
  uint16_t descriptor_set[3];
  while (true) {
    if (alloc_descriptors(descriptor_set)) {
      break;
    }
    sleep(free_descriptor_stack, &block_device_lock);
  }

  // init req
  struct virtio_blk_req *req = &operation_req[descriptor_set[0]];

  req->type = write ? VIRTIO_BLK_T_OUT : VIRTIO_BLK_T_IN;
  req->reserved = 0;
  req->sector = buffer->sector_id;
  device_event[descriptor_set[0]].buffer = buffer;
  device_event[descriptor_set[0]].status = true;
  device_event[descriptor_set[0]].notify = false;

  device_virtq.desc[descriptor_set[0]].addr = (uint64_t)req;
  device_virtq.desc[descriptor_set[0]].len = sizeof(struct virtio_blk_req);
  device_virtq.desc[descriptor_set[0]].flags = VIRTIO_DESC_F_NEXT;
  device_virtq.desc[descriptor_set[0]].next = descriptor_set[1];

  device_virtq.desc[descriptor_set[1]].addr = (uint64_t)buffer->data;
  device_virtq.desc[descriptor_set[1]].len = SECTOR_SIZE;
  device_virtq.desc[descriptor_set[1]].flags = write ? 0 : VIRTIO_DESC_F_WRITE;
  device_virtq.desc[descriptor_set[1]].flags |= VIRTIO_DESC_F_NEXT;
  device_virtq.desc[descriptor_set[1]].next = descriptor_set[2];

  device_virtq.desc[descriptor_set[2]].addr = (uint64_t)&device_event[descriptor_set[0]].status; 
  device_virtq.desc[descriptor_set[2]].len = sizeof(bool);
  device_virtq.desc[descriptor_set[2]].flags = VIRTIO_DESC_F_WRITE;
  device_virtq.desc[descriptor_set[2]].next = 0;

  device_virtq.avail->ring[device_virtq.avail->idx % VIRTQ_NUM] = descriptor_set[0];
  __sync_synchronize();
  device_virtq.avail->idx++;
  __sync_synchronize();
  *VIRTIO_MMIO_REG(MMIO_QUEUE_NOTIFY) = 0;

  while (!device_event[descriptor_set[0]].notify) {
    sleep(buffer, &block_device_lock);
  }

  device_event[descriptor_set[0]].buffer = 0;
  free_descriptors(descriptor_set);

  release_lock(&block_device_lock);
}

/**
 * 设备读操作
 *
 * @param buffer 缓冲块地址
 *
 * @return void 无返回
 */
void device_read(struct buffer_block *buffer) {
  device_operation(buffer, false);
}

/**
 * 设备写操作
 *
 * @param buffer 缓冲块地址
 *
 * @return void 无返回
 */
void device_write(struct buffer_block *buffer) {
  device_operation(buffer, true);
}


/**
 * virito设备中断处理程序
 *
 * @return void 无返回
 */
void virtio_interrupt_handler() {
  Log("Block device interrupt!");

  acquire_lock(&block_device_lock);

  while (handled_idx != device_virtq.used->idx) {
    __sync_synchronize();

    uint32_t event_idx = device_virtq.used->ring[handled_idx % VIRTQ_NUM].id;
    Assert(device_event[event_idx].status == false, "Something Wrong with device read/write!");

    device_event[event_idx].buffer->dirty = false;
    wakeup(device_event[event_idx].buffer);

    handled_idx++;
  }

  *VIRTIO_MMIO_REG(MMIO_INTERRUPT_ACK) = BITS(*VIRTIO_MMIO_REG(MMIO_INTERRUPT_STATUS), 1, 0);
  __sync_synchronize();

  release_lock(&block_device_lock);
}
