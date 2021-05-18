#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "include/lpc_client.h"
#include "include/lpc_memory.h"
#include "include/lpc_utils.h"

// exemple de test avec succès
static void test_fun_hello_succes(memory *mem) {
  char *fun_name = "hello";
  char *cl = "client";
  size_t len = strlen(cl) + sizeof(pid_t) + 2;
  char s[len];
  snprintf(s, len, "%s%d", cl, getpid());

  lpc_string *string = lpc_make_string(s, len * 10);

  int rc = lpc_call(mem, fun_name, STRING, string, NOP);

  if (rc == -1) ERREXIT("%s %s\n", "lpc_call", strerror(errno));

  printf("%s\n", string->string);
}

// exemple de test sans succès
static void test_fun_hello_failure(memory *mem) {
  char *fun_name = "hello";
  char *cl = "client";
  size_t len = strlen(cl) + sizeof(pid_t) + 2;
  char s[len];
  snprintf(s, len, "%s%d", cl, getpid());

  lpc_string *string = lpc_make_string(s, len);

  int rc = lpc_call(mem, fun_name, STRING, string, NOP);

  if (rc == -1) ERREXIT("%s %s\n", "lpc_call", strerror(errno));

  printf("%s\n", string->string);
}

// exemple de test avec succès
static void test_fun_print_n_times_succes(memory *mem) {
  char *fun_name = "print_n_times";
  char *cl = "client";
  int n_times = 5;
  size_t len = strlen(cl) + sizeof(pid_t) + 2;
  char s[len];
  snprintf(s, len, "%s%d", cl, getpid());

  lpc_string *string = lpc_make_string(s, len * 10);

  int rc = lpc_call(mem, fun_name, INT, &n_times, STRING, string, NOP);

  if (rc == -1) ERREXIT("%s %s\n", "lpc_call", strerror(errno));

  printf("%s\n", string->string);
}

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

  printf("%s\n", string->string);
}

// exemple de test avec succès
static void test_fun_divide_double_succes(memory *mem) {
  char *fun_name = "divide_double";
  double num = 4.5, denom = 3;

  int rc = lpc_call(mem, fun_name, DOUBLE, &num, DOUBLE, &denom, NOP);

  if (rc == -1) ERREXIT("%s %s\n", "lpc_call", strerror(errno));

  printf("%lf\n", num);
}

// exemple de test sans succès
static void test_fun_divide_double_failure(memory *mem) {
  char *fun_name = "divide_double";
  double num = 4.5, denom = 0;

  int rc = lpc_call(mem, fun_name, DOUBLE, &num, DOUBLE, &denom, NOP);

  if (rc == -1) ERREXIT("%s %s\n", "lpc_call", strerror(errno));

  printf("%lf\n", num);
}

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("usage: %s shmo_name\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  char *shmo_name = start_with_slash(argv[1]);
  memory *mem = lpc_connect(shmo_name);

 
  test_fun_hello_succes(mem);

  //test_fun_hello_failure(mem);
 
  //test_fun_print_n_times_succes(mem);
 
  //test_fun_print_n_times_failure(mem);

  //test_fun_divide_double_succes(mem);  

  //test_fun_divide_double_failure(mem);  

  return 0;
}