//--------------------------------------------------------------------------------------------------
/// @brief SnuPL target specification
/// @author Bernhard Egger <bernhard@csap.snu.ac.kr>
/// @section changelog Change Log
/// 2020/07/31 Bernhard Egger created
/// 2020/09/27 Bernhard Egger removed non-generic targets & backends for assignment 2
/// 2020/11/22 Bernhard Egger added AMD64 backend as default target for assignment 5
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

#include "target.h"
#include "environment.h"
#include "backendAMD64.h"
using namespace std;


//--------------------------------------------------------------------------------------------------
// CTarget
//
CTarget::CTarget(const string key, const string name,
                 unsigned int machine_word_size)
  : _key(key), _name(name), _machine_word_size(machine_word_size)
{
}

CTarget::~CTarget(void)
{
}

ostream& CTarget::print(ostream &out, int indent) const
{
  string ind(indent, ' ');

  out << ind << "Target '" << GetName() << "' (" << GetKey() << ")"
             << endl << dec
      << ind << "  machine word size: " << GetMachineWordSize() << " bytes"
      << endl;
  return out;
}

ostream& operator<<(ostream &out, const CTarget &t)
{
  return t.print(out);
}

ostream& operator<<(ostream &out, const CTarget *t)
{
  return t->print(out);
}

void RegisterTargets(CEnvironment *e)
{
  assert(e != NULL);

  e->AddTarget(new CTarget32());
  e->AddTarget(new CTarget64());
  e->AddTarget(new CTargetAMD64(), true);
}

//--------------------------------------------------------------------------------------------------
// CTargetAMD64
//
CBackend* CTargetAMD64::GetBackend(ostream &out) const
{
  return new CBackendAMD64(out);
}
