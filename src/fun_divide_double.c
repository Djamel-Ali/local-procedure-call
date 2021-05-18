#include "include/fun_divide_double.h"
#include <errno.h>
#include <string.h>

int divide_double(void *params) {
    double numerator, res;
    int denominator;

    memcpy(&numerator, params, sizeof(double));
    memcpy(&denominator, params + sizeof(double), sizeof(int));

    if (denominator == 0) {
        errno = EDOM;
        return -1;
    }

    res = numerator / denominator;
    memcpy(params, &res, sizeof(double ));

    return 0;
}
