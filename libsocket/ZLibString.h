//
//  ZLibString.h
//  hhl
//
//  Created by guodi on 15/10/26.
//
//

#ifndef __hhl__ZLibString__
#define __hhl__ZLibString__

#define  MAXBUFFERSIZE 1024*1024 //1m
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include "zlib.h"

using namespace std;

class CZlibMgr
{
public:
    CZlibMgr();
    ~CZlibMgr();
    static CZlibMgr* getInstance();
	// 压缩，pcContentBuf 要压缩的内容 pcCompBuf 压缩后的内容 ulCompLen 得到压缩后的长度
	bool Compress(const char* pcContentBuf, const unsigned long sourceSize, char* pcCompBuf, unsigned long& ulCompLen);
	// 解压,pcCompBuf 压缩的内容, pcUnCompBuf 解压后的内容  ulCompLen 传入压缩内容的长度 uncomprLen得到解压缩后的长度
	int UnCompress(const char* pcCompBuf, char* pcUnCompBuf, unsigned long ulCompLen,unsigned long& finalUncomprLen);

	// 解压,pcCompBuf 压缩的内容,ulCompLen 传入压缩内容的长度 finalUncomprLen得到解压缩后的长度
	char* newUnCompressCharP(const char* pcCompBuf, unsigned long ulCompLen, unsigned long& finalUncomprLen);

	int getMaxBufferSize(){ return MAXBUFFERSIZE; }; 
private:
    Byte compr[MAXBUFFERSIZE];
    Byte uncompr[MAXBUFFERSIZE];
    //单例
    static CZlibMgr* instance;
    
};


#endif /* defined(__hhl__ZLibString__) */
