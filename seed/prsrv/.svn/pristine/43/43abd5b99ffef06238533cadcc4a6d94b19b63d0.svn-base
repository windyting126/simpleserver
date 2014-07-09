/*
* =======================================================================
* Copyright (c) 2011-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: isrconlinecmd.h
* Description	: 
* -----------------------------------------------------------------------
* Author        : wangtiefeng (wtf), wangtf418@163.com
* Company		: 
* Date			: 2011-03-25
* -----------------------------------------------------------------------
* History		: 
*		<author>		  <time>		<version>		<description>
*      ---------------------------------------------------------------
*		wangtiefeng		2011-03-25			1.0			
* =======================================================================
*/
#ifndef __ISRCONLINECMD_H_453EC723_1544_409E_94E8_BFE1F87ED61F_
#define __ISRCONLINECMD_H_453EC723_1544_409E_94E8_BFE1F87ED61F_
#include "seedbasecommand.h"

struct TIsRcOnline
{
	string m_strPeerId;
	int Decode(CBufMgr& BufMgr)
	{
		if (BufMgr.GetString(m_strPeerId) != 0) return -1;
		if (m_strPeerId.size() != PEERLENGTH) return -1;
		return 0;
	}
};

struct TIsRcOnlineRes
{
	char m_nResult;
	int m_nIfNeedReport;
	int Encode(CBufMgr& BufMgr)
	{
		int nRet = 0; 
		nRet += BufMgr.PutChar(m_nResult);
		nRet += BufMgr.PutInt(m_nIfNeedReport);
		return nRet;
	}
	int GetSize()
	{
		int nLen = 0;
		nLen += sizeof(m_nResult);
		nLen += sizeof(m_nIfNeedReport);
		return nLen;
	}
};

class CIsRcOnlieCmd : public CSeedBaseCmd<TIsRcOnline, TIsRcOnlineRes>
{
public:
	CIsRcOnlieCmd(char *pBuf, int nLen) : CSeedBaseCmd<TIsRcOnline, TIsRcOnlineRes>(pBuf, nLen)
	{
		m_outHead.m_nCmd = SEEDCMD_ISRC_ONLINE_RES;
	}

	~CIsRcOnlieCmd()
	{

	}
protected:
	virtual int ProcessCmd();
	
};



#endif

