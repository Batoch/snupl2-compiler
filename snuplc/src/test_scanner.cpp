//--------------------------------------------------------------------------------------------------
/// @brief SnuPL scanner test
/// @author Bernhard Egger <bernhard@csap.snu.ac.kr>
/// @section changelog Change Log
/// 2012/09/14 Bernhard Egger created
/// 2019/09/15 Bernhard Egger support stdin input streams
///
/// @section license_section License
/// Copyright (c) 2012-2020, Computer Systems and Platforms Laboratory, SNU
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

#include <cstdlib>
#include <iostream>
#include <fstream>

#include "scanner.h"
using namespace std;

int main(int argc, char *argv[])
{
  int i = 1;
  bool use_stdin = argc == 1;

  while ((i < argc) || (use_stdin)) {
    istream *in;

    if (use_stdin) {
      in = &cin;
      cout << "scanning from standard input..." << endl;
    } else {
      in = new ifstream(argv[i]);
      cout << "scanning '" << argv[i] << "'..." << endl;
    }

    CScanner *s = new CScanner(in);

    if (!s->Good()) cout << "  cannot open input stream: " << s->Peek() << endl;

    while (s->Good()) {
      CToken t = s->Get();
      cout << "  " << t << endl;
      if (t.GetType() == tEOF) break;
    }

    cout << endl << endl;

    i++;

    delete s;
    if (!use_stdin) delete in;
    use_stdin = false;
  }

  cout << "Done." << endl;

  return EXIT_SUCCESS;
}
