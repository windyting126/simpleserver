/*
* =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: longconnrecvandsendthread.cpp
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
#include "longconnrecvandsendthread.h"
#include "longconndef.h"
#include "longconnrecvandsendtask.h"

CLongConnRecvAndSendThread::CLongConnRecvAndSendThread(int nEntityType, int nEntityId) : CNetProcessThread(nEntityType, nEntityId)
{

}

CLongConnRecvAndSendThread::~CLongConnRecvAndSendThread()
{

}

int CLongConnRecvAndSendThread::Process(CMsg* pMsg)
{
	TMsg* pTMsg = pMsg->GetTMsg();

	if (pTMsg->msgType == MSGTYPE_LONGCONN_COMMON_RES)
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
	else if (pTMsg->msgType == MSGTYPE_LONGCONN_ACCEPT)
	{
		CRecvMsgBody* pBody = (CRecvMsgBody*)pMsg->GetMsgBody();
		CTcpSocket* pSocket = new CTcpSocket(pBody->GetSockAddr(), sizeof(sockaddr_in), pBody->GetSocket());
		CSocketTask* pTmpSocketTask = NULL;
		pTmpSocketTask = CMyserver::GetInstance()->GetClassFactory()->GenSocketTask(this, pSocket, LONGCONNRECVANDSENDTASK);
		pTmpSocketTask->Init();
		pTmpSocketTask->SetTimeStat(pBody->GetTimeStat());
	}
	return 0;
}



