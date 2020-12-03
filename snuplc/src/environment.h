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

#ifndef __SnuPL_ENVIRONMENT_H__
#define __SnuPL_ENVIRONMENT_H__

#include <iostream>
#include <map>
#include <vector>

#include "target.h"
using namespace std;

/// @brief parameter types
enum EParmType {
  ptFlag,         ///< on/off flag
  ptSwitch,       ///< switch (set if given, no option to turn off)
  ptSetting,      ///< setting requiring an additional argument
  ptTarget,       ///< target setting (next argument indicates target)
};


//--------------------------------------------------------------------------------------------------
/// @brief environment class
///
/// singleton class provinging compiler and other environmental settings during 
/// the compilation process.
///
class CEnvironment {
  friend void RegisterTargets(CEnvironment*);  // from target.cpp

  public:
    /// @brief return the global type manager
    static CEnvironment* Get(void);


    /// @name command line argument parsing and help
    /// @{

    /// @brief print syntax, optional message @a msg, and exit program
    /// @param msg informational/error message to be printed along with syntax
    void Syntax(string msg);


    /// @brief parse command line arguments
    /// @parm argc number of arguments
    /// @parm argv argument aray
    void ParseArguments(int argc, char *argv[]);

    /// @e}



    /// @name configuration store
    /// @{

    bool GetFlag(const string key, bool &value) const;
    bool GetSwitch(const string key, bool &value) const;
    bool GetSetting(const string key, string &value) const;
    bool GetConfig(const string key, string &value) const;


    /// @}


    /// @name target management
    /// @{

    /// @brief set the target architecture to @a key
    /// @param key target
    /// @retval true if target was found and has been set
    /// @retval false target @a key not available
    bool SetTarget(const string key);

    /// @brief return the currently active target
    /// @retval CTarget* active target
    CTarget* GetTarget(void) const;

    /// @}


    /// @name file management
    /// @{

    /// @brief add a @a file to the list of files to be compiled
    /// @param file file to compile
    void AddFile(string file);

    /// @brief next file to be compiled. Returns the empty string if
    ///        no further files are to be compiled.
    string GetNextFile(void);

    /// @}


    /// @brief print all types to an output stream
    ///
    /// @param out output stream
    /// @param indent indentation
    ostream& print(ostream &out, int indent=0) const;

  private:
    /// @name constructor/destructor
    /// @{

    CEnvironment(void);
    virtual ~CEnvironment(void);

    /// @}

    /// @brief initialize configuration with defaut values
    void InitDefaultConfig(void);

    /// @brief add a target to the list of supported targets
    /// @parm t target
    /// @parm active if true register @t as default target
    void AddTarget(CTarget *t, bool active=false);


    map<string,
      tuple<EParmType, const char*, string>> _config; ///< configuration
    map<string, CTarget*>  _target; ///< supported targets
    CTarget*        _active_target; ///< active target
    vector<string>         _files;  ///< files to compile
    string            _active_file; ///< active file (currently being compiled)

    static CEnvironment  *_globenv; ///< global CEnvironment instance
};

/// @name CEnvironment output operators
/// @{

/// @brief CEnvironment output operator
///
/// @param out output stream
/// @param e reference to CEnvironment
/// @retval output stream
ostream& operator<<(ostream &out, const CEnvironment &e);

/// @brief CEnvironment output operator
///
/// @param out output stream
/// @param e reference to CEnvironment
/// @retval output stream
ostream& operator<<(ostream &out, const CEnvironment *e);

/// @}



#endif // __SnuPL_ENVIRONMENT_H_
