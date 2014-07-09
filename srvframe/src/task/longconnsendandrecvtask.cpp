/*
* =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: longconnsendandrecvtask.cpp
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
#include <sys/types.h>
#include <sys/socket.h>
#include <string>

#include "longconnsendandrecvtask.h"

const int LONGCONNECT_MAX_BUFFLEN = 64 * 1024 * 1024;
const int LONGCONNECT_RECVSNED_NUM = 20;

CLongConnSendAndRecvTask::CLongConnSendAndRecvTask(CTcpSocket* pSocket, CNetProcessThread* pWorkThread, int nTaskTimeout /* = LONGCONNTASK_TIMEOUT */) 
	: CRecvAndSendTask(pSocket, pWorkThread)
	  , m_uReceivePos(0)	
	  , m_uReceiveSize(0)
	  , m_state(ReadPacketHead)
{
	m_bIfConnectSuc = false;
	m_nLastReadTime = CMyserver::GetInstance()->GetTimeVal().tv_sec;
	m_nLastWriteTime = CMyserver::GetInstance()->GetTimeVal().tv_sec;
	ResetRecvBuffer(0, TFrameLongConnectHead::FLCH_HEADLEN);
	m_nTcpSocketTimeout = nTaskTimeout;
}

CLongConnSendAndRecvTask::~CLongConnSendAndRecvTask()
{

}

int CLongConnSendAndRecvTask::DestroySelf()
{
	delete this;
	return 0;
}

int CLongConnSendAndRecvTask::Init()
{
	return 0;
}

int CLongConnSendAndRecvTask::ConnectServer(const TLongConnectOption& ConnectOption)
{
	m_ConnectOption = ConnectOption;
	unsigned int nSrvIp = m_ConnectOption.m_nSrvIp;
	unsigned short nSrvPort = m_ConnectOption.m_nSrvPort;
	
	int nSocket = -1;	
	int nRet = CMySocket::CreateNoListenSock(SOCK_STREAM,  nSocket); 
	if (nRet < 0)
	{
		return -1;
	}
	else
	{
		struct sockaddr_in siDestSrv;		
		memset(&siDestSrv, 0, sizeof(siDestSrv));
		siDestSrv.sin_family = AF_INET;
		siDestSrv.sin_addr.s_addr = htonl(nSrvIp);
		siDestSrv.sin_port = htons(nSrvPort);
		m_pTcpSocket = new CTcpSocket(siDestSrv, sizeof(siDestSrv), nSocket);
		SetNonBlocking(nSocket);

		int nSockBufLen = 8 * 1024 * 1024;//set socket buflen 8MB
		setsockopt(nSocket, SOL_SOCKET, SO_SNDBUF, &nSockBufLen, sizeof(int));  
		setsockopt(nSocket, SOL_SOCKET, SO_RCVBUF, &nSockBufLen, sizeof(int));   	

		connect(nSocket, (sockaddr*)&siDestSrv, sizeof(siDestSrv));
	}
	AddToEpoll();
	return 0;
}

int CLongConnSendAndRecvTask::AddToEpoll()
{
	int nEvent = MYPOLLERR | MYPOLLHUP | MYPOLLIN;

	if (m_sendList.GetNum() != 0 || m_pCurSendMsg != NULL || m_bIfConnectSuc == false)
	{
		nEvent = nEvent | MYPOLLOUT;
	}
	CNetProcessThread* pNetThread = (CNetProcessThread*)m_pThread;
	CEpoll* pEoll = pNetThread->GetEpoll();
	pEoll->AddToEpoll(this, nEvent);
	return 0;
}

int CLongConnSendAndRecvTask::ModifyEpollAddWrite()
{
	int nEvent = MYPOLLERR | MYPOLLHUP | MYPOLLIN | MYPOLLOUT;
	CNetProcessThread* pNetThread = (CNetProcessThread*)m_pThread;
	CEpoll* pEoll = pNetThread->GetEpoll();
	pEoll->ModifyFromEpoll(this, nEvent);
	return 0;

}

int CLongConnSendAndRecvTask::ModifyEpollDelWrite()
{
	if (m_sendList.GetNum() != 0 || m_pCurSendMsg != NULL || m_bIfConnectSuc == false)
	{
		return 0;
	}
	int nEvent = MYPOLLERR | MYPOLLHUP | MYPOLLIN;
	CNetProcessThread* pNetThread = (CNetProcessThread*)m_pThread;
	CEpoll* pEoll = pNetThread->GetEpoll();
	pEoll->ModifyFromEpoll(this, nEvent);
	
	return 0;
}

