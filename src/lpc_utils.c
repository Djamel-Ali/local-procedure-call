#include "include/lpc_utils.h"

#include <stdlib.h>
#include <string.h>

char *start_with_slash(const char *str) {
  size_t len = strlen(str);
  char *name = calloc(len + 2, sizeof(char));
  name[0] = '/';
  for (size_t i = 0, j = 1; i < len; i++) {
    if (str[i] != '/') {
      name[j] = str[i];
      j++;
    }
  }
  return name;
}
