#include "test.h"
#include "debug.h"
#include "kernel.h"
#include "common.h"
#include "macro.h"

void test() {
  Log("test");

  struct buffer_block *buffer;

  buffer = buffer_acquire(BLOCK_DEVICE, 0);

  for (int i = 0; i < 512; ++i) {
    printf("id %d, data %x\n", i, buffer->data[i]);
  }

  buffer->data[0] = 'm';
  buffer->data[1] = 'p';
  buffer->dirty = true;

  buffer_release(buffer);

  for (int i = 0; i < 512; ++i) {
    printf("id %d, data %x\n", i, buffer->data[i]);
  }
}
