#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <pthread.h>

#include "lpc_types.h"

#define PARAMS_SIZE 1024
#define NAMELEN 48

typedef struct {
  pthread_mutex_t mutex;
  /* condition pour faire un appel de fonction */
  pthread_cond_t call_cond;
  /* condition pour récupérer le/les résultat(s) d'un appel de fonction */
  pthread_cond_t res_cond;
  /* condition pour effectuer la première communication avec le serveur (pour
   * envoyer son pid )*/
  pthread_cond_t new_cond;

  pid_t pid; /* pid du nouveau client */
  int new;   /* new == 1 s'il y a un nouveau client qui a envoyer son pid */
  int call;  /* call == 1 s'il y a une fonction à appeler */
  int res;   /* res == 1 s'il y a un resultat à un appel de fonction */
  int rc;    /* valeur retournée par la fonction appelée: 0 ou -1. */
  int er;    /* valeur de errno. */
  int end;   /* end == 1 pour terminer la communication avec le serveur */

} header;

typedef struct {
  char fun_name[NAMELEN];   /*Le nom se termine par le caratère null.*/
  char params[PARAMS_SIZE]; /*liste des paramètres de la fonction (lpc_call)*/
} data;

typedef struct {
  header header;
  data data;
} memory;

#endif  // __MEMORY_H__