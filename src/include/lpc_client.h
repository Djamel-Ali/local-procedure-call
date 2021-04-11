#ifndef __LPC_CLIENT_H__
#define __LPC_CLIENT_H__

typedef enum { STRING, DOUBLE, INT, NOP } lpc_type;

typedef struct {
  int slen;
  char string[];
} lpc_string;

void *lpc_open(const char *name);

int lpc_close(void *mem);

int lpc_call(void *memory, const char *fun_name, ...);

lpc_string *lpc_make_string(const char *s, int taille);

#endif  // __LPC_CLIENT_H__