#include "debug.h"

/// 内核栈
__attribute__ (( aligned(16) )) char boot_stack[4096];

void main() {
  Log("YOS starting...");
}
