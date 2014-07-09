/*
* =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: longconnsendandrecvthread.cpp
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
#include <string>

#include "commmsg.h"
#include "longconnsendandrecvthread.h"
#include "longconndef.h"
#include "longconnsendandrecvtask.h"

CLongConnSendAndRecvThread::CLongConnSendAndRecvThread(int nEntityType, int nEntityId) : CNetProcessThread(nEntityType, nEntityId)
{
	m_nLastCheckConnectTime = (long long)CMyserver::GetInstance()->GetTimeVal().tv_sec;
}

CLongConnSendAndRecvThread::~CLongConnSendAndRecvThread()
{

}

int CLongConnSendAndRecvThread::Process(CMsg* pMsg)
{
	TMsg* pTMsg = pMsg->GetTMsg();

	if(pTMsg->msgType == MSGTYPE_LONGCONN_COMMON)
	{
		CLongConnBufMsgBody* pMsgBody = (CLongConnBufMsgBody*)pMsg->GetMsgBody();
		if (!pMsgBody)
		{
			return 0;
		}	
		CSocketTask* pSocketTask = FindSocketTask(pTMsg->taskId);
		if (pSocketTask)
		{
			pSocketTask->PutMsgToSendList(pMsgBody->GetBuf(), pMsgBody->GetBufLen());
			pMsgBody->DetachBuf();
		}
	}
	else if (pTMsg->msgType == MSGTYPE_LONGCONN_CONNECT)
	{
		int nRet;
		CLongConnectMsgBody *pMsgBody = (CLongConnectMsgBody*)pMsg->GetMsgBody();
		if (!pMsgBody)
		{
			return 0;
		}						
		TLongConnectOption& LongConnectOption = pMsgBody->GetConnectOption();
		char szConnIp[INET_ADDRSTRLEN];
		int nIp = htonl(LongConnectOption.m_nSrvIp);
		inet_ntop(AF_INET, &nIp, szConnIp, sizeof(szConnIp));
		WriteRunInfo::WriteLog("Connect [type=%d,index=%d],[ip=%s,port=%d]", LongConnectOption.m_nLongConnType, LongConnectOption.m_nLongConnIndex, szConnIp, LongConnectOption.m_nSrvPort);
		CLongConnSendAndRecvTask* pSocketTask = (CLongConnSendAndRecvTask*)CMyserver::GetInstance()->GetClassFactory()->GenSocketTask(this, NULL, LONGCONNSENDANDRECVTASK);
		pSocketTask->Init();
		nRet = pSocketTask->ConnectServer(LongConnectOption);
		if (nRet != 0)
		{
			pSocketTask->SendConnectState(TLongConnectState::CCR_CREATESOCKET);
			delete pSocketTask;
		}
	}
	else if (pTMsg->msgType == MSGTYPE_LONGCONN_DISCONNECT)
	{
		CSocketTask* pSocketTask = FindSocketTask(pTMsg->taskId);
		if (pSocketTask)
		{
			delete pSocketTask;
		}
	}
	return 0;	
}

int CLongConnSendAndRecvThread::OtherProcess()
{
	CheckConnectValidity();
	return 0;
}

int CLongConnSendAndRecvThread::CheckConnectValidity()
{
	time_t nCurTime = CMyserver::GetInstance()->GetTimeVal().tv_sec;
	if (nCurTime > m_nLastCheckConnectTime)
	{
		int nRet = 0;
		map<ULONG64, CSocketTask*>::iterator itrMap = m_pMapSocketList.begin();
		while (itrMap != m_pMapSocketList.end())
		{
			map<ULONG64, CSocketTask*>::iterator itrCur = itrMap;
			itrMap ++;
			nRet += (itrCur->second)->CheckTimeOut();
		}
		m_nLastCheckConnectTime = nCurTime;
	}
	return 0;

}


