#ifndef _VIRTIO_QUEUE_H
#define _VIRTIO_QUEUE_H

#include <stdint.h>

#define VIRTIO_DESC_F_NEXT      1
#define VIRTIO_DESC_F_WRITE     2
#define VIRTIO_DESC_F_INDIRECT  4

#define VIRTQ_USED_F_NO_NOTIFY      1
#define VIRTQ_AVAIL_F_NO_INTERRUPT  1

struct virtq_desc {
  uint64_t addr;
  uint32_t len;
  uint16_t flags;
  uint16_t next;
} __attribute__((packed));

struct virtq_avail {
  uint16_t flags;
  uint16_t idx;
  uint16_t ring[];
#ifdef USE_VIRTIO_F_EVENT_IDX
  uint16_t used_event;
#endif
} __attribute__((packed));

struct virtq_used_elem {
  uint32_t id;
  uint32_t len;
} __attribute__((packed));

struct virtq_used {
  uint16_t flags;
  uint16_t idx;
  struct virtq_used_elem ring[];
#ifdef USE_VIRTIO_F_EVENT_IDX
  uint16_t avail_event;
#endif
} __attribute__((packed));

struct virtq {
  unsigned int num;

  struct virtq_desc *desc;
  struct virtq_avail *avail;
  struct virtq_used *used;
};

#endif
