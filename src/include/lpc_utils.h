#ifndef __LPC_UTILS_H__
#define __LPC_UTILS_H__

#include <stdarg.h>

#define BUFSIZE 1024

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

char *start_with_slash(const char *name);

#endif  // __LPC_UTILS_H__