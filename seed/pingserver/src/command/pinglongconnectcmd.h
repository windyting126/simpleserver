/*
* =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: pinglongconnectcmd.h
* Description	: 
* -----------------------------------------------------------------------
* Author        : wangtiefeng (wtf), wangtf418@163.com
* Company		: 
* Date			: 2009-08-04
* -----------------------------------------------------------------------
* History		: 
*		<author>		  <time>		<version>		<description>
*      ---------------------------------------------------------------
*		wangtiefeng		2009-08-04			1.0			
* =======================================================================
*/
#ifndef _PINGLONGCONNECTCMD_H_639A3CD2_6AB8_42dc_9581_A5FD5B4146FB_
#define _PINGLONGCONNECTCMD_H_639A3CD2_6AB8_42dc_9581_A5FD5B4146FB_

#include <vector>
#include "framecommon/framecommon.h"
#include "basecommand.h"
#include "pingpeerdef.h"

using namespace std;
using namespace MYFRAMECOMMON;

struct TSynBaseCommand
{
	int m_nProtocolVersion;		//协议版本
	char m_nCmd;
	TSynBaseCommand()
	{
		m_nProtocolVersion = 256;
		m_nCmd = 0;
	}
	int Encode(CNewBufMgr &bufMgr)
	{
		int ret = 0;
		ret += bufMgr.PutInt(m_nProtocolVersion);
		ret += bufMgr.PutChar(m_nCmd);
		return ret;
	}
	int Decode(CNewBufMgr &bufMgr)
	{
		int ret = 0;
		ret += bufMgr.GetInt(m_nProtocolVersion);
		ret += bufMgr.GetChar(m_nCmd);
		return ret;
	}
	static int GetSize()
	{
		int iLength = 0;
		iLength += sizeof(int);
		iLength += sizeof(char);
		return iLength;
	}
	void Printf()
	{
		printf("version is %d\n", m_nProtocolVersion);
	}
};

template <class T> 
class CSynEncodeCommand
{
public:
	CSynEncodeCommand(char *pBuf, int nBufLen, T &out, char nCmdType, int nCmdVersion = SYN_VERSION)
		: m_outBuf(NULL, 0, 0, 0), m_out(out)
	{
		m_outHead.m_nProtocolVersion = nCmdVersion;
		m_outHead.m_nCmd = nCmdType;
		m_outBuf.AttachBuf(pBuf, nBufLen);
	}
	virtual ~CSynEncodeCommand(){}
	char * GetPacket()
	{
		return m_outBuf.GetBuf();
	}
	void Attach(char *pBuf, int nBufLen)
	{
		m_outBuf.AttachBuf(pBuf, nBufLen);
	}
	int GetPacketLen()
	{
		return m_outBuf.GetRealBufLen();
	}
	int Encode()
	{
		int ret=0;
		ret+=m_outBuf.PutInt(m_outHead.m_nProtocolVersion);
		ret+=m_outBuf.PutChar(m_outHead.m_nCmd);
		ret += m_out.Encode(m_outBuf);
		return ret;
	}
	CTmpBufMgr & GetBufMgr()
	{
		return m_outBuf;
	}
protected:	
	TSynBaseCommand m_outHead;
	CTmpBufMgr m_outBuf;
	T &m_out;
};

template <class T> 
class CSynDecodeCommand
{
public:
	static int GetCommandId(char *pBuf, int nBufLen) //zz
	{
		if(nBufLen < SYN_MSG_HEAD_LEN) 
		{
			WriteRunInfo::WriteLog("The cmd buf len is less than %d, "
				"too short", SYN_MSG_HEAD_LEN);
			return INVALID_CMD_ID;
		}

		unsigned char uchCmdId;
		memcpy(&uchCmdId, pBuf + SYN_CMDINFO_OFF, sizeof(uchCmdId));
		return uchCmdId;		//ntohs(uchCmdId);//根据情况看是否需要转换字节序
	}
	CSynDecodeCommand(char *pBuf, int nBufLen, T &in)
		: m_inBuf(NULL, 0, 0, 0), m_in(in)
	{
		m_inBuf.AttachBuf(pBuf, nBufLen);
		m_nIfDecode=0;
	}
	virtual ~CSynDecodeCommand(){}
	short GetMsgType()
	{
		return m_inHead.m_nCmd;
	}
	int Decode()
	{
		int ret=0;
		if (m_nIfDecode == 0)
		{
			ret+=m_inBuf.GetInt((int&)m_inHead.m_nProtocolVersion);
			ret+=m_inBuf.GetChar((char&)m_inHead.m_nCmd);

			ret+=m_in.Decode(m_inBuf);	
			m_nIfDecode = 1;
		}            
		return ret;
	}
	CBufMgr & GetBufMgr()
	{
		return m_inBuf;
	}
protected:
	int m_nIfDecode;
	TSynBaseCommand m_inHead;	
	CBufMgr m_inBuf;
	T &m_in;
};

struct TPingSynCmd
{
	TPingPeerInfo m_PeerInfo;
	//////////////////////////////////////////////////////////////////////////
	int Decode(CBufMgr &bufMgr);
	int Encode (CTmpBufMgr &bufMgr);
	int GetSize();
};

struct TLogoutSynCmd
{
	int nInternalIp;
	string strPeerId;
	//////////////////////////////////////////////////////////////////////////
	int Decode(CBufMgr &bufMgr);
	int Encode (CTmpBufMgr &bufMgr);
	int GetSize();
};

struct THandShakeCmd
{
	char nMachineType;
	short nMachineIndex;
	//////////////////////////////////////////////////////////////////////////
	int Decode(CBufMgr &bufMgr);
	int Encode (CTmpBufMgr &bufMgr);
	int GetSize();
};

struct TSynAffirmCmd
{
	char nMachineType;
	short nMachineIndex;
	//////////////////////////////////////////////////////////////////////////
	int Decode(CBufMgr &bufMgr);
	int Encode (CTmpBufMgr &bufMgr);
	int GetSize();
};

struct TOntimeSynCmd
{
	int nHashBucketLen;
	int nListNum;
	list<TPingPeerInfo> lstPeerList;
	//////////////////////////////////////////////////////////////////////////
	TOntimeSynCmd(int nHashBucketLen, int nListNum);
	int Decode(CBufMgr &bufMgr);
	int DecodeGetPeerList(CBufMgr &bufMgr, int &nHashNum, int &PeerNum);
	int Encode (CTmpBufMgr &bufMgr);
	int EncodeAddPeerList(CTmpBufMgr &bufMgr, int nHashNum, list<TPingPeerInfo> &lstPeerList);
private:
	TPingSynCmd m_PingSynCmd;
};

#endif


