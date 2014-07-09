/*
* =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: longconnmgr.h
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
#ifndef _LONGCONNECTIONMGR_H_
#define _LONGCONNECTIONMGR_H_
#include <map>
#include "framecommon/framecommon.h"
#include "longconndef.h"
#include "longconndispatchthread.h"

using namespace std;
using namespace MYFRAMECOMMON;

class CLongConnMgr
{
friend class CLongConnDispatchThread;

public:
	static CLongConnMgr* GetInstance()
	{
		return m_hInstance;
	}
	static char* GenResPacket(char* pBuf, int iBufLen, TFrameLongConnectHead* pHead, int& iOutBufLen, int nLongConnMsgId = LONGCONN_CMDID_S2C);
private:
	static int Create(CLongConnDispatchThread* pThread);
	static int Destory();
private:
	CLongConnMgr();
	~CLongConnMgr();

private:
	int Init(CLongConnDispatchThread* pThread);

public:
	int SetEntitySize(int iEntityType, int iEntitySize);
	int GetEntityId(int iEntityType);
	int DelEntityId(int iEntityType, int iEntityId);
	int CreateConnect(const TLongConnectOption& ConnectOption);
	int DestoryConnect(int iType, int iIndex);
	int ReConnect(const TLongConnectOption& ConnectOption);
	int SendLongConnectMsg(CWorkThread* pThread, int iType, int iIndex, CLongConnBufMsgBody* pMsgBody, ULONG64 ulSrcTaskId);
	int SendLongConnectMsg(CWorkThread* pThread, int iType, int iIndex, ULONG64 ulSrcTaskId, char* pBuf, int iBufLen, int nLongConnMsgId = LONGCONN_CMDID_C2S, int nVersion = 0, int nSequenceNum = 0);
	int GetConnectState(int iType, int iIndex);
	
private:
	int OnTime();
	int OnTimeProcess(int iType, int iIndex);
	int OnConnectState(const TLongConnectOption& ConnectOption, const TLongConnectState& ConnectState);
	
	int SendConnect(int iType, int iIndex);
	int SendPing(int iType, int iIndex);
	
private:
	static CLongConnMgr* m_hInstance;
	CLongConnDispatchThread* m_pThread;

private:
	TLongConnectOption m_arrLongConnOption[MAX_LONGCONTYPE_SIZE][MAX_LONGCONINDEX_SIZE];
	TLongConnectState m_arrLongConnState[MAX_LONGCONTYPE_SIZE][MAX_LONGCONINDEX_SIZE];
	CThreadRwLock m_arrLongConnLock[MAX_LONGCONTYPE_SIZE][MAX_LONGCONINDEX_SIZE];
	map<int, TEntityLongConnInfo> m_mapEntityLongConnInfo;
	CThreadMutex m_lockEntityLongConnInfo;
	int m_nSequence;
};

#endif
