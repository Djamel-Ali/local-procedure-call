#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "../src/include/lpc_client.h"
#include "../src/include/lpc_utils.h"

// exemple de test sans succès
static void test_fun_divide_double_failure(memory *mem) {
    char *fun_name = "divide_double";
    double num = 4.5, denom = 0;

    int rc = lpc_call(mem, fun_name, DOUBLE, &num, DOUBLE, &denom, NOP);

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
    test_fun_divide_double_failure(mem_cl);

    return 0;
}