int CLongConnSendAndRecvTask::HandleProcess(int nEvent)
{
	if ((nEvent & MYPOLLERR) == MYPOLLERR || (nEvent & MYPOLLHUP) == MYPOLLHUP)
	{
		WriteRunInfo::WriteError("socket error, event=%d socket=%d error=%d [threadtype = %d]", nEvent, m_pTcpSocket->GetSocket(), errno, m_pThread->GetEntityType());
		CMyserver::GetInstance()->GetCommonStat().AddNormalStat(SOCKETERRORTYPE);
		SendConnectState(TLongConnectState::CCR_CONNECTFAIL);
		DestroySelf();
		return 0;
	}

	m_nStartTime = CMyserver::GetInstance()->GetTimeVal().tv_sec;
	
	int nRet = 0;
	if ((nEvent & MYPOLLIN) == MYPOLLIN)
	{
		m_nLastReadTime = CMyserver::GetInstance()->GetTimeVal().tv_sec;
		nRet = RecvProcess();
		if (nRet == -1)
		{
			SendConnectState(TLongConnectState::CCR_RECVERROR);
			DestroySelf();
			return -1;
		}
	}
	
	if ((nEvent & MYPOLLOUT) == MYPOLLOUT)
	{
		m_nLastWriteTime = CMyserver::GetInstance()->GetTimeVal().tv_sec;
		if (m_bIfConnectSuc)
		{
			nRet = SendProcess(); 
			if (nRet == -1)
			{
				SendConnectState(TLongConnectState::CCR_SENDERROR);
				DestroySelf();
				return -1;                	
			}
		}
		else
		{
			int nSocket = m_pTcpSocket->GetSocket();
			int nError;
			socklen_t nSockLen = sizeof(nError);
			getsockopt(nSocket, SOL_SOCKET, SO_ERROR, &nError, &nSockLen);
			if (nError != 0)
			{
				SendConnectState(TLongConnectState::CCR_CONNECTFAIL);
				DestroySelf();
				return -1;
			}
			m_bIfConnectSuc = true;
			ModifyEpollDelWrite();
			SendConnectState(TLongConnectState::CCR_CONNECTSUC);
			m_nLastReadTime = m_nLastWriteTime;
		}
	}               
	return 0;
}


int CLongConnSendAndRecvTask::PacketFinished()
{
	return (m_uReceiveSize == m_uReceivePos) ? 1 : 0;
}

void CLongConnSendAndRecvTask::ResetRecvBuffer(unsigned int uReceivePos, unsigned int uReceiveSize)
{
	m_uReceivePos = uReceivePos;
	m_uReceiveSize = uReceiveSize;
	if ((unsigned int)m_vReceiveBuffer.size() < m_uReceiveSize)
	{
		m_vReceiveBuffer.resize(m_uReceiveSize);
	}
}

int CLongConnSendAndRecvTask::RecvProcess()
{
	unsigned int uMaxReceive;
    int nRecvLen = 0;

	int ii = 0;
	while (ii < LONGCONNECT_RECVSNED_NUM)
	{
		uMaxReceive = m_uReceiveSize - m_uReceivePos;
		nRecvLen = recv(GetSocket(), &m_vReceiveBuffer[0] + m_uReceivePos,  uMaxReceive, MSG_DONTWAIT);

		if (nRecvLen == 0)
		{
			WriteRunInfo::WriteTrace("CLongConnSendAndRecvTask::RecvProcess : recv client close , close the socket");
			return -1;
		}
		
		if (nRecvLen < 0)
		{
			if (errno != EINTR && errno != EAGAIN)
			{
				WriteRunInfo::WriteLog("CLongConnSendAndRecvTask::RecvProcess : recv client error(%d) , close the socket", errno);
				return -1;
			}
			return 0;
		}
		
		m_uReceivePos += nRecvLen;
		switch(m_state) 
		{
			case ReadPacketHead:
				{
					if (!PacketFinished()) break;
					int iNeedRecvLen =  TFrameLongConnectHead::GetBodyLen((char*)(&m_vReceiveBuffer[0]), m_uReceivePos);
					if (iNeedRecvLen > LONGCONNECT_MAX_BUFFLEN || iNeedRecvLen < 0)
					{
						return -1;
					}
					if (iNeedRecvLen == 0)  //如果长连接头后面长度为0，表明只是ping包！
					{
						ResetRecvBuffer(0, TFrameLongConnectHead::FLCH_HEADLEN);	
					}
					else
					{
						m_state = ReadPacket;
						ResetRecvBuffer(TFrameLongConnectHead::FLCH_HEADLEN, iNeedRecvLen + TFrameLongConnectHead::FLCH_HEADLEN);
					}    			
					break;
				}
			case ReadPacket:
				{
					if (!PacketFinished()) break;
					if (ProcessPacket() < 0)
					{
						return -1;
					}

					m_state = ReadPacketHead;
					ResetRecvBuffer(0, TFrameLongConnectHead::FLCH_HEADLEN);
					break;
				}
			default:
				{
					WriteRunInfo::WriteError("CLongConnSendAndRecvTask::RecvProcess : wrong state!!");
					return -1;
					break;
				}
		}
		ii++;
	}
	return 0;
}

