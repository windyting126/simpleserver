/*
* =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: longconnmgr.cpp
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

#include "longconnmgr.h"
#include "threadgroup.h"

//static member
CLongConnMgr* CLongConnMgr::m_hInstance = NULL;

int CLongConnMgr::Create(CLongConnDispatchThread* pThread)
{
	int nRet = 0;
	if (!m_hInstance)
	{
		m_hInstance = new CLongConnMgr();
		nRet = m_hInstance->Init(pThread);
		if (nRet != 0)
		{
			delete m_hInstance;
			m_hInstance = NULL;
		}
	}
	return nRet;
}

int CLongConnMgr::Destory()
{
	if (m_hInstance)
	{
		delete m_hInstance;
		m_hInstance = NULL;
	}
	return 0;
}

char* CLongConnMgr::GenResPacket(char* pBuf, int iBufLen, TFrameLongConnectHead* pHead, int& iOutBufLen, int nLongConnMsgId /* = LONGCONN_CMDID_S2C */)
{
	if (!pBuf || iBufLen <= 0 || !pHead)
	{
		return NULL;
	}
	int iTmpBufLen = TFrameLongConnectHead::FLCH_HEADLEN + iBufLen;
	char* pTmpBuf = new char[iTmpBufLen];
	CBufMgr BufMgr(pTmpBuf, iTmpBufLen);
	TFrameLongConnectHead CmdHead;
	CmdHead.m_nVersion = pHead->m_nVersion;
	CmdHead.m_nSequence = pHead->m_nSequence;
	CmdHead.m_nCmdId = nLongConnMsgId;
	CmdHead.m_nSrcEntityType = pHead->m_nDstEntityType;
	CmdHead.m_nSrcEntityId = pHead->m_nDstEntityId;
	CmdHead.m_nSrcTaskId = pHead->m_nDstTaskId;
	CmdHead.m_nDstEntityType = pHead->m_nSrcEntityType;
	CmdHead.m_nDstEntityId = pHead->m_nSrcEntityId;
	CmdHead.m_nDstTaskId = pHead->m_nSrcTaskId;
	CmdHead.m_nBodyLen = iBufLen;
	CmdHead.Encode(BufMgr);
	BufMgr.PutCharBuf(pBuf, iBufLen);
	BufMgr.DistachBuf();
	iOutBufLen = iTmpBufLen;
	return pTmpBuf;
}

//member
CLongConnMgr::CLongConnMgr()
{
	m_pThread = NULL;
	memset(m_arrLongConnOption, 0, sizeof(m_arrLongConnOption));
	memset(m_arrLongConnState, 0, sizeof(m_arrLongConnState));
	m_nSequence = 0;
}

CLongConnMgr::~CLongConnMgr()
{
	
}

int CLongConnMgr::Init(CLongConnDispatchThread* pThread)
{
	if (!pThread)
	{
		return -1;
	}
	m_pThread = pThread;
	return 0;
}	

int CLongConnMgr::GetConnectState(int iType, int iIndex)
{
	if(iType < 0 ||  iType >= MAX_LONGCONTYPE_SIZE || iIndex < 0 || iIndex >= MAX_LONGCONINDEX_SIZE)
	{
		WriteRunInfo::WriteLog("CLongConnMgr::GetConnectState : Long Conn Type[%d] or Index[%d] out of range", iType, iIndex);
		return -1;
	}
	int nState = TLongConnectState::CCR_INIT;
	m_arrLongConnLock[iType][iIndex].GetReadLock();
	nState = m_arrLongConnState[iType][iIndex].m_nResult;
	m_arrLongConnLock[iType][iIndex].ReleaseLock();
	return nState;
}

int CLongConnMgr::SetEntitySize(int iEntityType, int iEntitySize)
{
	if (iEntityType < 0 || iEntitySize <= 0)
	{
		return -1;
	}
	CThreadLock ThreadLock(&m_lockEntityLongConnInfo);
	map<int, TEntityLongConnInfo>::iterator itrMap = m_mapEntityLongConnInfo.find(iEntityType);
	if (itrMap != m_mapEntityLongConnInfo.end())
	{
		return -1;
	}
	TEntityLongConnInfo EntityLongConnInfo;
	EntityLongConnInfo.m_nEntityType = iEntityType;
	EntityLongConnInfo.m_nEntitySize = iEntitySize;
	EntityLongConnInfo.m_vecLongConnNum.reserve(iEntitySize);
	for (int nA = 0; nA < iEntitySize; nA ++)
	{
		EntityLongConnInfo.m_vecLongConnNum.push_back(0);	
	}
	m_mapEntityLongConnInfo.insert(make_pair(iEntityType, EntityLongConnInfo));
	return 0;
}

