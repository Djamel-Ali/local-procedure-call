#include "include/lpc_sync.h"

int init_mutex(pthread_mutex_t *pmutex) {
  pthread_mutexattr_t mutexattr;
  int rc;
  if ((rc = pthread_mutexattr_init(&mutexattr)) != 0) return rc;

  if ((rc = pthread_mutexattr_setpshared(&mutexattr, PTHREAD_PROCESS_SHARED)) !=
      0)
    return rc;
  rc = pthread_mutex_init(pmutex, &mutexattr);
  return rc;
}

int init_cond(pthread_cond_t *pcond) {
  pthread_condattr_t condattr;
  int rc;
  if ((rc = pthread_condattr_init(&condattr)) != 0) return rc;
  if ((rc = pthread_condattr_setpshared(&condattr, PTHREAD_PROCESS_SHARED)) !=
      0)
    return rc;
  rc = pthread_cond_init(pcond, &condattr);
  return rc;
}