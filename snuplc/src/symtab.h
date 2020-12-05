//--------------------------------------------------------------------------------------------------
/// @brief SnuPL symbol table
/// @author Bernhard Egger <bernhard@csap.snu.ac.kr>
/// @section changelog Change Log
/// 2012/09/14 Bernhard Egger created
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

#ifndef __SnuPL_SYMTAB_H__
#define __SnuPL_SYMTAB_H__

#include <iostream>
#include <map>
#include <vector>

#include "data.h"
#include "type.h"
using namespace std;

//--------------------------------------------------------------------------------------------------
/// @brief SnuPL symbol storage location types
///
enum EStorageLocation {
  slUndefined,      ///< undefined
  slMemoryAbs,      ///< absolute memory address: location = mem[offset]
  slMemoryRel,      ///< absolute memory address: location = mem[rf[register] + offset]
  slRegister,       ///< register:                location = rf[register]
  slLabel,          ///< named label:             location = label (resolved by assembler)
};


//--------------------------------------------------------------------------------------------------
/// @brief SnuPL storage location class
///
/// class to manage different storage locations
///
class CStorage {
  public:
    /// @name constructor/destructor
    /// @{

    /// @brief constructor
    /// @param location storage location
    /// @param base     label or base register
    /// @param offset   absolute address or offset to base
    CStorage(EStorageLocation location, const string base, long long offset);

    /// @brief destructor
    virtual ~CStorage(void);

    /// @}


    /// @name properties
    /// @{

    /// @brief return storage location
    EStorageLocation GetLocation(void) const;

    /// @brief return storage base
    string GetBase(void) const;

    /// @brief return storage offset
    long long GetOffset(void) const;

    /// @}


    /// @brief print the storage to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual ostream&  print(ostream &out, int indent=0) const;

  private:
    EStorageLocation _location;   ///< storage location
    string         _base;         ///< base
    long long      _offset;       ///< offset

};

/// @name CStorage output operators
/// @{

/// @brief CStorage output operator
///
/// @param out output stream
/// @param s reference to CStorage
/// @retval output stream
ostream& operator<<(ostream &out, const CStorage &s);

/// @brief CStorage output operator
///
/// @param out output stream
/// @param s reference to CStorage
/// @retval output stream
ostream& operator<<(ostream &out, const CStorage *s);

/// @}


//--------------------------------------------------------------------------------------------------
/// @brief SnuPL symbol types
///
enum ESymbolType {
  stGlobal,         ///< global symbol
  stLocal,          ///< local symbol
  stParam,          ///< parameter symbol
  stProcedure,      ///< procedure symbol
  stConstant,       ///< constant symbol
  stReserved,       ///< reserved names
};

class CSymtab;

//--------------------------------------------------------------------------------------------------
/// @brief SnuPL symbol
///
/// base class for all symbols
///
class CSymbol {
  friend class CSymtab;

  public:
    /// @name constructor/destructor
    /// @{

    /// @brief constructor
    ///
    /// @param name symbol name (identifier)
    /// @param symboltype symbol type
    /// @param datatype data type of the symbol
    CSymbol(const string name, ESymbolType symboltype, const CType *datatype);

    /// @brief destructor
    virtual ~CSymbol(void);

    /// @}

    /// @name symbol handling
    /// @{

    /// @brief return the symbol's identifier
    /// @retval string name
    string GetName(void) const;

    /// @brief return the symbol's type
    /// @retval ESymbolType symbol type
    ESymbolType GetSymbolType(void) const;

    /// @brief set the symbol's type
    /// @param datatype symbol type
    void SetDataType(const CType *datatype);

    /// @brief return the symbol's type
    /// @retval CType symbol type
    const CType* GetDataType(void) const;

    /// @brief return the symbol table of this symbol
    /// @retval CSymtab symbol table
    CSymtab* GetSymbolTable(void) const;

    /// @}

    /// @name data handling
    /// @{

