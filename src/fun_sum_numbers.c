//
// Created by djamel.
//

#include "fun_sum_numbers.h"

double sum_double(int nb, ...){
    double res = 0;
    int i;
    va_list p_list_of_args;
    va_start(p_list_of_args, nb);
    for (i = 0; i < nb; i++)
        res += va_arg(p_list_of_args, double);
    va_end(p_list_of_args);
    return(res);
}