#include "debug.h"
#include "kernel.h"
#include "macro.h"

struct virtq device_virtq;

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

  Log("Initalized device!");
}

struct virtio_blk_req new_req;

void test() {
  new_req.type = VIRTIO_BLK_T_IN;
  new_req.sector = 0;
  new_req.status = -1;
 
  device_virtq.desc[0].addr = (uint64_t)&new_req;
  device_virtq.desc[0].len = 16;
  device_virtq.desc[0].flags = VIRTIO_DESC_F_NEXT;
  device_virtq.desc[0].next = 1;

  device_virtq.desc[1].addr = (uint64_t)&new_req.data;
  device_virtq.desc[1].len = SECTOR_SIZE;
  device_virtq.desc[1].flags = VIRTIO_DESC_F_NEXT;
  device_virtq.desc[1].flags |= VIRTIO_DESC_F_WRITE;
  device_virtq.desc[1].next = 2;

  device_virtq.desc[2].addr = (uint64_t)&new_req.status;
  device_virtq.desc[2].len = 1;
  device_virtq.desc[2].flags = VIRTIO_DESC_F_WRITE;

  device_virtq.avail->ring[device_virtq.avail->idx % device_virtq.num] = 0;

  __sync_synchronize();

  device_virtq.avail->idx++;

  __sync_synchronize();

  *VIRTIO_MMIO_REG(MMIO_QUEUE_NOTIFY) = 0;

  while (true);
}

void virtio_interrupt_handler() {
  for (int i = 0; i < SECTOR_SIZE; ++i) {
    printf("read id %d's value %x\n",i, new_req.data[i]);
  }
}
