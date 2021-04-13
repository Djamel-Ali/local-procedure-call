#ifndef __LPC_TYPES_H__
#define __LPC_TYPES_H__

typedef enum { STRING, DOUBLE, INT, NOP } lpc_type;

typedef struct {
  int slen;
  char string[];
} lpc_string;

#endif  // __LPC_TYPES_H__