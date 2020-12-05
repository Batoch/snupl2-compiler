//--------------------------------------------------------------------------------------------------
/// @brief SnuPL backend
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

#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cassert>

#include "backendAMD64.h"
using namespace std;

//#define DEBUG


//--------------------------------------------------------------------------------------------------
// AMD64 registers
//
struct {
  char const *n64, *n32, *n16, *n8;     //          Calling convention
} EAMD64RegisterName[NUMREGS] = {       //          Function      Save
  { "rax",  "eax",  "ax",   "al"   },   // rAX       ret.val    caller
  { "rcx",  "ecx",  "cx",   "cl"   },   // rCX       arg #4     caller
  { "rdx",  "edx",  "dx",   "dl"   },   // rDX       arg #3     caller
  { "rbx",  "ebx",  "bx",   "bl"   },   // rBX                  callee
  { "rsi",  "esi",  "si",   NULL   },   // rSI       arg #2     caller
  { "rdi",  "edi",  "di",   NULL   },   // rDI       arg #1     caller
  { "rsp",  "esp",  "sp",   NULL   },   // rSP      stack ptr
  { "rbp",  "ebp",  "bp",   NULL   },   // rBP                  callee
  { "r8",   "r8d",  "r8w",  "r8b"  },   // r8        arg #5     caller
  { "r9",   "r9d",  "r9w",  "r9b"  },   // r9        arg #6     caller
  { "r10",  "r10d", "r10w", "r10b" },   // r10                  caller
  { "r11",  "r11d", "r11w", "r11b" },   // r11                  caller
  { "r12",  "r12d", "r12w", "r12b" },   // r12                  callee
  { "r13",  "r13d", "r13w", "r13b" },   // r13                  callee
  { "r14",  "r14d", "r14w", "r14b" },   // r14                  callee
  { "r15",  "r15d", "r15w", "r15b" },   // r15                  callee
};

//--------------------------------------------------------------------------------------------------
// CBackendAMD64
//
CBackendAMD64::CBackendAMD64(ostream &out)
  : CBackend(out), _curr_scope(NULL)
{
  _ind = string(4, ' ');
}

CBackendAMD64::~CBackendAMD64(void)
{
}

void CBackendAMD64::EmitHeader(void)
{
  _out << "##################################################" << endl
       << "# " << _m->GetName() << endl
       << "#" << endl
       << endl;
}

void CBackendAMD64::EmitCode(void)
{
  _out << _ind << "#-----------------------------------------" << endl
       << _ind << "# text section" << endl
       << _ind << "#" << endl
       << _ind << ".text" << endl
       << _ind << ".align 8" << endl
       << endl
       << _ind << "# entry point" << endl
       << _ind << ".global main" << endl
       << endl
       << _ind << "# external subroutines" << endl;

  // emit external function declarations
  CSymtab *st = _m->GetSymbolTable(); assert(st != NULL);
  vector<CSymbol*> sym = st->GetSymbols();
  auto symit = sym.cbegin();
  while (symit != sym.cend()) {
    CSymbol *s = *symit++;
    if (s->GetSymbolType() == stProcedure) {
      CSymProc *p = dynamic_cast<CSymProc*>(s); assert(p != NULL);

      if (p->IsExternal()) {
        _out << _ind << ".extern " << p->GetName() << endl;
      }
    }
  }

  _out << endl
       << endl;

  // emit scope & subscopes
  // TODO

  _out << _ind << "# end of text section" << endl
       << _ind << "#-----------------------------------------" << endl
       << endl;
}

void CBackendAMD64::EmitData(void)
{
  _out << _ind << "#-----------------------------------------" << endl
       << _ind << "# global data section" << endl
       << _ind << "#" << endl
       << _ind << ".data" << endl
       << _ind << ".align 8" << endl
       << endl;

  EmitGlobalData(_m);

  _out << _ind << "# end of global data section" << endl
       << _ind << "#-----------------------------------------" << endl
       << endl;
}

void CBackendAMD64::EmitFooter(void)
{
  _out << _ind << ".end" << endl
       << "##################################################" << endl;
}

void CBackendAMD64::SetScope(CScope *scope)
{
  _curr_scope = scope;
}

CScope* CBackendAMD64::GetScope(void) const
{
  return _curr_scope;
}

