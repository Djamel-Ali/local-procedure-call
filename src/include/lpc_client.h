#ifndef __LPC_CLIENT_H__
#define __LPC_CLIENT_H__

#include "lpc_types.h"  //  struct lpc_string, enum lpc_type

void *lpc_open(const char *name);

int lpc_close(void *mem);

int lpc_call(void *memory, const char *fun_name, ...);

lpc_string *lpc_make_string(const char *s, int taille);

#endif  // __LPC_CLIENT_H__