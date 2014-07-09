/*
* =======================================================================
* Copyright (c) 2011-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: RServerRecvAndSendTask.cpp
* Description	: 
* -----------------------------------------------------------------------
* Author        : zhang zhou
* Company		: 
* Date			: 2009-2-12
* -----------------------------------------------------------------------
* History		: 
*		<author>		  <time>		<version>		<description>
*      ---------------------------------------------------------------
*		zhang zhou		2009-02-12			1.0
*		wangtiefeng		2011-03-25			2.0			
* =======================================================================
*/
#include "RServerRecvAndSendTask.h"
#include "rsrvdef.h"
#include "mycommmsg.h"
#include "pingpeerhashtable.h"
#include <string>
#include "isrconlinecmd.h"
#include "rserver.h"
#include "myclassfactory.h"
using namespace std;

const int MAX_RETURN_BUFLEN = 2 * 1024;
RServerRecvAndSendTask::RServerRecvAndSendTask(CTcpSocket *pSocket, CNetProcessThread *pWorkThread,  
					   unsigned char ucMachineType, unsigned short usMachineIndex, 
					   bool bActive, int iListType, int iUploadInterval) 
					   : CPingSynRecvTask(pSocket, pWorkThread, ucMachineType, usMachineIndex, bActive, iListType)
{
	m_iUploadInterval = iUploadInterval;
	m_pReturnBuf = new char[MAX_RETURN_BUFLEN];
}

RServerRecvAndSendTask::~RServerRecvAndSendTask()
{
	delete [] m_pReturnBuf;
}

int RServerRecvAndSendTask::DestroySelf()
{
	delete this;
	return 0;
}

int RServerRecvAndSendTask::ProcessPacket(PSERVERCOMMANDPACKET pServerCommandPacket)
{
	unsigned int nClientPublicIp = ntohl(pServerCommandPacket->uSrcClientIp); 	
	if (ntohs(pServerCommandPacket->usMsgId) == ONLINE_SYN)
	{
		return ProcessOnlinePacket(((char*)pServerCommandPacket) + sizeof(SERVERCOMMANDPACKET), ntohl(pServerCommandPacket->iMsgLen));	
	}
	unsigned int nBufSize = ntohl(pServerCommandPacket->iMsgLen);

	CPRServer* pServer = (CPRServer*)CMyserver::GetInstance();
	CMyClassFactory* pClassFactory = (CMyClassFactory*)(pServer->GetClassFactory());
	CCommand* pCmd = pClassFactory->GenBaseCommand((char*)(pServerCommandPacket->serverCommandPacketContent), (int)nBufSize, nClientPublicIp);
	if (!pCmd)
	{
		return -1;
	}
	int nProcessRet = 0;
	int nRet = pCmd->Process(nProcessRet);
	if (nRet == 0)
	{
		int nRespPackSize = 0;
		char* pRespPackBuf = pCmd->GetResBuf(nRespPackSize);
		if (NULL != pRespPackBuf && 0 != nRespPackSize)
		{
			int nSrvInnerCmdPacketLen = (int)sizeof(SERVERINNERCOMMANDPACKET);	
			SERVERINNERCOMMANDPACKET srvCommHeader;			
			memset(&srvCommHeader, 0, nSrvInnerCmdPacketLen);	
			memcpy(&srvCommHeader.serverCommandPacket, pServerCommandPacket, sizeof(SERVERCOMMANDPACKET));
			srvCommHeader.serverCommandPacket.iDestProcessId = pServerCommandPacket->iSrcProcessId;
			srvCommHeader.serverCommandPacket.ullDestTaskId = pServerCommandPacket->ullSrcTaskId;
			srvCommHeader.serverCommandPacket.iMsgLen = htonl(nRespPackSize);	
			memcpy(m_pReturnBuf, &srvCommHeader, nSrvInnerCmdPacketLen);	
			memcpy(m_pReturnBuf + nSrvInnerCmdPacketLen, pRespPackBuf, nRespPackSize);
			PutMsgToSendList((const char*)m_pReturnBuf, nRespPackSize + nSrvInnerCmdPacketLen);
		}
		CCmdMsgBody* pBody = new CCmdMsgBody(pCmd, nProcessRet);
		m_pThread->SendMsg(MAINTAINENTITY, RANDOMENTITYID, 0, MSGTYPE_UPDATE_TO_DB, pBody, 0); 
	}
	else
	{
		delete pCmd;
	}
	return nRet;
}

