#include "kernel.h"
#include "debug.h"
#include "macro.h"

void plic_init() {
  Log("PLIC initalizing..");

  SET_SOURCE_PRIORITY(VIRTIO_IRQ);
  SET_ENABLE_BIT(SUPERVISOR_CONTEXT, VIRTIO_IRQ);
  SET_PRIORITY_THRESHOLD(SUPERVISOR_CONTEXT, PLIC_NO_MASK);

  Log("PLIC initalized");
}
