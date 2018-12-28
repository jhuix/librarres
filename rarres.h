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

#ifndef _RARRES_INCLUDE_
#define _RARRES_INCLUDE_

#include "librarres.h"
#include <map>

namespace RARRES {

#if !defined(DISALLOW_COPY_AND_ASSIGN)

  // A macro to disallow the copy constructor and operator= functions
  // This should be used in the private: declarations for a class
#define DISALLOW_COPY_AND_ASSIGN(T) \
  T(const T&);                      \
  void operator=(const T&)

#endif  // !DISALLOW_COPY_AND_ASSIGN
  
  struct RES_FILEHEADER : BlockHeader {
    int64 Pos;
    int64 PackSize;
    int64 UnpSize;
    std::string FileName;
  };

  class CRarRes : public IRarRes {
  public:
    explicit CRarRes();
    ~CRarRes();

    virtual void Release();
    virtual bool Load(const char* filename);
    virtual bool Load(const wchar* filename);
    virtual void* LoadResource(const char* id, char** buf, size_t& bufsize);
    virtual void* LoadResource(const wchar* id, char** buf, size_t& bufsize);
    virtual void FreeResource(void* res);
    virtual int GetErrorCode();
    virtual void Clear();
#ifdef _WIN32
    virtual IStream* LoadResource(const char* id);
    virtual IStream* LoadResource(const wchar* id);
#endif

  protected:
    bool CheckUnpVer();
    bool ListFiles();
    void ListFileHeader(FileHeader &hd);

    CommandData cmd_;
    Archive arc_;
    ComprDataIO dio_;
    Unpack* unp_;
    std::map<std::string, RES_FILEHEADER* > fileheaders_;

  private:
    unsigned int  flags_;
    int64 total_packsize_, total_unpsize_;
    DISALLOW_COPY_AND_ASSIGN(CRarRes);
  };
};

#endif  //_RARRES_INCLUDE_