    /// @brief set the symbol's value (for initialized symbols)
    /// @param data data initializer
    virtual void SetData(CDataInitializer *data);

    /// @brief return the symbol's data initializer
    /// @retval CDataInitializer data initializer
    virtual const CDataInitializer* GetData(void) const;

    /// @}

    /// @name storage location
    /// @{

    /// @brief set storage location for this symbol
    /// @param location CStorage instance
    void SetLocation(CStorage *location);

    /// @brief get storage of this symbol
    const CStorage* GetLocation(void) const;

    /// @}

    /// @brief print the symbol to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual ostream&  print(ostream &out, int indent=0) const;

  private:
    /// @name symbol handling
    /// @{

    /// @brief set the symbol table owning this symbol
    /// @param symtab symbol table
    void SetSymbolTable(CSymtab *symtab);

    /// @}

    CSymtab       *_symtab;       ///< symbol table owning this symbol
    string         _name;         ///< name
    ESymbolType    _symboltype;   ///< symbol type
    const CType   *_datatype;     ///< data type
    CStorage      *_location;     ///< storage location

  protected:
    const CDataInitializer *_data;///< data initializer
};

/// @name CSymbol output operators
/// @{

/// @brief CSymbol output operator
///
/// @param out output stream
/// @param t reference to CSymbol
/// @retval output stream
ostream& operator<<(ostream &out, const CSymbol &t);

/// @brief CSymbol output operator
///
/// @param out output stream
/// @param t reference to CSymbol
/// @retval output stream
ostream& operator<<(ostream &out, const CSymbol *t);

/// @}


//--------------------------------------------------------------------------------------------------
/// @brief global variable
///
/// class representing global variables
///
class CSymGlobal : public CSymbol {
  public:
    /// @name constructor/destructor
    /// @{

    /// @brief constructor
    ///
    /// @param name symbol name (identifier)
    /// @param type symbol type
    CSymGlobal(const string name, const CType *type);

    /// @}

    /// @brief print the symbol to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual ostream&  print(ostream &out, int indent=0) const;

  private:
};


//--------------------------------------------------------------------------------------------------
/// @brief local variable
///
/// class representing local (i.e., stack-allocated) variables
///
class CSymLocal : public CSymbol {
  public:
    /// @name constructor/destructor
    /// @{

    /// @brief constructor
    ///
    /// @param name symbol name (identifier)
    /// @param type symbol type
    CSymLocal(const string name, const CType *type);

    /// @}

    /// @name property handling
    /// @{

    /// @}

    /// @brief print the symbol to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual ostream&  print(ostream &out, int indent=0) const;

  protected:
    /// @name constructor/destructor
    /// @{

    /// @brief private constructor invoked by CSymParam
    ///
    /// @param name symbol name (identifier)
    /// @param stype symbol type
    /// @param type data type
    CSymLocal(const string name, ESymbolType stype, const CType *type);

    /// @}
};


//--------------------------------------------------------------------------------------------------
/// @brief procedure parameter
///
/// class representing procedure/function parameters
///
class CSymParam : public CSymLocal {
  public:
    /// @name constructor/destructor
    /// @{

    /// @brief constructor
    ///
    /// @param index parameter index (starting at 0)
    /// @param name symbol name (identifier)
    /// @param type symbol type
    CSymParam(int index, const string name, const CType *type);

    /// @}

    /// @name property handling
    /// @{

    /// @brief return the index of the paramter (0-based index)
    int GetIndex(void) const;

    /// @}

    /// @brief print the symbol to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual ostream&  print(ostream &out, int indent=0) const;

  private:
    int            _index;        ///< index
};


//--------------------------------------------------------------------------------------------------
/// @brief procedure symbol
///
/// class representing a procedure/function symbol
///
class CSymProc : public CSymbol {
  public:
    /// @name constructor/destructor
    /// @{

