/****************************************************************************
**
**  Copyright  (c)  2008-2009,  Baofeng, Inc.  All Rights Reserved.
**
**  Subsystem:    ConnectServer
**  File:         	     EpollReactor.cpp
**  Created By:   fwx
**  Created On:   2009/01/08
**
**  Purpose:
**     connect to remote computer
**
**  History:
**  Programmer      Date         Version        Number      Description
**  --------------- --------   ---------- ------------------------------
**    fwx       2009/01/08         1.0            01          creation
****************************************************************************/
#include <sys/types.h>
#include <sys/socket.h>
#include "framecommon/framecommon.h"
#include "myserver.h"
#include "commmsg.h"
#include "EpollReactor.h"
using namespace MYFRAMECOMMON;

#define SEND_PING_INTERVAL 3

#define MAXCLIENT 1024
#define SCAN_TIME 1000

#define EPOLLREACTOR_MSG_CONNECT 0
#define EPOLLREACTOR_MSG_RECONNECT 1
#define EPOLLREACTOR_MSG_DISCONNECT 2
#define EPOLLREACTOR_MSG_LOSTCONNECT 3

//static member
CEpollReactor *CEpollReactor::m_hInstance = NULL;
CThreadMutex CEpollReactor::m_lockInstance;

CEpollReactor * CEpollReactor::GetInstance()
{
	CEpollReactor *pTmp = NULL;
	m_lockInstance.Lock();
	if (!m_hInstance)
	{
		m_hInstance = new CEpollReactor;
		if (m_hInstance->StartService() != 0)
		{
			delete m_hInstance;
			m_hInstance = NULL;
		}
	}
	pTmp = m_hInstance;
	m_lockInstance.UnLock();
	return pTmp;
}

void * CEpollReactor::WorkThreadFunction(void *pArg)
{
	CEpollReactor *pThis = (CEpollReactor *)pArg;
	pThis->WorkProcess();
	return NULL;
}

//member
CEpollReactor::CEpollReactor()
	: m_iEpFd(-1)
	, m_bIsStop(false)
	, m_bThreadOk(false)
{
	m_nPingInterval = SEND_PING_INTERVAL;
}

CEpollReactor::~CEpollReactor()
{
	StopService();
	
	if (m_iEpFd > 0)
	{
		close(m_iEpFd);
	}
}

int CEpollReactor::StartService()
{
	if (m_bThreadOk)
	{
		return 0;	//return 0 for success
	}

	m_iEpFd = epoll_create(MAXCLIENT);
	if (m_iEpFd <= 0)
	{
		return -1;	//return -1 for error(using 0 for error is not a good choice)
	}
	
	if(pthread_create(&m_iThreadId, NULL, CEpollReactor::WorkThreadFunction, this) != 0)
	{
		close(m_iEpFd);
		m_iEpFd = -1;
		return -1;	//return -1 for error(using 0 for error is not a good choice)
	}
	m_bThreadOk = true;

	return 0;		//return 0 for success
}

void CEpollReactor::StopService( void )
{
	if ( m_bIsStop ) return;
	m_bIsStop = true;
}

int CEpollReactor::AsyncConnect(const char *pcszRemoteIp, unsigned short usRemotePort,
								int iEntityType, int iEntityId,
								unsigned long long ullExpireTime,
								unsigned char ucMachineType, unsigned short usMachineIndex)
{
	if (!pcszRemoteIp)
	{
		WriteRunInfo::WriteError("Epoll Reactor : pcszRemoteIp is NULL");
		return -1;
	}
	return Connect(ucMachineType, usMachineIndex, pcszRemoteIp, usRemotePort, 3, iEntityType, iEntityId);
}

