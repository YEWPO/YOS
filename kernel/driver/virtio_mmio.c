#include "debug.h"
#include "kernel.h"
#include "macro.h"

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
}
