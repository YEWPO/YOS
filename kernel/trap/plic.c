#include "kernel.h"
#include "debug.h"
#include "macro.h"

void plic_init() {
  Log("PLIC initalizing..");

  SET_SOURCE_PRIORITY(VIRTIO_IRQ);
  SET_ENABLE_BIT(VIRTIO_IRQ);
  SET_PRIORITY_THRESHOLD(0);

  Log("PLIC initalized..");
}
