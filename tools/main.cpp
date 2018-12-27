// rarres.cpp: 定义控制台应用程序的入口点。
//
#ifdef _WIN32
#ifndef STRICT
#define STRICT 1
#endif
#define WINVER 0x0501
#define _WIN32_WINNT 0x0501
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <wchar.h>
#include <wctype.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#include "librarres.h"
#include "librespak.h"

void print_help()
{
//#ifdef RARRES_CRYPT
//  printf("\n");
//  printf("RarRes 使用帮助：\n");
//  printf("接口：加密RAR文件（只支持无密码加密的RAR文件）\n");
//  printf("-e srcfile destfile\n");
//  printf("srcfile：  待加密文件（可包括路径）\n");
//  printf("destfile： 加密后文件（可包括路径）\n");
//  printf("\n");
//  printf("接口：解密RAR文件（只支持该工具加密的RAR文件）\n");
//  printf("-d srcfile destfile\n");
//  printf("srcfile：  待解密文件（可包括路径）\n");
//  printf("destfile： 解密后文件（可包括路径）\n");
//#endif
  printf("\n");
  printf("接口：解压RAR文件中的单个文件（支持该工具加密和未加密RAR文件）\n");
  printf("-x srcfile file destfile\n");
  printf("srcfile：  待解压文件（可包括路径）\n");
  printf("file：     待解压文件中的文件名（包括文件相对路径）\n");
  printf("destfile： 解压后文件（可包括路径）\n");
  printf("\n");
}

int main(int argc, char *argv[]) {
  //Default print help
  if (argc > 1) {
    char* param = argv[1];
//#ifdef RARRES_CRYPT
//    if (_stricmp(param, "-e") == 0) {
//      if (argc > 3) {
//        if (RARRES::Encode(argv[2], argv[3])) {
//          printf("\nRarRes: 成功加密文件\"");
//          printf(argv[2]);
//          printf("\"至目标文件\"");
//          printf(argv[3]);
//          printf("\"。\n");
//          return 0;
//        }
//      }
//    }
//    else if (_stricmp(param, "-d") == 0) {
//      if (argc > 3) {
//        if (RARRES::Decode(argv[2], argv[3])) {
//          printf("\nRarRes: 成功解密文件\"");
//          printf(argv[2]);
//          printf("\"至目标文件\"");
//          printf(argv[3]);
//          printf("\"。\n");
//          return 0;
//        }
//      }
//    }
//#endif
    if (_stricmp(param, "-x") == 0) {
      if (argc > 4) {
        if (RARRES::ExtractFile(argv[2], argv[3], argv[4])) {
          printf("\nRarRes: 成功解压文件\"");
          printf(argv[3]);
          printf("\"至目标文件\"");
          printf(argv[4]);
          printf("\"。\n");
          return 0;
        }
      }
    }
  }

  print_help();
  return -1;
}

