#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "include/lpc_client.h"
#include "include/lpc_memory.h"
#include "include/lpc_utils.h"

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("usage: %s shmo_name\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  char *shmo_name = start_with_slash(argv[1]);

  int fd = shm_open(shmo_name, O_RDWR, 0);
  if (fd < 0) ERREXIT("%s %s\n", "shm_open", strerror(errno));

  memory *mem =
      mmap(0, sizeof(memory), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (mem == MAP_FAILED) ERREXIT("%s %s\n", "mem", strerror(errno));

  int rc;

  /* etablir la première connexion */

  DEBUG("client[%d]: lock\n", getpid());
  rc = pthread_mutex_lock(&mem->header.mutex);
  if (rc != 0) ERREXIT("%s %s\n", "pthread_mutex_lock", strerror(rc));
  DEBUG("client[%d]: acquire lock\n", getpid());

  while (mem->header.new) {
    DEBUG("client[%d]: release lock and wait\n", getpid());
    rc = pthread_cond_wait(&mem->header.new_cond, &mem->header.mutex);
    if (rc != 0) ERREXIT("%s %s\n", "pthread_cond_wait", strerror(rc));
    DEBUG("client[%d]: acquire lock after wait\n", getpid());
  }

  mem->header.pid = getpid();
  mem->header.call = 1;
  rc = msync(mem, sizeof(memory), MS_SYNC);
  if (rc < 0) ERREXIT("%s %s\n", "msync", strerror(errno));

  rc = pthread_mutex_unlock(&mem->header.mutex);
  if (rc != 0) ERREXIT("%s %s\n", "pthread_mutex_unlock", strerror(rc));
  DEBUG("client[%d]: release lock\n", getpid());

  rc = pthread_cond_signal(&mem->header.call_cond);
  if (rc != 0) ERREXIT("%s %s\n", "pthread_cond_signal", strerror(rc));

  rc = munmap(mem, sizeof(memory));
  if (rc != 0) ERREXIT("%s %s\n", "munmap", strerror(rc));
  close(fd);

  /* attendre que le nouveau shared memory soit crée */
  char name[BUFSIZE] = {0};
  snprintf(name, BUFSIZE, "%s%d", shmo_name, getpid());
  while (1) {
    fd = shm_open(name, O_RDWR, 0);
    if (fd != -1) break;
    if (errno != ENOENT) ERREXIT("%s %s\n", "shm_open", strerror(errno));
  }
  DEBUG("client[%d]: new shared memory created %s\n\n", getpid(), name);

  mem = mmap(0, sizeof(memory), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (mem == MAP_FAILED) ERREXIT("%s %s\n", "mmap", strerror(errno));

  int i = 0;
  while (1) {
    memset(&mem->data, 0, sizeof(mem->data));
    if (++i < 5) {
      char *fun_name = "hello";
      char *cl = "client ";
      size_t len =
          strlen(cl) + sizeof(pid_t) + 1;  // 1 caractère de fin de chaine
      char s[len];
      snprintf(s, len, "%s%d", cl, getpid());
      int taille = 60;
      lpc_string *lpc_str = lpc_make_string(s, taille);
      memcpy(mem->data.fun_name, fun_name, strlen(fun_name));
      memcpy(mem->data.params, lpc_str, sizeof(lpc_string) + taille);
    } else {
      mem->header.end = 1;
    }

    mem->header.res = 0;
    mem->header.call = 1;

    rc = msync(mem, sizeof(memory), MS_SYNC);
    if (rc != 0) ERREXIT("%s %s\n", "msync", strerror(errno));

    rc = pthread_mutex_unlock(&mem->header.mutex);
    if (rc != 0) ERREXIT("%s %s\n", "pthread_mutex_unlock", strerror(rc));

    rc = pthread_cond_signal(&mem->header.call_cond);
    if (rc != 0) ERREXIT("%s %s\n", "pthread_cond_signal", strerror(rc));

    while (!mem->header.res) { /*tant qu'il n'y a pas de resultat à lire*/
      DEBUG("client[%d]: release lock and wait\n", getpid());
      rc = pthread_cond_wait(&mem->header.res_cond, &mem->header.mutex);
      if (rc != 0) ERREXIT("%s %s\n", "pthread_cond_wait", strerror(rc));
      DEBUG("client[%d]: acquire lock after wait\n", getpid());
    }

    if (mem->header.rc == -1) {
      fprintf(stderr, "hello: %s\n", strerror(mem->header.er));
    } else {
      lpc_string *str = (lpc_string *)mem->data.params;
      printf("slen: %d\n", str->slen);
      printf("string: %s\n", str->string);
    }

    rc = pthread_mutex_unlock(&mem->header.mutex);
    if (rc != 0) ERREXIT("%s %s\n", "pthread_mutex_unlock", strerror(rc));
    if (i == 5) break;
  }

  munmap(mem, sizeof(memory));
  close(fd);

  return mem->header.rc;
}
