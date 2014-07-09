/*
* =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: ExternalCheckCmd.h
* Description	: 
* -----------------------------------------------------------------------
* Author        : wangtiefeng (wtf), wangtf418@163.com
* Company		: 
* Date			: 2010-03-05
* -----------------------------------------------------------------------
* History		: 
*		<author>		  <time>		<version>		<description>
*      ---------------------------------------------------------------
*		wangtiefeng		2010-03-05			1.0			
* =======================================================================
*/
#ifndef _EXTERNALCHECKCMD_H_8D725779_BD5C_42A4_935E_343E6E9E7F1B_
#define _EXTERNALCHECKCMD_H_8D725779_BD5C_42A4_935E_343E6E9E7F1B_

#include "basemaintaincmd.h"
#include "pingpeerhashtable.h"
#include "commmsg.h"


class CExternalCheckCmd : public CBaseMaintainCmd
{
public:
	CExternalCheckCmd(int nMsgType, int nBodyLen, char* pBody) : CBaseMaintainCmd(nMsgType ,nBodyLen ,pBody)
	{
		m_pPeerHashTable = CPingPeerHashTable::GetHashTable();
	}
	virtual ~CExternalCheckCmd()
	{
	}
	virtual int Process();
protected:
	CPingPeerHashTable* m_pPeerHashTable;
private:
	int DumpHashTable();
	int LoadHashTable();
	void SendMsg(int destEntityType, int destEntityId, ULONG64 lTaskId,  int msgType,  CMsgBody *pMsgBody, ULONG64 lSrcTaskId=0);
};

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

