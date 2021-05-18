#include "include/fun_divide_double.h"

#include <errno.h>
#include <string.h>

int divide_double(void *params) {
  char *p = (char *)params;
  double numerator, denominator, res;

  memcpy(&numerator, p, sizeof(double));
  memcpy(&denominator, p + sizeof(double), sizeof(double));

  if (denominator == 0) {
    errno = EDOM;
    return -1;
  }

  res = numerator / denominator;
  memcpy(params, &res, sizeof(double));

  return 0;
}
