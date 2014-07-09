/*
* =======================================================================
* Copyright (c) 2011-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: insertrccmd.h
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
#ifndef __INSERTRCCMD_H_F68ED828_0FE6_4F93_BCD2_FEB047D1017C_
#define __INSERTRCCMD_H_F68ED828_0FE6_4F93_BCD2_FEB047D1017C_
#include "seedbasecommand.h"

struct TInsertRcCmd
{
	string m_strPeerId;
	string m_strCid;
	long long m_nFileSize;
	string m_strGcid;
	int m_nCapability;
	//102版本开始添加的项
	unsigned char m_nDownloadScale;
	vector<unsigned char> m_vecBlockDownloadScale;
	TInsertRcCmd()
	{
		m_nFileSize = 0;
		m_nCapability = 0;
		m_nDownloadScale = g_nDownPercentUnknow;
	}
	int Decode(CBufMgr& BufMgr)
	{
		if (BufMgr.GetString(m_strPeerId) != 0) return -1;
		if (m_strPeerId.length() != PEERLENGTH) return -1;

		if (BufMgr.GetString(m_strCid) != 0) return -1;

		if (BufMgr.GetLong64(m_nFileSize) != 0) return -1;

		if (BufMgr.GetString(m_strGcid) != 0) return -1;
		if (m_strGcid.length() != CIDLEN) return -1;

		if (BufMgr.GetInt(m_nCapability) != 0) return -1;
		return 0;
	}
	int DecodeNew(CBufMgr& BufMgr)
	{
		if (BufMgr.GetString(m_strPeerId) != 0) return -1;
		if (m_strPeerId.length() != PEERLENGTH) return -1;

		if (BufMgr.GetString(m_strCid) != 0) return -1;

		if (BufMgr.GetLong64(m_nFileSize) != 0) return -1;

		if (BufMgr.GetString(m_strGcid) != 0) return -1;
		if (m_strGcid.length() != CIDLEN) return -1;

		//这部分是102版本添加的////////////////////////////////////////////////////////////////////////
		if (BufMgr.GetChar((char&)m_nDownloadScale) != 0) return -1;
		if (m_nDownloadScale > 100 && m_nDownloadScale != g_nDownPercentUnknow) return -1;

		int nBlockCount = 0;
		if (BufMgr.GetInt(nBlockCount) != 0) return -1;
		if (nBlockCount < 0 || nBlockCount > g_nMaxFileBlockNum) return -1;

		for (int nA = 0; nA < nBlockCount; ++ nA)
		{
			unsigned char nDownPercent = 0;
			if (BufMgr.GetChar((char&)nDownPercent) != 0) return -1;
			if (nDownPercent > 100 && nDownPercent != g_nDownPercentUnknow) return -1;
			m_vecBlockDownloadScale.push_back(nDownPercent);
		}
		//102版添加结束////////////////////////////////////////////////////////////////////////
		if (BufMgr.GetInt(m_nCapability) != 0) return -1;
		return 0;
	}
};

struct TInsertRcCmdRes
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


class CInsertRcCmd : public CSeedBaseCmd<TInsertRcCmd, TInsertRcCmdRes>
{
public:
	CInsertRcCmd(char* pBuf, int nLen): CSeedBaseCmd<TInsertRcCmd, TInsertRcCmdRes>(pBuf, nLen)
	{
		m_outHead.m_nCmd = SEEDCMD_INSERT_RC_RES;
		m_nIfRes = 0;
	}
	~CInsertRcCmd()
	{

	}
protected:
	int Decode()
	{
		if (m_bIfDecode) return 0;

		if(m_inHead.Decode(m_inBuf) != 0) return -1;
		if (m_inHead.m_nBodylen + TSeedBaseCmd::CMDID_OFFSET != m_inBuf.GetBufLen()) return -1;
		if (m_inHead.m_nProtocolVersion == NEW_VERSION102)
		{
			if(m_inBody.DecodeNew(m_inBuf) != 0) return -1;	
		}
		else
		{
			if(m_inBody.Decode(m_inBuf) != 0) return -1;	
		}			
		m_bIfDecode = true;     
		return 0;
	}
protected:
	virtual int ProcessCmd();
};



#endif


