//
//  ZLibString.cpp
//  hhl
//
//  Created by guodi on 15/10/26.
//
//

#include "ZLibString.h"

CZlibMgr* CZlibMgr::instance = NULL;

CZlibMgr::CZlibMgr()
{
}

CZlibMgr::~CZlibMgr()
{
}

CZlibMgr* CZlibMgr::getInstance()
{
    if (instance == NULL) {
        instance = new CZlibMgr();
    }
    return instance;
}

bool CZlibMgr::Compress(const char* pcContentBuf, const unsigned long sourceSize, char* pcCompBuf, unsigned long& ulCompLen)
{
	if (pcContentBuf == NULL || sourceSize == NULL || sourceSize <= 0)
    {
        return false;
    }
	
    memset(compr, 0, MAXBUFFERSIZE);
    
    uLong comprLen;
    int err;
    
	uLong len = sourceSize;
    comprLen = sizeof(compr) / sizeof(compr[0]);
    
    err = compress(compr, &comprLen, (const Bytef*)pcContentBuf, len);
    if (err != Z_OK)
    {
        cout << "compess error: " << err << endl;
        return false;
    }
    //cout << "orignal size: " << len << " , compressed size : " << comprLen << endl;
    memcpy(pcCompBuf, compr, comprLen);
    ulCompLen = comprLen;
    
    return true;
}

int CZlibMgr::UnCompress(const char* pcCompBuf, char* pcUnCompBuf, unsigned long ulCompLen, unsigned long& finalUncomprLen)
{
    if (pcCompBuf == NULL)
    {
        cout <<__FUNCTION__ << "================> pcCompBuf is null please to check " << endl;
        return -100;
    }
    
    if (strlen(pcCompBuf) == 0)
    {
        cout <<__FUNCTION__ << "strlen(pcCompBuf) == 0  ========================> " << endl;
        return -100;
    }
	
    memset(uncompr, 0, MAXBUFFERSIZE);
    uLong uncomprLen = MAXBUFFERSIZE;
    int err;
    
    err = uncompress(uncompr, &uncomprLen, (const Bytef *)pcCompBuf, ulCompLen);
	if (err != Z_OK)
    {
        cout << "uncompess error: " << err << endl;
		return err;
    }
    
    //cout << "compress size: " << ulCompLen << "  uncompressed size : " << uncomprLen << endl;
    memcpy(pcUnCompBuf, uncompr, uncomprLen);
	finalUncomprLen = uncomprLen;

	return Z_OK;
}

char* CZlibMgr::newUnCompressCharP(const char* pcCompBuf, unsigned long ulCompLen, unsigned long& finalUncomprLen)
{
	if (pcCompBuf == NULL)
	{
		cout << __FUNCTION__ << "================> pcCompBuf is null please to check " << endl;
		return NULL;
	}

	memset(uncompr, 0, MAXBUFFERSIZE);
	uLong uncomprLen = MAXBUFFERSIZE;
	int err;

	err = uncompress(uncompr, &uncomprLen, (const Bytef *)pcCompBuf, ulCompLen);
	if (err != Z_OK)
	{
		cout << "uncompess error: " << err << endl;
		return NULL;
	}

	//cout << "compress size: " << ulCompLen << "  uncompressed size : " << uncomprLen << endl;
	char *pcUnCompBuf = new char[uncomprLen];
	memcpy(pcUnCompBuf, uncompr, uncomprLen);
	finalUncomprLen = uncomprLen;

	return pcUnCompBuf;
}