#ifndef PROG_SYSTEM_AV_PROJECT_FUN_SUM_INT_H
#define PROG_SYSTEM_AV_PROJECT_FUN_SUM_INT_H

/**
 * @brief c'est une fonction à nombre variable d'args de type entier, et qui commence par 2 paramètres formels fix,
 * le 1er étant de type entier (qui contient le nombre d'entier à sommer)
 * le 2nd de type pointeur sur un entier (qui contiendra la somme de tous les entiers qui viennent après)
 *
 *  La somme va se retrouver dans
 * @return int (0 in success, else -1)
 */
int sum_int(int nb, int *res, ...);

#endif //PROG_SYSTEM_AV_PROJECT_FUN_SUM_INT_H
