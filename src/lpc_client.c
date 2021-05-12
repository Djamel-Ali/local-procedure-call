#include <sys/mman.h> // shm_open ...
#include <fcntl.h> // O_RDWR ...
#include <string.h> // strerror()
#include <errno.h> // errno

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

int lpc_call(void *memory, const char *fun_name, ...) {

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
