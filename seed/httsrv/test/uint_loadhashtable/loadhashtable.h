/*
* =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: loadhashtable.h
* Description	: 
* -----------------------------------------------------------------------
* Author        : wangtiefeng (wtf), wangtf418@163.com
* Company		: 
* Date			: 2010-05-27
* -----------------------------------------------------------------------
* History		: 
*		<author>		  <time>		<version>		<description>
*      ---------------------------------------------------------------
*		wangtiefeng		2010-05-27			1.0			
* =======================================================================
*/
#ifndef _LOADHASHTABLE_H_7068FF9C_DD5A_48CB_B36E_B15DD302BE79_
#define _LOADHASHTABLE_H_7068FF9C_DD5A_48CB_B36E_B15DD302BE79_
#include <string>
#include <list>
#include <map>
#include "framecommon/framecommon.h"

using namespace MYFRAMECOMMON;

class CLoadHashTable
{
public:
	CLoadHashTable(const string& strFileName)
	{
		m_nFileFd = -1;
		m_nFileLength = 0;
		m_nCurOffset = 0;
		m_bIfOpenFile = false;
		m_strFileName = strFileName;
		m_nListSize = 0;
		m_nLastError = 0;
		m_pBuf = NULL;
	}
	~CLoadHashTable()
	{
		if (m_bIfOpenFile)
		{
			close(m_nFileFd);
			m_nFileFd = -1;
		}

		if (m_pBuf)
		{
			delete [] m_pBuf;
		}		
	}
	string GetLine();
	int GetLastError()
	{
		return m_nLastError;
	}
	int Init();
	enum 
	{
		E_READ_BUFSIZE = 1024 * 1024,
		MAX_LINE = 1024
	};
private:
	int m_nFileFd;
	bool m_bIfOpenFile;
	string m_strFileName;
	long long m_nFileLength;
	long long m_nCurOffset;
private:
	char* m_pBuf;
	string m_strData;
	int m_nHashLength;
	int m_nLastError;
private:
	list<string> m_lstLine;
	int m_nListSize;
};

#endif

