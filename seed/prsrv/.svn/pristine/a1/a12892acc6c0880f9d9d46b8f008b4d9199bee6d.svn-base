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
#ifndef __INSERTRCCMD_H__
#define __INSERTRCCMD_H__
#include "seedbasecommand.h"
#include "framecommon/framecommon.h"

struct TInsertRc
{
	TInsertRc()
	{
		m_llFileSize = 0;
		m_nCapability = 0;
		m_nDownloadScale = 0;
	}

	int Encode(CBufMgr& bufMgr)
	{
		int nRet = 0;
		nRet += bufMgr.PutString(m_strPeerId);
		nRet += bufMgr.PutString(m_strCid);
		nRet += bufMgr.PutLong64(m_llFileSize);
		nRet += bufMgr.PutString(m_strGcid);
		nRet += bufMgr.PutChar(m_nDownloadScale);
		int nVecLen = m_vecBlockDownloadScale.size();
		nRet += bufMgr.PutInt(nVecLen);
		for (int i = 0; i < nVecLen; i ++)
		{
			nRet += bufMgr.PutChar(m_vecBlockDownloadScale[i]);
		}
		nRet += bufMgr.PutInt(m_nCapability);
		return nRet;
	}

	int GetSize()
	{
		int nLen = 0;
		nLen += sizeof(int) + m_strPeerId.length();
		nLen += sizeof(int) + m_strCid.length();
		nLen += sizeof(m_llFileSize);
		nLen += sizeof(int) + m_strGcid.length();
		nLen += sizeof(m_nDownloadScale);
		int nVecLen = m_vecBlockDownloadScale.size();
		nLen += sizeof(nVecLen);
		for (int i = 0; i < nVecLen; i ++)
		{
			nLen += sizeof(m_vecBlockDownloadScale[i]);
		}
		nLen += sizeof(m_nCapability);
		return nLen;
	}

	string m_strPeerId;
	string m_strCid;
	long long m_llFileSize;
	string m_strGcid;
	int m_nCapability;
	unsigned char m_nDownloadScale;
	vector<unsigned char> m_vecBlockDownloadScale;
};

struct TInsertRcRes
{
	char m_cResult;

	int Decode(CBufMgr &bufMgr)
	{
		if (bufMgr.GetChar(m_cResult)) {return -1;}
		return 0;
	}
};

class CInsertRcCmd: public EncodeCmd<TCmdHead, TInsertRc>
{
public:
	CInsertRcCmd(string strGcid, string strPeerId, long long llFileSize, int nCapability, int nDownLoadPercent);
	virtual ~CInsertRcCmd(){}
protected:
	int ProcessCmd();
private:
};

class CDeleteCmdRes: public DecodeCmd<TCmdHead, TInsertRcRes>
{
public:
	CDeleteCmdRes(char* pBuf, int nLen)
		: DecodeCmd<TCmdHead, TInsertRcRes>(pBuf, nLen){}
	virtual ~CDeleteCmdRes(){}
protected:
	int ProcessCmd();
private:
};

#endif
