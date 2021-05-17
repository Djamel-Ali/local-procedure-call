#define _XOPEN_SOURCE 500

#include "include/lpc_server.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "include/fun_hello.h"
#include "include/lpc_sync.h"
#include "include/lpc_utils.h"

static lpc_function FUNCTIONS[] = {{"hello", hello}};

static memory *create_shom(const char *shom_name, size_t size) {
    char *name = prefix_slash(shom_name);

    int fd = shm_open(name, O_CREAT | O_RDWR, S_IWUSR | S_IRUSR);
    if (fd < 0) ERREXIT("%s %s\n", "shm_open", strerror(errno));

    if (ftruncate(fd, size) < 0) ERREXIT("%s %s\n", "ftruncate", strerror(errno));

    memory *mem =
            mmap(NULL, sizeof(memory), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if ((void *) mem == MAP_FAILED) ERREXIT("%s %s\n", "mmap", strerror(errno));

    return mem;
}

static void init_header(memory *mem, char *shm_name) {
    mem->header.new = 0;  /* tout client peut écrire dans le shared memory*/
    mem->header.call = 0; /* pas encore de fonction à appeler*/
    mem->header.res = 0;  /* pas de resultat d'appel de fonction*/
    mem->header.er = 0;  /*errno == 0*/
    mem->header.rc = 0;
    mem->header.end = 0;
    mem->header.shm_name = shm_name;

    int rc;
    rc = init_mutex(&mem->header.mutex);
    if (rc != 0) ERREXIT("%s %s\n", "init_mutex", strerror(errno));

    rc = init_cond(&mem->header.call_cond);
    if (rc != 0) ERREXIT("%s %s\n", "init_cond", strerror(errno));

    rc = init_cond(&mem->header.res_cond);
    if (rc != 0) ERREXIT("%s %s\n", "init_cond", strerror(errno));
}

memory *lpc_create(const char *shmo_name, size_t capacity) {
    if (capacity < 1) return NULL;
    memory *mem = create_shom(shmo_name, sysconf(_SC_PAGESIZE) * capacity);
    init_header(mem, NULL);
    return mem;
}

int (*lpc_get_fun(const char *fun_name))(void *) {
    int len = sizeof(FUNCTIONS) / sizeof(lpc_function);
    for (size_t i = 0; i < len; i++) {
        if (!strcmp(fun_name, FUNCTIONS[i].fun_name)) {
            return FUNCTIONS[i].fun;
        }
    }
    return (void *) 0;
}

void lpc_call_fun(memory *mem) {
    int (*f)(void *) = lpc_get_fun(mem->data.fun_name);
    int rc = f(mem->data.params);
    if (rc == -1) mem->header.er = errno;
    mem->header.rc = rc;
}

void lpc_free(const char *shmo_name) {
    if (shmo_name != NULL && shm_unlink(shmo_name) == -1)
        ERREXIT("%s %s\n", "shm_unlink", strerror(errno));
}