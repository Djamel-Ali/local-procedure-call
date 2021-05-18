#include "include/fun_hello.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>  // getpid()

#include "include/lpc_types.h"

int hello(void *params) {
  lpc_string *lpc_str = (lpc_string *)params;

  char *greeting = "Bonjour";
  char *suffix = "je suis le serveur";
  int size = strlen(greeting) + strlen(lpc_str->string) + strlen(suffix) +
             sizeof(pid_t) + 2;
             
  if (size > lpc_str->slen) {
    errno = ENOMEM;
    lpc_str->slen = -1;
    return -1;
  }
  char s[size+4];
  int rc = snprintf(s, size+4, "%s %s!! %s%d", greeting, lpc_str->string, suffix,
                    getpid());
  memset(lpc_str->string, 0, lpc_str->slen);
  memcpy(lpc_str->string, s, rc);
  return 0;
}
