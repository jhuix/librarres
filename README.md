Librarres
=============

librarres 工程是一读取由RAR压缩文件做为资源包的库。

## UnRAR

RAR文件分析和解压缩采用 [www.rarlab.com](https://www.rarlab.com) 的解压缩[UnRAR 5.6.8](https://www.rarlab.com/rar/unrarsrc-5.6.8.tar.gz)开源版本源码。

## Example

#### 1. 获取一个test.rar资源包里的名为"skin\\ui\\index.html"的内容至缓存中:

```
IRarRes* rarres = CreateRarRes();
if (rarres) {
    if (rarres->Load("test.rar")) {
        char* buf = nullptr;
        size_t bufsize = 0;
        void* res = rarres->LoadResource("skin\\ui\\index.html", &buf, bufsize);
        if (res) {
            //TO DO USE data of "buf"
            rarres->FreeResource(res);
        }
    }
    rarres->Release();
}
```

#### 2. 在windows平台下，获取一个test.rar资源包里的名为"skin\\img\\caotion.png"的内容至IStream中:

```
IRarRes* rarres = CreateRarRes();
if (rarres) {
    if (rarres->Load("test.rar")) {
        IStream* res = rarres->LoadResource("skin\\img\\caotion.png");
        if (res) {
            //TO DO USE IStream interface methods of "res";
            //For examples: <Seek>|<Read>|<Write> methods etc.
            res->Release();
        }
    }
    rarres->Release();
}
```

## License

[MIT](http://opensource.org/licenses/MIT)

Copyright (c) 2018-present, [Jhuix](jhuix0117@gmail.com) (Hui Jin)