int CEpollReactor::Connect(unsigned char nMachineType, unsigned short nMachineIndex, const string &strRemoteIp, unsigned short nRemotePort, unsigned long long nConnectTimeout, int nEntityType, int nEntityId)
{
	if (strRemoteIp == "")
	{
		WriteRunInfo::WriteError("Epoll Reactor Connect: no Remote Ip");
		return -1;
	}
	TEpollConnectMsgBody *pMsgBody = new TEpollConnectMsgBody;
	pMsgBody->m_nMachineType = nMachineType;
	pMsgBody->m_nMachineIndex = nMachineIndex;
	pMsgBody->m_nSrcEntityType = nEntityType;
	pMsgBody->m_nSrcEntityId = nEntityId;
	pMsgBody->m_strSrvIp = strRemoteIp;
	pMsgBody->m_nSrvPort = nRemotePort;
	pMsgBody->m_nConnectTimeout = nConnectTimeout;
	CEpollMsg *pMsg = new CEpollMsg(EPOLLREACTOR_MSG_CONNECT, pMsgBody);
	m_msgQueue.Put(pMsg);
	WriteRunInfo::WriteLog("LongConnect : Connect [%u,%u] ip=%s,port=%u [%d,%d]", nMachineType, nMachineIndex, strRemoteIp.c_str(), nRemotePort, nEntityType, nEntityId);
	return 0;
}

int CEpollReactor::ReConnect(unsigned char nMachineType, unsigned short nMachineIndex, const string &strRemoteIp, unsigned short nRemotePort, unsigned long long nConnectTimeout)
{
	if (strRemoteIp == "")
	{
		WriteRunInfo::WriteError("Epoll Reactor ReConnect: no Remote Ip");
		return -1;
	}
	TEpollReConnectMsgBody *pMsgBody = new TEpollReConnectMsgBody;
	pMsgBody->m_nMachineType = nMachineType;
	pMsgBody->m_nMachineIndex = nMachineIndex;
	pMsgBody->m_strSrvIp = strRemoteIp;
	pMsgBody->m_nSrvPort = nRemotePort;
	pMsgBody->m_nConnectTimeout = nConnectTimeout;
	CEpollMsg *pMsg = new CEpollMsg(EPOLLREACTOR_MSG_RECONNECT, pMsgBody);
	m_msgQueue.Put(pMsg);
	WriteRunInfo::WriteLog("LongConnect : ReConnect [%u,%u] ip=%s,port=%u", nMachineType, nMachineIndex, strRemoteIp.c_str(), nRemotePort);
	return 0;
}

int CEpollReactor::DisConnect(unsigned char nMachineType, unsigned short nMachineIndex)
{
	TEpollDisConnectMsgBody *pMsgBody = new TEpollDisConnectMsgBody;
	pMsgBody->m_nMachineType = nMachineType;
	pMsgBody->m_nMachineIndex = nMachineIndex;
	CEpollMsg *pMsg = new CEpollMsg(EPOLLREACTOR_MSG_DISCONNECT, pMsgBody);
	m_msgQueue.Put(pMsg);
	WriteRunInfo::WriteLog("LongConnect : DisConnect [%u,%u]", nMachineType, nMachineIndex);
	return 0;
}

int CEpollReactor::LostConnect(unsigned char nMachineType, unsigned short nMachineIndex)
{
	TEpollLostConnectMsgBody *pMsgBody = new TEpollLostConnectMsgBody;
	pMsgBody->m_nMachineType = nMachineType;
	pMsgBody->m_nMachineIndex = nMachineIndex;
	CEpollMsg *pMsg = new CEpollMsg(EPOLLREACTOR_MSG_LOSTCONNECT, pMsgBody);
	m_msgQueue.Put(pMsg);
	WriteRunInfo::WriteLog("LongConnect : Lost Connect [%u,%u]", nMachineType, nMachineIndex);
	return 0;
}

