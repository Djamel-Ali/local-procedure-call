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
  memory *mem = lpc_connect(shmo_name);

  char *fun_name = "hello";
  char *cl = "client";
  size_t len = strlen(cl) + sizeof(pid_t) + 2;
  char s[len];
  snprintf(s, len, "%s%d", cl, getpid());
  
  lpc_string *string = lpc_make_string(s, len * 10);
  
  int rc = lpc_call(mem, fun_name, STRING, string, NOP);
  
  if(rc == -1) ERREXIT("%s %s\n", "lpc_call", strerror(errno));

  printf("%s\n",string->string);

  return 0;
}
