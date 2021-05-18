########################################################################
############################## Variables ###############################
########################################################################

# Compiler settings 
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic -g -DLPCDEBUG=1 `pkg-config --cflags check gmodule-2.0`
LDLIBS = -lrt -pthread `pkg-config --libs check gmodule-2.0`


SRC=$(wildcard *.c)
SRC_OBJ=$(filter-out ../src/server.o ../src/client.o, $(wildcard ../src/*.o))
OBJ=$(SRC:.c=.o)
TESTS=$(SRC:%.c=%)


# Makefile settings 
EXT = .c
SRC = $(wildcard *$(EXT))
SRC_OBJ=$(filter-out server.o client.o, $(wildcard ../src/*.o))
OBJ=$(SRC:%.c=%.o)
TESTS=$(SRC:%.c=%)
DEP = $(OBJ:%.o=%.d)

########################################################################
############################## Targets #################################
########################################################################
.PHONY: all
all: 
	@echo $(OBJ)
	@echo $(SRC_OBJ)

################################ Compiling #############################

# Builds apps
$(APP): $(OBJ)
	@echo "* Linking $@ ..."
	@$(CC) $^ $(SRC_OBJ) $(LDLIBS) $(CFLAGS) -o $@
	@echo "done."

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
	@echo "* cleaning obj ..."
	@rm -rf $(OBJ) 
	@echo "done."

cleandep:
	@echo "* cleaning dep ..."
	@rm -rf $(DEP)
	@echo "done."

cleanall: clean cleandep
	@echo "* cleaning bin ..."
	@rm -rf $(APP)
	@echo "done."
