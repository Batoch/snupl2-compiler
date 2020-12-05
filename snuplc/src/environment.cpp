//--------------------------------------------------------------------------------------------------
/// @brief SnuPL environment
/// @author Bernhard Egger <bernhard@csap.snu.ac.kr>
/// @section changelog Change Log
/// 2020/07/31 Bernhard Egger created
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


#include <cassert>
#include <cstring>
#include <iomanip>
#include <tuple>

#include "environment.h"
using namespace std;

//--------------------------------------------------------------------------------------------------
// default environment
//

/// @brief default environment settings
struct {
  const char *key;
  EParmType   type;
  const char *desc;
  string      dval;
} Settings[] =
{
  { "ast",     ptFlag,   "(do not) output the AST in textual/graphical form.",  "0" },
  { "tac",     ptFlag,   "(do not) output the IR in textual/graphical form.",   "0" },
  { "dot",     ptFlag,   "(do not) output the AST/IR in graphical form.",       "0" },
  { "run-dot", ptFlag,   "(do not) run the dot command automatically.",         "0" },
  { "console", ptFlag,   "output assembly code to console (instead of a file).","0" },
  { "exe",     ptFlag,   "(do not) run assembler on generated assembly code.",  "0" },
  { "lib-path",ptSetting,"path to SnuPL/1 libraries.",                       "rte/" },
  { "target",  ptTarget, "target architecture.",                           "x86-64" },
  { "help",    ptSwitch, "print this help.",                                    "0" },
  { NULL }
};

//--------------------------------------------------------------------------------------------------
// CEnvironment
//

CEnvironment* CEnvironment::_globenv= NULL;

CEnvironment::CEnvironment(void)
  : _active_target(NULL), _active_file("")
{
  InitDefaultConfig();
}

CEnvironment::~CEnvironment(void)
{
  auto it = _target.cbegin();

  while (it != _target.cend()) {
    delete it->second;
    it++;
  }

  _target.clear();
  _files.clear();
  _config.clear();
}

void CEnvironment::InitDefaultConfig(void)
{
  _config.clear();

  unsigned int i = 0;
  while (Settings[i].key != NULL) {
    tuple<EParmType, const char*, string>
      t(Settings[i].type, Settings[i].desc, Settings[i].dval);

    _config[string(Settings[i].key)] = t;
    i++;
  }
}

CEnvironment* CEnvironment::Get(void)
{
  if (_globenv == NULL) {
    _globenv = new CEnvironment();
    RegisterTargets(_globenv);
  }

  return _globenv;
}

void CEnvironment::Syntax(string msg)
{
  if (msg != "") cout << msg << endl << endl;

  InitDefaultConfig();

  cout << "Usage: snuplc [OPTIONS] FILES..." << endl
       << "Compile each source file in FILES using OPTIONS." << endl
       << "Example: snuplc fibonacci.mod" << endl
       << endl
       << "Command line parameters:" << endl
       << "  Flags:" << endl
       << setfill(' ') << left;

  auto cit = _config.cbegin();
  while (cit != _config.cend()) {
    if (get<0>(cit->second) == ptFlag) {
      cout << "    "
           << "--[no-]" << setw(10) << cit->first
           << "  "
           << setw(52) << get<1>(cit->second)
           << "  Default: "
           << (get<2>(cit->second) == "0" ? "off" : "on")
           << endl;
    }
    cit++;
  }

  cout << endl
       << "  Settings:" << endl;
  cit = _config.cbegin();
  while (cit != _config.cend()) {
    if (get<0>(cit->second) == ptSetting) {
      cout << "    "
           << "--" << setw(10) << cit->first
           << "       "
           << setw(52) << get<1>(cit->second)
           << "  Default: "
           << get<2>(cit->second)
           << endl;
    }
    cit++;
  }

  cout << endl
       << "  Switches:" << endl;
  cit = _config.cbegin();
  while (cit != _config.cend()) {
    if (get<0>(cit->second) == ptSwitch) {
      cout << "    "
           << "--" << setw(10) << cit->first
           << "       "
           << setw(52) << get<1>(cit->second)
           << endl;
    }
    cit++;
  }

  cout << endl
       << "  Target architecture:" << endl;
  cit = _config.cbegin();
  while (cit != _config.cend()) {
    if (get<0>(cit->second) == ptTarget) {
      cout << "    "
           << "--" << setw(10) << cit->first
           << "       "
           << setw(52) << get<1>(cit->second)
           << "  Default: "
           << get<2>(cit->second)
           << endl;
    }
    cit++;
  }

  cout << endl
       << "    Available targets:" << endl;
  auto tit = _target.cbegin();
  while (tit != _target.cend()) {
    cout << "      "
         << setw(10) << tit->first
         << "       "
         << setw(52) << tit->second->GetName()
         << endl;
    tit++;
  }

  cout << endl << endl
       << "Examples:" << endl
       << "  compile fibonacci.mod and write generated assembly code to fibonacci.mod.s" << endl
       << "  $ snuplc fibonacci.mod" << endl
       << endl
       << "  compile fibonacci.mod to fibonacci.mod.s, then generate fibonacci executable" << endl
       << "  $ snuplc --exe fibonacci.mod" << endl
       << endl
       << "  compile fibonacci.mod and also output the AST in textual and graphical form" << endl
       << "  The AST is saved in fibonacci.mod.ast (textual) and fibonacci.mod.ast.dot (graphical form)" << endl
       << "  $ snuplc --ast fibonacci.mod" << endl
       << endl
       << "  compile fibonacci.mod and also output the IR in textual and graphical form" << endl
       << "  The IR is saved in fibonacci.mod.tac (textual) and fibonacci.mod.tac.dot (graphical form)" << endl
       << "  $ snuplc --tac fibonacci.mod" << endl
       << endl;

  exit(EXIT_FAILURE);
}

