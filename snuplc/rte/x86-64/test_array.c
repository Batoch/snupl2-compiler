//--------------------------------------------------------------------------------------------------
/// @brief test cases for AMD64 ARRAY.s
/// @author Bernhard Egger <bernhard@csap.snu.ac.kr>
/// @section changelog Change Log
/// 2016/04/16 Bernhard Egger created
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

#include <stdio.h>

#include "ARRAY.h"


int a[] = {
  1,    // # dimensions
  10,   // size of 1st dimension
        // data
  1, 2, 3, 4, 5, 6, 7, 8, 9, 10
};

int b[] = {
  2,    // # dimensions
  3,    // size of 1st dimension
  5,    // size of 2nd dimension
  0,    // padding
        // data
  11, 12, 13, 14, 15,
  21, 22, 23, 24, 25,
  31, 32, 33, 34, 35,
};

void dump(void *a)
{
  int dim, dofs, elem, size;

  dim = DIM(a, 0);
  dofs = DOFS(a);

  printf("dumping a (%p, %d dimensions, %d offset to data)\n", a, dim, dofs);

  elem = 1;
  while (dim > 0) {
    int d = DIM(a, dim);
    printf("  dim %d: %d\n", dim, d);
    elem = elem * d;
    dim--;
  }
  size = elem*sizeof(int);

  printf("  # elements: %d\n", elem);
  printf("  data size: %d\n", size);

  void *p = a + dofs, *end = a + dofs + size;
  printf("  data beginning at %p:\n", p);
  printf("  data ending    at %p:\n", end);
  while (p < end) {
    printf("  %d", *(int*)p);
    p += 4;
  }
  printf("\n");
}

int main(void)
{
  dump(a);
  dump(b);
  return 0;
}
