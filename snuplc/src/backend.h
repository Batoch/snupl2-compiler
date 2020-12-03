//--------------------------------------------------------------------------------------------------
/// @brief SnuPL backend
/// @author Bernhard Egger <bernhard@csap.snu.ac.kr>
/// @section changelog Change Log
/// 2012/11/28 Bernhard Egger created
/// 2013/06/09 Bernhard Egger adapted to SnuPL/0
/// 2016/04/04 Bernhard Egger adapted to SnuPL/1
/// 2020/08/04 Bernhard Egger split backends into separate files, add error reporting functionality
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

#ifndef __SnuPL_BACKEND_H__
#define __SnuPL_BACKEND_H__

#include <iostream>
#include <vector>

#include "symtab.h"
#include "ir.h"

using namespace std;

//--------------------------------------------------------------------------------------------------
/// @brief backend base class
///
/// base class for backends
///

class CBackend {
  public:
    /// @name constructors/destructors
    /// @{

    CBackend(ostream &out);
    virtual ~CBackend(void);

    /// @}

    /// @name output method
    /// @{

    virtual bool Emit(CModule *m);

    /// @}

    /// @name error handling
    ///@{

    /// @brief indicates whether there was an error while emitting the assembly code
    /// @retval true if there was an error
    /// @retval false otherwise
    bool HasError(void) const { return _abort; };

    /// @brief returns a human-readable error message
    /// @retval error message
    string GetErrorMessage(void) const;
    ///@}

  protected:
    /// @name detailed output methods
    /// @{

    virtual void EmitHeader(void);
    virtual void EmitCode(void);
    virtual void EmitData(void);
    virtual void EmitFooter(void);

    /// @}

    /// @brief sets the token causing an error along with a message
    /// @param message human-readable error message
    void SetError(const string message);


    CModule *_m;                    ///< module
    ostream &_out;                  ///< output stream

    /// @name error handling
    string        _message;       ///< error message
    bool          _abort;         ///< error flag
};


#endif // __SnuPL_BACKEND_H__
