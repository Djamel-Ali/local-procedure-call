#ifndef __LPC_SYNC_H__
#define __LPC_SYNC_H__

#include <pthread.h>

int init_mutex(pthread_mutex_t *pmutex);
int init_cond(pthread_cond_t *pcond);

#endif  // __LPC_SYNC_H__