/*
* =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: PingLongConnectThread.cpp
* Description	: 
* -----------------------------------------------------------------------
* Author        : wangtiefeng (wtf), wangtf418@163.com
* Company		: 
* Date			: 2009-08-03
* -----------------------------------------------------------------------
* History		: 
*		<author>		  <time>		<version>		<description>
*      ---------------------------------------------------------------
*		wangtiefeng		2009-08-03			1.0			
* =======================================================================
*/
#include "PingLongConnectThread.h"

#include "PingServerDef.h"
#include "commmsg.h"
#include "LongConnectTask.h"

int CPingLongConnectThread::Process(CMsg *pMsg)
{
	TMsg *pTMsg = pMsg->GetTMsg();
	if ( !pTMsg )
	{
		return 0;
	}

	if (pTMsg->srcEntityType == EPOLLREACTORENTITY && pTMsg->msgType == CONNECTMSGTYPE)
	{
			CConnectMsgBody *pBody = (CConnectMsgBody*)pMsg->GetMsgBody();
			if (!pBody)
			{
				return 0;
			}
			if (pBody->GetState() == 0)
			{
				//////////////////////////////////////////////////////////////////////////
				std::string tmpStr = pBody->GetRemoteIp();
				CTcpSocket *pSocket = new CTcpSocket(tmpStr, pBody->GetRemotePort(), pBody->GetSocket());
				if (!pSocket)
				{
					return 0;
				}
				CSocketTask *tmpSocketTask = NULL;
				CCommonTaskArg commonTaskArg(pBody->GetDestMachineType(), pBody->GetDestMachineIndex(), true, m_iListType);
				tmpSocketTask = CMyserver::GetInstance()->GetClassFactory()->GenSocketTask(this,  
					pSocket, COMMONTASK, &commonTaskArg);
				tmpSocketTask->Init();
				WriteRunInfo::WriteLog("connect to %d %d %d succ ", (int)pBody->GetDestMachineType(), (int)pBody->GetDestMachineIndex(), pBody->GetSocket());

				//长连接重新连接成功，向定时同步线程发送重新同步消息
				char *pMachineIndex = new char[sizeof(char) + sizeof(int)];
				*(unsigned char *)pMachineIndex = pBody->GetDestMachineType();
				*(unsigned short *)(pMachineIndex + 1) = pBody->GetDestMachineIndex();
				CBufMsgBody *pCMsgBody = new CBufMsgBody(pMachineIndex, sizeof(int));
				SendMsg(PINGONTIMESYN_ENTITY_TYPE, RANDOMENTITYID, 0, RESYN_MSGTYPE, (CMsgBody *)pCMsgBody);
			}
			else
			{
				WriteRunInfo::WriteLog("EPOLLREACTORENTITY reconnect %d %d", (int)pBody->GetDestMachineType(), (int)pBody->GetDestMachineIndex());
				if (m_pEpollReactor)
				{
					m_pEpollReactor->AsyncConnect(pBody->GetRemoteIp(), pBody->GetRemotePort(), LONGCONNECTEDENTITY, 0, GetTime(3), pBody->GetDestMachineType(), pBody->GetDestMachineIndex());
				}
			}
	}
	else if (pTMsg->srcEntityType == UDPSENDANDPROCESSENTITY || pTMsg->srcEntityType == PINGONTIMESYN_ENTITY_TYPE || pTMsg->srcEntityType == CHECKTIMEOUT_ENTITY_TYPE )
	{
		if (pTMsg->msgType == SYN_MSGTYPE || pTMsg->msgType == ONTIMESYN_MSGTYPE)
		{
			CClientMsgBody *pBody = (CClientMsgBody *)pMsg->GetMsgBody();
			if ( !pBody )
			{
				return 0;
			}
			if (m_iListType)
			{
				std::list<CSocketTask*>::iterator it = m_pSocketList.begin();
				while (it != m_pSocketList.end())
				{
					CSocketTask * pSocketTask = (*it);
					if (pSocketTask)
					{
						pSocketTask->PutMsgToSendList(pBody->GetBuffer(), pBody->GetLen());
					}
					it ++;
				}
			}
			else
			{
				map<ULONG64, CSocketTask*>::iterator it = m_pMapSocketList.begin();
				while(it != m_pMapSocketList.end())
				{
					CSocketTask * pSocketTask = (*it).second;
					if (pSocketTask)
					{
						pSocketTask->PutMsgToSendList(pBody->GetBuffer(), pBody->GetLen());
					}
					it ++;
				}
			}
		}
	}
	else return CLongConnectedThread::Process(pMsg);
	return 0;
}

string CPingLongConnectThread::GetSendListLog()
{
	stringstream statStr;
	statStr.str("");
	int nA = 0;
	if (m_iListType)
	{
		std::list<CSocketTask*>::iterator it = m_pSocketList.begin();
		while (it != m_pSocketList.end())
		{
			CSocketTask * pSocketTask = (*it);
			if (pSocketTask)
			{
				statStr << "[" << nA << ":" << ((CLongConnectTask*) pSocketTask)->GetSendListLen() << "]";
			}
			it++;
			nA++;
		}
	}
	else
	{
		map<ULONG64, CSocketTask*>::iterator it = m_pMapSocketList.begin();
		while(it != m_pMapSocketList.end())
		{
			CSocketTask * pSocketTask = (*it).second;
			if (pSocketTask)
			{
				statStr << "[" << nA << ":" << ((CLongConnectTask*) pSocketTask)->GetSendListLen() << "]";
			}
			it++;
			nA++;
		}
	}
	statStr << "\n";
	return statStr.str();
}