void CBackendAMD64::EmitScope(CScope *scope)
{
  assert(scope != NULL);

  string label;

  if (scope->GetParent() == NULL) label = "main";
  else label = scope->GetName();

#ifdef DEBUG
  cout << endl << endl
       << "Encoding '" << label << "'..." << endl
       << endl;
#endif

  // label
  _out << _ind << "# scope " << scope->GetName() << endl
       << label << ":" << endl;

  //
  // TODO
  //

  // 1. compute the size of locals
  StackFrame paf = {
    .return_address   = 8,        // 1 * 8
    .saved_registers  = 0*8,      // number of saved registers * 8
    .padding          = 0,
    .saved_parameters = 0,
    .local_variables  = 0,
    .argument_build   = 0,
    .size             = 0
  };
  ComputeStackOffsets(scope, paf);

  // 2. emit function prologue
  //    - store saved registrs
  //    - adjust stack pointer to make room for PAF
  //    - save parameters to stack (not necessary if we do register allocation)
  //    - set argument build & local variable area to 0
  //    - initialize local arrays (EmitLocalData)

  // 3. emit code

  // 4. emit function epilogue

  _out << endl;
}

void CBackendAMD64::EmitGlobalData(CScope *scope)
{
  assert(scope != NULL);

  // emit the globals for the current scope
  CSymtab *st = scope->GetSymbolTable();
  assert(st != NULL);

  bool header = false;

  vector<CSymbol*> slist = st->GetSymbols();

  _out << dec;

  size_t size = 0;

  for (size_t i=0; i<slist.size(); i++) {
    CSymbol *s = slist[i];

    // filter out constant symbols
    if (dynamic_cast<CSymConstant*>(s) != NULL) continue;

    const CType *t = s->GetDataType();

    if (s->GetSymbolType() == stGlobal) {
      if (!header) {
        _out << _ind << "# scope: " << scope->GetName() << endl;
        header = true;
      }

      // insert alignment only when necessary
      if ((t->GetAlign() > 1) && (size % t->GetAlign() != 0)) {
        size += t->GetAlign() - size % t->GetAlign();
        _out << setw(4) << " " << ".align "
             << right << setw(3) << t->GetAlign() << endl;
      }

      _out << left << setw(36) << s->GetName() + ":" << "# " << t << endl;

      if (t->IsArray()) {
        const CArrayType *a = dynamic_cast<const CArrayType*>(t);
        assert(a != NULL);
        int dim = a->GetNDim();

        _out << setw(4) << " "
          << ".long " << right << setw(4) << dim << endl;

        for (int d=0; d<dim; d++) {
          assert(a != NULL);

          _out << setw(4) << " "
            << ".long " << right << setw(4) << a->GetNElem() << endl;

          a = dynamic_cast<const CArrayType*>(a->GetInnerType());
        }
      }

      const CDataInitializer *di = s->GetData();
      if (di != NULL) {
        const CDataInitString *sdi = dynamic_cast<const CDataInitString*>(di);
        assert(sdi != NULL);  // only support string data initializers for now

       //cout << "data initializer: " << sdi->GetData() << endl;
        _out << left << setw(4) << " "
          << ".asciz " << '"' << sdi->GetData() << '"' << endl;
      } else {
        _out  << left << setw(4) << " "
          << ".skip " << dec << right << setw(4) << t->GetDataSize()
          << endl;
      }

      size += t->GetSize();
    }
  }

  _out << endl;

  // emit globals in subscopes (necessary if we support static local variables)
  vector<CScope*>::const_iterator sit = scope->GetSubscopes().begin();
  while (sit != scope->GetSubscopes().end()) EmitGlobalData(*sit++);
}

void CBackendAMD64::EmitLocalData(CScope *scope)
{
  assert(scope != NULL);

  //
  // TODO
  // emit the variables for the current scope
  //
}

void CBackendAMD64::EmitCodeBlock(CCodeBlock *cb, StackFrame &paf)
{
  assert(cb != NULL);

  const list<CTacInstr*> &instr = cb->GetInstr();
  list<CTacInstr*>::const_iterator it = instr.begin();

  while (it != instr.end()) EmitInstruction(*it++, paf);
}

void CBackendAMD64::EmitInstruction(CTacInstr *i, StackFrame &paf)
{
  assert(i != NULL);

  ostringstream cmt;
  string mnm, mnm2, mnmpf;
  cmt << i;

  EOperation op = i->GetOperation();

  switch (op) {
    // binary operators
    // dst = src1 op src2

    // unary operators
    // dst = src1

    // memory operations
    // dst = src1

    // pointer operations
    // dst = &src1
    // dst = *src1

    // unconditional branching
    // goto dst

    // conditional branching
    // if src1 relOp src2 then goto dst

    // function call-related operations

    // special
    case opLabel:
      _out << Label(dynamic_cast<CTacLabel*>(i)) << ":" << endl;
      break;

    case opNop:
      EmitInstruction("nop", "", cmt.str());
      break;


    default:
      EmitInstruction("# ???", "not implemented", cmt.str());
  }
}