void CEpollReactor::WorkProcess()
{
	long long nLastSendPing = 0;
	while (!m_bIsStop)
	{
		CEpollMsg *pMsg = NULL;
		while (m_msgQueue.Get(pMsg))
		{
			if (!pMsg)
			{
				WriteRunInfo::WriteError("Epoll Reactor : NULL msg");
				continue;
			}
			Process(pMsg);
			delete pMsg;
		}
		Dispatch();

		long long nCurTime = CMyserver::GetInstance()->GetTimeVal().tv_sec;
		if (nCurTime - nLastSendPing >= m_nPingInterval)
		{
			nLastSendPing = nCurTime;
			SendPing();
		}
	}
	return;
}

int CEpollReactor::Process(CEpollMsg *pMsg)
{
	int nRet = 0;
	switch(pMsg->GetMsgType())
	{
		case EPOLLREACTOR_MSG_CONNECT:
			{
				nRet = ProcessConnect(pMsg);
				break;
			}
		case EPOLLREACTOR_MSG_RECONNECT:
			{
				nRet = ProcessReConnect(pMsg);
				break;
			}
		case EPOLLREACTOR_MSG_DISCONNECT:
			{
				nRet = ProcessDisConnect(pMsg);
				break;
			}
		case EPOLLREACTOR_MSG_LOSTCONNECT:
			{
				nRet = ProcessLostConnect(pMsg);
				break;
			}
		default:
			{
				nRet = -1;
				WriteRunInfo::WriteError("Epoll Rector recv error msg = %d", pMsg->GetMsgType());
				break;
			}
	}
	return nRet;
}

int CEpollReactor::ProcessConnect(CEpollMsg *pMsg)
{
	TEpollConnectMsgBody *pMsgBody = (TEpollConnectMsgBody*)pMsg->GetMsgBody();
	TConDstSrvInfo ConInfo;
	ConInfo.m_nServerType = pMsgBody->m_nMachineType;
	ConInfo.m_nServerIndex = pMsgBody->m_nMachineIndex;

	
	map<TConDstSrvInfo, TConnectItem>::iterator itrMap = m_mapConnect.find(ConInfo);
	if (itrMap != m_mapConnect.end())
	{
		WriteRunInfo::WriteError("Epoll Reactor Connect : duplicate connect [%u,%u]", ConInfo.m_nServerType, ConInfo.m_nServerIndex);
		return -1;
	}
	else
	{
		map<TConDstSrvInfo, TConnectItem>::iterator itrMapDis = m_mapDisConnect.find(ConInfo);
		if (itrMapDis != m_mapDisConnect.end())
		{
			m_mapDisConnect.erase(itrMapDis);
		}

		TConnectItem ConnectItem(pMsgBody->m_nMachineType,
				pMsgBody->m_nMachineIndex,
				pMsgBody->m_strSrvIp,
				pMsgBody->m_nSrvPort,
				pMsgBody->m_nConnectTimeout,
				pMsgBody->m_nSrcEntityType,
				pMsgBody->m_nSrcEntityId);
		m_mapConnect.insert(make_pair(ConInfo, ConnectItem));
	}
	return 0;
}

int CEpollReactor::ProcessReConnect(CEpollMsg *pMsg)
{
	TEpollReConnectMsgBody *pMsgBody = (TEpollReConnectMsgBody*)pMsg->GetMsgBody();
	TConDstSrvInfo ConInfo;
	ConInfo.m_nServerType = pMsgBody->m_nMachineType;
	ConInfo.m_nServerIndex = pMsgBody->m_nMachineIndex;
	map<TConDstSrvInfo, TConnectItem>::iterator itrMap = m_mapConnect.find(ConInfo);
	if (itrMap == m_mapConnect.end())
	{
		WriteRunInfo::WriteError("Epoll Reactor ReConnect : not find [%u,%u]", ConInfo.m_nServerType, ConInfo.m_nServerIndex);
		return -1;
	}
	itrMap->second.m_sRemoteIp = pMsgBody->m_strSrvIp;
	itrMap->second.m_usRemotePort = pMsgBody->m_nSrvPort;
	itrMap->second.m_ullExpireTime = pMsgBody->m_nConnectTimeout;

	if (itrMap->second.m_nState == TConnectItem::EIS_ESTABLISH)
	{
		SendDestoryConnectMsg(itrMap->second.m_ucMachineType, itrMap->second.m_usMachineIndex, itrMap->second.m_iDestEntityType, itrMap->second.m_iDestEntityId);
	}
	else
	{
		if (itrMap->second.m_nSocketId >= 0)
		{
			struct epoll_event ev = {0, {0}};
			ev.data.ptr = &(itrMap->second);
			epoll_ctl(m_iEpFd, EPOLL_CTL_DEL, itrMap->second.m_nSocketId, &ev);
			close(itrMap->second.m_nSocketId);
			itrMap->second.m_nSocketId = -1;
		}
		itrMap->second.m_nTryConnTime = GetTime(0);
	}
	return 0;
}

