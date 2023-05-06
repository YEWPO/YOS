#include "lib/sbi.h"

/*
 * @eid: extension ID
 * @arg0: first argument
 * @arg1: second argument
 * @arg2: thrid argument
 * @arg3: fourth argument
 * call the function mapped by eid
 */
static uint64_t sbi_call(uint64_t eid, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
    uint64_t ret;
    asm volatile(
        "mv a7, %[eid]\n"
        "mv a0, %[arg0]\n"
        "mv a1, %[arg1]\n"
        "mv a2, %[arg2]\n"
        "mv a3, %[arg3]\n"
        "ecall\n"
        "mv %[ret], a0"
        : [ret] "=r" (ret)
        : [eid] "r" (eid), [arg0] "r" (arg0), [arg1] "r" (arg1), [arg2] "r" (arg2), [arg3] "r" (arg3)
        : "memory"
    );
    return ret;
}

/*
 * @stime_value: set time value
 * Programs the clock for next event after stime_value time. This function also clears the pending timer
 * interrupt bit.
 * This SBI call returns 0 upon success or an implementation specific negative error code.
 */
int64_t sbi_set_timer(uint64_t stime_value) {
    return sbi_call(SBI_SET_TIMER, stime_value, 0, 0, 0);
}

/*
 * @ch: the char needed to print
 * Write data present in ch to debug console.
 * This SBI call returns 0 upon success or an implementation specific negative error code.
 */
int64_t sbi_console_putchar(int32_t ch) {
    return sbi_call(SBI_CONSOLE_PUTCHAR, ch, 0, 0, 0);
}

/*
 * Read a byte from debug console.
 * The SBI call returns the byte on success, or -1 for failure.
 */
int64_t sbi_console_getchar(void) {
    return sbi_call(SBI_CONSOLE_GETCHAR, 0, 0, 0, 0);
}

int64_t sbi_clear_ipi(void) {
    return sbi_call(SBI_CLEAR_IPI, 0, 0, 0, 0);
}

void sbi_shutdown(void) {
    sbi_call(SBI_SHUTDOWN, 0, 0, 0, 0);
}
