#ifndef _TRANS_H
#define _TRANS_H

#include "lock/spinlock.h"

void sleep(void *sleeplock, struct spinlock *spinlock);
void wakeup(void *sleeplock);

#endif
