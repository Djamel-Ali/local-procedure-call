#include <sys/mman.h> // shm_open ...
#include <fcntl.h> // O_RDWR ...
#include <string.h> // strerror()
#include <errno.h> // errno
#include <unistd.h>

#include "include/lpc_client.h"
#include "include/lpc_utils.h" // prefix_slash, ERREXIT ...
#include "include/lpc_memory.h" // memory

void *lpc_open(const char *name) {
    char *shm_name = prefix_slash(name);

    int fd;
    if ((fd = shm_open(shm_name, O_RDWR, 0)) < 0) ERREXIT("shm_open %s\n", strerror(errno));

    memory *pMem = mmap(0, sizeof(memory), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if ((void *) pMem == MAP_FAILED) return NULL;

    return pMem;
}

int lpc_close(void *mem) {
    if (mem != NULL) {
        free(((memory *) mem)->data.fun_name);
        free(((memory *) mem)->data.params);
    }
    if (munmap(mem, sizeof(memory)) < 0) ERREXIT("munmap %s\n", strerror(errno));

    return 0;
}

int lpc_call(void *pMemory, const char *fun_name, ...) {
    // local declarations
    int code;

    // cast to 'memory *'
    memory * pMem = (memory *)pMemory;

    ///start of mutex to modify the shared pMemory.

    DEBUG("client[%d]: lock\n", getpid());
    code = pthread_mutex_lock(&pMem->header.mutex);
    if (code != 0) ERREXIT("pthread_mutex_lock %s\n", strerror(code));
    DEBUG("client[%d]: acquire lock\n", getpid());

    while (pMem->header.call){
        DEBUG("client[%d]: release lock and wait\n\n", getpid());
        code = pthread_cond_wait(&pMem->header.call_cond, &pMem->header.mutex);
        if (code != 0) ERREXIT("pthread_cond_wait %s\n", strerror(code));
    }
    DEBUG("client[%d]: acquire lock after wait\n", getpid());

    /* critic section */

    pMem->header.pid = getpid();
    //todo: copy args to shared mem 'pMem'

    // ici ça marche aussi sans msync (je ne sais pas encore si c'est important de le laisser !)
    if ((msync(pMem, sizeof (memory), MS_SYNC)) < 0) ERREXIT("msync %s\n", strerror(errno));

    pMem->header.call = 1; // call == 1 s'il y a une fonction à appeler
    pMem->header.new = 1; // new == 1 s'il y a un client qui a écrit dans le shared memory

    /* end critic section */

    if ((code = pthread_mutex_unlock(&pMem->header.mutex)) != 0) ERREXIT("pthread_mutex_unlock %s\n", strerror(code));
    if ((code = pthread_cond_signal(&pMem->header.new_cond))  != 0) ERREXIT("pthread_cond_signal %s\n", strerror(code));

    ///end of mutex (after the shared memory has been modified).

    return 0;
}

lpc_string *lpc_make_string(const char *s, int taille) {
    lpc_string *pLpcString;
    if (taille > 0 && s == NULL){
        pLpcString = (lpc_string *)malloc(sizeof(lpc_string) + taille + 1);
        memset(pLpcString->string, 0, taille + 1);
        pLpcString->slen = taille;
    }
    else if (taille <= 0 && s != NULL){
        pLpcString = (lpc_string *)malloc(sizeof(lpc_string) + strlen(s) + 1);
        strncpy(pLpcString->string, s, strlen(s));
        pLpcString->slen = (int)strlen(s) + 1;
    }
    else if (taille > strlen(s) + 1){
        pLpcString = (lpc_string *)malloc(sizeof(lpc_string) + taille + 1);
        strncpy(pLpcString->string, s, strlen(s));
        pLpcString->slen = taille;
    }
    else return NULL;

    return pLpcString;
}
