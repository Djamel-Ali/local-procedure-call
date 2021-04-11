#define _XOPEN_SOURCE 500
#include "include/lpc_server.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "include/lpc_sync.h"
#include "include/lpc_utils.h"

static int fun_dificile(void *params) { return 0; }

static lpc_function FUNCTIONS[] = {{"fun_dificile", fun_dificile}};

static memory *create_shom(const char *shom_name, size_t size) {
  char *name = start_with_slash(shom_name);

  int fd = shm_open(name, O_CREAT | O_RDWR, S_IWUSR | S_IRUSR);
  if (fd < 0) err_exit("shm_open", __FILE__, __LINE__);

  if (ftruncate(fd, size) < 0) err_exit("ftruncate", __FILE__, __LINE__);

  memory *mem =
      mmap(NULL, sizeof(memory), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if ((void *)mem == MAP_FAILED) err_exit("mmap", __FILE__, __LINE__);

  return mem;
}

static void init_header(memory *mem) {
  mem->header.call = 0; /* pas encore de fonction à appeler*/
  mem->header.res = 0;  /* pas de resultat d'appel de fonction*/
  mem->header.er = 0;
  mem->header.rc = 0;

  int rc;
  rc = init_mutex(&mem->header.mutex);
  if (rc != 0) thread_error(__FILE__, __LINE__, rc, "init_mutex");

  rc = init_cond(&mem->header.call_cond);
  if (rc != 0) thread_error(__FILE__, __LINE__, rc, "init_cond");

  rc = init_cond(&mem->header.res_cond);
  if (rc != 0) thread_error(__FILE__, __LINE__, rc, "init_cond");
}

memory *lpc_create(const char *shom_name, size_t capacity) {
  if (capacity < 1) return NULL;
  memory *mem = create_shom(shom_name, sysconf(_SC_PAGESIZE) * capacity);
  init_header(mem);
  return mem;
}

void *lpc_get_fun(const char *fun_name) {
  int len = sizeof(FUNCTIONS) / sizeof(lpc_function);
  for (size_t i = 0; i < len; i++) {
    if (!strcmp(fun_name, FUNCTIONS[i].fun_name)) {
      return FUNCTIONS[i].fun;
    }
  }
  return NULL;
}
