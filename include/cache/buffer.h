#ifndef _BUFFER_H
#define _BUFFER_H

#include "driver/block_device.h"
#include <stdbool.h>

struct buffer_block {
  bool valid;
  bool dirty;
  uint32_t device_id;
  uint32_t sector_id;
  uint8_t data[SECTOR_SIZE];
};

#define NBUFFER 32

extern struct buffer_block buffer[NBUFFER];

#endif
