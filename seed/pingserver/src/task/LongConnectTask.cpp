/*
* =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: LongConnectTask.cpp
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
#include <iostream>
using namespace std;

#include "PingServerDef.h"
#include "LongConnectTask.h"
#include "PingServer.h"
#include "protocol.h"
#include "commmsg.h"

int CLongConnectTask::ProcessPacket(PSERVERCOMMANDPACKET pServerCommandPacket)
{
	if (!pServerCommandPacket)
	{
		return -1;
	}

	if(htons(pServerCommandPacket->usMsgId) == ONLINE_SYN)
	{
		int nCurCmdId = GetCurCmdId((char *)pServerCommandPacket);
		if (nCurCmdId == MSG_LONGCONNECT_SYN_AFFIRMRES)
		{
			TMsg tMsg;
			tMsg.destEntityType = PINGONTIMESYN_ENTITY_TYPE;
			tMsg.srcEntityType= m_pThread->GetEntityType();
			tMsg.msgType = SYN_AFFIRM_MSGTYPE;

			char *pTmpBuf = new char[(int)ntohl(pServerCommandPacket->iMsgLen)];
			memcpy(pTmpBuf, pServerCommandPacket->serverCommandPacketContent, (int)ntohl(pServerCommandPacket->iMsgLen));

			CMsgBody * pMsgBody = new CBufMsgBody(pTmpBuf, (int)ntohl(pServerCommandPacket->iMsgLen));
			CMsg *pMsg = new CMsg(tMsg, pMsgBody);
			CMyserver::GetInstance()->PutMsgToThread(pMsg);
		}		
	}
	else
	{
		WriteRunInfo::WriteError( "%s:%d:%s : Ping Long Connect Task recv a wrong inner packet!" , __FILE__ , __LINE__ , __FUNCTION__ );
	}

	return 0;
}

int CLongConnectTask::SendProcess()
{
	int iSendFlag = 0;
	int retValue = 0;
	int nCurCmdId = 0;
	if (m_pCurSendMsg != NULL)
	{
		nCurCmdId = GetCurCmdId(m_pCurSendMsg->GetBody());
		if(nCurCmdId == -1)
			iSendFlag = -1;
		retValue = SendData();
		if(retValue == 1 && iSendFlag == 0)
			iSendFlag = 1;
	}
	else
	{
		m_pCurSendMsg = GetMsgFromSendList();
		if (m_pCurSendMsg != NULL)
		{
			nCurCmdId = GetCurCmdId(m_pCurSendMsg->GetBody());
			if(nCurCmdId == -1)
				iSendFlag = -1;
			retValue = SendData();
			if(retValue == 1 && iSendFlag == 0)
				iSendFlag = 1;		
		}                        
		else
		{
			int tmpEvent = MYPOLLERR|MYPOLLHUP|MYPOLLIN;
			CNetProcessThread *pNetThread = (CNetProcessThread*)m_pThread;
			CEpoll *pEoll = pNetThread->GetEpoll();
			pEoll->ModifyFromEpoll(this, tmpEvent);
			retValue = 1;
		}
	}
	if(iSendFlag == 1)
		if(nCurCmdId == MSG_LONGCONNECT_SYN_ONTIME)
		{
			CPingServer *pServer = (CPingServer *)CMyserver::GetInstance();
			if (pServer)
			{
				sem_post(pServer->GetSynSendSem());
				
			}
		}
	return retValue;
}

int CLongConnectTask::GetCurCmdId(char *pServerCommandPacket)
{
	if (!pServerCommandPacket)
	{
		return -1;
	}
	PSERVERCOMMANDPACKET pSrvCmdPacket = (PSERVERCOMMANDPACKET)pServerCommandPacket;
	if (pSrvCmdPacket->usMsgId == (unsigned)ntohs(-1) || pSrvCmdPacket->iMsgLen == 0)
	{
		return MSG_LONGCONNECT_SYN_PING; 
	}

	return *(pServerCommandPacket+ sizeof(SERVERCOMMANDPACKET) + sizeof(int));
}

