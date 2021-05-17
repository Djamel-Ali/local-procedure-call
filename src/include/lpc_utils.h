#ifndef __LPC_UTILS_H__
#define __LPC_UTILS_H__

#include <stdarg.h>
#include <stdio.h>   // fprintf
#include <stdlib.h>  // exit, EXIT_FAILURE ...

#define BUFSIZE 1024
#define NAME_MAX 255

#define DEBUG(fmt, ...)                    \
  do {                                     \
    if (LPCDEBUG) {                        \
      fprintf(stderr, (fmt), __VA_ARGS__); \
    }                                      \
  } while (0)

#define ERREXIT(fmt, ...)                          \
  do {                                             \
    fprintf(stderr, "%s:%d:", __FILE__, __LINE__); \
    fprintf(stderr, (fmt), __VA_ARGS__);           \
    exit(EXIT_FAILURE);                            \
  } while (0)

/* Assure que [name] commence par un '/' et qu'il ne contient q'un seul '/'.
 * Si ce n'est pas le cas un '/' est ajouté au début de [name], ou les autres
 * '/' sont supprimés. [name] doit terminer par le caractère nul.*/
char *start_with_slash(const char *name);
#endif  // __LPC_UTILS_H__