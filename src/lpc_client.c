#include "include/lpc_client.h"

#include <stdlib.h>

void *lpc_open(const char *name) { return NULL; }

int lpc_close(void *mem) { return 0; }

int lpc_call(void *memory, const char *fun_name, ...) { return 0; }

lpc_string *lpc_make_string(const char *s, int taille) { return NULL; }
