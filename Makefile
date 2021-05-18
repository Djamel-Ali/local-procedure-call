########################################################################
############################## Variables ###############################
########################################################################

# Compiler settings 
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic -g -DLPCDEBUG=0
LDLIBS = -lrt -pthread


# Makefile settings
APP = server tests/test_fun_hello_success tests/test_fun_hello_failure \
			 tests/test_fun_print_n_times_success tests/test_fun_print_n_times_failure \
			 tests/test_fun_divide_double_success tests/test_fun_divide_double_failure \
			 tests/test_fun_unknown
EXT = .c
SRCDIR = src

SRC = $(wildcard $(SRCDIR)/*$(EXT))
OBJ = $(SRC:$(SRCDIR)/%$(EXT)=%.o)
COMOBJ = $(filter-out server.o tests/test_fun_hello_success.o tests/test_fun_hello_failure \
							   tests/test_fun_print_n_times_success.o tests/test_fun_print_n_times_failure.o \
							   tests/test_fun_divide_double_success.o tests/test_fun_divide_double_failure.o \
							   tests/test_fun_unknown.o ,  $(OBJ))
DEP = $(OBJ:%.o=%.d)

########################################################################
############################## Targets #################################
########################################################################
.PHONY: all
all: $(APP)

################################ Compiling #############################

# Builds apps
$(APP): $(COMOBJ)
	@echo "* Linking $@ ..."
	@$(CC) $^ $(LDLIBS) $(CFLAGS) -o $@
	@echo "√ done."

# Creates the dependecy rules
tests/test_fun_hello_success: tests/test_fun_hello_success.o
tests/test_fun_hello_failure: tests/test_fun_hello_failure.o
tests/test_fun_print_n_times_success: tests/test_fun_print_n_times_success.o
tests/test_fun_print_n_times_failure: tests/test_fun_print_n_times_failure.o
tests/test_fun_divide_double_success: tests/test_fun_divide_double_success.o
tests/test_fun_divide_double_failure: tests/test_fun_divide_double_failure.o
tests/test_fun_unknown: tests/test_fun_unknown.o
server: server.o

%.d: $(SRCDIR)/%$(EXT)
	@$(CPP) $(CFLAGS) $< -MM -MT $(@:%.d=%.o) >$@

# Includes all .h files
-include $(DEP)

# Building rule for .o files and its .c in combination with all .h
%.o: $(SRCDIR)/%$(EXT)
	@echo "* Building $(@:%.o=%) ..."
	@$(CC) $(CFLAGS) -o $@ -c $<

################################ Cleaning #############################

# Cleans complete project
.PHONY: clean cleandep cleanall
clean:
	@echo "~ cleaning obj ..."
	@rm -rf $(OBJ)
	@rm -rf tests/*.o
	@echo "√ done."

cleandep:
	@echo "~ cleaning dep ..."
	@rm -rf $(DEP)
	@rm -rf rests/*.d
	@echo "√ done."

cleanall: clean cleandep
	@echo "~ cleaning bin ..."
	@rm -rf $(APP)
	@echo "√ done."
