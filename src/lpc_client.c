#include "include/lpc_client.h"

#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "include/lpc_memory.h"
#include "include/lpc_types.h"
#include "include/lpc_utils.h"

void *lpc_open(const char *name) {
    char *shm_name = start_with_slash(name);

    int fd = shm_open(shm_name, O_RDWR, 0);
    if (fd < 0) ERREXIT("shm_open : %s\n", strerror(errno));

    memory *mem =
            mmap(0, sizeof(memory), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mem == MAP_FAILED) return NULL;

    return mem;
}

int lpc_close(void *mem) {
    int rc = munmap(mem, sizeof(memory));
    if (rc < 0) ERREXIT("munmap : %s\n", strerror(errno));
    return 0;
}

/* etablir la première connexion */
memory *lpc_connect(char *shmo_name) {
    int fd = shm_open(shmo_name, O_RDWR, 0);
    if (fd < 0) ERREXIT("%s %s\n", "shm_open", strerror(errno));

    memory *mem =
            mmap(0, sizeof(memory), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mem == MAP_FAILED) ERREXIT("%s %s\n", "mem", strerror(errno));

    DEBUG("client[%d]: lock\n", getpid());
    int rc = pthread_mutex_lock(&mem->header.mutex);
    if (rc != 0) ERREXIT("%s %s\n", "pthread_mutex_lock", strerror(rc));
    DEBUG("client[%d]: acquire lock\n", getpid());

    while (mem->header.new) {
        DEBUG("client[%d]: release lock and wait\n", getpid());
        rc = pthread_cond_wait(&mem->header.new_cond, &mem->header.mutex);
        if (rc != 0) ERREXIT("%s %s\n", "pthread_cond_wait", strerror(rc));
        DEBUG("client[%d]: acquire lock after wait\n", getpid());
    }

    mem->header.pid = getpid();
    mem->header.call = 1;

    rc = msync(mem, sizeof(memory), MS_SYNC);
    if (rc < 0) ERREXIT("%s %s\n", "msync", strerror(errno));

    rc = pthread_mutex_unlock(&mem->header.mutex);
    if (rc != 0) ERREXIT("%s %s\n", "pthread_mutex_unlock", strerror(rc));
    DEBUG("client[%d]: release lock\n", getpid());

    rc = pthread_cond_signal(&mem->header.call_cond);
    if (rc != 0) ERREXIT("%s %s\n", "pthread_cond_signal", strerror(rc));

    lpc_close(mem);
    close(fd);

    /* attendre que le nouveau shared memory soit crée */
    char name[BUFSIZE] = {0};
    snprintf(name, BUFSIZE, "%s%d", shmo_name, getpid());
    while (1) {
        fd = shm_open(name, O_RDWR, 0);
        if (fd != -1) break;
        if (errno != ENOENT) ERREXIT("%s %s\n", "shm_open", strerror(errno));
    }

    DEBUG("client[%d]: new shared memory created %s\n\n", getpid(), name);

    mem = mmap(0, sizeof(memory), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mem == MAP_FAILED) ERREXIT("%s %s\n", "mmap", strerror(errno));

    return mem;
}

static void copy_params_to_mem(memory *mem, va_list ap) {
    int insert_index = 0;
    lpc_type current_lpc_type;

    do {
        current_lpc_type = va_arg(ap, lpc_type);
        switch (current_lpc_type) {
            case INT:;
                const int *tmp_int = va_arg(ap, int *);
                memcpy(mem->data.params + insert_index, tmp_int, sizeof(int));
                insert_index += sizeof(int);
                break;

            case DOUBLE:;
                const double *tmp_double = va_arg(ap, double *);
                memcpy(mem->data.params + insert_index, tmp_double, sizeof(double));
                insert_index += sizeof(double);
                break;

            case STRING:;
                const lpc_string *tmp_string = va_arg(ap, lpc_string *);
                int len = sizeof(lpc_string) + tmp_string->slen;
                memcpy(mem->data.params + insert_index, tmp_string, len);
                insert_index += len;
                break;
            default:
                break;
        }
    } while (current_lpc_type != NOP);
}

