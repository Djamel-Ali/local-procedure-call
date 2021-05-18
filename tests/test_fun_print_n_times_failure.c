#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "include/lpc_client.h"
#include "include/lpc_memory.h"
#include "include/lpc_utils.h"

// exemple de test sans succès
static void test_fun_print_n_times_failure(memory *mem) {
    char *fun_name = "print_n_times";
    char *cl = "client";
    int n_times = 10;
    size_t len = strlen(cl) + sizeof(pid_t) + 2;
    char s[len];
    snprintf(s, len, "%s%d", cl, getpid());

    lpc_string *string = lpc_make_string(s, len * 5);

    int rc = lpc_call(mem, fun_name, INT, &n_times, STRING, string, NOP);

    if (rc == -1) ERREXIT("%s %s\n", "lpc_call", strerror(errno));

    printf("KO\n");
}

int main(int argc, char **argv){
    if (argc != 2) {
        printf("usage: %s shmo_name\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    memory *mem = lpc_open(argv[1]);
    memory *mem_cl = lpc_connect(mem, argv[1]);

    // exemple de test sans succès
    test_fun_print_n_times_failure(mem_cl);

    return 0;
}