#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/include/lpc_client.h"
#include "../src/include/lpc_utils.h"

// le nom de la fontion défini par le client n'est pas connu par le serveur
static void test_fun_unknown(memory *mem) {
  char *fun_name = "fun_unknown";
  int rc = lpc_call(mem, fun_name, NOP);
  lpc_deconnect(mem);
  if (rc == -1) ERREXIT("%s %s\n", "lpc_call", strerror(errno));
  printf("KO\n");
}

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("usage: %s shmo_name\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  memory *mem = lpc_open(argv[1]);
  memory *mem_cl = lpc_connect(mem, argv[1]);

  // le nom de la fontion défini par le client n'est pas connu par le serveur
  test_fun_unknown(mem_cl);

  return 0;
}