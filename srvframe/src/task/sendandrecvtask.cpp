/*
 * =====================================================================================
 *
 *       Filename:  sendandrecvtask.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2012年06月29日 15时15分15秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  wangtiefeng (), wangtf418@163.com
 *        Company:  
 *
 * =====================================================================================
 */
#include "sendandrecvtask.h"

CSendAndRecvTask::CSendAndRecvTask(CTcpSocket* pSocket, CNetProcessThread* pWorkThread, int nTaskTimeout /* = SENDANDRECV_TASK_TIMEOUT */)
	: CRecvAndSendTask(pSocket, pWorkThread)
{
	m_uReceivePos = 0;
	m_uReceiveSize = 0;
	m_State = ReadHead;
	m_nErrorNum = ESR_OK;
	m_bIfConnectSuc = false;
	m_nTcpSocketTimeout = nTaskTimeout;
	m_pRecvBuf = NULL;
}

CSendAndRecvTask::~CSendAndRecvTask()
{
	if (m_pRecvBuf)
	{
		delete [] m_pRecvBuf;
	}
}


int CSendAndRecvTask::DestroySelf()
{
	delete this;
	return 0;
}

int CSendAndRecvTask::Init()
{
	return 0;
}

int CSendAndRecvTask::ConnectServer()
{
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
		inet_pton(AF_INET, m_DstServerInfo.m_strIp.c_str(), &siDestSrv.sin_addr);
		siDestSrv.sin_port = htons(m_DstServerInfo.m_nPort);
		m_pTcpSocket = new CTcpSocket(siDestSrv, sizeof(siDestSrv), nSocket);
		SetNonBlocking(nSocket);
		connect(nSocket, (sockaddr*)&siDestSrv, sizeof(siDestSrv));
	}
	AddToEpoll();
	m_nStartTime = CMyserver::GetInstance()->GetTimeVal().tv_sec;
	return 0;
}

int CSendAndRecvTask::AddToEpoll()
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

int CSendAndRecvTask::ModifyEpollAddWrite()
{
	int nEvent = MYPOLLERR | MYPOLLHUP | MYPOLLIN | MYPOLLOUT;
	CNetProcessThread* pNetThread = (CNetProcessThread*)m_pThread;
	CEpoll* pEoll = pNetThread->GetEpoll();
	pEoll->ModifyFromEpoll(this, nEvent);
	return 0;
}

int CSendAndRecvTask::ModifyEpollDelWrite()
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

int CSendAndRecvTask::HandleProcess(int nEvent)
{
	if ((nEvent & MYPOLLERR) == MYPOLLERR || (nEvent & MYPOLLHUP) == MYPOLLHUP)
	{
		WriteRunInfo::WriteError("sr socket error, event=%d socket=%d error=%d [threadtype = %d]", nEvent, GetSocket(), errno, m_pThread->GetEntityType());
		CMyserver::GetInstance()->GetCommonStat().AddNormalStat(SOCKETERRORTYPE);
		m_nErrorNum = ESR_Epoll;
		DestroySelf();
		return -1;
	}

	m_nStartTime = CMyserver::GetInstance()->GetTimeVal().tv_sec;

	int nRet = 0;
	do
	{
		if ((nEvent & MYPOLLIN) == MYPOLLIN)
		{
			nRet = RecvProcess();	
			if (nRet == -1)
			{
				m_nErrorNum = ESR_RecvFail;
				break;
			}
		}

		if ((nEvent & MYPOLLOUT) == MYPOLLOUT)
		{
			if (m_bIfConnectSuc)
			{
				nRet = SendProcess();
				if (nRet == -1)
				{
					m_nErrorNum = ESR_SendFail;
					break;
				}
			}
			else
			{
				int nSocket = GetSocket();
				int nError = 0;
				socklen_t nSockLen = sizeof(nError);
				getsockopt(nSocket, SOL_SOCKET, SO_ERROR, &nError, &nSockLen);
				if (nError != 0)
				{
					m_nErrorNum = ESR_ConnectFail;
					break;
				}
				m_bIfConnectSuc = true;
				ModifyEpollDelWrite();
			}
		}
	} while (0);
	if (m_nErrorNum != ESR_OK)
	{
		DestroySelf();
	}
	return 0;
}

void CSendAndRecvTask::ResetRecvBuffer(unsigned int uReceivePos, unsigned int uReceiveSize)
{
	m_uReceivePos = uReceivePos;
	m_uReceiveSize = uReceiveSize;
}

int CSendAndRecvTask::RecvProcess()
{
	int nRet = 0;
	int nNeedReadLen = m_uReceiveSize - m_uReceivePos;
	if (nNeedReadLen <= 0)
	{
		WriteRunInfo::WriteError("CSendAndRecvTask::RecvProcess : err head");
		return -1;
	}
	int nRecvLen = recv(GetSocket(), m_pRecvBuf + m_uReceivePos, nNeedReadLen, MSG_DONTWAIT);
	if (nRecvLen == 0)
	{
		WriteRunInfo::WriteTrace("CSendAndRecvTask::RecvProcess : recv client close , close the socket");
		return -1;
	}
	if (nRecvLen < 0)
	{
		if (errno != EINTR && errno != EAGAIN)
		{
			WriteRunInfo::WriteLog("CSendAndRecvTask::RecvProcess : recv error(%d) ", errno);
			return -1;
		}
		return 0;
	}
	m_uReceivePos += nRecvLen;

	switch (m_State)
	{
		case ReadHead:
			{
				nRet = ProcessHead();
				break;
			}
		case ReadBody:
			{
				nRet = ProcessBody();
				break;
			}
	}
	return nRet;
}

int CSendAndRecvTask::SendProcess()
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

int CSendAndRecvTask::Process(CMsg* pMsg)
{
	return 0;
}

int CSendAndRecvTask::SetServerInfo(TDstServerInfo& DstServerInfo)
{
	m_DstServerInfo = DstServerInfo;
	return 0;
}

int CSendAndRecvTask::PutMsgToSendList(const char* pBuf, int nLen)
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

int CSendAndRecvTask::CheckTimeOut()
{
	int nCurTime = CMyserver::GetInstance()->GetTimeVal().tv_sec;
	bool bIfTimeout = false;
	if ((nCurTime - m_nStartTime) >= m_nTcpSocketTimeout)
	{
		bIfTimeout = true;
	}

	if (bIfTimeout)
	{
		CMyserver::GetInstance()->GetCommonStat().AddNormalStat(SOCKETTIMEOUTSTAT);
		m_nErrorNum = ESR_Timeout;
		DestroySelf();
		return 1;
	}
	return 0;
}




