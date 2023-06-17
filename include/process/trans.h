#ifndef _TRANS_H
#define _TRANS_H

void sleep(void *condition, struct spinlock *spinlock);
void wakeup(void *condition);

#endif