int CLongConnMgr::GetEntityId(int iEntityType)
{
	if (iEntityType < 0)
	{
		return RANDOMENTITYID;
	}
	CThreadLock ThreadLock(&m_lockEntityLongConnInfo);
	map<int, TEntityLongConnInfo>::iterator itrMap = m_mapEntityLongConnInfo.find(iEntityType);
	if (itrMap == m_mapEntityLongConnInfo.end())
	{
		return RANDOMENTITYID;
	}
	int iEntitiId = 0;
	int iMinConnectNum = itrMap->second.m_vecLongConnNum[0];
	for (int nA = 1; nA < itrMap->second.m_nEntitySize; nA ++)
	{
		if (itrMap->second.m_vecLongConnNum[nA] < iMinConnectNum)
		{
			iMinConnectNum = itrMap->second.m_vecLongConnNum[nA];
			iEntitiId = nA;
		}
	}
	itrMap->second.m_vecLongConnNum[iEntitiId] ++;
	return iEntitiId;
}

int CLongConnMgr::DelEntityId(int iEntityType, int iEntityId)
{
	if (iEntityType < 0 || iEntityId < 0)
	{
		return -1;
	}
	CThreadLock ThreadLock(&m_lockEntityLongConnInfo);
	map<int, TEntityLongConnInfo>::iterator itrMap = m_mapEntityLongConnInfo.find(iEntityType);
	if (itrMap == m_mapEntityLongConnInfo.end())
	{
		return -1;
	}
	if (iEntityId >= itrMap->second.m_nEntitySize)
	{
		return -1;
	}
	itrMap->second.m_vecLongConnNum[iEntityId] --;
	return 0;
}

int CLongConnMgr::CreateConnect(const TLongConnectOption& ConnectOption)
{
	int iType = ConnectOption.m_nLongConnType;
	int iIndex = ConnectOption.m_nLongConnIndex;
	int iEntityType = ConnectOption.m_nDstEntityType;
	if (GetConnectState(iType, iIndex) != TLongConnectState::CCR_INIT)
	{
		WriteRunInfo::WriteLog("CLongConnMgr::CreateConnect : duplicate create connect[%d:%d]", iType, iIndex);
		return -1;
	}

	m_arrLongConnLock[iType][iIndex].GetWriteLock();

	m_arrLongConnOption[iType][iIndex] = ConnectOption;
	m_arrLongConnState[iType][iIndex].m_nEntityType = iEntityType;
	m_arrLongConnState[iType][iIndex].m_nEntityId = GetEntityId(iEntityType);
	
	SendConnect(iType, iIndex);

	m_arrLongConnLock[iType][iIndex].ReleaseLock();

	WriteRunInfo::WriteLog("CLongConnMgr::CreateConnect : Connect [%d][%d]", iType, iIndex);
	return 0;
}

int CLongConnMgr::DestoryConnect(int iType, int iIndex)
{
	if (GetConnectState(iType, iIndex) < 0)	
	{
		WriteRunInfo::WriteLog("CLongConnMgr::DestoryConnect : GetConnectState[%d][%d] fail", iType, iIndex);
		return -1;
	}

	m_arrLongConnLock[iType][iIndex].GetWriteLock();

	m_pThread->SendMsg(m_arrLongConnState[iType][iIndex].m_nEntityType, m_arrLongConnState[iType][iIndex].m_nEntityId, m_arrLongConnState[iType][iIndex].m_nTaskId, MSGTYPE_LONGCONN_DISCONNECT, NULL, 0);	
	DelEntityId(m_arrLongConnState[iType][iIndex].m_nEntityType, m_arrLongConnState[iType][iIndex].m_nEntityId);
	memset(&m_arrLongConnOption[iType][iIndex], 0, sizeof(TLongConnectOption));
	memset(&m_arrLongConnState[iType][iIndex], 0, sizeof(TLongConnectState));

	m_arrLongConnLock[iType][iIndex].ReleaseLock();

	WriteRunInfo::WriteLog("CLongConnMgr::DestoryConnect : DisConnect[%d][%d]", iType, iIndex);
	return 0;
}

