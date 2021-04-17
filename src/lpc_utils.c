#include "include/lpc_utils.h"

#include <stdlib.h>
#include <string.h>

char *start_with_slash(const char *str) {
  int len = strlen(str);
  //TODO name should not contains /
  char *name = malloc(len + 1);
  name[0] = '/';
  memcpy(name + 1, str, len);

  name[len + 1] = '\0';
  return name;
}


