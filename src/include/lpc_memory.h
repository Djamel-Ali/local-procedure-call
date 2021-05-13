#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <pthread.h>

#include "lpc_types.h"

#define PARAMS_SIZE 1024
#define NAMELEN 48

typedef struct {
    /*les mutex, les conditions pour synchroniser l’accès mémoire de clients et de serveur*/
    pthread_mutex_t mutex;
    pthread_cond_t call_cond; // Condition à vérifier pour pouvoir faire un appel de fonction
    pthread_cond_t res_cond; // Condition à vérifier pour pouvoir récupérer un/des résultat(s) (d'un appel de fonction)
    pthread_cond_t new_cond; /*Condition à vérifier par le serveur pour qu'il puisse accéder à la mémoire partagée et voir
                            * ce que le client avait écrit (les nouvelles var que le client avait copié vers cette mem*/
    pid_t pid;
    int new;  /* new == 1 s'il y a un client qui a écrit dans le shared memory.*/
    int call; /* call == 1 s'il y a une fonction à appeler*/
    int res;  /* res == 1 s'il y un resultat à un appel de fonction*/
    int rc;   /*Valeur retournée par la fonction appelée: 0 ou -1.*/
    int er;   /*Valeur de errno.*/
    int end;

} header;

typedef struct {
    char fun_name[NAMELEN]; /*Le nom se termine par le caratère null.*/
    char params[PARAMS_SIZE]; /*liste des paramètres de la fonction (lpc_call)*/
} data;

typedef struct {
    header header;
    data data;
} memory;

#endif  // __MEMORY_H__