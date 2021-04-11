#include "include/lpc_sync.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void thread_error(const char *file, int line, int code, char *msg) {
  if (msg != NULL)
    fprintf(stderr, "[pid %d] [%s] in file %s in line %d :  %s\n",
            (int)getpid(), msg, file, line, strerror(code));
  else
    fprintf(stderr, "[pid %d] in file %s in line %d :  %s\n", (int)getpid(),
            file, line, strerror(code));
  exit(1);
}

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