########################################################################
############################## Variables ###############################
########################################################################

# Compiler settings 
CC = gcc
CFLAGS = -std=c11 -Wall -g
LDLIBS = -lrt -pthread


# Makefile settings 
APP = client server
EXT = .c
SRCDIR = src

SRC = $(wildcard $(SRCDIR)/*$(EXT))
OBJ = $(SRC:$(SRCDIR)/%$(EXT)=%.o)
COMOBJ = $(filter-out server.o client.o,  $(OBJ))
DEP = $(OBJ:%.o=%.d)

########################################################################
############################## Targets #################################
########################################################################

all: $(APP)

################################ Compiling #############################

# Builds apps
$(APP): $(COMOBJ)
	@echo "* Linking $@ ..."
	@$(CC) $^ $(LDLIBS) $(CFLAGS) -o $@
	@echo "done."

# Creates the dependecy rules
client: client.o 
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
.PHONY: clean cleanall
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