static void copy_params_from_mem(memory *mem, va_list ap) {
    // TODO
    int insert_index = 0;
    int *tmp_int;
    double *tmp_double;
    lpc_string *tmp_string;
    lpc_type current_lpc_type;

    do {
        current_lpc_type = va_arg(ap, lpc_type);
        switch (current_lpc_type) {
            case INT:
                tmp_int = va_arg(ap, int *);
                memcpy(tmp_int, mem->data.params + insert_index, sizeof(int));
                insert_index += sizeof(int);
                break;

            case DOUBLE:
                tmp_double = va_arg(ap, double *);
                memcpy(tmp_double, mem->data.params + insert_index, sizeof(double));
                insert_index += sizeof(double);
                break;

            case STRING:
                tmp_string = va_arg(ap, lpc_string *);
                int len = sizeof(lpc_string) + tmp_string->slen;
                memcpy(tmp_string, mem->data.params + insert_index, len);
                insert_index += len;
                break;
            default:
                break;
        }
    } while (current_lpc_type != NOP);
}

static void notify_call(memory *mem) {
    mem->header.res = 0;
    mem->header.call = 1;

    int rc = msync(mem, sizeof(memory), MS_SYNC);
    if (rc != 0) ERREXIT("%s %s\n", "msync", strerror(errno));

    rc = pthread_mutex_unlock(&mem->header.mutex);
    if (rc != 0) ERREXIT("%s %s\n", "pthread_mutex_unlock", strerror(rc));

    rc = pthread_cond_signal(&mem->header.call_cond);
    if (rc != 0) ERREXIT("%s %s\n", "pthread_cond_signal", strerror(rc));
}

int lpc_call(void *mem, const char *fun_name, ...) {
    memory *lpc_mem = (memory *) mem;

    int rc = pthread_mutex_lock(&lpc_mem->header.mutex);
    if (rc != 0) ERREXIT("pthread_mutex_lock : %s\n", strerror(rc));

    memset(&lpc_mem->data, 0, sizeof(lpc_mem->data));
    memcpy(lpc_mem->data.fun_name, fun_name, strlen(fun_name));

    va_list ap, aq;
    va_start(ap, fun_name);
    va_copy(aq, ap);

    copy_params_to_mem(lpc_mem, ap);
    notify_call(lpc_mem);

    va_end(ap);

    while (!lpc_mem->header.res) { /*tant qu'il n'y a pas de resultat à lire*/
        DEBUG("client[%d]: release lock and wait\n", getpid());
        rc = pthread_cond_wait(&lpc_mem->header.res_cond, &lpc_mem->header.mutex);
        if (rc != 0) ERREXIT("%s %s\n", "pthread_cond_wait", strerror(rc));
        DEBUG("client[%d]: acquire lock after wait\n", getpid());
    }

    if (lpc_mem->header.rc == -1) {
        errno = lpc_mem->header.er;
    } else {
        copy_params_from_mem(lpc_mem, aq);
    }

    va_end(aq);

    rc = pthread_mutex_unlock(&lpc_mem->header.mutex);
    if (rc != 0) ERREXIT("%s %s\n", "pthread_mutex_unlock", strerror(rc));

    return lpc_mem->header.rc;
}

lpc_string *lpc_make_string(const char *s, int taille) {
    lpc_string *lpc_str = NULL;

    if (taille > 0 && s == NULL) {
        lpc_str = (lpc_string *) malloc(sizeof(lpc_string) + taille + 1);
        memset(lpc_str->string, 0, taille + 1);
        lpc_str->slen = taille;
    } else if (taille <= 0 && s != NULL) {
        int lo = strlen(s);
        lpc_str = (lpc_string *) malloc(sizeof(lpc_string) + lo + 1);
        strncpy(lpc_str->string, s, lo);
        lpc_str->slen = lo;
    } else if (s != NULL && taille > (int) strlen(s) + 1) {
        lpc_str = (lpc_string *) malloc(sizeof(lpc_string) + taille + 1);
        strncpy(lpc_str->string, s, strlen(s));
        lpc_str->slen = taille;
    }

    return lpc_str;
}
