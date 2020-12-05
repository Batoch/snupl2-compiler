//--------------------------------------------------------------------------------------------------
/// @brief SNUPL type system
/// @author Bernhard Egger <bernhard@csap.snu.ac.kr>
/// @section changelog Change Log
/// 2012/09/14 Bernhard Egger created
/// 2016/03/12 Bernhard Egger adapted to SnuPL/1
/// 2020/07/31 Bernhard Egger added support for longint plus other improvements
/// 2020/09/27 Bernhard Egger assignment 2: removed type checking code
///
/// @section license_section License
/// Copyright (c) 2012-2019, Computer Systems and Platforms Laboratory, SNU
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

#include "type.h"
#include "environment.h"
using namespace std;


//--------------------------------------------------------------------------------------------------
// CType
//
CType::CType(const string name)
  : _name(name)
{
}

CType::~CType(void)
{
}

unsigned int CType::GetDataSize(void) const
{
  return GetSize();
}

bool CType::Compare(const CType *t) const
{
  return Match(t);
}

ostream& operator<<(ostream &out, const CType &t)
{
  return t.print(out);
}

ostream& operator<<(ostream &out, const CType *t)
{
  return t->print(out);
}


//--------------------------------------------------------------------------------------------------
// CScalarType
//
CScalarType::CScalarType(const string name)
  : CType(name)
{
}

bool CScalarType::Match(const CType *t) const
{
	return (t == this);
}


//--------------------------------------------------------------------------------------------------
// CNullType
//
CNullType::CNullType(void)
  : CScalarType("NULL")
{
}

ostream& CNullType::print(ostream &out, int indent) const
{
  string ind(indent, ' ');

  out << ind << "<" << GetName() << ">";
  return out;
}


//--------------------------------------------------------------------------------------------------
// CBoolType
//
CBoolType::CBoolType(void)
  : CScalarType("boolean")
{
}

ostream& CBoolType::print(ostream &out, int indent) const
{
  string ind(indent, ' ');

  out << ind << "<" << GetName();
  //out << "," << GetSize() << "," << GetAlign();
  out << ">";
  return out;
}


//--------------------------------------------------------------------------------------------------
// CCharType
//
CCharType::CCharType(void)
  : CScalarType("char")
{
}

ostream& CCharType::print(ostream &out, int indent) const
{
  string ind(indent, ' ');

  out << ind << "<" << GetName();
  //out << "," << GetSize() << "," << GetAlign();
  out << ">";
  return out;
}


//--------------------------------------------------------------------------------------------------
// CIntType
//
CIntType::CIntType(const string name)
  : CScalarType(name)
{
}


//--------------------------------------------------------------------------------------------------
// CIntegerType
//
CIntegerType::CIntegerType(void)
  : CIntType("integer")
{
}

ostream& CIntegerType::print(ostream &out, int indent) const
{
  string ind(indent, ' ');

  out << ind << "<" << GetName();
  //out << "," << GetSize() << "," << GetAlign();
  out << ">";
  return out;
}


//--------------------------------------------------------------------------------------------------
// CLongintType
//
CLongintType::CLongintType(void)
  : CIntType("longint")
{
}

ostream& CLongintType::print(ostream &out, int indent) const
{
  string ind(indent, ' ');

  out << ind << "<" << GetName();
  //out << "," << GetSize() << "," << GetAlign();
  out << ">";
  return out;
}


//--------------------------------------------------------------------------------------------------
// CPointerType
//
CPointerType::CPointerType(const CType *basetype)
  : CScalarType("ptr"), _basetype(basetype)
{
}

unsigned int CPointerType::GetSize(void) const {
  return CEnvironment::Get()->GetTarget()->GetMachineWordSize();
}

unsigned int CPointerType::GetAlign(void) const {
  return CEnvironment::Get()->GetTarget()->GetMachineWordSize();
}

bool CPointerType::Match(const CType *t) const
{
	bool match = false;
	if (Compare(t)) match = true;

	const CPointerType *pointer = dynamic_cast<const CPointerType*>(t);

	// Dynamiccast allow that
	if (pointer->IsNull())
		return false;

	// Match if void pointer or match
	if (GetBaseType()->IsNull())
		match = true;
	if (GetBaseType()->Match(pointer))
		match = true;
	return match;
}

bool CPointerType::Compare(const CType *t) const
{
	// Check if t is a pointer or not
	if (t->IsNull())
		return false;
	if (!t->IsPointer())
		return false;

	bool match = false;
	const CPointerType *pointer = dynamic_cast<const CPointerType*>(t);

	if (pointer->IsNull())
		return false;

	if (GetBaseType()->IsNull())
		match = true;
	if (GetBaseType()->Compare(pointer))
		match = true;
	return match;
}

ostream& CPointerType::print(ostream &out, int indent) const
{
  string ind(indent, ' ');

  out << ind << "<" << GetName() << "(" << GetSize() << ") to ";
  if (_basetype != NULL) out << _basetype; else out << "void";
  out << ">";
  return out;
}


//--------------------------------------------------------------------------------------------------
// CArrayType
//
CArrayType::CArrayType(int nelem, const CType *innertype)
  : CType("array"), _nelem(nelem), _innertype(innertype)
{
  assert((_nelem > 0) || (_nelem == OPEN));
  assert(_innertype != NULL);
}

CArrayType::~CArrayType(void)
{
  // do not free _innertype; all types (also inner types) are
  // owned by the type manager
}

unsigned int CArrayType::GetSize(void) const
{
  return 4 + 4*GetNDim() + GetDataSize();
}

