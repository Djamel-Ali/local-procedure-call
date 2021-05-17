#include <sys/mman.h> // shm_open ...
#include <fcntl.h> // O_RDWR ...
#include <string.h> // strerror()
#include <errno.h> // errno
#include <unistd.h>

#include "include/lpc_client.h"
#include "include/lpc_utils.h" // prefix_slash, ERREXIT ...
#include "include/lpc_memory.h" // memory
#include "include/lpc_types.h"

void *lpc_open(const char *name) {
    char *shm_name = prefix_slash(name);

    int fd;
    if ((fd = shm_open(shm_name, O_RDWR, 0)) < 0) ERREXIT("shm_open : %s\n", strerror(errno));

    memory *p_mem = mmap(0, sizeof(memory), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (p_mem == MAP_FAILED) return NULL;

    return p_mem;
}

int lpc_close(void *mem) {
    if (munmap(mem, sizeof(memory)) < 0) ERREXIT("munmap : %s\n", strerror(errno));

    return 0;
}

int lpc_call(void *p_memory, const char *fun_name, ...) {
    // local declarations

    int code;

    // pointer to the list of parameters of the call
    va_list p_list_args;

    // initialisation of this pointer
    va_start(p_list_args, fun_name);

    // cast to 'memory *'
    memory *p_mem = (memory *) p_memory;

    /* etablir la première connexion */
///start of mutex to modify the shared memory (to modify the memory shared with the distributed server.)

    DEBUG("client[%d]: lock (to modify the memory shared with the distributed server.)\n", getpid());
    code = pthread_mutex_lock(&p_mem->header.mutex);
    if (code != 0) ERREXIT("pthread_mutex_lock : %s\n", strerror(code));
    DEBUG("client[%d]: acquire lock (for principal shared memory)\n", getpid());

    // Tant qu'il y a un client en cours de traitment par le serveur principal
    // (qui va faire fork et laisse le serveur fils s'en occuper), on attend ...
    while (p_mem->header.new) {
        DEBUG("client[%d]: release lock and wait (for principal shared memory)\n\n", getpid());
        code = pthread_cond_wait(&p_mem->header.new_cond, &p_mem->header.mutex);
        if (code != 0) ERREXIT("pthread_cond_wait : %s\n", strerror(code));
    }
    DEBUG("client[%d]: acquire lock after wait (for principal shared memory)\n", getpid());

/* critic section */

    //todo:à supprimer !?? pourquoi ? il a besoin de son id pour lui créer
    // une zone mémoire avec comme nom l'ancien nom suivi de son pid ...

    // ici le client annonce au serveur son identité pour qu'il lui crée un process fils qui s'en occupera de lui
    p_mem->header.pid = getpid();

    p_mem->header.new = 1; /* prévenir tout le monde (server distribué et les autres clients qui veulent aussi
                            * entrer en communication avec ce server) qu'il y a actuellement une communication
                            * courante entre le server principal et un client */

    //todo: à confirmer si 'new' seul suffit ( pas besoin de call = 1 ici je crois)
    p_mem->header.call = 1; //  prévenir le serveur qu'il y a un client qui veut faire un appel à une fonction

    if ((msync(p_mem, sizeof(memory), MS_SYNC)) < 0) ERREXIT("msync : %s\n", strerror(errno));

/* end critic section */

    if ((code = pthread_mutex_unlock(&p_mem->header.mutex)) != 0)
        ERREXIT("pthread_mutex_unlock : %s\n", strerror(code));
    if ((code = pthread_cond_signal(&p_mem->header.new_cond)) != 0)
        ERREXIT("pthread_cond_signal : %s\n", strerror(code));

///end of mutex (to modify the memory shared with the distributed server; after this shared memory has been modified by client).

/* Se préparer pour ouvrir la projection mémoire qui sera créée par le process enfant */
    char shm_name[BUFSIZE] = {0};
    snprintf(shm_name, BUFSIZE, "%s%d", p_mem->header.shm_name, getpid());

    // terminer la projection mémoire avec la mémoire principale partagée avec le server distribué
    munmap(p_mem, sizeof(memory));

    /* attendre que le nouveau shared memory soit créé */
    memory *p_new_memory;
    while (1) {
        p_new_memory = (memory *) lpc_open(shm_name);
        if (p_new_memory == NULL) ERREXIT("lpc_open : %s\n", strerror(errno));
        break;
    }
    // le client a maintenant un pointeur valide vers la mémoire partagée avec le server enfant

///start of mutex to modify the shared memory (to modify the memory shared with the child server)


    DEBUG("client[%d]: lock (to modify the memory shared with the child server)\n", getpid());
    code = pthread_mutex_lock(&p_new_memory->header.mutex);
    if (code != 0) ERREXIT("pthread_mutex_lock : %s\n", strerror(code));
    DEBUG("client[%d]: acquire lock (for the memory shared with the child server)\n", getpid());

    // Tant que le server enfant n'a pas encore fait le précédent appel de fonction, on attend ...
    while (p_new_memory->header.call) {
        DEBUG("client[%d]: release lock and wait (for the memory shared with the child server)\n\n", getpid());
        code = pthread_cond_wait(&p_new_memory->header.call_cond, &p_new_memory->header.mutex);
        if (code != 0) ERREXIT("pthread_cond_wait : %s\n", strerror(code));
    }
    DEBUG("client[%d]: acquire lock after wait (for the memory shared with the child server)\n", getpid());

/* critic section */

    p_new_memory->header.call = 1; //  prévenir le serveur enfant que le client veut faire un appel à une fonction

    //Preparation to copy args to shared mem 'p_new_memory'
    memset(&p_new_memory->data, 0, sizeof(p_new_memory->data)); // d'abord mettre tout à 0

    // copier le nom de la fonction
    memcpy(p_new_memory->data.fun_name, fun_name, strlen(fun_name) + 1);

    // copier les paramètres de la fonction (les valeurs d'entrée) dans la mémoire partagée
    int insert_index = 0;
    lpc_type *p_current_lpc_type = NULL;
    do {
        *p_current_lpc_type = va_arg(p_list_args, lpc_type);

        if (*p_current_lpc_type == INT) {
            const int *temp_int = va_arg(p_list_args, int *);
            memmove(p_new_memory->data.params + sizeof(int) + insert_index, temp_int, sizeof(int));
            insert_index += sizeof(int);

        } else if (*p_current_lpc_type == DOUBLE) {
            const double *temp_double = va_arg(p_list_args, double *);
            memmove(p_new_memory->data.params + sizeof(int) + insert_index, temp_double, sizeof(double));
            insert_index += sizeof(double);

        } else if (*p_current_lpc_type == STRING) {
            const lpc_string *temp_lpc_string = va_arg(p_list_args, lpc_string *);
            memmove(p_new_memory->data.params + sizeof(int) + insert_index, temp_lpc_string->string,
                    temp_lpc_string->slen);
            insert_index += temp_lpc_string->slen;
        }
    } while (*p_current_lpc_type != NOP);

    /* On écrit au début de la zone mémoire pointée par 'params' la taille totale
     * (qui est la somme des tailles de tous les arguments de la fonction) */
    memmove(p_new_memory->data.params, &insert_index, sizeof(int));

    if ((msync(p_new_memory, sizeof(memory), MS_SYNC)) < 0) ERREXIT("msync : %s\n", strerror(errno));

/* end critic section */

    /* On libère le mutex (pour laisser le server enfant nous calculer le résultat) */
    if ((code = pthread_mutex_unlock(&p_new_memory->header.mutex)) != 0)
        ERREXIT("pthread_mutex_unlock : %s\n", strerror(code));

    /* On réveille le server enfant */
    if ((code = pthread_cond_signal(&p_new_memory->header.call_cond)) != 0)
        ERREXIT("pthread_cond_signal : %s\n", strerror(code));

///start of mutex to get the results from the shared memory (the memory shared with the child server)

    /* Ce même client doit se suspendre par un appel à pthread_cond_wait POUR RÉCUPÉRER LES RESULTS; */
    DEBUG("client[%d]: lock (to get the results from the memory shared with the child server)\n", getpid());
    code = pthread_mutex_lock(&p_new_memory->header.mutex);
    if (code != 0) ERREXIT("pthread_mutex_lock : %s\n", strerror(code));
    DEBUG("client[%d]: acquire lock (to get the results from the memory shared with the child server)\n", getpid());

    // Tant que le server enfant n'a pas encore fait le précédent appel de fonction, on attend ...
    while (!p_new_memory->header.res) {
        DEBUG("client[%d]: release lock and wait (to get the results from the memory shared with the child server)\n\n", getpid());
        code = pthread_cond_wait(&p_new_memory->header.res_cond, &p_new_memory->header.mutex);
        if (code != 0) ERREXIT("pthread_cond_wait : %s\n", strerror(code));
    }
    DEBUG("client[%d]: acquire lock after wait (to get the results from the memory shared with the child server)\n", getpid());

    // Copier les résultat vers la mémoire du client

///end of mutex to get the results from the shared memory (the memory shared with the child server)


///end of mutex (to modify the memory shared with the child server; after this shared memory has been modified by client).

    return 0;
}

lpc_string *lpc_make_string(const char *s, int taille) {
    lpc_string *pLpcString;

    if (taille > 0 && s == NULL) {
        pLpcString = (lpc_string *) malloc(sizeof(lpc_string) + taille + 1);
        memset(pLpcString->string, 0, taille + 1);
        pLpcString->slen = taille;

    } else if (taille <= 0 && s != NULL) {
        pLpcString = (lpc_string *) malloc(sizeof(lpc_string) + strlen(s) + 1);
        strncpy(pLpcString->string, s, strlen(s));
        pLpcString->slen = (int) strlen(s) + 1;

    } else if (taille > strlen(s) + 1) {
        pLpcString = (lpc_string *) malloc(sizeof(lpc_string) + taille + 1);
        strncpy(pLpcString->string, s, strlen(s));
        pLpcString->slen = taille;

    } else return NULL;

    return pLpcString;
}
