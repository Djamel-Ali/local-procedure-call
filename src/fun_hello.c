#include "include/fun_hello.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

int hello(void* params) {
  int slen, rc;
  memcpy(&slen, params, sizeof(int));
  char str[slen];
  memcpy(str, params + sizeof(int), slen);

  char* gratting = "Bonjour";
  int size = strlen(str) + strlen(gratting) + 1;
  if (size > slen) {
    rc = -1;
    memcpy(params, &rc, sizeof(int));
    errno = ENOMEM;
    return -1;
  }
  char s[size];
  rc = snprintf(s, size + 1, "Bonjour %s", str);
  memset(params, 0, rc);
  memcpy(params, &rc, sizeof(int));
  memcpy(params + sizeof(int), s, rc);

  return 0;
}
