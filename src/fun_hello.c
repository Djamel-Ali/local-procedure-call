#include "include/fun_hello.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h> // getpid()
#include <sys/types.h>

int hello(void *params) {
    int slen, rc;
    memcpy(&slen, params, sizeof(int));
    char str[slen];
    memcpy(str, params + sizeof(int), slen);

    char *gratting = "Bonjour ";
    char *suffix = "; je suis : \"Serveur ";
    size_t size = strlen(gratting) + strlen(str) + strlen(suffix) + sizeof(pid_t) + 1;
    if (size > slen) {
        rc = -1;
        memcpy(params, &rc, sizeof(int));
        errno = ENOMEM;
        return -1;
    }
    char s[size];
    rc = snprintf(s, size + 4, "%s\"%s\"%s%d\"", gratting, str, suffix,
                  getpid()); // (size + 3 * '\"' + 1car fin de chaine)
    memset(params, 0, rc);
    memcpy(params, &rc, sizeof(int));
    memcpy(params + sizeof(int), s, rc);

    return 0;
}