unsigned int CArrayType::GetDataSize(void) const
{
  unsigned int n = GetNElem();
  return n == OPEN ? 0 : n*GetInnerType()->GetDataSize();
}

unsigned int CArrayType::GetAlign(void) const
{
  // arrays must be 4-aligned since we have integer meta-data at the beginning
  return 4;
}

const CType* CArrayType::GetBaseType(void) const
{
  const CType *t = GetInnerType();
  assert(t != NULL);

  if (t->IsArray()) return dynamic_cast<const CArrayType*>(t)->GetBaseType();
  else return t;
}

unsigned int CArrayType::GetNDim(void) const
{
  const CType *t = GetInnerType();
  assert(t != NULL);

  if (t->IsArray()) return dynamic_cast<const CArrayType*>(t)->GetNDim() + 1;
  else return 1;
}

bool CArrayType::Match(const CType *t) const
{
	// TODO: Recursivity
	if (!t->IsArray())
		return false;
	const CArrayType *array = dynamic_cast<const CArrayType*>(t);
	if (array->IsNull())
		return false;

	if (array->GetNElem()==OPEN)
		return true;

	if (GetNElem()==array->GetNElem())
		return true;
	return false;
}

bool CArrayType::Compare(const CType *t) const
{
	// TODO: Recursivity
	if (!t->IsArray())
		return false;
	const CArrayType *array = dynamic_cast<const CArrayType*>(t);
	if (array->IsNull())
		return false;

	if (GetNElem()==array->GetNElem())
		return true;
	return false;
}

ostream& CArrayType::print(ostream &out, int indent) const
{
  string ind(indent, ' ');
  unsigned int n = GetNElem();

  out << ind << GetName();
  if (n != OPEN) out << " " << n << " ";
  out << "of "; GetInnerType()->print(out);
  //out << "," << GetSize() << "," << GetAlign();
  out << ">";
  return out;
}


//--------------------------------------------------------------------------------------------------
// CTypeManager
//
CTypeManager* CTypeManager::_global_tm = NULL;

CTypeManager::CTypeManager(void)
{
  _null = new CNullType();
  _boolean = new CBoolType();
  _char = new CCharType();
  _integer = new CIntegerType();
  _longint = new CLongintType();
  _voidptr = new CPointerType(_null);
  _ptr.push_back(_voidptr);

  unsigned int bits = 8*CEnvironment::Get()->GetTarget()->GetMachineWordSize();
  if (bits == 32) _register = _integer;
  else if (bits == 64) _register = _longint;
  else assert(false);
}

CTypeManager::~CTypeManager(void)
{
  delete _null;
  delete _boolean;
  delete _char;
  delete _integer;
  delete _longint;
  //_register is a pointer to _integer or _longint
  // _voidptr is a member of _pointer and will get deleted just below
  for (size_t i=0; i<_ptr.size(); i++) delete _ptr[i];
  for (size_t i=0; i<_array.size(); i++) delete _array[i];
}

CTypeManager* CTypeManager::Get(void)
{
  if (_global_tm == NULL) _global_tm = new CTypeManager();

  return _global_tm;
}

const CNullType* CTypeManager::GetNull(void) const
{
  return _null;
}

const CBoolType* CTypeManager::GetBool(void) const
{
  return _boolean;
}

const CCharType* CTypeManager::GetChar(void) const
{
  return _char;
}

const CIntegerType* CTypeManager::GetInteger(void) const
{
  return _integer;
}

const CLongintType* CTypeManager::GetLongint(void) const
{
  return _longint;
}

const CIntType* CTypeManager::GetRegisterType(void) const
{
  return _register;
}

const CPointerType* CTypeManager::GetVoidPtr(void) const
{
  return _voidptr;
}

const CPointerType* CTypeManager::GetPointer(const CType *basetype)
{
  for (size_t i=0; i<_ptr.size(); i++) {
    if ((_ptr[i]->GetBaseType()->Compare(basetype))) {
      return _ptr[i];
    }
  }

  CPointerType *p = new CPointerType(basetype);
  _ptr.push_back(p);

  return p;
}

const CArrayType* CTypeManager::GetArray(unsigned int nelem,
                                         const CType *innertype)
{
  if (innertype == NULL) return NULL;

  for (size_t i=0; i<_array.size(); i++) {
    if ((_array[i]->GetNElem() == nelem) &&
        (_array[i]->GetInnerType()->Compare(innertype))) {
      return _array[i];
    }
  }

  unsigned long long size = innertype->GetDataSize();
  if (nelem != CArrayType::OPEN) size = size * nelem + 8;
  else size = size + 8; // open arrays have a data size of 0
  if (size > CArrayType::MAX_SIZE) return NULL;

  CArrayType *a = new CArrayType(nelem, innertype);
  _array.push_back(a);

  return a;
}

ostream& CTypeManager::print(ostream &out, int indent) const
{
  string ind(indent, ' ');

  out << ind << "[[ type manager" << endl
      << ind << "  base types:" << endl
      << ind << "    " << _null << endl
      << ind << "    " << _boolean << endl
      << ind << "    " << _char << endl
      << ind << "    " << _integer << endl
      << ind << "    " << _longint << endl
      << ind << "    " << _voidptr << endl << endl
      << ind << "  machine register type:" << endl
      << ind << "    " << _register << endl << endl
      << ind << "  pointer types:" << endl;
  for (size_t i=0; i<_ptr.size(); i++) {
    out << ind << "    " << _ptr[i] << endl;
  }
  out << endl
      << ind << "  array types:" << endl;
  for (size_t i=0; i<_array.size(); i++) {
    out << ind << "    " << _array[i] << endl;
  }
  out << ind << "]]" << endl;

  return out;
}
