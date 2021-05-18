#ifndef __LPC_CLIENT_H__
#define __LPC_CLIENT_H__

#include "lpc_types.h" //  struct lpc_string, enum lpc_type
#include "lpc_memory.h"

void *lpc_open(const char *name);

int lpc_close(void *mem);

memory * lpc_connect(char *shmo_name);

void lpc_deconnect(memory *mem);

int lpc_call(void *memory, const char *fun_name, ...);

lpc_string *lpc_make_string(const char *s, int taille);

memory *lpc_connect(char *shmo_name);

#endif  // __LPC_CLIENT_H__