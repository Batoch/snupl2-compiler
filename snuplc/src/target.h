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

#ifndef __SnuPL_TARGET_H__
#define __SnuPL_TARGET_H__

#include <iostream>
#include <map>

#include "backend.h"
using namespace std;


//--------------------------------------------------------------------------------------------------
/// @brief SnuPL target architecture
///
/// target architecture
///
class CTarget {

  public:
    /// @name constructor/destructor
    /// @{

    CTarget(const string key, const string name,
            unsigned int machine_word_size);
    virtual ~CTarget(void);

    /// @}

    /// @name property querying
    /// @{

    /// @brief return the key (short name) of the target architecture
    string GetKey(void) const { return string(_key); }

    /// @brief return the descriptive name of the target architecture
    string GetName(void) const { return string(_name); }

    /// @brief return the machine word size (in bytes)
    unsigned int GetMachineWordSize(void) const { return _machine_word_size; }

    /// @brief return an instance of the target backend
    virtual CBackend* GetBackend(ostream &out) const {
      return NULL;
    }

    /// @brief return standard library
    virtual string GetStdLibrary(void) const {
      // filename: libsnupl.a
      return "snupl";
    }

    /// @}

    /// @brief print the target to an output stream
    ///
    /// @param out output stream
    /// @param indent indentation
    virtual ostream&  print(ostream &out, int indent=0) const;


  private:
    string         _key;          ///< null base type
    string         _name;         ///< null base type
    unsigned int   _machine_word_size; ///< machine word size (register size)
};

/// @name CTarget output operators
/// @{

/// @brief CTarget output operator
///
/// @param out output stream
/// @param t reference to CTarget
/// @retval output stream
ostream& operator<<(ostream &out, const CTarget &t);

/// @brief CTarget output operator
///
/// @param out output stream
/// @param t reference to CTarget
/// @retval output stream
ostream& operator<<(ostream &out, const CTarget *t);

/// @}


//--------------------------------------------------------------------------------------------------
/// @brief generic 32-bit target
///
/// generic 32-bit target
///
class CTarget32 : public CTarget {

  public:
    /// @name constructor/destructor
    /// @{

    CTarget32(void) : CTarget("32-bit", "Generic 32-bit Target", 4) { };

    /// @}
};


//--------------------------------------------------------------------------------------------------
/// @brief generic 64-bit target
///
/// generic 64-bit target
///
class CTarget64 : public CTarget {

  public:
    /// @name constructor/destructor
    /// @{

    CTarget64(void) : CTarget("64-bit", "Generic 64-bit Target", 8) { };

    /// @}
};

//--------------------------------------------------------------------------------------------------
/// @brief AMD64 target
///
/// AMD64 (x86_64) target architecture
///
class CTargetAMD64 : public CTarget {

  public:
    /// @name constructor/destructor
    /// @{

    CTargetAMD64(void) : CTarget("x86-64", "AMD64 (x86-64)", 8) { };

    /// @}

    /// @name property querying
    /// @{

    /// @brief return an instance of the target backend
    virtual CBackend* GetBackend(ostream &out) const;

    /// @}
};

/// @brief register available targets in CEnvironment instance @a e
/// @param e CEnvironment instance to register targets with
class CEnvironment;
void RegisterTargets(CEnvironment *e);


#endif // __SnuPL_TARGET_H__
