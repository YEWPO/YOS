#include "debug.h"
#include "macro.h"
#include "common.h"
#include "kernel.h"

/// 内核栈
__attribute__ (( aligned(16) )) char boot_stack[PAGE_SIZE * NCPU];

/**
 * 初始化相关寄存器的值
 *
 * @return 无返回
 */
static void env_init() {
  // 将硬件线程号写入tp寄存器
  uint64_t hartid = READ_GRR(a0);
  WRITE_GRR(tp, hartid);

  // 设置sstatus的SIE位，启用中断
  SET_SSTATUS_SIE;

  // 设置SIE寄存器
  uint64_t sie = 0;
  sie = SET_BIT(sie, SIE_SEIE);
  sie = SET_BIT(sie, SIE_STIE);
  sie = SET_BIT(sie, SIE_SSIE);
  WRITE_CSR(s, ie, sie);
}

static inline void logo() {
  printf(
  "\n"
  "   ____     __   ,-----.       .-'''-.  \n"
  "   \\   \\   /  /.'  .-,  '.    / _     \\ \n"
  "    \\  _. /  '/ ,-.|  \\ _ \\  (`' )/`--' \n"
  "     _( )_ .';  \\  '_ /  | :(_ o _).    \n"
  " ___(_ o _)' |  _`,/ \\ _/  | (_,_). '.  \n"
  "|   |(_,_)'  : (  '\\_/ \\   ;.---.  \\  : \n"
  "|   `-'  /    \\ `\"/  \\  ) / \\    `-'  | \n"
  " \\      /      '. \\_/``\".'   \\       /  \n"
  "  `-..-'         '-----'      `-...-'   \n"
  "\n");
}

/**
 * 操作系统主函数
 *
 * @return 无返回
 */
void main() {
  env_init();

  int hartid = READ_GRR(tp);
  if (hartid == 0) {
    Log("YOS starting...");

    logo();

    physic_memory_init();
    kernel_pagetable_init();
    satp_init();

    kernel_trap_init();
    plic_init();

    proc_init();

    device_init();

    root_proc_init();
  } else {
    while (1) {};
  }

  switch2user();
}
