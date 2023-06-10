#ifndef _PLIC_H
#define _PLIC_H

#include <stdint.h>
#include "macro.h"
#include "memory/vmlayout.h"

/// virtio interrupt request id, get from qemu --dumpdtb
#define VIRTIO_IRQ 0x1

// plic registers offset
#define INTERRUPT_SOURCE_PRIORITY 0x0
#define INTERRUPT_PENDING_BITS    0x1000
#define INTERRUPT_ENABLE_BITS     0x2000
#define PRIORITY_THRESHOLD        0x200000
#define INTERRUPT_CLAIM_REG       0x200004
#define INTERRUPT_COMPLETION_REG  0x200004

/// read or write plic registers
#define PLIC_REG(addr) ((volatile uint32_t *)(PLIC + addr))

/// set source priority
#define SET_SOURCE_PRIORITY(irq) (*PLIC_REG(INTERRUPT_SOURCE_PRIORITY + irq * 4) = 0x1)
/// set hart context interrupt enable
#define SET_ENABLE_BIT(context, irq) (*PLIC_REG(INTERRUPT_ENABLE_BITS + context * 0x80 + (irq / 32) * 4) = SET_BIT(0, irq))
/// set hart context priority threshold
#define SET_PRIORITY_THRESHOLD(context, level) (*PLIC_REG(PRIORITY_THRESHOLD + context * 0x1000) = level)
/// get hart context interrupt claim value
#define GET_CLAIM_REG(context) (*PLIC_REG(INTERRUPT_CLAIM_REG + context * 0x1000))
/// set hart context interrupt completion value
#define SET_COMPLETION_REG(context, irq) (*PLIC_REG(INTERRUPT_COMPLETION_REG + context * 0x1000) = irq)

void plic_init();

#endif
