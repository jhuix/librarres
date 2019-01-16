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
#include "rarres.h"
#include "VERSION"

namespace RARRES {

  CRarRes::CRarRes(bool ignorecase)
    : unp_(nullptr)
    , arc_(&cmd_)
    , flags_(0)
    , total_packsize_(0)
    , total_unpsize_(0)
    , ignorecase_(ignorecase) {
  }

  CRarRes::~CRarRes() {
    Close();
  }

  void CRarRes::Close() {
    if (unp_) {
      delete unp_;
      unp_ = nullptr;
    }
    arc_.Close();
    for (auto it : fileheadersW_) {
      if (it.second->Data) {
        free(it.second->Data);
        it.second->Data = nullptr;
      }
      delete it.second;
    }
    fileheadersW_.clear();
    fileheadersA_.clear();
    flags_ = 0;
    total_packsize_ = 0;
    total_unpsize_ = 0;
  }

  void CRarRes::Release() {
    delete this;
  }

  bool CRarRes::Open(const char* filename, char path_sep) {
    wchar_t FileName[NM];
    UtfToWide(filename, FileName, ASIZE(FileName));
    wchar_t sep = 0;
    ((char*)&sep)[0] = path_sep;
    return Open(FileName, sep);
  }

  bool CRarRes::Open(const wchar_t* filename, wchar_t path_sep) {
    Close();
    cmd_.Init();
    cmd_.AddArcName(filename);
    cmd_.Overwrite = OVERWRITE_ALL;
    cmd_.VersionControl = 1;
    cmd_.OpenShared = true;

    if (!arc_.Open(filename, FMF_OPENSHARED)) {
      ErrHandler.OpenErrorMsg(filename);
      return false;
    }
    if (!arc_.IsArchive(true)
      || arc_.GetHeaderType() != HEAD_MAIN) {
      arc_.Close();
      ErrHandler.OpenErrorMsg(filename);
      return false;
    }

    if (arc_.Volume)
      flags_ |= 0x01;
    if (arc_.MainComment)
      flags_ |= 0x02;
    if (arc_.Locked)
      flags_ |= 0x04;
    if (arc_.Solid)
      flags_ |= 0x08;
    if (arc_.NewNumbering)
      flags_ |= 0x10;
    if (arc_.Signed)
      flags_ |= 0x20;
    if (arc_.Protected)
      flags_ |= 0x40;
    if (arc_.Encrypted)
      flags_ |= 0x80;
    if (arc_.FirstVolume)
      flags_ |= 0x100;
    return ListFiles(path_sep);
  }

  bool CRarRes::CheckUnpVer()
  {
    bool WrongVer;
    if (arc_.Format == RARFMT50) // Both SFX and RAR can unpack RAR 5.0 archives.
      WrongVer = arc_.FileHead.UnpVer > VER_UNPACK5;
    else
    {
      // All formats since 1.3 for RAR.
      WrongVer = arc_.FileHead.UnpVer<13 || arc_.FileHead.UnpVer>VER_UNPACK;
    }

    // We can unpack stored files regardless of compression version field.
    if (arc_.FileHead.Method == 0)
      WrongVer = false;

    if (WrongVer)
    {
      ErrHandler.UnknownMethodMsg(arc_.FileName, arc_.FileHead.FileName);
      uiMsg(UIERROR_NEWERRAR, arc_.FileName);
    }
    return !WrongVer;
  }

  bool CRarRes::ListFiles(wchar_t path_sep) {
    if (!arc_.IsOpened())
      return false;

    uint FileCount = 0;
    wchar_t VolNumText[50];
    *VolNumText = 0;
    while (arc_.ReadHeader() > 0)
    {
      HEADER_TYPE HeaderType = arc_.GetHeaderType();
      switch (HeaderType) {
      case HEAD_FILE:
        ListFileHeader(arc_.FileHead, path_sep);
        if (!arc_.FileHead.SplitBefore)
        {
          total_unpsize_ += arc_.FileHead.UnpSize;
          FileCount++;
        }
        total_packsize_ += arc_.FileHead.PackSize;
        break;
      case HEAD_SERVICE:
        ListFileHeader(arc_.SubHead, path_sep);
        break;
      }
      arc_.SeekToNext();
    }
    return (bool)(FileCount > 0);
  }