int CLongConnSendAndRecvTask::SendProcess()
{
	int nRet = 0;
	if (m_pCurSendMsg != NULL)
	{
		nRet = SendData();
	}
	else
	{
		m_pCurSendMsg = GetMsgFromSendList();
		if (m_pCurSendMsg != NULL)
		{
			nRet = SendData(); 
		} 
		else
		{
			ModifyEpollDelWrite();
		}
	}
	return nRet;
}

int CLongConnSendAndRecvTask::ProcessPacket()
{
	char* pBuf = (char*)(&m_vReceiveBuffer[0]);
	int nBufLen = (int)m_uReceiveSize;
	
	CBufMgr BufMgr(pBuf, nBufLen);
	TFrameLongConnectHead FrameLongConnectHead;
	int nRet = FrameLongConnectHead.Decode(BufMgr);
	BufMgr.DistachBuf();
	if (nRet != 0)
	{
		return -1;
	}
	
	int iLength = FrameLongConnectHead.m_nBodyLen;
	if(iLength + TFrameLongConnectHead::FLCH_HEADLEN != nBufLen)
	{
		return -1;
	}

	char* pTmpBuf = new char[iLength];
	memcpy(pTmpBuf , pBuf + TFrameLongConnectHead::FLCH_HEADLEN, iLength);
	CLongConnBufMsgBody* pMsgBody = new CLongConnBufMsgBody(pTmpBuf, iLength);

	m_pThread->SendMsg(FrameLongConnectHead.m_nDstEntityType, FrameLongConnectHead.m_nDstEntityId, FrameLongConnectHead.m_nDstTaskId, MSGTYPE_LONGCONN_COMMON_RES, pMsgBody, GetTaskId());
	return nRet;
}

int CLongConnSendAndRecvTask::PutMsgToSendList(const char* pBuf, int nLen)
{
	if (pBuf == NULL || nLen == 0)
	{
		return -1;
	}

	CSendMsg* pSendMsg = new CSendMsg(nLen, (char*)pBuf);
	m_sendList.push_back(pSendMsg);
	ModifyEpollAddWrite();
	return 0;
}

int CLongConnSendAndRecvTask::CheckTimeOut()
{
	int nCurTime = CMyserver::GetInstance()->GetTimeVal().tv_sec;
	bool bIfTimeout = false;
	if ((nCurTime - m_nStartTime) > m_nTcpSocketTimeout)
	{
		bIfTimeout = true;
	}
	else if((nCurTime - m_nLastReadTime) > m_nTcpSocketTimeout)
	{
		bIfTimeout = true;
	}
	else if ((nCurTime - m_nLastWriteTime) > m_nTcpSocketTimeout)
	{
		bIfTimeout = true;
	}

	if (bIfTimeout)
	{
		CMyserver::GetInstance()->GetCommonStat().AddNormalStat(SOCKETTIMEOUTSTAT);
		SendConnectState(TLongConnectState::CCR_TIMEOUT);
		DestroySelf();
		return 1;
	}
	return 0;
}

int CLongConnSendAndRecvTask::SendConnectState(int nResult)
{
	TLongConnectState ConnectState;
	ConnectState.m_nResult = nResult;
	ConnectState.m_nEntityType = m_pThread->GetEntityType();
	ConnectState.m_nEntityId = m_pThread->GetEntityId();
	ConnectState.m_nTaskId = m_lTaskId;

	WriteLogConnectState(nResult);
	CLongConnectStateMsgBody* pMsgBody = new CLongConnectStateMsgBody(m_ConnectOption, ConnectState);
	m_pThread->SendMsg(ENTITYTYPE_LONGCONN_DISPATCH, 0, -1, MSGTYPE_LONGCONN_CONNECT_RES, pMsgBody, m_lTaskId);
	return 0;
}

int CLongConnSendAndRecvTask::WriteLogConnectState(int nConnectResult)
{
	char szClientIp[INET_ADDRSTRLEN];
	int nIp = htonl(m_ConnectOption.m_nSrvIp);
	inet_ntop(AF_INET, &nIp, szClientIp, sizeof(szClientIp));
	WriteRunInfo::WriteLog("\tLongConnectRet : IP:[%s]:Port[%d] == Ret[%d]:Type[%d]:Index[%d],thread[%d]:[%d]", szClientIp, m_ConnectOption.m_nSrvPort, nConnectResult, m_ConnectOption.m_nLongConnType, m_ConnectOption.m_nLongConnIndex, m_pThread->GetEntityType(), m_pThread->GetEntityId());
	return 0;
}


