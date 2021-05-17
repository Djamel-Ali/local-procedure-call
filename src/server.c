#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

#include "include/lpc_server.h"
#include "include/lpc_sync.h"
#include "include/lpc_utils.h"

/* Bloquer le serveur jusqu'à ce qu'un client écrive dans la mémoire [mem] */
void wait_for_call(memory *mem) {
  int rc;
  DEBUG("server[%d]: lock\n", getpid());

  rc = pthread_mutex_lock(&mem->header.mutex);
  if (rc != 0) ERREXIT("%s %s\n", "pthread_mutex_lock", strerror(rc));

  DEBUG("server[%d]: acquire lock\n", getpid());

  while (!mem->header.call) {
    DEBUG("server[%d]: release lock and wait\n\n", getpid());
    
    rc = pthread_cond_wait(&mem->header.call_cond, &mem->header.mutex);
    if (rc != 0) ERREXIT("%s %s\n", "pthread_cond_wait", strerror(rc));
    
    DEBUG("server[%d]: acquire lock after wait\n", getpid());
  }
}

/* Reveiller les clients qui attendent une modification de la mémoire [mem] */
void notify_response(memory *mem) {
  int rc;
  mem->header.res = 1;
  mem->header.call = 0;

  rc = msync(mem, sizeof(memory), MS_SYNC);
  if (rc < 0) ERREXIT("%s %s\n", "msync", strerror(errno));

  rc = pthread_mutex_unlock(&mem->header.mutex);
  if (rc != 0) ERREXIT("%s %s\n", "pthread_mutex_unlock", strerror(rc));
  DEBUG("server[%d]: release lock\n\n", getpid());

  rc = pthread_cond_signal(&mem->header.res_cond);
  if (rc != 0) ERREXIT("%s %s\n", "pthread_cond_signal", strerror(rc));
}

/* Assure la communication entre un client spécifique et un prossessus files du serveur */
void run(memory *mem, char *shmo_name) {
  int rc = pthread_mutex_lock(&mem->header.mutex);
  if (rc != 0) ERREXIT("%s %s\n", "pthread_mutex_lock", strerror(rc));

  /* lire le pid du processus client */
  pid_t pid = mem->header.pid;

  /* signaler que d'autre clients peuvent écrire dans le shared memory */
  mem->header.new = 0;
  rc = msync(mem, sizeof(memory), MS_SYNC);
  if (rc < 0) ERREXIT("%s %s\n", "msync", strerror(errno));
  rc = pthread_cond_signal(&mem->header.new_cond);
  if (rc != 0) ERREXIT("%s %s\n", "pthread_cond_signal", strerror(rc));
  rc = munmap(mem, sizeof(memory));
  if (rc != 0) ERREXIT("%s %s\n", "munmap", strerror(rc));

  /* créer le nouveau shared memory */
  char name[BUFSIZE] = {0};
  snprintf(name, BUFSIZE, "%s%d", shmo_name, pid);
  memory *client_mem = lpc_create(name, 1);
  DEBUG("server[%d]: create new shared memory %s\n\n", getpid(), name);

  /* communiquer avec le client */
  while (1) {
    wait_for_call(client_mem);
    if (client_mem->header.end) {
      lpc_free(name);
      DEBUG("server[%d]: end\n\n", getpid());
      exit(EXIT_SUCCESS);
    }
    lpc_call_fun(client_mem);
    notify_response(client_mem);
  }
}

int main(int argc, const char **argv) {
  if (argc != 2) {
    printf("usage: %s shmo_name\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  char *shmo_name = start_with_slash(argv[1]);
  memory *mem = lpc_create(shmo_name, 1);
  int rc;
  while (1) {
    wait_for_call(mem);
    switch (fork()) {
      case -1:
        ERREXIT("%s %s\n", "fork", strerror(errno));
      case 0:
        run(mem, shmo_name);
        break;
      default:
        mem->header.new = 1;
        mem->header.call = 0;
        rc = msync(mem, sizeof(memory), MS_SYNC);
        if (rc < 0) ERREXIT("%s %s\n", "msync", strerror(errno));

        rc = pthread_mutex_unlock(&mem->header.mutex);
        if (rc != 0) ERREXIT("%s %s\n", "pthread_mutex_unlock", strerror(rc));
        break;
    }
  }
  return 0;
}
