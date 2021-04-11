#include "include/lpc_utils.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *start_with_slash(const char *str) {
  int len = strlen(str);
  char *name = malloc(len + 1);
  name[0] = '/';
  memcpy(name + 1, str, len);
  name[len + 1] = '\0';
  return name;
}

// TODO : use a macro function instead of a function
void err_exit(const char *msg, const char *file, int line) {
  if (msg == NULL) {
    fprintf(stderr, "%s:%d %s\n", file, line, strerror(errno));
  } else {
    fprintf(stderr, "%s - %s:%d %s\n", msg, file, line, strerror(errno));
  }
  exit(EXIT_FAILURE);
}

