//--------------------------------------------------------------------------------------------------
/// @brief SnuPL AMD64 backend
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

#ifndef __SnuPL_BACKEND_AMD64_H__
#define __SnuPL_BACKEND_AMD64_H__

#include "backend.h"

using namespace std;

//--------------------------------------------------------------------------------------------------
/// @brief AMD64 registers
///
enum EAMD64Register {
  r0  = 0, rAX = 0,                 ///< rax
  r1  = 1, rCX = 1,                 ///< rcx
  r2  = 2, rDX = 2,                 ///< rdx
  r3  = 3, rBX = 3,                 ///< rbx
  r4  = 4, rSI = 4,                 ///< rsi
  r5  = 5, rDI = 5,                 ///< rdi
  r6  = 6, rSP = 6,                 ///< rsp
  r7  = 7, rBP = 7,                 ///< rbp
  r8  = 8,                          ///< r8
  r9  = 9,                          ///< r9
  r10 = 10,                         ///< r10
  r11 = 11,                         ///< r11
  r12 = 12,                         ///< r12
  r13 = 13,                         ///< r13
  r14 = 14,                         ///< r14
  r15 = 15,                         ///< r15
  NUMREGS = 16
};

//--------------------------------------------------------------------------------------------------
/// @brief stack frame
///
typedef struct {
  size_t return_address;            ///< size of return address
  size_t saved_registers;           ///< size of saved registers
  size_t padding;                   ///< size of padding
  size_t saved_parameters;          ///< size of saved parameters
  size_t local_variables;           ///< size of local variables
  size_t argument_build;            ///< size of argument build area
  size_t size;                      ///< total size
  vector<CTacTemp*> argbuild;       ///< CTacTemp pointing to argument build area
} StackFrame;

//--------------------------------------------------------------------------------------------------
/// @brief AMD64 backend
///
/// backend for Intel 64-bit (i686)
///
class CBackendAMD64 : public CBackend {
  public:
    /// @name constructors/destructors
    /// @{

    CBackendAMD64(ostream &out);
    virtual ~CBackendAMD64(void);

    /// @}

  protected:
    /// @name detailed output methods
    /// @{

    virtual void EmitHeader(void);
    virtual void EmitCode(void);
    virtual void EmitData(void);
    virtual void EmitFooter(void);

    /// @}

    /// @name additional methods
    /// @{

    /// @brief set the current scope
    void SetScope(CScope *scope);

    /// @brief get the current scope
    CScope* GetScope(void) const;

    /// @brief emit a scope
    virtual void EmitScope(CScope *scope);

    /// @brief emit global data
    virtual void EmitGlobalData(CScope *s);

    /// @brief emit local data
    ///
    /// EmitLocalData() initializes local data (i.e., arrays)
    virtual void EmitLocalData(CScope *s);

    /// @brief emit code for code block @a cb and stack frame @a paf
    virtual void EmitCodeBlock(CCodeBlock *cb, StackFrame &paf);

    /// @brief emit instruction @a i and stack frame @a paf
    virtual void EmitInstruction(CTacInstr *i, StackFrame &paf);

    /// @brief emit an instruction

    virtual void EmitInstruction(string mnemonic, string args="",
                                 string comment="");

    /// @brief emit a load instruction
    void Load(EAMD64Register dst, CTacAddr *src, string comment="");

    /// @brief emit a store instruction
    void Store(CTac *dst, EAMD64Register src, string comment="");

    /// @brief return an operand string for @a op
    /// @param op the operand
    string Operand(const CTac *op);

    /// @brief return an immediate for @a value
    string Imm(int value) const;

    /// @brief return a x86-label for CTaclabel @a label
    string Label(const CTacLabel *label) const;

    /// @brief return a x86-label for string @a label
    string Label(string label) const;

    /// @brief return the condition suffix for a binary comparison operation
    string Condition(EOperation cond) const;

    /// @brief compute the size of operator @t
    int OperandSize(CTac *t) const;

    /// @brief return a string denoting the location of a symbol
    /// @param s the symbol
    /// @param ofs optional offset to add/subtract from location
    string Location(const CSymbol *s, long long ofs=0);

    /// @brief return the full register name for base register @a base and a given data @a size
    /// @param reg register
    /// @param size size of data type in bytes
    string Reg(EAMD64Register reg, int size=8);

    /// @brief compute the location of local variables, temporaries and arguments on the stack
    /// @param scope scope
    /// @param paf [in/out] StackFrame (return_address and saved_register must be set)
    void ComputeStackOffsets(CScope *scope, StackFrame &paf);

    /// @}

    string _ind;                    ///< indentation
    CScope *_curr_scope;            ///< current scope
};


#endif // __SnuPL_BACKEND_AMD64_H__
