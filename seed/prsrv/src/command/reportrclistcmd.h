/*
* =======================================================================
* Copyright (c) 2011-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: reportrclistcmd.h
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
#ifndef __REPORTRCLISTCMD_H_4EDB0BAB_10D3_4B80_A081_A56626E10AD9_
#define __REPORTRCLISTCMD_H_4EDB0BAB_10D3_4B80_A081_A56626E10AD9_
#include "seedbasecommand.h"

struct TReportRc
{
	string m_strCid;
	long long m_nFileSize;
	string m_strGcid;
	//102版本后添加
	unsigned char m_nDownloadScale;
	vector<unsigned char> m_vecBlockDownloadScale;
	TReportRc()
	{
		m_nFileSize = 0;
		m_nDownloadScale = g_nDownPercentUnknow;
	}
	int Decode(CBufMgr& BufMgr)
	{	
		int nStructLen = 0;
		if(BufMgr.GetInt(nStructLen) != 0) return -1;		//结构体长度（历史遗留问题）

		if (BufMgr.GetString(m_strCid) != 0) return -1;
		if (m_strCid.size() != CIDLEN) return -1;

		if (BufMgr.GetLong64(m_nFileSize) != 0) return -1;

		if (BufMgr.GetString(m_strGcid) != 0) return -1;
		return 0;
	}

	int DecodeNew(CBufMgr& BufMgr)
	{	
		int nStructLen;
		if(BufMgr.GetInt(nStructLen) != 0) return -1;		//结构体长度（历史遗留问题）

		if (BufMgr.GetString(m_strCid) != 0) return -1;
		if (m_strCid.size() != CIDLEN) return -1;

		if (BufMgr.GetLong64(m_nFileSize) != 0) return -1;

		if (BufMgr.GetString(m_strGcid) != 0) return -1;

		//102版本添加////////////////////////////////////////////////////////////////////////
		if (BufMgr.GetChar((char&)m_nDownloadScale) != 0) return -1;
		if (m_nDownloadScale > 100 && m_nDownloadScale != g_nDownPercentUnknow) return -1;

		int nBlockCount = 0;
		if (BufMgr.GetInt(nBlockCount) != 0) return -1;
		if (nBlockCount < 0 && nBlockCount > g_nMaxFileBlockNum) return -1;

		for (int nA = 0; nA < nBlockCount; ++ nA)
		{
			unsigned char nDownPercent = 0;
			if (BufMgr.GetChar((char&)nDownPercent) != 0) return -1;
			if (nDownPercent > 100 && nDownPercent != g_nDownPercentUnknow) return -1;
			m_vecBlockDownloadScale.push_back(nDownPercent);
		}
		//102版本添加结束//////////////////////////////////////////////////////////////
		return 0;
	}
};


struct TReportRcListCmd
{
	string m_strPeerId;    
	vector<TReportRc> m_vecReportRcList;	
	int m_nCapability;

	int Decode(CBufMgr& BufMgr)
	{
		if (BufMgr.GetString(m_strPeerId) != 0) return -1;
		if (m_strPeerId.length() != PEERLENGTH) return -1;

		int nRcCount = 0;
		if (BufMgr.GetInt(nRcCount) != 0) return -1;
		if (nRcCount < 0 || nRcCount >= g_nMaxFileBlockNum) return -1;

		m_vecReportRcList.clear();
		for (int nA =0; nA < nRcCount; ++ nA)
		{
			TReportRc ReportRc;
			if  (ReportRc.Decode(BufMgr) != 0) return -1;
			m_vecReportRcList.push_back(ReportRc);
		}

		if (BufMgr.GetInt(m_nCapability) != 0) return -1;
		return 0;
	}

	int DecodeNew(CBufMgr& BufMgr)
	{
		if (BufMgr.GetString(m_strPeerId) != 0) return -1;
		if (m_strPeerId.length() != PEERLENGTH) return -1;

		int nRcCount = 0;
		if (BufMgr.GetInt(nRcCount) != 0) return -1;
		m_vecReportRcList.clear();
		for (int nA = 0; nA < nRcCount; ++ nA) //zz
		{
			TReportRc ReportRc;
			if (ReportRc.DecodeNew(BufMgr) != 0) return -1;
			m_vecReportRcList.push_back(ReportRc);
		}

		if (BufMgr.GetInt(m_nCapability) != 0) return -1;
		return 0;
	}
};

struct TReportRcListCmdRes
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

class CReportRcListCmd : public CSeedBaseCmd<TReportRcListCmd, TReportRcListCmdRes>
{
public:
	CReportRcListCmd(char* pBuf, int nLen) : CSeedBaseCmd<TReportRcListCmd, TReportRcListCmdRes>(pBuf, nLen)
	{
		m_outHead.m_nCmd = SEEDCMD_REPORT_RCLIST_RES;
		m_nIfRes = 0;
	}

	~CReportRcListCmd()
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
			if (m_inBody.DecodeNew(m_inBuf) != 0) return -1;
		}
		else
		{
			if (m_inBody.Decode(m_inBuf) != 0) return -1;
		}						
		m_bIfDecode = true;
		return 0;
	}
protected:
	int ProcessCmd();
};

#endif


