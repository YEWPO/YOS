#include "macro.h"
#include "kernel.h"
#include "debug.h"
#include "lib/sbi.h"

/// timer gap
#define INTERRUPT_TIME 0x100000l

/**
 * read current time
 *
 * @return uint64_t current time
 */
static uint64_t read_time() {
  return READ_CSR(, time);
}

/**
 * set next timer_interrupt time
 *
 * @return void no return
 */
void set_next_timer() {
  sbi_set_timer(read_time() + INTERRUPT_TIME);
}

/**
 * handle timer interrupt
 *
 * @return no return 
 */
void timer_handler() {
  Log("Timer Interrupt!");
  set_next_timer();
}
