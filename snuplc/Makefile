#---------------------------------------------------------------------------------------------------
# SnuPL/2 Makefile
#
# Author: Bernhard Egger <bernhard@csap.snu.ac.kr>
#


#
# variables
#

# sign-parsing mode.
# Possible options:
#   INT_CONST_RELAXED
#   INT_CONST_SIMPLE
#   INT_CONST_STRICT
INT_MODE=-DINT_CONST_RELAXED
#INT_MODE=-DINT_CONST_SIMPLE
#INT_MODE=-DINT_CONST_STRICT

# directories
SRC_DIR=src
OBJ_DIR=obj
DEP_DIR=.deps

# compilation w/ automatic dependency generation
CC=g++
CCFLAGS=-std=c++11 -Wall -g -O0 $(INT_MODE)
DEPFLAGS=-MMD -MP -MT $@ -MF $(DEP_DIR)/$*.d

# sources for various targets
BACKEND=backend.cpp \
	backendIA32.cpp \
	backendAMD64.cpp
BASE=environment.cpp \
	target.cpp \
	$(BACKEND)
SCANNER=scanner.cpp
PARSER=parser.cpp \
	type.cpp \
	symtab.cpp \
	data.cpp \
	ast.cpp \
	ir.cpp
IR=cfg.cpp
SOURCES=$(BASE) $(SCANNER) $(PARSER) $(IR)

# object files of various targets
DEPS=$(SOURCES:.cpp=$(DEP_DIR)/.d)
OBJ_SCANNER=$(patsubst %.cpp,$(OBJ_DIR)/%.o, $(SCANNER))
OBJ_PARSER=$(patsubst %.cpp,$(OBJ_DIR)/%.o, $(BASE) $(SCANNER) $(PARSER))
OBJ_IR=$(patsubst %.cpp,$(OBJ_DIR)/%.o, $(BASE) $(SCANNER) $(PARSER) $(IR))
OBJ_SNUPLC=$(patsubst %.cpp,$(OBJ_DIR)/%.o, $(BASE) $(SCANNER) $(PARSER) $(IR))

# Doxygen configuration file
DOXYFILE=doc/Doxyfile

#
# compilations rules
#
.PHONY: doc clean mrproper

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(DEP_DIR)
	$(CC) $(CCFLAGS) $(DEPFLAGS) -c -o $@ $<

$(DEP_DIR):
	@mkdir -p $(DEP_DIR)

-include $(DEPS)

all: snuplc

test_scanner: $(OBJ_DIR)/test_scanner.o $(OBJ_SCANNER)
	$(CC) $(CCFLAGS) -o $@ $(OBJ_DIR)/test_scanner.o $(OBJ_SCANNER)

test_parser: $(OBJ_DIR)/test_parser.o $(OBJ_PARSER)
	$(CC) $(CCFLAGS) -o $@ $(OBJ_DIR)/test_parser.o $(OBJ_PARSER)

test_semanal: $(OBJ_DIR)/test_semanal.o $(OBJ_PARSER)
	$(CC) $(CCFLAGS) -o $@ $(OBJ_DIR)/test_semanal.o $(OBJ_PARSER)

test_ir: $(OBJ_DIR)/test_ir.o $(OBJ_IR)
	$(CC) $(CCFLAGS) -o $@ $(OBJ_DIR)/test_ir.o $(OBJ_IR)

snuplc: $(OBJ_DIR)/snuplc.o $(OBJ_SNUPLC)
	$(CC) $(CCFLAGS) -o $@ $(OBJ_DIR)/snuplc.o $(OBJ_SNUPLC)

doc:
	doxygen $(DOXYFILE)

clean:
	rm -rf $(OBJ_DIR)/*.o $(DEP_DIR)

mrproper: clean
	rm -rf doc/html/* test_scanner test_parser test_semanal test_ir snuplc

