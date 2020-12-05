#---------------------------------------------------------------------------------------------------
# SnuPL/2 Runtime (AMD64) Makefile
#
# Author: Bernhard Egger <bernhard@csap.snu.ac.kr>
#
CC=gcc
CCFLAGS=-m64 -march=x86-64 -std=c99 -O1 -fno-pic
AR=ar
ARFLAGS=rcs

RTE_OBJ=ARRAY.o IO.o
STD_LIB_NAME=snupl
STD_LIB=lib$(STD_LIB_NAME).a

IO_TESTS=-DTEST_WRITEINT -DTEST_WRITESTR -DTEST_WRITECHAR -DTEST_WRITELN -DTEST_READINT

.PHONY: clean test

%.o: %.s
	$(CC) $(CCFLAGS) -c -o $@ $^

%.o: %.c
	$(CC) $(CCFLAGS) -c -o $@ $^

$(STD_LIB): $(RTE_OBJ)
	$(AR) $(ARFLAGS) $@ $^

rte: $(STD_LIB)

test_io: test_io.c $(STD_LIB)
	$(CC) $(CCFLAGS) $(IO_TESTS) -L. -o $@ $< -l$(STD_LIB_NAME)

test_array: test_array.c $(STD_LIB)
	$(CC) $(CCFLAGS) -L. -o $@ $< -l$(STD_LIB_NAME)

clean:
	rm -rf *.o $(STD_LIB) test_io test_array
