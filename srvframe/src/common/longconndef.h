/*
* =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: longconndef.h
* Description	: 
* -----------------------------------------------------------------------
* Author        : Chen Hui (Buck), chenhui101@gmail.com
* Company		: 
* Date			: 2012-1-12
* -----------------------------------------------------------------------
* History		: 
*		<author>		  <time>		<version>		<description>
* -----------------------------------------------------------------------
*		Chen Hui		2012-1-12			1.0			
*		Wang Tiefeng	2012-02-17			1.0			Optimization
* =======================================================================
*/

#ifndef _LONGCONNDEF_H_
#define _LONGCONNDEF_H_
#include "framecommon/framecommon.h"
#include "commmsg.h"

using namespace std;
using namespace MYFRAMECOMMON;


const int LONGCONN_CMDID_C2S = 1000;
const int LONGCONN_CMDID_S2C = 1001;

//const variable
const int MAX_LONGCONTYPE_SIZE = 32;
const int MAX_LONGCONINDEX_SIZE = 128;

//app struct////////////////////////////////////////////////////
struct TLongConnectOption
{
	unsigned char  m_nLongConnType;
	unsigned short m_nLongConnIndex;
	int m_nDstEntityType;
	
	unsigned int m_nSrvIp;
	unsigned short m_nSrvPort;
	int m_nSequence;
	TLongConnectOption()
	{
		m_nSequence = 0;
	}
};

struct TLongConnectState
{
	int m_nResult;
	int m_nEntityType;
	int m_nEntityId;
	long long m_nTaskId;
	enum EChannelConnectState
	{
		CCR_INIT = 0,
		CCR_WAIT = 1,
		CCR_CONNECTSUC = 2,
		CCR_CREATESOCKET = 3,
		CCR_CONNECTFAIL = 4,
		CCR_CONNECTCLOSE = 5,
		CCR_RECVERROR = 6,
		CCR_SENDERROR = 7,
		CCR_TIMEOUT = 8,
	};
};

//msg body
class CLongConnectMsgBody : public CMsgBody
{
public:
	CLongConnectMsgBody(const TLongConnectOption& ConnectOption)
	{
		m_ConnectOption = ConnectOption;
	}
	virtual ~CLongConnectMsgBody()
	{

	}
public:
	virtual int Size()
	{
		return 0;
	}
	virtual CMsgBody* Clone()
	{
		CLongConnectMsgBody* pMsgBody = new CLongConnectMsgBody(m_ConnectOption);
		return pMsgBody;
	}
public:
	TLongConnectOption& GetConnectOption()
	{
		return m_ConnectOption;
	}
private:
	TLongConnectOption m_ConnectOption;
};


class CLongConnectStateMsgBody : public CMsgBody
{
public:
	CLongConnectStateMsgBody(const TLongConnectOption& ConnectOption, const TLongConnectState& ConnectState)
	{
		m_ConnectOption = ConnectOption;
		m_LongConnectState = ConnectState;
	}
	virtual ~CLongConnectStateMsgBody()
	{

	}
public:
	virtual int Size()
	{
		return 0;
	}
	virtual CMsgBody* Clone()
	{
		CLongConnectStateMsgBody* pMsgBody = new CLongConnectStateMsgBody(m_ConnectOption, m_LongConnectState);
		return pMsgBody;
	}
public:
	TLongConnectOption& GetConnectOption()
	{
		return m_ConnectOption;
	}
	TLongConnectState& GetConnectResult()
	{
		return m_LongConnectState;
	}
private:
	TLongConnectOption m_ConnectOption; 
	TLongConnectState m_LongConnectState;
};

struct TEntityLongConnInfo
{
	int m_nEntityType;
	int m_nEntitySize;
	vector<int> m_vecLongConnNum;	
};

struct TFrameLongConnectHead
{
	int m_nVersion;
	int m_nSequence;
	short m_nCmdId;
	int m_nSrcEntityType;
	int m_nSrcEntityId;
	long long m_nSrcTaskId;
	int m_nDstEntityType;
	int m_nDstEntityId;
	long long m_nDstTaskId;
	int m_nBodyLen;
	char szMsgBody[0];