  void CRarRes::ListFileHeader(FileHeader &hd, wchar_t path_sep) {
    if (!hd.Dir) {
      RARRES_FILEHEADER* rhd = new RARRES_FILEHEADER;
      *((BlockHeader*)rhd) = hd;
      rhd->Pos = arc_.CurBlockPos;
      rhd->PackSize = hd.PackSize < 0 ? 0 : hd.PackSize;
      rhd->UnpSize = hd.UnpSize < 0 ? 0 : hd.UnpSize;
      rhd->FileName = hd.FileName;
#ifdef _WIN32
      rhd->Mtime = hd.mtime.GetWin();
      rhd->Ctime = hd.ctime.GetWin();
#else
      rhd->Mtime = hd.mtime.GetUnixNS();
      rhd->Ctime = hd.ctime.GetUnixNS();
#endif
      rhd->FileAttr = hd.FileAttr;
      rhd->Data = nullptr;
      //Path sep default value is L'\\'
      if ((path_sep && path_sep != L'\\')) {
        wchar_t* ch = (wchar_t*)hd.FileName;
        while (*ch) {
          if (L'\\' == *ch) *ch = path_sep;
          ++ch;
        }
      }
      if (ignorecase_) {
        wchar_t* ch = (wchar_t*)hd.FileName;
        while (*ch) {
          if ((*ch) >= L'A' && (*ch) <= L'Z')
            *ch = (*ch) + 20;
          ++ch;
        }
      }
      fileheadersW_[hd.FileName] = rhd;
      char NameA[NM];
      WideToUtf(hd.FileName, NameA, ASIZE(NameA));
      fileheadersA_[NameA] = rhd;
    }
  }

  void* CRarRes::LoadResource(const char* id, char** buf, size_t& bufsize) {
    if (!arc_.IsOpened()) {
      ErrHandler.SetErrorCode(RARX_OPEN);
      return nullptr;
    }

    std::map<std::string, RARRES_FILEHEADER* >::iterator it = fileheadersA_.find(id);
    if (it == fileheadersA_.end()) {
      ErrHandler.SetErrorCode(RARX_NOFILES);
      return nullptr;
    }

    RARRES_FILEHEADER* rhd = it->second;
    if (NULL == rhd) {
      ErrHandler.SetErrorCode(RARX_NOFILES);
      return nullptr;
    }

    return Extract(rhd, buf, bufsize);
  }

  void* CRarRes::Extract(RARRES_FILEHEADER* rhd, char** buf, size_t& bufsize) {
    if (!buf || ((*buf) && rhd->UnpSize > (int64)bufsize)) {
      bufsize = (size_t)rhd->UnpSize;
      ErrHandler.SetErrorCode(RARX_SUCCESS);
      return nullptr;
    }

    dio_.UnpArcSize = arc_.FileLength();
    dio_.UnpVolume = false;
    arc_.Seek(rhd->Pos, SEEK_SET);

    void* result = nullptr;
    if (arc_.ReadHeader() > 0) {
      if (!CheckUnpVer()) {
        ErrHandler.SetErrorCode(RARX_FATAL);
        return nullptr;
      }

      if (!unp_) {
        unp_ = new Unpack(&dio_);
        if (!unp_) {
          ErrHandler.SetErrorCode(RARX_MEMORY);
          return nullptr;
        }
      }
      uint threads = GetNumberOfThreads();
      unp_->SetThreads(threads);
      dio_.UnpVolume = arc_.FileHead.SplitAfter;
      dio_.NextVolumeMissing = false;
      arc_.Seek(arc_.NextBlockPos - arc_.FileHead.PackSize, SEEK_SET);

      dio_.CurUnpRead = 0;
      dio_.CurUnpWrite = 0;
      dio_.UnpHash.Init(arc_.FileHead.FileHash.Type, threads);
      dio_.PackedDataHash.Init(arc_.FileHead.FileHash.Type, threads);
      dio_.SetPackedSizeToRead(arc_.FileHead.PackSize);
      dio_.SetFiles(&arc_, NULL);
      bufsize = (size_t)arc_.FileHead.UnpSize;
      if (!(*buf)) {
        result = malloc(bufsize);
        *buf = (char*)result;
      }
      dio_.SetUnpackToMemory((byte*)(*buf), (uint)bufsize);
      dio_.SetTestMode(arc_.Solid);
      dio_.SetSkipUnpCRC(arc_.Solid);

      if (arc_.FileHead.Method == 0) {
        int64 dest_unpsize = arc_.FileHead.UnpSize;
        Array<byte> buffer(File::CopyBufferSize());
        while (true)
        {
          int readsize = dio_.UnpRead(&buffer[0], buffer.Size());
          if (readsize <= 0)
            break;
          int writesize = (int64)readsize < dest_unpsize ? readsize : (int)dest_unpsize;
          if (writesize > 0)
          {
            dio_.UnpWrite(&buffer[0], writesize);
            dest_unpsize -= writesize;
          }
        }
      }
      else {
        unp_->Init(arc_.FileHead.WinSize, arc_.FileHead.Solid);
        unp_->SetDestSize(arc_.FileHead.UnpSize);
        if (arc_.Format != RARFMT50 && arc_.FileHead.UnpVer <= 15)
          unp_->DoUnpack(15, arc_.Solid);
        else
          unp_->DoUnpack(arc_.FileHead.UnpVer, arc_.FileHead.Solid);
      }
    }
    rhd->Data = result;
    return rhd;
  }

