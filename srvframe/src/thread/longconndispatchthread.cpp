/*
* =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: LongConnDispatchThread.cpp
* Description	: 
* -----------------------------------------------------------------------
* Author        : Chen Hui (Buck), chenhui101@gmail.com
* Company		: 
* Date			: 2012-1-13
* -----------------------------------------------------------------------
* History		: 
*		<author>		  <time>		<version>		<description>
* -----------------------------------------------------------------------
*		Chen Hui		2012-1-13			1.0			
*		Wang Tiefeng	2012-02-17			1.0			Optimization
* =======================================================================
*/

#include "myserver.h"
#include "longconndispatchthread.h"
#include "longconnmgr.h"

//static member
int CLongConnDispatchThread::m_nObjectNum = 0;
CThreadMutex CLongConnDispatchThread::m_lockObjectNum;

//member
CLongConnDispatchThread::CLongConnDispatchThread(int nEntityType, int nEntityId) : CSynWorkThread(nEntityType, nEntityId)
{
	CLongConnMgr::Create(this);
}

CLongConnDispatchThread::~CLongConnDispatchThread()
{

}

int CLongConnDispatchThread::Init()
{
	CThreadLock ThreadLock(&m_lockObjectNum);
	m_nObjectNum ++;
	if (m_nObjectNum > 1)
	{
		return -1;
	}
	AddOneTimer(0, 1);
	return 0;
}


int CLongConnDispatchThread::Process(CMsg* pMsg)
{
	TMsg* pTMsg = pMsg->GetTMsg();
	int nRet = 0;
	switch(pTMsg->msgType)
	{
		case MSGTYPE_LONGCONN_CONNECT_RES:
			{
				nRet = ProcessConnectServerRes(pMsg);
				break;
			}
		case TIMERREACHTYPE:
			{
				nRet = ProcessDispatch();
				AddOneTimer(0, 1);
				break;
			}
		default:
			{
				WriteRunInfo::WriteError("CLongConnDispatchThread::Process : recv wrong msg %d", pTMsg->msgType);
				nRet = -1;
				break;
			}
	}
	return nRet;
}

int CLongConnDispatchThread::AddOneTimer(int nEvent, int nTimeLen)
{
	TTimeEvent TimeEvent;	
	TimeEvent.m_iTimerId = CTimer::GentTimerId();
	TimeEvent.m_nEvent = nEvent;
	TimeEvent.m_nTimerPoint = (CMyserver::GetInstance()->GetTimerPoint() + nTimeLen) % TIMERCYCLE;

	char* pBuf = new char[sizeof(TimeEvent)];
	memcpy(pBuf, &TimeEvent, sizeof(TimeEvent));
	CBufMsgBody* pMsgBody=  new CBufMsgBody(pBuf, sizeof(TimeEvent));
	SendMsg(TIMERENTITY, 0, 0, TIMERADDTYPE, pMsgBody, 0);
	return 0;
}

int CLongConnDispatchThread::ProcessConnectServerRes(CMsg* pMsg)
{
	CLongConnectStateMsgBody* pMsgBody = (CLongConnectStateMsgBody*) pMsg->GetMsgBody();
	CLongConnMgr::GetInstance()->OnConnectState(pMsgBody->GetConnectOption(), pMsgBody->GetConnectResult());
	return 0;
}

int CLongConnDispatchThread::ProcessDispatch()
{
	CLongConnMgr::GetInstance()->OnTime();
	return 0;
}

