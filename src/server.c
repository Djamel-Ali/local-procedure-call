#define _XOPEN_SOURCE 700
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "include/lpc_server.h"
#include "include/lpc_sync.h"
#include "include/lpc_utils.h"

char *lpc_shom = NULL;

/* Bloquer le serveur jusqu'à ce qu'un client écrive dans la mémoire [mem] */
static void wait_for_call(memory *mem) {
  DEBUG("-->wait_for_call<--\nserver[%d]: lock\n", getpid());

  int rc = pthread_mutex_lock(&mem->header.mutex);
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
static void notify_response(memory *mem) {
  DEBUG("-->notify_response<--server[%d]\n", getpid());

  mem->header.res = 1;
  mem->header.call = 0;

  int rc = msync(mem, sizeof(memory), MS_SYNC);
  if (rc < 0) ERREXIT("%s %s\n", "msync", strerror(errno));

  rc = pthread_mutex_unlock(&mem->header.mutex);
  if (rc != 0) ERREXIT("%s %s\n", "pthread_mutex_unlock", strerror(rc));
  DEBUG("server[%d]: release lock\n\n", getpid());

  rc = pthread_cond_signal(&mem->header.res_cond);
  if (rc != 0) ERREXIT("%s %s\n", "pthread_cond_signal", strerror(rc));
}

static void notify_connection(memory *mem) {
  DEBUG("-->notify_connection<--\nserver[%d]\n", getpid());

  mem->header.end = 1;
  mem->header.new = 0;

  int rc = msync(mem, sizeof(memory), MS_SYNC);
  if (rc < 0) ERREXIT("%s %s\n", "msync", strerror(errno));

  rc = pthread_mutex_unlock(&mem->header.mutex);
  if (rc != 0) ERREXIT("%s %s\n", "pthread_mutex_unlock", strerror(rc));
  DEBUG("server[%d]: release lock\n\n", getpid());

  rc = pthread_cond_signal(&mem->header.new_cond);
  if (rc != 0) ERREXIT("%s %s\n", "pthread_cond_signal", strerror(rc));

  rc = pthread_cond_signal(&mem->header.end_cond);
  if (rc != 0) ERREXIT("%s %s\n", "pthread_cond_signal", strerror(rc));

  rc = munmap(mem, sizeof(memory));
  if (rc != 0) ERREXIT("%s %s\n", "munmap", strerror(rc));
}

/* Assure la communication entre un client spécifique et un prossessus files du
 * serveur */
static void run(memory *mem, char *shmo_name) {
  int rc = pthread_mutex_lock(&mem->header.mutex);
  if (rc != 0) ERREXIT("%s %s\n", "pthread_mutex_lock", strerror(rc));

  /* créer le nouveau shared memory */
  char name[NAME_MAX] = {0};
  snprintf(name, NAME_MAX, "%s%d", shmo_name, mem->header.pid);
  memory *client_mem = lpc_create(name, 1);
  lpc_init_header(client_mem);
  DEBUG("server[%d]: create new shared memory %s\n\n", getpid(), name);

  notify_connection(mem);

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

static void handler_terminate(int sig) {
  if (sig == SIGINT || sig == SIGTERM) {
    lpc_free(lpc_shom);
    char *msg="server closed.\n";
    write(1, msg, strlen(msg));
    _exit(EXIT_SUCCESS);
  }
}

int main(int argc, const char **argv) {
  if (argc != 2) {
    printf("usage: %s shmo_name\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  struct sigaction act;

  act.sa_handler = handler_terminate;
  act.sa_flags = 0;
  sigfillset(&act.sa_mask);
  if (sigaction(SIGINT, &act, NULL) == -1)
    ERREXIT("%s %s\n", "sigaction", strerror(errno));

  act.sa_handler = handler_terminate;
  act.sa_flags = 0;
  sigfillset(&act.sa_mask);
  if (sigaction(SIGTERM, &act, NULL) == -1)
    ERREXIT("%s %s\n", "sigaction", strerror(errno));

  lpc_shom = start_with_slash(argv[1]);
  memory *mem = lpc_create(lpc_shom, 1);
  lpc_init_header(mem);

  int rc;
  pid_t pid;
  while (1) {
    wait_for_call(mem);
    switch (pid = fork()) {
      case -1:
        ERREXIT("%s %s\n", "fork", strerror(errno));
      case 0:
        run(mem, lpc_shom);
        break;
      default:
        DEBUG("server[%d]: new process child created %d\n", getpid(), pid);

        mem->header.new = 1;
        mem->header.call = 0;
        rc = msync(mem, sizeof(memory), MS_SYNC);
        if (rc < 0) ERREXIT("%s %s\n", "msync", strerror(errno));

        DEBUG("server[%d]: release lock after fork\n\n", getpid());
        rc = pthread_mutex_unlock(&mem->header.mutex);
        if (rc != 0) ERREXIT("%s %s\n", "pthread_mutex_unlock", strerror(rc));

        pid = waitpid(-1, 0, WNOHANG);
        if (pid < 0)
          ERREXIT("%s %s\n", "waitpid", strerror(errno));
        else
          DEBUG("server[%d]: zombi %d deleted\n\n", getpid(), pid);

        break;
    }
  }
  return 0;
}
