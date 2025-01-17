//--------------------------------------------------------------------------------------------------
/// @brief SNUPL IR test
/// @author Bernhard Egger <bernhard@csap.snu.ac.kr>
/// @section changelog Change Log
/// 2020/08/03 Bernhard Egger created from test_semanal.cpp
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

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cassert>
#include <string.h>

#include "scanner.h"
#include "parser.h"
#include "ir.h"
using namespace std;

int main(int argc, char *argv[])
{
  int i = 1;
  bool use_stdin = argc == 1;
  char *fn;

  while ((i < argc) || (use_stdin)) {
    istream *in;

    if (use_stdin) {
      fn = strdup("stdin");
      in = &cin;
      cout << "parsing from standard input..." << endl;
    } else {
      fn = argv[i];
      in = new ifstream(fn);
      cout << "parsing '" << fn << "'..." << endl;
    }

    CScanner *s = new CScanner(in);
    CParser *p = new CParser(s);

    CAstNode *n = p->Parse();

    if (p->HasError()) {
      const CToken *error = p->GetErrorToken();
      cout << "syntax error at " << error->GetLineNumber() << ":"
           << error->GetCharPosition() << " : " << p->GetErrorMessage() << endl;

      delete n;
    } else {
      CAstModule *ast = dynamic_cast<CAstModule*>(n);
      assert(ast != NULL);

      cout << "successfully parsed." << endl
           << "running semantic analysis..." << endl;

      CToken t;
      string msg;
      if (!ast->TypeCheck(&t, &msg)) {
        cout << "semantic error at " << t.GetLineNumber() << ":"
          << t.GetCharPosition() << " : " << msg << endl;
      } else {
        cout << "semantic analysis completed." << endl;

        // AST to TAC conversion
        cout << "converting to TAC..." << endl;
        CModule *m = new CModule(ast);

        // print TAC to console
        cout << m << endl;
        cout << endl;

        // output TAC as .dot and generate a PDF file from it
        ofstream out(string(fn) + ".dot");
        out << "digraph IR {" << endl
            << "  graph [fontname=\"Times New Roman\",fontsize=10];" << endl
            << "  node  [fontname=\"Courier New\",fontsize=10];" << endl
            << "  edge  [fontname=\"Times New Roman\",fontsize=10];" << endl
            << endl;
        m->toDot(out, 2);
        const vector<CScope*> &proc = m->GetSubscopes();
        for (size_t p=0; p<proc.size(); p++) {
          proc[p]->toDot(out, 2);
        }
        out << "}" << endl;
        out.flush();

        ostringstream cmd;
        cmd << "dot -Tpdf -o" << fn << ".pdf " << fn << ".dot";
        cout << "run the following command to convert the .dot file into a PDF:" << endl
             << "  " << cmd.str() << endl;

        delete m;
      }

      delete ast;
    }

    cout << endl << endl;

    i++;

    delete p;
    delete s;
    if (!use_stdin) delete in;
    use_stdin = false;
  }

  cout << "Done." << endl;

  return EXIT_SUCCESS;
}