int CEpollReactor::ProcessDisConnect(CEpollMsg *pMsg) 
{
	TEpollLostConnectMsgBody *pMsgBody = (TEpollLostConnectMsgBody*)pMsg->GetMsgBody();
	TConDstSrvInfo ConInfo;
	ConInfo.m_nServerType = pMsgBody->m_nMachineType;
	ConInfo.m_nServerIndex = pMsgBody->m_nMachineIndex;

	map<TConDstSrvInfo, TConnectItem>::iterator itrMapDis = m_mapDisConnect.find(ConInfo);
	if (itrMapDis != m_mapDisConnect.end())
	{
		WriteRunInfo::WriteError("Epoll Reactor DisConnect : duplicate disconnect [%u,%u]", ConInfo.m_nServerType, ConInfo.m_nServerIndex);
		return -1;
	}

	map<TConDstSrvInfo, TConnectItem>::iterator itrMap = m_mapConnect.find(ConInfo);
	if (itrMap == m_mapConnect.end())
	{
		WriteRunInfo::WriteError("Epoll Reactor DisConnect : not find connect [%u,%u]", ConInfo.m_nServerType, ConInfo.m_nServerIndex);
		return -1;
	}
	if (itrMap->second.m_nState == TConnectItem::EIS_ESTABLISH)
	{
		SendDestoryConnectMsg(itrMap->second.m_ucMachineType, itrMap->second.m_usMachineIndex, itrMap->second.m_iDestEntityType, itrMap->second.m_iDestEntityId);
		m_mapDisConnect.insert(make_pair(ConInfo, itrMap->second));
		m_mapConnect.erase(itrMap);
	}
	else
	{
		if (itrMap->second.m_nSocketId >= 0)
		{
			struct epoll_event ev = {0, {0}};
			ev.data.ptr = &(itrMap->second);
			epoll_ctl(m_iEpFd, EPOLL_CTL_DEL, itrMap->second.m_nSocketId, &ev);
			close(itrMap->second.m_nSocketId);
		}
		m_mapConnect.erase(itrMap);
	}
	return 0;
}

int CEpollReactor::ProcessLostConnect(CEpollMsg *pMsg) 
{
	TEpollLostConnectMsgBody *pMsgBody = (TEpollLostConnectMsgBody*)pMsg->GetMsgBody();
	TConDstSrvInfo ConInfo;
	ConInfo.m_nServerType = pMsgBody->m_nMachineType;
	ConInfo.m_nServerIndex = pMsgBody->m_nMachineIndex;
	
	map<TConDstSrvInfo, TConnectItem>::iterator itrMap = m_mapConnect.find(ConInfo);
	if (itrMap != m_mapConnect.end())
	{
		itrMap->second.m_nState = TConnectItem::EIS_INIT;
		itrMap->second.m_nTryConnTime = GetTime(0);
		itrMap->second.m_nSocketId = -1;
	}
	else
	{
		map<TConDstSrvInfo, TConnectItem>::iterator itrMapDis = m_mapDisConnect.find(ConInfo);
		if (itrMapDis != m_mapDisConnect.end())
		{
			m_mapDisConnect.erase(itrMapDis);		
			WriteRunInfo::WriteLog("Epoll Reactor DisConnect : success [%u,%u]", ConInfo.m_nServerType, ConInfo.m_nServerIndex);
		}
		else
		{
			WriteRunInfo::WriteError("Epoll Reactor Lost Connect : not find connect [%u,%u]", ConInfo.m_nServerType, ConInfo.m_nServerIndex);
			return -1;
		}
	}
	return 0;
}

