#include <errno.h>
#include <stdio.h>

#include "include/fun_print_n_times.h"
#include "include/lpc_types.h"
#include "string.h"

int print_n_times(void *params) {
    int n;
    char *p = (char *)params;

    lpc_string *lpc_str = (lpc_string *) params;
    memcpy(&n, p + (sizeof(lpc_string) + lpc_str->slen), sizeof(int));

    int strlen_string = strlen(lpc_str->string) + 1;

    if (n * strlen_string > lpc_str->slen) {
        errno = ENOMEM;
        lpc_str->slen = -1;
        return -1;
    }
    char s[n * strlen_string];
    char tmp_str[strlen_string + 1]; // +1 '\n'

    snprintf(s, strlen_string + 1, "%s\n", lpc_str->string);
    strncpy(tmp_str, s, strlen(s));

    for (int i = 1; i < n; i++) {
        strcat(s, tmp_str);
    }
    memset(lpc_str->string, 0, lpc_str->slen);
    memcpy(lpc_str->string, s, n * (strlen_string + 1));

    return 0;
}
