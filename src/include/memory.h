#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <pthread.h>

#define PARAMS_SIZE 1024
#define NAMELEN 48

typedef struct {
  pthread_mutex_t mutex;
  pthread_cond_t call_cond;
  pthread_cond_t res_cond;
  int call; /* call == 1 s'il y a une fonction à appeler*/
  int res;  /* res == 1 s'il y un resultat à un appel de fonction*/
  int rc;   /*Valeur retournée par la fonction appelée: 0 ou -1.*/
  int er;   /*Valeur de errno.*/

} header;

typedef struct {
  char fun_name[NAMELEN]; /*Le nom se termine par le caratère null.*/
  char params[PARAMS_SIZE];
} data;

typedef struct {
  header header;
  data data;
} memory;

#endif  // __MEMORY_H__