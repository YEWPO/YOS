#ifndef _VIRTIO_MMIO_H
#define _VIRTIO_MMIO_H

#include "memory/vmlayout.h"
#include <stdint.h>

// MMIO Device Register Layout
#define MMIO_MAGIC_VALUE         0x000
#define MMIO_VERSION             0x004
#define MMIO_DEVICE_ID           0x008
#define MMIO_VENDOR_ID           0x00c
#define MMIO_DEVICE_FEATURES     0x010
#define MMIO_DEVICE_FEATURES_SEL 0x014
#define MMIO_DRIVER_FEATURES     0x020
#define MMIO_DRIVER_FEATURES_SEL 0x024
#define MMIO_QUEUE_SEL           0x030
#define MMIO_QUEUE_NUM_MAX       0x034
#define MMIO_QUEUE_NUM           0x038
#define MMIO_QUEUE_READY         0x044
#define MMIO_QUEUE_NOTIFY        0x050
#define MMIO_INTERRUPT_STATUS    0x060
#define MMIO_INTERRUPT_ACK       0x064
#define MMIO_STATUS              0x070
#define MMIO_QUEUE_DESC_LOW      0x080
#define MMIO_QUEUE_DESC_HIGH     0x084
#define MMIO_QUEUE_DRIVER_LOW    0x090
#define MMIO_QUEUE_DRIVER_HIGH   0x094
#define MMIO_QUEUE_DEVICE_LOW    0x0a0
#define MMIO_QUEUE_DEVICE_HIGH   0x0a4
#define MMIO_SHM_SEL             0x0ac
#define MMIO_SHM_LEN_LOW         0x0b0
#define MMIO_SHM_LEN_HIGH        0x0b4
#define MMIO_SHM_BASE_LOW        0x0b8
#define MMIO_SHM_BASE_HIGH       0x0bc
#define MMIO_QUEUE_RESET         0x0c0
#define MMIO_CONFIG_GENERATION   0x0fc
#define MMIO_CONFIG              0x100

// read mmio resgister macro
#define VIRTIO_MMIO_REG(name) ((volatile uint32_t *)(VIRTIO_MMIO + (name)))

#endif