void CBackendAMD64::EmitInstruction(string mnemonic, string args, string comment)
{
  // goes to some lengths to avoid trailing spaces
  bool hasArgs    = args != "";
  bool hasComment = comment != "";

  _out << left << _ind << setw(hasArgs || hasComment ? 7 : 0)
       << mnemonic
       << (hasArgs || hasComment ? " " : "")
       << setw(hasComment ? 23 : 0) << args;
  if (hasComment) _out << " # " << comment;
  _out << endl;
}

void CBackendAMD64::Load(EAMD64Register dst, CTacAddr *src, string comment)
{
  assert(src != NULL);
  int size = OperandSize(src);
  string mnm = "mov", mod, reg = Reg(dst); //, size);

  // set operator modifier based on operand size
  switch (size) {
    case 1: mod = "zbq"; break;
    case 2: mod = "zwq"; break;
    case 4: mod = "slq"; break;
    case 8: mod = "q"; break;
    default: SetError("Data type not supported by this backend.");
  }

  // emit a load instruction
  EmitInstruction(mnm + mod, Operand(src) + ", " + reg, comment);
}

void CBackendAMD64::Store(CTac *dst, EAMD64Register src, string comment)
{
  assert(dst != NULL);
  int size = OperandSize(dst);
  string mod, reg = Reg(src, size);

  // set operator modifier based on operand size
  switch (size) {
    case 1: mod = "b"; break;
    case 2: mod = "w"; break;
    case 4: mod = "l"; break;
    case 8: mod = "q"; break;
    default: SetError("Data type not supported by this backend.");
  }

  // emit a store instruction
  EmitInstruction("mov" + mod, reg + ", " + Operand(dst), comment);
}

string CBackendAMD64::Operand(const CTac *op)
{
  //
  // TODO
  // return a string representing op
  // Hint: references (op of type CTacReference) require special care

  return "?";
}

string CBackendAMD64::Imm(long long value) const
{
  ostringstream o;
  o << "$" << dec << value;
  return o.str();
}

string CBackendAMD64::Label(const CTacLabel* label) const
{
  CScope *cs = GetScope();
  assert(cs != NULL);

  return "l_" + cs->GetName() + "_" + label->GetLabel();
}

string CBackendAMD64::Label(string label) const
{
  CScope *cs = GetScope();
  assert(cs != NULL);

  return "l_" + cs->GetName() + "_" + label;
}

string CBackendAMD64::Condition(EOperation cond) const
{
  //
  // TODO
  // return condition postfix in dependence of cond
  //
  return "?";
}

int CBackendAMD64::OperandSize(CTac *t) const
{
  // TODO
  // compute the size for operand t of type CTacName
  // Hint: also here references (incl. references to pointers) and arrays need special care.
  //       Compare your output to that of the reference implementation if you are unsure.
  //
  return 0;
}

string CBackendAMD64::Location(const CSymbol *s, long long ofs)
{
  //
  // TODO
  // return a string denoting the location of a symbol
  //
  return "?";
}

string CBackendAMD64::Reg(EAMD64Register reg, int size)
{
	//
	// return the full register name for base register @a base and a given data @a size
	// Hint: this is a simple lookup into EAMD64RegisterName
	//

	switch (size)
	{
	case 8:
	{
		return EAMD64RegisterName[reg].n8;
	}
	case 16:
	{
		return EAMD64RegisterName[reg].n16;
	}
	case 32:
	{
		return EAMD64RegisterName[reg].n32;
	}
	case 64:
	{
		return EAMD64RegisterName[reg].n64;
	}
	default:
		return "%?";
	}
}


void CBackendAMD64::ComputeStackOffsets(CScope *scope, StackFrame &paf)
{
  //
  // TODO
  // compute the location of local variables, temporaries and arguments on the stack
  //
  // Hint: this is the most complicated part of this phase. The reference implementation is
  // ~270 lines long (~100 of which are comments or empty lines)
  //
  // Study the details of the AMD64 PAF, then decide whether you are going to use base pointer-based
  // or stack pointer-based addressing. Depending on that, addressing of variables is relative to 
  // the base pointer (rbp) or the stack pointer (rsp).
  // Iterate over all variables in this scope (locals, parameters), compute their locations, then 
  // store that location in the symbol (CSymbol->SetLocation).
  //
  // The outputs of this function are
  // - the sizes of the differnt section in the PAF (store in provided 'paf' parameter)
  // - CStorage() class instances assigned to all local variables and parameters that indicate
  //   the location of the variable/parameter on the stack
  //
}