    /// @brief constructor
    ///
    /// @param name procedure/function identifier
    /// @param return_type return type (NULL if none)
    /// @param external indicate if the subroutine is defined externally
    CSymProc(const string name, const CType *return_type, bool external=false);

    /// @}


    /// @name property handling
    /// @{

    /// @brief set if a subroutine is defined externally
    /// @param external flag
    void SetExternal(bool external);

    /// @brief check if a subroutine is defined externally
    /// @retval true define externally
    /// @retval false otherwise
    bool IsExternal(void) const;

    /// @brief add a parameter
    /// @param param parameter
    void AddParam(CSymParam *param);

    /// @brief return the number of parameters
    /// @retval unsigned int number of parameters
    unsigned int GetNParams(void) const;

    /// @brief return the @a index-th parameters
    /// @retval CSymParam* parameter
    const CSymParam* GetParam(unsigned int index) const;

    /// @}


    /// @brief print the symbol to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual ostream&  print(ostream &out, int indent=0) const;

  private:
    bool _external;                 ///< external flag
    vector<CSymParam*> _param;      ///< parameter list
};


//--------------------------------------------------------------------------------------------------
/// @brief constant variable
///
/// class representing constant variables
///
class CSymConstant : public CSymbol {
  public:
    /// @name constructor/destructor
    /// @{

    /// @brief constructor
    ///
    /// @param name symbol name (identifier)
    /// @param type symbol type
    /// @param data constant value
    CSymConstant(const string name, const CType *type, 
                 const CDataInitializer *data);

    /// @}

    /// @brief print the symbol to an output stream
    /// @param out output stream
    /// @param indent indentation
    virtual ostream&  print(ostream &out, int indent=0) const;

  private:
};


//--------------------------------------------------------------------------------------------------
/// @brief search scope enumeration
///
enum EScope {
  sLocal,                         ///< local
  sGlobal,                        ///< global
};

//--------------------------------------------------------------------------------------------------
/// @brief SnuPL symbol table
///
/// hierarchical symbol table
///
class CSymtab {
  public:
    /// @name constructor/destructor
    /// @{

    /// @brief constructor for top-level symbol table
    CSymtab(void);

    /// @brief constructor for subordinate symbol tables
    /// @param parent superordinate symbol table (must not be NULL)
    CSymtab(CSymtab *parent);

    /// @brief destructor
    virtual ~CSymtab(void);

    ///@}


    /// @name scope handling
    /// @{

    /// @brief return parent symbol table
    /// @retval CSymtab* parent symbol table
    /// @retval NULL if this instance is the global symbol table
    CSymtab* GetParent(void) const;

    /// @}


    /// @name symbol handling
    /// @{

    /// @brief add a symbol to the symbol table
    /// @retval true if the symbol was inserted successfully
    /// @retval false if such a symbol already exists in the local symbol table
    bool AddSymbol(CSymbol *s);

    /// @brief return a symbol with a given name
    /// @param name symbol name (identifier)
    /// @param scope search scope (default: sGlobal)
    /// @retval CSymbol matching symbol or NULL if not found
    const CSymbol* FindSymbol(const string name, EScope scope=sGlobal) const;

    /// @brief return a list of all symbols
    vector<CSymbol*> GetSymbols(void) const;

    /// @}


    /// @brief print the symbol table to an output stream
    ///
    /// @param out output stream
    /// @param indent indentation
    ostream&  print(ostream &out, int indent=0) const;

  private:
    map<string, CSymbol*> _symtab;///< local symbol table
    CSymtab       *_parent;       ///< parent
};

/// @name CSymtab output operators
/// @{

/// @brief CSymtab output operator
///
/// @param out output stream
/// @param t reference to CSymtab
/// @retval output stream
ostream& operator<<(ostream &out, const CSymtab &t);

/// @brief CSymtab output operator
///
/// @param out output stream
/// @param t reference to CSymtab
/// @retval output stream
ostream& operator<<(ostream &out, const CSymtab *t);

/// @}


#endif // __SnuPL_SYMTAB_H__
