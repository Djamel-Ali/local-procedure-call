#ifndef PROG_SYSTEM_AV_PROJECT_FUN_SUM_NUMBERS_H
#define PROG_SYSTEM_AV_PROJECT_FUN_SUM_NUMBERS_H

/**
 * @brief c'est une fonction à nombre variable d'args de type double ou entier,
 * et qui commence par 2 paramètres formels fix,
 * le 1er étant de type entier (qui contient le nombre d'entier à sommer)
 * le 2nd de type pointeur sur un double (qui contiendra la somme de tous les nombre qui viennent après)
 *
 *
 * @return int (0 in success, else -1)
 */
int sum_double(int nb, double *res, ...);

#endif //PROG_SYSTEM_AV_PROJECT_FUN_SUM_NUMBERS_H
