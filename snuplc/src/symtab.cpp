//--------------------------------------------------------------------------------------------------
/// @brief SnuPL symbol table
/// @author Bernhard Egger <bernhard@csap.snu.ac.kr>
/// @section changelog Change Log
/// 2012/09/14 Bernhard Egger created
/// 2016/04/05 Bernhard Egger bugfix in CSymtab::print
/// 2019/09/15 Bernhard Egger added support for constant expressions
/// 2020/08/05 Bernhard Egger added support for different storage locations
/// 2020/08/11 Bernhard Egger adapted to SnuPL/2
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
#include <iomanip>
#include <sstream>

#include "symtab.h"
using namespace std;


//--------------------------------------------------------------------------------------------------
// CStorage
//
CStorage::CStorage(EStorageLocation location, const string base, long long offset)
  : _location(location), _base(base), _offset(offset)
{
}

CStorage::~CStorage(void)
{
}

EStorageLocation CStorage::GetLocation(void) const
{
  return _location;
}

string CStorage::GetBase(void) const
{
  return _base;
}

long long CStorage::GetOffset(void) const
{
  return _offset;
}


ostream& CStorage::print(ostream &out, int indent) const
{
  ostringstream o;

  o << hex;
  switch (_location) {
    case slUndefined: o << "undefined"; break;
    case slMemoryAbs: o << "0x" << _offset; break;
    case slMemoryRel: o << _base << "+0x" << _offset; break;
    case slRegister:  o << _base; break;
    case slLabel:     o << _base; break;
  }

  string ind(indent, ' ');
  out << ind << setw(12) << o.str();
  return out;
}

ostream& operator<<(ostream &out, const CStorage &s)
{
  return s.print(out);
}

ostream& operator<<(ostream &out, const CStorage *s)
{
  return s->print(out);
}


//--------------------------------------------------------------------------------------------------
// CSymbol
//
CSymbol::CSymbol(const string name, ESymbolType stype, const CType *dtype)
  : _symtab(NULL), _name(name), _symboltype(stype), _datatype(dtype), _location(NULL), _data(NULL)
{
  assert(_name != "");
  assert(_datatype != NULL);
}

CSymbol::~CSymbol(void)
{
  if (_location != NULL) delete _location;
}

string CSymbol::GetName(void) const
{
  return _name;
}

ESymbolType CSymbol::GetSymbolType(void) const
{
  return _symboltype;
}

void CSymbol::SetDataType(const CType *datatype)
{
  _datatype = datatype;
}

const CType* CSymbol::GetDataType(void) const
{
  return _datatype;
}

void CSymbol::SetSymbolTable(CSymtab *symtab)
{
  _symtab = symtab;
}

CSymtab* CSymbol::GetSymbolTable(void) const
{
  return _symtab;
}

void CSymbol::SetData(CDataInitializer *data)
{
  _data = data;
}

const CDataInitializer* CSymbol::GetData(void) const
{
  return _data;
}

void CSymbol::SetLocation(CStorage *location)
{
  if (_location != NULL) delete _location;
  _location = location;
}

const CStorage* CSymbol::GetLocation(void) const
{
  return _location;
}

ostream& CSymbol::print(ostream &out, int indent) const
{
  string ind(indent, ' ');

  out << ind << "[ " << left << setw(8) << GetName() << right << " ";
  GetDataType()->print(out);
  out << ind << " ]";
  return out;
}

ostream& operator<<(ostream &out, const CSymbol &s)
{
  return s.print(out);
}

ostream& operator<<(ostream &out, const CSymbol *s)
{
  return s->print(out);
}


//------------------------------------------------------------------------------
// CSymGlobal
//
CSymGlobal::CSymGlobal(const string name, const CType *type)
  : CSymbol(name, stGlobal, type)
{
  SetLocation(new CStorage(slLabel, name, 0));
}

ostream& CSymGlobal::print(ostream &out, int indent) const
{
  string ind(indent, ' ');

  out << ind << "[ @" << left << setw(8) << GetName() << right << " ";
  GetDataType()->print(out);
  if (GetLocation() != NULL) out << GetLocation();
  out << ind << " ]";
  return out;
}


//------------------------------------------------------------------------------
// CSymLocal
//
CSymLocal::CSymLocal(const string name, const CType *type)
  : CSymbol(name, stLocal, type)
{
}

CSymLocal::CSymLocal(const string name, ESymbolType stype, const CType *type)
  : CSymbol(name, stype, type)
{
}

ostream& CSymLocal::print(ostream &out, int indent) const
{
  string ind(indent, ' ');

  out << ind << "[ $" << left << setw(8) << GetName() << right << " ";
  GetDataType()->print(out);
  if (GetLocation() != NULL) out << GetLocation();
  out << ind << " ]";
  return out;
}


