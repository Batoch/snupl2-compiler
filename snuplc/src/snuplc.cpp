//--------------------------------------------------------------------------------------------------
/// @brief SnuPL compiler driver
/// @author Bernhard Egger <bernhard@csap.snu.ac.kr>
/// @section changelog Change Log
/// 2012/09/14 Bernhard Egger created
/// 2013/06/06 Bernhard Egger adapted to new IR/backend for SnuPL/0
/// 2014/11/30 Bernhard Egger proper argument handling
/// 2020/07/31 Bernhard Egger adopted to new config environment
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

#include <cassert>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>

#include "environment.h"
#include "scanner.h"
#include "parser.h"
#include "ir.h"
#include "backend.h"
using namespace std;


void RunCompile(string file, CTarget *target)
{
  bool b;
  if (CEnvironment::Get()->GetFlag("exe", b) && b) {
    assert(target != NULL);
    string arch = target->GetKey();

    ostringstream cmd;

    string exe(file);
    exe.erase(exe.find(".mod"));

    string lib_path;
    if (!CEnvironment::Get()->GetSetting("lib-path", lib_path)) {
      lib_path = "";
    }

    cmd << "gcc"
        << " -m" << dec << target->GetMachineWordSize() * 8
        << " -march=" << arch
        //<< " -no-pie"  // supported as of gcc >=6
        << " -L" << lib_path << "/" << arch
        << " -o " << exe
        << " " << file
        << " -l" << target->GetStdLibrary();

    cout << "  running command '" << cmd.str() << "'..." << endl;
    if (system(cmd.str().c_str()) < 0) {
      cout << "  failed to run gcc." << endl;
    }
  }
}

void RunDOT(string file)
{
  bool b;

  if (CEnvironment::Get()->GetFlag("run-dot", b) && b) {
    ostringstream cmd;
    cmd << "dot -Tpdf -o" << file << ".pdf " << file;

    cout << "  running command '" << cmd.str() << "'..." << endl;
    if (system(cmd.str().c_str()) < 0) {
      cout << "  failed to run dot." << endl;
    }
  }
}

void DumpAST(string file, CAstModule *ast)
{
  bool b;

  if (CEnvironment::Get()->GetFlag("ast", b) && b) {
    assert(ast != NULL);

    // output AST in textual form
    ofstream out(file + ".ast");
    out << file << ":" << endl;
    ast->print(out, 4);
    out << endl << endl
      << "  symbol table:" << endl;
    ast->GetSymbolTable()->print(out, 4);
    out << endl;

    // output AST in graphical form
    if (CEnvironment::Get()->GetFlag("dot", b) && b) {
      string fn = file + ".ast.dot";
      ofstream dot(fn);
      dot << "digraph AST {" << endl
          << "  graph [fontname=\"Times New Roman\",fontsize=10];" << endl
          << "  node  [fontname=\"Courier New\",fontsize=10];" << endl
          << "  edge  [fontname=\"Times New Roman\",fontsize=10];" << endl
          << endl;
      ast->toDot(dot, 2);
      dot << "}" << endl;
      dot.flush();

      RunDOT(fn);
    }
  }
}

void DumpTAC(string file, CModule *m)
{
  bool b;

  if (CEnvironment::Get()->GetFlag("tac", b) && b) {
    assert(m != NULL);

    // output TAC in textual form
    ofstream out(file + ".tac");
    out << file << ":" << endl
        << m << endl;

    // output TAC in graphical form
    if (CEnvironment::Get()->GetFlag("dot", b) && b) {
      string fn = file + ".tac.dot";
      ofstream dot(fn);

      dot << "digraph IR {" << endl
          << "  graph [fontname=\"Times New Roman\",fontsize=10];" << endl
          << "  node  [fontname=\"Courier New\",fontsize=10];" << endl
          << "  edge  [fontname=\"Times New Roman\",fontsize=10];" << endl
          << endl;
      m->toDot(dot, 2);
      const vector<CScope*> &proc = m->GetSubscopes();
      for (size_t p=0; p<proc.size(); p++) {
        proc[p]->toDot(dot, 2);
      }
      dot<< "}" << endl;
      dot.flush();

      RunDOT(fn);
    }
  }
}

int main(int argc, char *argv[])
{
  CEnvironment *env = CEnvironment::Get();
  env->ParseArguments(argc, argv);

  CTarget *target = env->GetTarget();
  if (target == NULL) {
    env->Syntax("Target not available.");
  }

  //CTypeManager::Get()->print(cout);

  string file = env->GetNextFile();

  if (file == "") env->Syntax("No input files.");

  while (file != "") {
    //
    // scanning, parsing
    //
    CScanner *s = new CScanner(new ifstream(file));
    CParser *p = new CParser(s);

    cout << "compiling " << file << "..." << endl;
    CAstNode *ast = p->Parse();

    if (p->HasError()) {
      const CToken *error = p->GetErrorToken();
      cout << "syntax error at " << error->GetLineNumber() << ":"
           << error->GetCharPosition() << " : "
           << p->GetErrorMessage() << endl;
    } else {
      CAstModule *m = dynamic_cast<CAstModule*>(ast);
      assert(m != NULL);

      //
      // semantic analysis
      //
      CToken t;
      string msg;
      if (!m->TypeCheck(&t, &msg)) {
        cout << "semantic error at " << t.GetLineNumber() << ":"
          << t.GetCharPosition() << " : " << msg << endl;
      } else {
        DumpAST(file, m);

        //
        // AST to TAC conversion
        //
        CModule *m = new CModule(ast);

        DumpTAC(file, m);

        // output assembly to console or file
        ostream *out = &cout;
        ofstream *sout = NULL;

        bool b;
        if (CEnvironment::Get()->GetFlag("console", b) && !b) {
          sout = new ofstream(file + ".s");
          out = sout;
        }

        //
        // code generation
        //
        CBackend *be = target->GetBackend(*out);
        if (be == NULL) {
          cout << "No backend available for target '"
            << target->GetName() << "'." << endl;
          return EXIT_FAILURE;
        }

        be->Emit(m);

        if (sout != NULL) {
          sout->flush();
          delete sout;
        }

        if (be->HasError()) {
          cout << "code generation error: " << be->GetErrorMessage() << endl;
        } else {
          RunCompile(file + ".s", target);
        }

        delete be;
      }

      delete m;
    }

    delete p;
    delete s;

    file = env->GetNextFile();
  }

  return EXIT_SUCCESS;
}