int CLongConnMgr::ReConnect(const TLongConnectOption& ConnectOption)
{
	int iType = ConnectOption.m_nLongConnType;
	int iIndex = ConnectOption.m_nLongConnIndex;
	DestoryConnect(iType, iIndex);
	CreateConnect(ConnectOption);
	return 0;	
}

int CLongConnMgr::SendLongConnectMsg(CWorkThread* pThread, int iType, int iIndex, CLongConnBufMsgBody* pMsgBody, ULONG64 ulSrcTaskId)
{
	if (iType < 0 || iType >= MAX_LONGCONTYPE_SIZE || iIndex < 0 || iIndex >= MAX_LONGCONINDEX_SIZE)
	{
		WriteRunInfo::WriteLog("CLongConnMgr::SendLongConnectMsg : wrong type or index [%d:%d]", iType, iIndex);
		return -1;
	}

	int nRet = 0;
	m_arrLongConnLock[iType][iIndex].GetReadLock();
	if (m_arrLongConnState[iType][iIndex].m_nResult ==  TLongConnectState::CCR_CONNECTSUC)
	{
		pThread->SendMsg(m_arrLongConnState[iType][iIndex].m_nEntityType, m_arrLongConnState[iType][iIndex].m_nEntityId, m_arrLongConnState[iType][iIndex].m_nTaskId, MSGTYPE_LONGCONN_COMMON, pMsgBody, ulSrcTaskId);	
		nRet = 1;
	}
	m_arrLongConnLock[iType][iIndex].ReleaseLock();
	return nRet;
}

int CLongConnMgr::SendLongConnectMsg(CWorkThread* pThread, int iType, int iIndex, ULONG64 ulSrcTaskId, char* pBuf, int iBufLen, int nLongConnMsgId /*  = LONGCONN_CMDID_C2S */, int nVersion /*  = 0 */, int nSequenceNum /* = 0 */)
{
	if (iType < 0 || iType >= MAX_LONGCONTYPE_SIZE || iIndex < 0 || iIndex >= MAX_LONGCONINDEX_SIZE || !pBuf || iBufLen <= 0)
	{
		WriteRunInfo::WriteLog("CLongConnMgr::SendLongConnectMsg : wrong type or index [%d:%d]", iType, iIndex);
		return -1;
	}
	int nRet = 0;
	m_arrLongConnLock[iType][iIndex].GetReadLock();
	if (m_arrLongConnState[iType][iIndex].m_nResult ==  TLongConnectState::CCR_CONNECTSUC)
	{
		int iTmpBufLen = TFrameLongConnectHead::FLCH_HEADLEN + iBufLen;
		char* pTmpBuf = new char[iTmpBufLen];
		CBufMgr BufMgr(pTmpBuf, iTmpBufLen);
		TFrameLongConnectHead CmdHead;
		memset(&CmdHead, 0, sizeof(CmdHead));
		CmdHead.m_nVersion = nVersion;
		CmdHead.m_nSequence = nSequenceNum;
		CmdHead.m_nCmdId = nLongConnMsgId;
		CmdHead.m_nSrcEntityType = pThread->GetEntityType();
		CmdHead.m_nSrcEntityId = pThread->GetEntityId();
		CmdHead.m_nSrcTaskId = ulSrcTaskId;
		CmdHead.m_nDstEntityType = m_arrLongConnState[iType][iIndex].m_nEntityType;
		CmdHead.m_nDstEntityId = m_arrLongConnState[iType][iIndex].m_nEntityId;
		CmdHead.m_nDstTaskId = m_arrLongConnState[iType][iIndex].m_nTaskId;
		CmdHead.m_nBodyLen = iBufLen;
		CmdHead.Encode(BufMgr);
		BufMgr.PutCharBuf(pBuf, iBufLen);

		CLongConnBufMsgBody* pMsgBody = new CLongConnBufMsgBody(BufMgr.GetBuf(), BufMgr.GetBufLen());	
		BufMgr.DistachBuf();

		pThread->SendMsg(m_arrLongConnState[iType][iIndex].m_nEntityType, m_arrLongConnState[iType][iIndex].m_nEntityId, m_arrLongConnState[iType][iIndex].m_nTaskId, MSGTYPE_LONGCONN_COMMON, pMsgBody, ulSrcTaskId);	
		nRet = 1;
	}
	m_arrLongConnLock[iType][iIndex].ReleaseLock();
	return nRet;
}

