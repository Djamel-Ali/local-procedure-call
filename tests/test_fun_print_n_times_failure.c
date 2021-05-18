#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../src/include/lpc_client.h"
#include "../src/include/lpc_utils.h"

// exemple de test sans succès
static void test_fun_print_n_times_failure(memory *mem) {
  char *fun_name = "print_n_times";
  char *cl = "client";
  int n_times = 10;
  size_t len = strlen(cl) + sizeof(pid_t) + 2;
  char s[len];
  snprintf(s, len, "%s%d", cl, getpid());

  lpc_string *string = lpc_make_string(s, len * 5);
  if (string != NULL) {
    int rc = lpc_call(mem, fun_name, INT, &n_times, STRING, string, NOP);
    lpc_deconnect(mem);
    free(string);
    if (rc == -1) ERREXIT("%s %s\n", "lpc_call", strerror(errno));
    printf("KO\n");
  } else {
    printf("%s:%d: KO", __FILE__, __LINE__);
  }
}

int main(int argc, char **argv) {
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