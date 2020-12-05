//--------------------------------------------------------------------------------------------------
/// @brief test program to test the I/O library
/// @author Bernhard Egger <bernhard@csap.snu.ac.kr>
/// @section changelog Change Log
/// 2016/04/16 Bernhard Egger created
/// 2020/08/06 Bernhard Egger added test routines for all I/O functions
///
/// @section license_section License
/// Copyright (c) 2016, Bernhard Egger
/// All rights reserved.
///
/// Redistribution and use in source and binary forms, with or without modification, are permitted
/// provided that the following conditions are met:
///
/// - Redistributions of source code must retain the above copyright notice, this list of condi-
///   tions and the following disclaimer.
/// - Redistributions in binary form must reproduce the above copyright notice, this list of condi-
///   tions and the following disclaimer in the documentation and/or other materials provided with
///   the distribution.
///
/// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
/// IMPLIED WARRANTIES, INCLUDING,  BUT NOT LIMITED TO,  THE IMPLIED WARRANTIES OF MERCHANTABILITY
/// AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
/// CONTRIBUTORS BE LIABLE FOR ANY DIRECT,  INDIRECT,  INCIDENTAL,  SPECIAL,  EXEMPLARY, OR CONSE-
/// QUENTIAL DAMAGES (INCLUDING,  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
/// LOSS OF USE,  DATA, OR PROFITS;  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
/// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
/// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
/// DAMAGE.
//--------------------------------------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <time.h>

#include "IO.h"

//--------------------------------------------------------------------------------------------------
// output tests
//
int test_WriteInt(void)
{
#ifdef TEST_WRITEINT
  int values[] = { 0, 1, -1, INT_MAX, INT_MIN, };

  printf("  WriteInt(int i)\n");
  for (int i=0; i<sizeof(values)/sizeof(int); i++) {
    printf("    %s(%12d): ", "WriteInt", values[i]); fflush(stdout);
    WriteInt(values[i]);
    printf("\n");
  }

  srand(time(0));
  for (int i=0; i<20; i++) {
    int v = rand() - RAND_MAX/2;

    printf("    %s(%12d): ", "WriteInt", v); fflush(stdout);
    WriteInt(v);
    printf("\n");
  }

  printf("\n");
#endif

  return 0;
}

int test_WriteLong(void)
{
#ifdef TEST_WRITELONG
  long long values[] = { 0, 1, -1, INT_MAX, INT_MIN, LLONG_MAX, LLONG_MIN, };

  printf("  WriteLong(long long l)\n");
  for (int i=0; i<sizeof(values)/sizeof(long long); i++) {
    printf("    %s(%24lld): ", "WriteLong", values[i]); fflush(stdout);
    WriteInt(values[i]);
    printf("\n");
  }

  srand(time(0));
  for (int i=0; i<20; i++) {
    long long v = rand() - RAND_MAX/2;

    printf("    %s(%24lld): ", "WriteLong", v); fflush(stdout);
    WriteLong(v);
    printf("\n");
  }

  printf("\n");
#endif

  return 0;
}

#ifdef TEST_WRITESTR
char* MakeSnuPL1String(const char *str)
{
  size_t len = strlen(str)+1;
  int *res = calloc(2 + (len+3)/sizeof(int), sizeof(int));

  res[0] = 1;      // 1 dimension
  res[1] = len;    // size of dimension
  memcpy(&res[2], str, len);

  return (char*)res;
}
#endif

int test_WriteStr(void)
{
#ifdef TEST_WRITESTR
  char *values[] = { "SnuPL/1", "Seoul National University Programming Language 1", };

  printf("  WriteStr(char c)\n");
  for (int i=0; i<sizeof(values)/sizeof(char*); i++) {
    printf("    %s('%s'): ", "WriteStr", values[i]); fflush(stdout);

    char *str = MakeSnuPL1String(values[i]);
    WriteStr(str);
    free(str);

    printf("\n");
  }

  printf("\n");
#endif

  return 0;
}

int test_WriteChar(void)
{
#ifdef TEST_WRITECHAR
  char values[] = { 'a', 'z', 'A', 'Z', '0', '9', };

  printf("  WriteChar(char c)\n");
  for (int i=0; i<sizeof(values)/sizeof(char); i++) {
    printf("    %s(%c): ", "WriteChar", values[i]); fflush(stdout);
    WriteChar(values[i]);
    printf("\n");
  }

  printf("\n");
#endif

  return 0;
}

int test_WriteLn(void)
{
#ifdef TEST_WRITELN
  printf("  WriteLn(void)\n");

  printf("    WriteLn(): "); fflush(stdout); WriteLn();
  printf("    WriteLn(): "); fflush(stdout); WriteLn();

  printf("\n");
#endif

  return 0;
}

int test_output(void)
{
  printf("Testing output functions\n");
  printf("\n");

  test_WriteInt();
  test_WriteLong();
  test_WriteStr();
  test_WriteChar();
  test_WriteLn();

  return 0;
}

//--------------------------------------------------------------------------------------------------
// input tests
//
int test_ReadInt(void)
{
#ifdef TEST_READINT
  printf("  int ReadInt(void)\n");
  printf("\n");

  int v;
  do {
    printf("    Enter number (0 to stop): "); fflush(stdout);
    v = ReadInt();
    printf("      value read: %d\n", v);
    printf("\n");
  } while (v != 0);

#endif
  return 0;
}

int test_ReadLong(void)
{
#ifdef TEST_READLONG
  printf("  int ReadLong(void)\n");
  printf("\n");

  long long v;
  do {
    printf("    Enter number (0 to stop): "); fflush(stdout);
    v = ReadLong();
    printf("      value read: %lld\n", v);
    printf("\n");
  } while (v != 0);

#endif
  return 0;
}

int test_input(void)
{
  printf("Testing output functions\n");
  printf("\n");

  test_ReadInt();
  test_ReadLong();

  return 0;
}


//--------------------------------------------------------------------------------------------------
// main routine
//
int main(int argc, char **argv)
{
  int res = 0;

  res += test_output();
  res += test_input();

  return (res == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
}