int CEpollReactor::Dispatch()
{
	DispatchConnect();
	EpollWait();
	CheckTimeout();
	return 0;
}

int CEpollReactor::SendPing()
{
	map<TConDstSrvInfo, TConnectItem>::iterator itrMap = m_mapConnect.begin();
	for (; itrMap != m_mapConnect.end(); itrMap ++)
	{
		if (itrMap->second.m_nState == TConnectItem::EIS_ESTABLISH)
		{
			SendPing(itrMap->second.m_ucMachineType, itrMap->second.m_usMachineIndex);
		}
	}
	return 0;
}

int CEpollReactor::CheckTimeout()
{
	map<TConDstSrvInfo, TConnectItem>::iterator itrMap = m_mapConnect.begin();
	for (; itrMap != m_mapConnect.end(); itrMap ++)
	{
		if ((itrMap->second.m_nState == TConnectItem::EIS_ESTABLISH) || (itrMap->second.m_nSocketId < 0))
		{
			continue;
		}
		else if (GetTime(0) - itrMap->second.m_nTryConnTime > itrMap->second.m_ullExpireTime)
		{
			struct epoll_event ev = {0, {0}};
			ev.data.ptr = &(itrMap->second);
			epoll_ctl(m_iEpFd, EPOLL_CTL_DEL, itrMap->second.m_nSocketId, &ev);
			close(itrMap->second.m_nSocketId);
			itrMap->second.m_nSocketId = -1;
			itrMap->second.m_nTryConnTime = GetTime(itrMap->second.m_ullExpireTime);

			WriteRunInfo::WriteLog("LongConnect : Timeout [%u,%u] ip=%s,port=%u [%d,%d]", itrMap->second.m_ucMachineType, itrMap->second.m_usMachineIndex, itrMap->second.m_sRemoteIp.c_str(), itrMap->second.m_usRemotePort, itrMap->second.m_iDestEntityType, itrMap->second.m_iDestEntityId);
		}
	}
	return 0;
}

