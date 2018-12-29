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

#include "rar.hpp"
#include "librespak.h"
#include "librarres.h"

#ifdef RESPAK
namespace RARRES {

  bool PASCAL ExtractFileA(const char* srcfile, const char* filename, const char* destfile) {
    JRES::IRes* rr = JRES::CreateRarRes(true);
    if (!rr || !rr->Load(srcfile, '\\'))
      return false;

    bool result = false;
    char* buf = nullptr;
    size_t size = 0;
    void* res = rr->LoadResource(filename, &buf, size);
    if (buf) {
      File dest;
      wchar_t dest_file[NM];
      CharToWide(destfile, dest_file, ASIZE(dest_file));
      if (dest.Create(dest_file, FMF_SHAREREAD | FMF_UPDATE)) {
        dest.Seek(0, SEEK_SET);
        result = dest.Write(buf, size);
      }
      dest.Close();
    }
    rr->FreeResource(res);
    rr->Release();
    return result;
  }

  bool PASCAL ExtractFileW(const wchar_t* srcfile, const wchar_t* filename, const wchar_t* destfile) {
    JRES::IRes* rr = JRES::CreateRarRes(true);
    if (!rr || !rr->Load(srcfile, L'\\'))
      return false;

    bool result = false;
    char* buf = nullptr;
    size_t size = 0;
    void* res = rr->LoadResource(filename, &buf, size);
    if (buf) {
      File dest;
      if (dest.Create(destfile, FMF_SHAREREAD | FMF_UPDATE)) {
        dest.Seek(0, SEEK_SET);
        result = dest.Write(buf, size);
      }
      dest.Close();
    }
    rr->FreeResource(res);
    rr->Release();
    return result;
  }

};
#endif