void CEnvironment::ParseArguments(int argc, char *argv[])
{
  int i = 1;

  while (i < argc) {
    char *str = argv[i];

    if ((strlen(str) > 2) && !strncmp(str, "--", 2)) {
      str += 2;

      bool bval = true;
      if (!strncmp(str, "no-", 3)) {
        str += 3;
        bval = false;
      }

      string key = string(str);

      auto c = _config.find(key);

      if (c == _config.end()) {
        Syntax("Unknown command line option '" + string(argv[i]) + "'.");

      } else if (get<0>(c->second) == ptFlag) {
        // flags can be turned on or off
        get<2>(c->second) = bval ? "1" : "0";

      } else if (get<0>(c->second) == ptSwitch) {
        // switches can only be turned on
        get<2>(c->second) = "1";

      } else if (get<0>(c->second) == ptSetting) {
        // settings take the following argument as a parameter
        if (i+1 == argc) Syntax("Missing argument after " + string(argv[i]));
        get<2>(c->second) = string(argv[++i]);

      } else if (get<0>(c->second) == ptTarget) {
        // target takes the following argument as a parameter
        if (i+1 == argc) Syntax("Missing argument after " + string(argv[i]));
        get<2>(c->second) = string(argv[++i]);

      } else {
        cerr << "Internal error in " << __FILE__ << ":" << __LINE__ 
             << " (" << __FUNCTION__ << ")" << endl;
        exit(EXIT_FAILURE);
      }
    }
    else AddFile(argv[i]);
    i++;
  }

  bool b;
  if (GetSwitch("help", b) && b) Syntax("");

  string t;
  if (GetConfig("target", t)) {
    if (!SetTarget(t)) {
      Syntax("Unsupported target: '" + t + "'.");
    }
  }
}

/*
bool CEnvironment::SetConfig(const string key, const string value)
{
  auto it = _config.find(key);

  if (it != _config.end()) get<2>(it->second) = value;

  return (it != _config.end());
}
*/

bool CEnvironment::GetFlag(const string key, bool &value) const
{
  auto it = _config.find(key);

  if ((it != _config.cend()) && (get<0>(it->second) == ptFlag)) {
    value = get<2>(it->second) == "1";
  }

  return (it != _config.end());
}

bool CEnvironment::GetSwitch(const string key, bool &value) const
{
  auto it = _config.find(key);

  if ((it != _config.cend()) && (get<0>(it->second) == ptSwitch)) {
    value = get<2>(it->second) == "1";
  }

  return (it != _config.end());
}

bool CEnvironment::GetSetting(const string key, string &value) const
{
  auto it = _config.find(key);

  if ((it != _config.cend()) && (get<0>(it->second) == ptSetting)) {
    value = get<2>(it->second);
  }

  return (it != _config.end());
}

bool CEnvironment::GetConfig(const string key, string &value) const
{
  auto it = _config.find(key);

  if (it != _config.cend()) value = get<2>(it->second);

  return (it != _config.end());
}

void CEnvironment::AddTarget(CTarget *t, bool active)
{
  assert(t != NULL);

  if (_target.find(t->GetKey()) == _target.end()) {
    _target.insert(pair<string, CTarget*>(t->GetKey(), t));
  }

  if (active) _active_target = t;
}

bool CEnvironment::SetTarget(const string name)
{
  auto it = _target.find(name);

  if (it != _target.end()) _active_target = it->second;
  else _active_target = NULL;

  return it != _target.end();
}

CTarget* CEnvironment::GetTarget(void) const
{
  return _active_target;
}

void CEnvironment::AddFile(const string file)
{
  _files.push_back(file);
}

string CEnvironment::GetNextFile(void)
{
  _active_file = "";

  if (!_files.empty()) {
    _active_file = _files[0];
    _files.erase(_files.begin());
  }

  return _active_file;
}

ostream& CEnvironment::print(ostream &out, int indent) const
{
  string ind(indent, ' ');

  out << ind << "Environment:" << endl;

  // configuration switches
  out << ind << "  Configuration:" << endl;
  auto cit = _config.cbegin();
  while (cit != _config.cend()) {
    out << ind << "    " << cit->first << " = " << get<2>(cit->second) << endl;
    cit++;
  }
  out << ind << endl;

  // target info
  out << ind << "  Available targets:" << endl;
  auto tit = _target.cbegin();
  while (tit != _target.cend()) {
    out << ind << "    " << tit->first
               << " (" << tit->second->GetName() <<")" << endl;
    tit++;
  }
  out << ind << endl;

  out << ind << "  Active target:" << endl;
  CTarget *t = GetTarget();
  if (t == NULL) out << ind << "    none" << endl;
  else t->print(out, indent+4);
  out << ind << endl;

  // file info
  out << ind << "  Files to compile:" << endl;
  auto fit = _files.cbegin();
  while (fit != _files.cend()) {
    out << ind << "    " << *fit << endl;
    fit++;
  }
  out << ind << endl;

  out << ind << "  Active file:" << endl
      << ind << "    ";
  if (_active_file == "") out << "none";
  else out << _active_file;
  out << endl
      << ind << endl;

  return out;
}

ostream& operator<<(ostream &out, const CEnvironment &e)
{
  return e.print(out);
}

ostream& operator<<(ostream &out, const CEnvironment *e)
{
  return e->print(out);
}


