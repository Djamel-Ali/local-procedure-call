#include "include/fun_sum_int.h"
int sum_int(int nb, int *res, ...){
    *res = 0;
    int i;
    va_list p_list_of_args;

    if (nb < 0) return -1;

    va_start(p_list_of_args, nb);
    for (i = 0; i < nb; i++)
        *res += va_arg(p_list_of_args, int);
    va_end(p_list_of_args);

    return(0);
}