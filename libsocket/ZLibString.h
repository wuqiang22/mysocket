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
	// ѹ����pcContentBuf Ҫѹ�������� pcCompBuf ѹ��������� ulCompLen �õ�ѹ����ĳ���
	bool Compress(const char* pcContentBuf, const unsigned long sourceSize, char* pcCompBuf, unsigned long& ulCompLen);
	// ��ѹ,pcCompBuf ѹ��������, pcUnCompBuf ��ѹ�������  ulCompLen ����ѹ�����ݵĳ��� uncomprLen�õ���ѹ����ĳ���
	int UnCompress(const char* pcCompBuf, char* pcUnCompBuf, unsigned long ulCompLen,unsigned long& finalUncomprLen);

	// ��ѹ,pcCompBuf ѹ��������,ulCompLen ����ѹ�����ݵĳ��� finalUncomprLen�õ���ѹ����ĳ���
	char* newUnCompressCharP(const char* pcCompBuf, unsigned long ulCompLen, unsigned long& finalUncomprLen);

	int getMaxBufferSize(){ return MAXBUFFERSIZE; }; 
private:
    Byte compr[MAXBUFFERSIZE];
    Byte uncompr[MAXBUFFERSIZE];
    //����
    static CZlibMgr* instance;
    
};


#endif /* defined(__hhl__ZLibString__) */
