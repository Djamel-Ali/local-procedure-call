#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../src/include/lpc_client.h"
#include "../src/include/lpc_utils.h"

// exemple de test avec succès
static void test_fun_hello_success(memory *mem) {
  char *fun_name = "hello";
  char *cl = "client";
  size_t len = strlen(cl) + sizeof(pid_t) + 2;
  char s[len];
  snprintf(s, len, "%s%d", cl, getpid());

  lpc_string *string = lpc_make_string(s, len * 10);
  if (string == NULL) {
    printf("%s:%d: KO", __FILE__, __LINE__);
    return;
  }
  int rc = lpc_call(mem, fun_name, STRING, string, NOP);
  lpc_deconnect(mem);
  if (rc == -1) {
    free(string);
    ERREXIT("%s %s\n", "lpc_call", strerror(errno));
  }
  printf("%s\n", string->string);
  free(string);
}

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("usage: %s shmo_name\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  memory *mem = lpc_open(argv[1]);
  memory *mem_cl = lpc_connect(mem, argv[1]);

  // exemple de test avec succès
  test_fun_hello_success(mem_cl);

  return 0;
}