// Copyright (c) 2018-present, Jhuix (Hui Jin) <jhuix0117@gmail.com>
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or 
// without modification, are permitted provided that the 
// following conditions are met.
//
// Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// Redistributions in binary form must reproduce the above 
// copyright notice, this list of conditions and the following
// disclaimer in the documentation and/or other materials 
// provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
// CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef _JRES_INTERFACE_INCLUDE_
#define _JRES_INTERFACE_INCLUDE_

#ifdef _WIN32
#include <objidl.h>
#endif

namespace JRES {

  struct IRes {
    virtual void Release() = 0;
    //path_sep value of 0 is default internal path separator
    virtual bool Open(const char* filename, char path_sep) = 0;
    virtual bool Open(const wchar_t* filename, wchar_t path_sep) = 0;
    virtual void* LoadResource(const char* id, char** buf, size_t& bufsize) = 0;
    virtual void* LoadResource(const wchar_t* id, char** buf, size_t& bufsize) = 0;
    virtual void FreeResource(void* res) = 0;
    virtual int GetErrorCode() = 0;
    virtual void Close() = 0;
#ifdef _WIN32
    virtual IStream* LoadResource(const char* id) = 0;
    virtual IStream* LoadResource(const wchar_t* id) = 0;
#endif
  };

};
#endif  //_JRES_INTERFACE_INCLUDE_