//------------------------------------------------------------------------------
// CSymParam
//
CSymParam::CSymParam(int index, const string name, const CType *type)
  : CSymLocal(name, stParam, type), _index(index)
{
}

ostream& CSymParam::print(ostream &out, int indent) const
{
  string ind(indent, ' ');

  out << ind << "[ %" << left << setw(8) << GetName() << right << " ";
  GetDataType()->print(out);
  if (GetLocation() != NULL) out << GetLocation();
  out << ind << " ]";
  return out;
}

int CSymParam::GetIndex(void) const
{
  return _index;
}


//------------------------------------------------------------------------------
// CSymProc
//
CSymProc::CSymProc(const string name, const CType *return_type, bool external)
  : CSymbol(name, stProcedure, return_type), _external(external)
{
}

void CSymProc::AddParam(CSymParam *param)
{
  _param.push_back(param);
}

void CSymProc::SetExternal(bool external)
{
  _external = external;
}

bool CSymProc::IsExternal(void) const
{
  return _external;
}

unsigned int CSymProc::GetNParams(void) const
{
  return _param.size();
}

const CSymParam* CSymProc::GetParam(unsigned int index) const
{
  assert((index >= 0) && (index < _param.size()));
  return _param[index];
}

ostream& CSymProc::print(ostream &out, int indent) const
{
  string ind(indent, ' ');

  out << ind << "[ *" << GetName() << "(";
  for (size_t i=0; i<_param.size(); i++) {
    const CType *t = _param[i]->GetDataType();
    if (i > 0) out << ",";
    t->print(out);
  }
  out << ") --> ";
  GetDataType()->print(out);
  out << ind << " ]";
  return out;
}


//------------------------------------------------------------------------------
// CSymConstant
//
CSymConstant::CSymConstant(const string name, const CType *type,
                           const CDataInitializer *data)
  : CSymbol(name, stConstant, type)
{
  assert(data != NULL);
  _data = data;
}

ostream& CSymConstant::print(ostream &out, int indent) const
{
  string ind(indent, ' ');

  out << ind << "[ =" << left << setw(8) << GetName() << right << " ";
  GetDataType()->print(out);
  out << ind << " ]";
  return out;
}


//------------------------------------------------------------------------------
// CSymtab
//
CSymtab::CSymtab(void)
  : _parent(NULL)
{
}

CSymtab::CSymtab(CSymtab *parent)
  : _parent(parent)
{
  assert(parent != NULL);
}

CSymtab::~CSymtab(void)
{
  map<string, CSymbol*>::const_iterator it = _symtab.begin();
  while (it != _symtab.end()) delete (*it++).second;
  _symtab.clear();
}

CSymtab* CSymtab::GetParent(void) const
{
  return _parent;
}

bool CSymtab::AddSymbol(CSymbol *s)
{
  assert(s != NULL);

  // global symbols always get pushed up to the global symbol table
  if ((s->GetSymbolType() == stGlobal) && (_parent != NULL)) {
    return _parent->AddSymbol(s);
  }

  if (!FindSymbol(s->GetName(), sLocal)) {
    _symtab[s->GetName()] = s;
    s->SetSymbolTable(this);
    return true;
  } else {
    return false;
  }
}

const CSymbol* CSymtab::FindSymbol(const string name, EScope scope) const
{
  map<string, CSymbol*>::const_iterator it = _symtab.find(name);

  if (it != _symtab.end()) return (*it).second;
  else {
    if ((scope == sLocal) || (_parent == NULL)) return NULL;
    else return _parent->FindSymbol(name, scope);
  }
}

vector<CSymbol*> CSymtab::GetSymbols(void) const
{
  vector<CSymbol*> _res;

  map<string, CSymbol*>::const_iterator it = _symtab.begin();
  while (it != _symtab.end()) {
    _res.push_back(it->second);
    it++;
  }

  return _res;
}

ostream& CSymtab::print(ostream &out, int indent) const
{
  string ind(indent, ' ');

  out << ind << "[[";
  map<string, CSymbol*>::const_iterator it = _symtab.begin();
  while (it != _symtab.end()) {
    out << endl;

    const CSymbol *s = (*it++).second;
    s->print(out, indent+2);

    const CDataInitializer *di = s->GetData();
    if (di != NULL) {
      out << endl;
      di->print(out, indent+4);
    }
  }
  out << endl << ind << "]]" << endl;

  return out;
}

ostream& operator<<(ostream &out, const CSymtab &t)
{
  return t.print(out);
}

ostream& operator<<(ostream &out, const CSymtab *t)
{
  return t->print(out);
}

