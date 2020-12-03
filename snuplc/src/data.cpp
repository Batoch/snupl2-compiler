//--------------------------------------------------------------------------------------------------
/// @brief SnuPL data initializers
/// @author Bernhard Egger <bernhard@csap.snu.ac.kr>
/// @section changelog Change Log
/// 2016/04/05 Bernhard Egger created
/// 2019/09/15 Bernhard Egger added support for int, char, and boolean constants
///
/// @section license_section License
/// Copyright (c) 2016-2020, Computer Systems and Platforms Laboratory, SNU
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

#include "data.h"
#include "scanner.h"
using namespace std;


//--------------------------------------------------------------------------------------------------
// CDataInitializer
//
CDataInitializer::CDataInitializer(void)
{
}

CDataInitializer::~CDataInitializer(void)
{
}

ostream& operator<<(ostream &out, const CDataInitializer &t)
{
  return t.print(out);
}

ostream& operator<<(ostream &out, const CDataInitializer *t)
{
  return t->print(out);
}


//--------------------------------------------------------------------------------------------------
// CDataInitLongint
//
CDataInitLongint::CDataInitLongint(const long long data)
  : CDataInitializer(), _data(data)
{
}

long long CDataInitLongint::GetData(void) const
{
  return _data;
}

ostream& CDataInitLongint::print(ostream &out, int indent) const
{
  string ind(indent, ' ');

  out << ind << "[ data: " << dec << _data << " ]";
  return out;
}


//--------------------------------------------------------------------------------------------------
// CDataInitInteger
//
CDataInitInteger::CDataInitInteger(const int data)
  : CDataInitializer(), _data(data)
{
}

int CDataInitInteger::GetData(void) const
{
  return _data;
}

ostream& CDataInitInteger::print(ostream &out, int indent) const
{
  string ind(indent, ' ');

  out << ind << "[ data: " << dec << _data << " ]";
  return out;
}


//--------------------------------------------------------------------------------------------------
// CDataInitBoolean
//
CDataInitBoolean::CDataInitBoolean(const bool data)
  : CDataInitializer(), _data(data)
{
}

bool CDataInitBoolean::GetData(void) const
{
  return _data;
}

ostream& CDataInitBoolean::print(ostream &out, int indent) const
{
  string ind(indent, ' ');

  out << ind << "[ data: " << (_data ? "true" : "false" ) << " ]";
  return out;
}


//--------------------------------------------------------------------------------------------------
// CDataInitChar
//
CDataInitChar::CDataInitChar(const char data)
  : CDataInitializer(), _data(data)
{
}

char CDataInitChar::GetData(void) const
{
  return _data;
}

ostream& CDataInitChar::print(ostream &out, int indent) const
{
  string ind(indent, ' ');
  string data;

  data = _data;

  out << ind << "[ data: '" << CToken::escape(tCharConst, data) << "' ]";
  return out;
}


//--------------------------------------------------------------------------------------------------
// CDataInitString
//
CDataInitString::CDataInitString(const string data)
  : CDataInitializer(), _data(data)
{
}

string CDataInitString::GetData(void) const
{
  return _data;
}

ostream& CDataInitString::print(ostream &out, int indent) const
{
  string ind(indent, ' ');

  out << ind << "[ data: \"" << _data << "\" ]";
  return out;
}