int CEpollReactor::DispatchConnect()
{
	map<TConDstSrvInfo, TConnectItem>::iterator itrMap = m_mapConnect.begin();
	for (; itrMap != m_mapConnect.end(); itrMap ++)
	{
		if (itrMap->second.m_nState != TConnectItem::EIS_INIT || itrMap->second.m_nSocketId >= 0)
		{
			continue;
		}
		if (itrMap->second.m_nTryConnTime > GetTime(0))
		{
			continue;
		}
		itrMap->second.m_nSocketId = socket(AF_INET, SOCK_STREAM, 0);
		if (itrMap->second.m_nSocketId < 0)
		{
			itrMap->second.m_nTryConnTime = GetTime(itrMap->second.m_ullExpireTime);
		}
		unsigned long nIp;
		if (SetNoBlock(itrMap->second.m_nSocketId) != 0 || inet_pton(AF_INET, itrMap->second.m_sRemoteIp.c_str(), &nIp) <= 0) 
		{
			close(itrMap->second.m_nSocketId);
			itrMap->second.m_nSocketId = -1;
			itrMap->second.m_nTryConnTime = GetTime(itrMap->second.m_ullExpireTime);
			continue;
		}
		itrMap->second.m_nTryConnTime = GetTime(0);

		sockaddr_in servaddr;
		memset(&servaddr , 0, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_addr.s_addr = nIp;
		servaddr.sin_port = htons(itrMap->second.m_usRemotePort);

		connect(itrMap->second.m_nSocketId, (struct sockaddr*)&servaddr, sizeof(servaddr));

		epoll_event ev = {0, {0}};
		ev.data.ptr = &(itrMap->second);
		ev.events = EPOLLOUT;
		epoll_ctl(m_iEpFd, EPOLL_CTL_ADD, itrMap->second.m_nSocketId, &ev);

		WriteRunInfo::WriteLog("LongConnect : Start Connect [%u,%u] ip=%s,port=%u [%d,%d]", itrMap->second.m_ucMachineType, itrMap->second.m_usMachineIndex, itrMap->second.m_sRemoteIp.c_str(), itrMap->second.m_usRemotePort, itrMap->second.m_iDestEntityType, itrMap->second.m_iDestEntityId);
	}
	return 0;
}

int CEpollReactor::EpollWait()
{
	epoll_event events[MAXCLIENT];
	int nFdNum = epoll_wait(m_iEpFd, events, MAXCLIENT, SCAN_TIME);
	if (nFdNum < 0)
	{
		if(EINTR == errno) return 0;
		WriteRunInfo::WriteError("epoll_wait return -1 error %d", errno);
		return -1;
	}
	for (int nA = 0; nA < nFdNum; nA ++)
	{
		int nSocketId = ((TConnectItem *)(events[nA].data.ptr))->m_nSocketId;
		TConnectItem *pConnectItem = (TConnectItem *)events[nA].data.ptr;
		
		//delete from epoll
		struct epoll_event ev = {0, {0}};
		ev.data.ptr = pConnectItem;
		epoll_ctl(m_iEpFd, EPOLL_CTL_DEL, nSocketId, &ev);

		if (events[nA].events & EPOLLOUT)
		{
			int nError;
			socklen_t nSockLen = sizeof(nError);
			if ((getsockopt(pConnectItem->m_nSocketId, SOL_SOCKET, SO_ERROR, &nError, &nSockLen) == 0) && (nError == 0))
			{
				pConnectItem->m_nState = TConnectItem::EIS_ESTABLISH;
				SendConnectMsg(pConnectItem->m_nSocketId,
						pConnectItem->m_sRemoteIp.c_str(), pConnectItem->m_usRemotePort,
						pConnectItem->m_ucMachineType, pConnectItem->m_usMachineIndex,
						pConnectItem->m_iDestEntityType, pConnectItem->m_iDestEntityId);
				WriteRunInfo::WriteLog("LongConnect : ConnectSuc [%u,%u] ip=%s,port=%u [%d,%d]", pConnectItem->m_ucMachineType, pConnectItem->m_usMachineIndex, pConnectItem->m_sRemoteIp.c_str(), pConnectItem->m_usRemotePort, pConnectItem->m_iDestEntityType, pConnectItem->m_iDestEntityId);
			}
			else
			{
				close(pConnectItem->m_nSocketId);
				pConnectItem->m_nSocketId = -1;
				pConnectItem->m_nTryConnTime = GetTime(pConnectItem->m_ullExpireTime);
				WriteRunInfo::WriteLog("LongConnect : ConnectFail [%u,%u] ip=%s,port=%u [%d,%d]", pConnectItem->m_ucMachineType, pConnectItem->m_usMachineIndex, pConnectItem->m_sRemoteIp.c_str(), pConnectItem->m_usRemotePort, pConnectItem->m_iDestEntityType, pConnectItem->m_iDestEntityId);
			}
		}
		else
		{
			close(pConnectItem->m_nSocketId);
			pConnectItem->m_nSocketId = -1;
			pConnectItem->m_nTryConnTime = GetTime(pConnectItem->m_ullExpireTime);
			WriteRunInfo::WriteLog("LongConnect : ConnectFail [%u,%u] ip=%s,port=%u [%d,%d]", pConnectItem->m_ucMachineType, pConnectItem->m_usMachineIndex, pConnectItem->m_sRemoteIp.c_str(), pConnectItem->m_usRemotePort, pConnectItem->m_iDestEntityType, pConnectItem->m_iDestEntityId);
		}
	}
	return 0;
}

int CEpollReactor::SendConnectMsg(int iSocketId,
							const char *pcszRemoteIp, unsigned short usRemotePort,
							unsigned char ucMachineType, unsigned short usMachineIndex,
							int iDestEntityType, int iDestEntityId)
{
	TMsg tMsg;
	tMsg.destEntityType = iDestEntityType;
	tMsg.destEntityId = iDestEntityId;
	tMsg.srcEntityType= EPOLLREACTORENTITY;
	tMsg.srcEntityId = 0;
	tMsg.srcTaskId = 0;
	tMsg.taskId = 0;
	tMsg.msgType = CONNECTMSGTYPE;

	CMsgBody *pMsgBody = new CConnectMsgBody(iSocketId, pcszRemoteIp, usRemotePort, ucMachineType, usMachineIndex, 0);
	CMsg *pMsg = new CMsg(tMsg, pMsgBody);
    CMyserver::GetInstance()->PutMsgToThread(pMsg);
	return 0;
}

int CEpollReactor::SendDestoryConnectMsg(unsigned char ucMachineType, unsigned short usMachineIndex, int iDestEntityType, int iDestEntityId)
{
	TMsg tMsg;
	tMsg.destEntityType = iDestEntityType;
	tMsg.destEntityId = iDestEntityId;
	tMsg.srcEntityType = EPOLLREACTORENTITY_NEW;
	tMsg.srcEntityId = 0;
	tMsg.srcTaskId = 0;
	tMsg.taskId = 0;
	tMsg.msgType = DISCONNECTMSGTYP;

	CMsgBody *pMsgBody = new CConnectMsgBody(0, "", 0, ucMachineType, usMachineIndex, 0);
	CMsg *pMsg = new CMsg(tMsg, pMsgBody);
    CMyserver::GetInstance()->PutMsgToThread(pMsg);
	return 0;
}


void CEpollReactor::SendPing(int nMachineType, int nMachineIndex)
{
	CTmpBufMgr GenPacketHelper;
	GenPacketHelper.SetIfNetTrans(0);
	int nTmpLen = sizeof(SERVERINNERCOMMANDPACKET);
	char* pTmpBuf = new char [nTmpLen];
	GenPacketHelper.AttachBuf(pTmpBuf, nTmpLen);

	SERVERINNERCOMMANDPACKET serverInnerCommandPacket;
	serverInnerCommandPacket.ucDestMachineType = nMachineType;
	serverInnerCommandPacket.usDestMachineIndex = nMachineIndex;
	serverInnerCommandPacket.serverCommandPacket.usMsgId = (unsigned short)htons (-1);
	serverInnerCommandPacket.serverCommandPacket.uSrcClientIp = 0;	//利用未使用的Ip来项，表示包顺序
	serverInnerCommandPacket.serverCommandPacket.usSrcClientPort = 0;
	serverInnerCommandPacket.serverCommandPacket.ullSrcTaskId = 0;
	serverInnerCommandPacket.serverCommandPacket.iSrcProcessId = 0;
	serverInnerCommandPacket.serverCommandPacket.iMsgLen = 0; 

	GenPacketHelper.Put (serverInnerCommandPacket);

	TMsg tMsg;
	tMsg.destEntityType = LONGCONNECTEDENTITY;
	tMsg.destEntityId = 0;
	tMsg.srcEntityType =EPOLLREACTORENTITY;
	tMsg.srcEntityId = RANDOMENTITYID;
	tMsg.srcTaskId = 0;
	tMsg.taskId = 0;
	tMsg.msgType = CLIENTMSGTYPE;

	CMsgBody* pMsgBody = new CClientMsgBody (GenPacketHelper.GetBuf(), GenPacketHelper.GetRealBufLen());
	GenPacketHelper.DisAttachBuf();
	CMsg* pMsg = new CMsg (tMsg, pMsgBody);
	CMyserver::GetInstance()->PutMsgToThread(pMsg);
}


