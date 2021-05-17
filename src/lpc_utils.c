#include "include/lpc_utils.h"

#include <stdlib.h>
#include <string.h>

char *start_with_slash(const char *str) {
  int len = strlen(str);
  char name[NAME_MAX] = {0};
  name[0] = '/';
  int j = 1;
  for (size_t i = 0; i < len, j < NAME_MAX; i++, j++) {
    if (str[i] != '/') name[j] = str[i];
  }
  name[j] = '\0';
  return name;
}
