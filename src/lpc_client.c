#include "include/lpc_client.h"

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "include/lpc_client.h"
#include "include/lpc_memory.h"
#include "include/lpc_sync.h"
#include "include/lpc_utils.h"

void *lpc_open(const char *name) {
  char *shmo_name = start_with_slash(name);
  int fd = shm_open(shmo_name, O_RDWR, 0);
  if (fd < 0) ERREXIT("%s %s\n", "shm_open", strerror(errno));
  
  memory *mem;
  mem = mmap(0, sizeof(memory), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (mem == MAP_FAILED) ERREXIT("%s %s\n", "mmap", strerror(errno));
  return mem;
}

int lpc_close(void *mem) { 
  return munmap(mem, sizeof(memory));
}

int lpc_call(void *memory, const char *fun_name, ...) { 
  return 0; 
}

lpc_string *lpc_make_string(const char *s, int taille) { return NULL; }
