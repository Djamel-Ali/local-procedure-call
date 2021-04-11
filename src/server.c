#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#include "include/lpc_server.h"
#include "include/lpc_sync.h"
#include "include/lpc_utils.h"

int main(int argc, const char **argv) {
  if (argc != 2) {
    printf("usage: %s shmo_name\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  memory *mem = lpc_create(argv[1], 1);
  int rc;
  while (1) {
    rc = pthread_mutex_lock(&mem->header.mutex);
    if (rc != 0) thread_error(__FILE__, __LINE__, rc, "pthread_mutex_lock");

    while (!mem->header.call) { /*tant qu'il n'y a pas une fonction à appeler*/
      rc = pthread_cond_wait(&mem->header.call_cond, &mem->header.mutex);
      if (rc != 0) thread_error(__FILE__, __LINE__, rc, "pthread_cond_wait");
    }

    printf("La fonction à appler est : %s\n", mem->data.fun_name);
    // TODO appeler la fonction
    // TODO mettre à jour les paramètres

    mem->header.res = 1;
    mem->header.call = 0;

    rc = msync(mem, sizeof(memory), MS_SYNC);
    if (rc < 0) err_exit("msync", __FILE__, __LINE__);

    rc = pthread_mutex_unlock(&mem->header.mutex);
    if (rc != 0) thread_error(__FILE__, __LINE__, rc, "pthread_mutex_unlock");

    rc = pthread_cond_signal(&mem->header.res_cond);
    if (rc != 0) thread_error(__FILE__, __LINE__, rc, "pthread_cond_signal");
  }

  return 0;
}
