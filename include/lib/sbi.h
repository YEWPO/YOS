#ifndef _SBI_H
#define _SBI_H

#include <stdint.h>

#define SBI_SET_TIMER  0x00
#define SBI_CONSOLE_PUTCHAR  0x01
#define SBI_CONSOLE_GETCHAR 0x02
#define SBI_CLEAR_IPI 0x03
#define SBI_SEND_IPI 0x04
#define SBI_REMOTE_FENCE_I 0x05
#define SBI_REMOTE_SFENCE_VMA 0x06
#define SBI_REMOTE_SFENCE_VMA_ASID 0x07
#define SBI_SHUTDOWN 0x08

int64_t sbi_set_timer(uint64_t stime_value);
int64_t sbi_console_putchar(int32_t ch);
int64_t sbi_console_getchar(void);
int64_t sbi_clear_ipi(void);
void sbi_shutdown(void);

#endif