int CLongConnMgr::OnTime()
{
	int nRet = 0;
	for(int i = 0; i < MAX_LONGCONTYPE_SIZE; i++)
	{
		for(int j = 0; j < MAX_LONGCONINDEX_SIZE; j++)
		{
			bool bIfUsed = false;
			m_arrLongConnLock[i][j].GetReadLock(); 
			if (m_arrLongConnOption[i][j].m_nSrvIp && m_arrLongConnOption[i][j].m_nSrvPort)	
			{
				bIfUsed = true;	
			}
			m_arrLongConnLock[i][j].ReleaseLock();
			if (bIfUsed)
			{
				OnTimeProcess(i, j);
			}
		}
	}
	return nRet;
}

int CLongConnMgr::OnTimeProcess(int iType, int iIndex)
{
	m_arrLongConnLock[iType][iIndex].GetReadLock(); 
	if (m_arrLongConnState[iType][iIndex].m_nResult == TLongConnectState::CCR_CONNECTSUC)	
	{
		SendPing(iType, iIndex);
	}
	else if (m_arrLongConnState[iType][iIndex].m_nResult != TLongConnectState::CCR_WAIT)
	{
		SendConnect(iType, iIndex);
	}
	m_arrLongConnLock[iType][iIndex].ReleaseLock();
	return 0;
}

int CLongConnMgr::SendPing(int iType, int iIndex)
{
	int nBufLen = TFrameLongConnectHead::FLCH_HEADLEN;
	char* pBuf = new char[nBufLen];
	CBufMgr BufMgr(pBuf, nBufLen);
	TFrameLongConnectHead CmdHead;
	memset(&CmdHead, 0, sizeof(CmdHead));
	CmdHead.Encode(BufMgr);

	CLongConnBufMsgBody* pMsgBody = new CLongConnBufMsgBody(BufMgr.GetBuf(), BufMgr.GetBufLen());	
	BufMgr.DistachBuf();

	m_pThread->SendMsg(m_arrLongConnState[iType][iIndex].m_nEntityType, m_arrLongConnState[iType][iIndex].m_nEntityId, m_arrLongConnState[iType][iIndex].m_nTaskId, MSGTYPE_LONGCONN_COMMON, pMsgBody, 0);
	return 0;
}

int CLongConnMgr::SendConnect(int iType, int iIndex)
{
	m_arrLongConnOption[iType][iIndex].m_nSequence = ++ m_nSequence;
	CLongConnectMsgBody* pMsgBody = new CLongConnectMsgBody(m_arrLongConnOption[iType][iIndex]);
	m_pThread->SendMsg(m_arrLongConnState[iType][iIndex].m_nEntityType, m_arrLongConnState[iType][iIndex].m_nEntityId, 0, MSGTYPE_LONGCONN_CONNECT, pMsgBody, 0);
	return 0;
}

int CLongConnMgr::OnConnectState(const TLongConnectOption& ConnectOption, const TLongConnectState& ConnectState)
{
	m_arrLongConnLock[ConnectOption.m_nLongConnType][ConnectOption.m_nLongConnIndex].GetWriteLock(); 
	if (ConnectOption.m_nSequence == m_arrLongConnOption[ConnectOption.m_nLongConnType][ConnectOption.m_nLongConnIndex].m_nSequence)
	{
		m_arrLongConnState[ConnectOption.m_nLongConnType][ConnectOption.m_nLongConnIndex] = ConnectState;
	}
	m_arrLongConnLock[ConnectOption.m_nLongConnType][ConnectOption.m_nLongConnIndex].ReleaseLock();
	return 0;
}




