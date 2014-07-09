/*
* =======================================================================
* Copyright (c) 2011-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: deleterclistcmd.h
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
#ifndef __DELETERCLISTCMD_H_464E775B_3E66_44A2_B0C4_629C0DB33D7E_
#define __DELETERCLISTCMD_H_464E775B_3E66_44A2_B0C4_629C0DB33D7E_
#include "seedbasecommand.h"

struct TDeleteRc
{
	string m_strCid;
	long long m_nFileSize;
	string m_strGcid;
	int Decode(CBufMgr& BufMgr)
	{
		int nStructLen;
		if (BufMgr.GetInt(nStructLen) != 0) return -1;

		if (BufMgr.GetString(m_strCid) != 0) return -1;
		if (m_strCid.size() != CIDLEN) return -1;

		if (BufMgr.GetLong64(m_nFileSize) != 0) return -1;

		if (BufMgr.GetString(m_strGcid) != 0) return -1;
		if (m_strGcid.size() != CIDLEN) return -1;
		return 0;
	}
};

struct TDeleteRcListCmd
{
	string m_strPeerId;
	vector<TDeleteRc> m_vecDeleteRcLst;

	int Decode(CBufMgr& BufMgr)
	{
		if (BufMgr.GetString(m_strPeerId) != 0) return -1;
		if (m_strPeerId.size() != PEERLENGTH) return -1;

		int nDeleteRcCnt = 0;
		if(BufMgr.GetInt(nDeleteRcCnt) != 0) return -1;
		if (nDeleteRcCnt <= 0) return -1;
		
		for(int nA = 0; nA < nDeleteRcCnt; ++ nA)
		{
			TDeleteRc DeleteRc;
			if(DeleteRc.Decode(BufMgr) != 0) return -1;
			m_vecDeleteRcLst.push_back(DeleteRc);
		}
		return 0;
	}
};

struct TDeleteRcListCmdRes
{
	char m_nResult;
	int Encode(CBufMgr& BufMgr)
	{
		int nRet = 0; 
		nRet += BufMgr.PutChar(m_nResult);
		return nRet;
	}
	int GetSize()
	{
		int nLen = 0;
		nLen += sizeof(m_nResult);
		return nLen;
	}
};

class CDeleteRcListCmd : public CSeedBaseCmd<TDeleteRcListCmd, TDeleteRcListCmdRes>
{
public:
	CDeleteRcListCmd(char* pBuf, int nLen) : CSeedBaseCmd<TDeleteRcListCmd, TDeleteRcListCmdRes>(pBuf, nLen)
	{
		m_outHead.m_nCmd = SEEDCMD_DELETE_RCLIST_RES;
		m_nIfRes = 0;
	}
	~CDeleteRcListCmd(){}
protected:
	int ProcessCmd();
};


#endif