	enum EFrameLongConnectHead
	{
		FLCH_SEQOFFSET = 4,
		FLCH_CMDOFFSET = 8,
		FLCH_BODYLENOFFSET = 42,
		FLCH_HEADLEN = 46
	};
	TFrameLongConnectHead()
	{
		Clear();
	}
	void Clear()
	{
		memset(this, 0, sizeof(TFrameLongConnectHead));
	}
	int GetSize()
	{
		return FLCH_HEADLEN;
	}
	static short GetCmdId(char* pBuf, int nBufLen)
	{
		if (nBufLen < FLCH_HEADLEN)
		{
			return -1;
		}
		return ntohs(*((short*)(pBuf + FLCH_CMDOFFSET)));
	}
	static int GetSequence(char* pBuf, int nBufLen)
	{
		if (nBufLen < FLCH_HEADLEN)
		{
			return -1;
		}
		return  ntohl(*((int*)(pBuf + FLCH_SEQOFFSET)));
	}
	static int GetBodyLen(char* pBuf, int nBufLen)
	{
		if (nBufLen < FLCH_HEADLEN)
		{
			return -1;
		}
		return  ntohl(*((int*)(pBuf + FLCH_BODYLENOFFSET)));
	}
	int Encode(CBufMgr& BufMgr)
	{
		int nRet = 0;
		nRet += BufMgr.PutInt(m_nVersion);
		nRet += BufMgr.PutInt(m_nSequence);
		nRet += BufMgr.PutShort(m_nCmdId);
		nRet += BufMgr.PutInt(m_nSrcEntityType);
		nRet += BufMgr.PutInt(m_nSrcEntityId);
		nRet += BufMgr.PutLong64(m_nSrcTaskId);
		nRet += BufMgr.PutInt(m_nDstEntityType);
		nRet += BufMgr.PutInt(m_nDstEntityId);
		nRet += BufMgr.PutLong64(m_nDstTaskId);
		nRet += BufMgr.PutInt(m_nBodyLen);
		return nRet;
	}	
	int Decode(CBufMgr& BufMgr)
	{
		int nRet = 0;
		nRet += BufMgr.GetInt(m_nVersion);
		nRet += BufMgr.GetInt(m_nSequence);
		nRet += BufMgr.GetShort(m_nCmdId);
		nRet += BufMgr.GetInt(m_nSrcEntityType);
		nRet += BufMgr.GetInt(m_nSrcEntityId);
		nRet += BufMgr.GetLong64(m_nSrcTaskId);
		nRet += BufMgr.GetInt(m_nDstEntityType);
		nRet += BufMgr.GetInt(m_nDstEntityId);
		nRet += BufMgr.GetLong64(m_nDstTaskId);
		nRet += BufMgr.GetInt(m_nBodyLen);
		return nRet;
	}
};

class CLongConnBufMsgBody : public CMsgBody
{
public:
	CLongConnBufMsgBody(char* pBuffer, int iBufLen, TFrameLongConnectHead* pHead = NULL) : m_pBuf(pBuffer), m_iBufLen(iBufLen), m_pHead(pHead)
	{

	}
	virtual ~CLongConnBufMsgBody()
	{
		if (m_pBuf)
		{
			delete [] m_pBuf;
		}
		if (m_pHead)
		{
			delete m_pHead;
		}
	}
public:
	virtual int Size()
	{
		return 0;
	}
	virtual CMsgBody* Clone()
	{
		return NULL;
	}

	char* GetBuf()
	{
		return m_pBuf;
	}
	int GetBufLen()
	{
		return m_iBufLen;
	}
	void DetachBuf()
	{
		m_pBuf = NULL;
		m_iBufLen = 0;
   	}
	TFrameLongConnectHead* GetHead()
	{
		return m_pHead;
	}
	void DetachHead()
	{
		m_pHead = NULL;
	}

private:
	char* m_pBuf;
	int m_iBufLen;
	TFrameLongConnectHead* m_pHead;
};

#endif



