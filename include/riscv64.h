#ifndef _RISCV64_H
#define _RISCV64_H

#include <stdint.h>

/// 读取通用寄存器宏
#define READ_GRR(name) ({ \
    uint64_t _tmp; \
    asm volatile("mv %0, " #name :"=r"(_tmp)); \
    _tmp; \
    }) 

/// 写入通用寄存器宏
#define WRITE_GRR(name, val) ({ \
    uint64_t _tmp = val; \
    asm volatile("mv " #name ", %0": :"r"(_tmp)); \
    })

/// 读取控制状态寄存器, lev表示M-MODE, S-MODE, U-MODE或者其他的
#define READ_CSR(lev, name) ({ \
    uint64_t _tmp; \
    asm volatile("csrr %0, " #lev #name :"=r"(_tmp)); \
    _tmp; \
    })

/// 写入控制状态寄存器, lev表示M-MODE, S-MODE, U-MODE或者其他的
#define WRITE_CSR(lev, name, val) ({ \
    uint64_t _tmp = val; \
    asm volatile("csrw " #lev #name ", %0": :"r"(_tmp)); \
    })

/// 更新TLB
#define FENCE_VMA do { asm volatile("sfence.vma zero, zero"); } while(0)

// sstatus
#define SSTATUS_SIE 1
#define SSTATUS_SPIE 5
#define SSTATUS_SPP 8

// stvec
#define STVEC_MODE 0

// sie
#define SIE_SSIE 1
#define SIE_STIE 5
#define SIE_SEIE 9

// satp
#define Sv39 (8ull << 60)
#define SET_SATP(addr) (Sv39 | (addr >> 12))

#endif
