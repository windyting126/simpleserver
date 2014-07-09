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
#ifndef __INSERTPEERCMD_H__
#define __INSERTPEERCMD_H__
#include "seedbasecommand.h"
#include "framecommon/framecommon.h"

struct TInsertPeer
{
	TInsertPeer()
	{
		m_nInternalIp = 0;
		m_nTcpPort = 0;
		m_nUpnpMapdeIp = 0;
		m_nUpnpMapedPort = 0;
		m_nCurUploadPipeStatus = -1;
		m_nMaxDownloadSpeed = 0;
		m_nMaxUploadSpeed = 0;
		m_nLocalBindPort = 0;
		m_nExternalIp = 0;
		m_nExternalPort = 0;
		m_ullLastReportTime = 0;
		m_nCurUploadPipeNum = 0;
		m_cLogoutFlag = LOGOUT_FLAG_NO;
	}

	int Encode(CBufMgr& bufMgr)
	{
		int nRet = 0;
		nRet += bufMgr.PutString(m_strPeerId);
		nRet += bufMgr.PutInt(m_nInternalIp);
		nRet += bufMgr.PutShort(m_nTcpPort);
		nRet += bufMgr.PutInt(m_nUpnpMapdeIp);
		nRet += bufMgr.PutShort(m_nUpnpMapedPort);
		nRet += bufMgr.PutChar(m_nCurUploadPipeStatus);
		nRet += bufMgr.PutShort(m_nMaxDownloadSpeed);
		nRet += bufMgr.PutShort(m_nMaxUploadSpeed);
		nRet += bufMgr.PutShort(m_nLocalBindPort);
		nRet += bufMgr.PutInt(m_nExternalIp);
		nRet += bufMgr.PutShort(m_nExternalPort);
		nRet += bufMgr.PutLong64(m_ullLastReportTime);
		nRet += bufMgr.PutChar(m_nCurUploadPipeNum);
		nRet += bufMgr.PutChar(m_cLogoutFlag);
		return nRet;
	}

	int GetSize()
	{
		int nLen = 0;
		nLen += sizeof(m_nInternalIp);
		nLen += sizeof(m_nTcpPort);
		nLen += sizeof(m_nUpnpMapdeIp);
		nLen += sizeof(m_nUpnpMapedPort);
		nLen += sizeof(m_nCurUploadPipeStatus);
		nLen += sizeof(m_nMaxDownloadSpeed);
		nLen += sizeof(m_nMaxUploadSpeed);
		nLen += sizeof(m_nLocalBindPort);
		nLen += sizeof(m_nExternalIp);
		nLen += sizeof(m_nExternalPort);
		nLen += sizeof(int) + m_strPeerId.length();
		nLen += sizeof(m_ullLastReportTime);
		nLen += sizeof(m_nCurUploadPipeNum);
		nLen += sizeof(m_cLogoutFlag);
		return nLen;
	}

	int m_nInternalIp;
	short m_nTcpPort;
	int m_nUpnpMapdeIp;
	short m_nUpnpMapedPort;
	char m_nCurUploadPipeStatus;
	short m_nMaxDownloadSpeed;
	short m_nMaxUploadSpeed;
	short m_nLocalBindPort;
	int m_nExternalIp;
	short m_nExternalPort;
	string m_strPeerId;
	unsigned long long m_ullLastReportTime;
	unsigned char m_nCurUploadPipeNum;
	char m_cLogoutFlag;
};

struct TInsertPeerRes
{
	char m_cResult;

	int Decode(CBufMgr &bufMgr)
	{
		if (bufMgr.GetChar(m_cResult)) {return -1;}
		return 0;
	}
};

class CInsertPeerCmd: public EncodeCmd2<TCmdHead2, TInsertPeer>
{
public:
	CInsertPeerCmd(string &strPeerId);
	virtual ~CInsertPeerCmd(){}
protected:
	int ProcessCmd();
private:
};

class CInsertPeerCmdRes: public DecodeCmd<TCmdHead2, TInsertPeerRes>
{
public:
	CInsertPeerCmdRes(char* pBuf, int nLen)
		: DecodeCmd<TCmdHead2, TInsertPeerRes>(pBuf, nLen){}
	virtual ~CInsertPeerCmdRes(){}
protected:
	int ProcessCmd();
private:
};

#endif