  void* CRarRes::LoadResource(const wchar_t* id, char** buf, size_t& bufsize) {
    if (!arc_.IsOpened()) {
      ErrHandler.SetErrorCode(RARX_OPEN);
      return nullptr;
    }

    std::map<std::wstring, RARRES_FILEHEADER* >::iterator it = fileheadersW_.find(id);
    if (it == fileheadersW_.end()) {
      ErrHandler.SetErrorCode(RARX_NOFILES);
      return nullptr;
    }

    RARRES_FILEHEADER* rhd = it->second;
    if (NULL == rhd) {
      ErrHandler.SetErrorCode(RARX_NOFILES);
      return nullptr;
    }

    return Extract(rhd, buf, bufsize);
  }

#ifdef _WIN32
  IStream* CRarRes::LoadResource(const char* id) {
    size_t res_size = 0;
    LoadResource(id, nullptr, res_size);
    if (!res_size)
      return nullptr;

    IStream* stream = nullptr;
    HGLOBAL buffer_handler = ::GlobalAlloc(GMEM_MOVEABLE, res_size);
    if (buffer_handler) {
      void* buffer = ::GlobalLock(buffer_handler);
      if (buffer) {
        void* res = LoadResource(id, (char**)&buffer, res_size);
        if (res) {
          ::CreateStreamOnHGlobal(buffer_handler, TRUE, &stream);
          FreeResource(res);
        }
        ::GlobalUnlock(buffer_handler);
      }
    }
    return stream;
  }

  IStream* CRarRes::LoadResource(const wchar_t* id) {
    size_t res_size = 0;
    LoadResource(id, nullptr, res_size);
    if (!res_size)
      return nullptr;

    IStream* stream = nullptr;
    HGLOBAL buffer_handler = ::GlobalAlloc(GMEM_MOVEABLE, res_size);
    if (buffer_handler) {
      void* buffer = ::GlobalLock(buffer_handler);
      if (buffer) {
        void* res = LoadResource(id, (char**)&buffer, res_size);
        if (res) {
          ::CreateStreamOnHGlobal(buffer_handler, TRUE, &stream);
          FreeResource(res);
        }
        ::GlobalUnlock(buffer_handler);
      }
    }
    return stream;
  }
#endif

  void CRarRes::FreeResource(void* res) {
    if (res) {
      RARRES_FILEHEADER* rhd = (RARRES_FILEHEADER*)res;
      if (rhd->Data) {
        free(rhd->Data);
        rhd->Data = nullptr;
      }
    }
  }

  int CRarRes::GetErrorCode() {
    return ErrHandler.GetErrorCode();
  }

};

namespace JRES {

  const char* PASCAL GetVersion() {
    return PRODUCT_VERSION;
  }

  IRes* PASCAL CreateRarRes(bool ignorecase) {
    return new RARRES::CRarRes(ignorecase);
  }

};
