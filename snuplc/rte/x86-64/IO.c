//--------------------------------------------------------------------------------------------------
/// @brief SnuPL AMD64 I/O library
/// @author Bernhard Egger <bernhard@csap.snu.ac.kr>
/// @section changelog Change Log
/// 2020/08/05 Bernhard Egger created
///
/// @section license_section License
/// Copyright (c) 2020, Computer Systems and Platforms Laboratory, SNU
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

#include <stdio.h>

#include "ARRAY.h"

int ReadInt(void)
{
  int i, res;

  res = fscanf(stdin, "%d", &i);

  return i;
}

int ReadLong(void)
{
  int res;
  long long l;

  res = fscanf(stdin, "%lld", &l);

  return l;
}

void WriteInt(int i)
{
  fprintf(stdout, "%d", i);
  fflush(stdout);
}

void WriteLong(long long l)
{
  fprintf(stdout, "%lld", l);
  fflush(stdout);
}

void WriteStr(void *p)
{
  char *str = p + DOFS(p);
  fprintf(stdout, "%s", str);
  fflush(stdout);
}

void WriteChar(char c)
{
  fputc(c, stdout);
  fflush(stdout);
}

void WriteLn(void)
{
  fprintf(stdout, "\n");
}

