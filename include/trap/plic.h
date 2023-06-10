#ifndef _PLIC_H
#define _PLIC_H

#include <stdint.h>
#include "macro.h"
#include "memory/vmlayout.h"

#define VIRTIO_IRQ 0x1

#define INTERRUPT_SOURCE_PRIORITY 0x0
#define INTERRUPT_PENDING_BITS    0x1000
#define INTERRUPT_ENABLE_BITS     0x2000
#define PRIORITY_THRESHOLD        0x200000
#define INTERRUPT_CLAIM_REG       0x200004
#define INTERRUPT_COMPLETION_REG  0x200004

#define PLIC_REG(addr) ((volatile uint32_t *)(PLIC + addr))

#define SET_SOURCE_PRIORITY(irq) (*PLIC_REG(INTERRUPT_SOURCE_PRIORITY + irq * 4) = 0x1)
#define SET_ENABLE_BIT(irq) (*PLIC_REG(INTERRUPT_ENABLE_BITS) = SET_BIT(0, irq))
#define SET_PRIORITY_THRESHOLD(value) (*PLIC_REG(PRIORITY_THRESHOLD) = value)

void plic_init();

#endif
