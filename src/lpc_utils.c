#include "include/lpc_utils.h"

#include <stdlib.h>
#include <string.h>

char *start_with_slash(const char *str) {
    size_t len = strlen(str);
    char *name = malloc(len + 2); // '/' at the beginning , strlen(str) in the middle and '\0' at the end,
    name[0] = '/';
    memcpy(name + 1, str, len);

    name[len + 1] = '\0';
    return name;
}

//todo: replace 'start_with_slash()' by 'prefix_slash()' because the 1st one accepts names like '//name'...
// c.f 'man shm_open' (...For portable use, a shared memory object should be identified by a name  of  the
//       form  /somename;  that  is,  a null-terminated string of up to NAME_MAX
//       (i.e., 255) characters consisting of an initial slash, followed by  one
//       or more characters, none of which are slashes.)
char *prefix_slash(const char *name) {
    char *new_name;
    if (name[0] != '/') {
        new_name = malloc(strlen(name) + 2); // '/' at the beginning , strlen(str) in the middle and '\0' at the end,
        new_name[0] = '/';
        new_name[1] = '\0';
        strncat(new_name, name, strlen(name) + 1);
    } else {
        new_name = malloc(strlen(name) + 1);
        strncpy(new_name, name, strlen(name) + 1);
        new_name[strlen(name)] = '\0';
    }

    return new_name;
}
