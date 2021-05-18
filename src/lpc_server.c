#define _XOPEN_SOURCE 500

#include "include/lpc_server.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "include/fun_divide_double.h"
#include "include/fun_hello.h"
#include "include/fun_print_n_times.h"
#include "include/lpc_sync.h"
#include "include/lpc_utils.h"

static lpc_function FUNCTIONS[] = {{"hello", hello},
                                   {"print_n_times", print_n_times},
                                   {"divide_double", divide_double}};

int (*lpc_get_fun(const char *fun_name))(void *) {
  int len = sizeof(FUNCTIONS) / sizeof(lpc_function);
  for (int i = 0; i < len; i++) {
    if (!strcmp(fun_name, FUNCTIONS[i].fun_name)) {
      return FUNCTIONS[i].fun;
    }
  }
  return (void *)0;
}

static int create_shom(const char *shom_name, size_t size) {
  char *name = start_with_slash(shom_name);
  int fd = shm_open(name, O_CREAT | O_RDWR, S_IWUSR | S_IRUSR);
  if (fd < 0) ERREXIT("%s %s\n", "shm_open", strerror(errno));
  if (ftruncate(fd, size) < 0) ERREXIT("%s %s\n", "ftruncate", strerror(errno));
  return fd;
}

memory *lpc_create(const char *shmo_name, size_t capacity) {
  if (capacity < 1) return NULL;
  int fd = create_shom(shmo_name, sysconf(_SC_PAGESIZE) * capacity);
  memory *mem =
      mmap(NULL, sizeof(memory), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if ((void *)mem == MAP_FAILED) ERREXIT("%s %s\n", "mmap", strerror(errno));
  return mem;
}

void lpc_init_header(memory *mem) {
  mem->header.new = 0;  /* tout client peut écrire dans la mémoire partagée*/
  mem->header.call = 0; /* pas encore de fonction à appeler*/
  mem->header.res = 0;  /* pas de resultat d'appel de fonction*/
  mem->header.er = 0;
  mem->header.rc = 0;
  mem->header.end = 0;

  int rc = init_mutex(&mem->header.mutex);
  if (rc != 0) ERREXIT("%s %s\n", "init_mutex", strerror(errno));

  rc = init_cond(&mem->header.call_cond);
  if (rc != 0) ERREXIT("%s %s\n", "init_cond", strerror(errno));

  rc = init_cond(&mem->header.res_cond);
  if (rc != 0) ERREXIT("%s %s\n", "init_cond", strerror(errno));

  rc = init_cond(&mem->header.new_cond);
  if (rc != 0) ERREXIT("%s %s\n", "init_cond", strerror(errno));

  rc = init_cond(&mem->header.end_cond);
  if (rc != 0) ERREXIT("%s %s\n", "init_cond", strerror(errno));

  rc = msync(mem, sizeof(memory), MS_SYNC);
  if (rc < 0) ERREXIT("%s %s\n", "msync", strerror(errno));
}

void lpc_call_fun(memory *mem) {
  DEBUG("-->lpc_call_fun<--\nserver[%d] %s\n", getpid(), mem->data.fun_name);
  int (*f)(void *) = lpc_get_fun(mem->data.fun_name);
  if(f == NULL){
    mem->header.rc = -1;
    mem->header.er = ENONET;
    return;
  }
  int rc = f(mem->data.params);
  mem->header.rc = rc;
  if (rc == -1) mem->header.er = errno;
}

void lpc_free(const char *shmo_name) {
  DEBUG("-->lpc_free<--\nserver[%d]\n", getpid());
  if (shmo_name != NULL && shm_unlink(shmo_name) == -1)
    ERREXIT("%s %s\n", "shm_unlink", strerror(errno));
}