#include "longconnlistentask.h"
#include "netprocessthread.h"
#include "myepoll.h"
#include "myserver.h"
#include "commmsg.h"

CLongConnListenTask::CLongConnListenTask(CTcpSocket *pSocket, CNetProcessThread* pWorkThread, int nRecvAndSendThreadType /* = ENTITYTYPE_LONGCONN_RECVANDSEND */) 
	: CSocketTask(pSocket,  pWorkThread)
{
	m_nRecvAndSendThreadType = nRecvAndSendThreadType;
	m_iTaskType = LONGCONNLISTENTASK;
	m_nTaskTimeOut = -1;
}

CLongConnListenTask::~CLongConnListenTask()
{

}

int CLongConnListenTask::Init()
{
	AddToEpoll();
	return 0;
}

int CLongConnListenTask::Process(CMsg *pMsg)
{
	return 0;
}

int CLongConnListenTask::HandleProcess(int events)
{
	if (events & (MYPOLLIN | MYPOLLRDNORM))
	{
		struct sockaddr_in tmpAddr;
		memset(&tmpAddr, 0, sizeof(sockaddr_in));
		int iSocketSize = sizeof(sockaddr_in);
		while (1)
		{                                 
			int iSocket = accept(m_pTcpSocket->GetSocket(), (struct sockaddr *)&tmpAddr, (socklen_t*)&iSocketSize);
			if (iSocket > 0)
			{
				WRITE_TRACE("Longconnect Recv a client connect request");
				CMyserver::GetInstance()->GetCommonStat().AddNormalStat(RECVPKGNUMSTAT);
				 
				CTimeStat timestat;
				timestat.Begin(); //打印开始的时间

				CRecvMsgBody *pBody = new CRecvMsgBody(iSocket, tmpAddr);
				pBody->SetTimeStat(timestat);
				m_pThread->SendMsg(m_nRecvAndSendThreadType, RANDOMENTITYID, 0, MSGTYPE_LONGCONN_ACCEPT, pBody, 0);
			}
			else  //句柄accept完毕
			{
				break;
			}
		}
	}
	else
	{
		WriteRunInfo::WriteRunLog("Receive invalid event %d", events);
	}
	return 0;
}

int CLongConnListenTask::PutMsgToSendList(const char *pBuf, int nLen)
{
	return 0;
}
	   
int CLongConnListenTask::DestroySelf()
{
	delete this;
	return 0;
}

int CLongConnListenTask::AddToEpoll()
{
	int tmpEvent = MYPOLLIN|MYPOLLRDNORM;
	CNetProcessThread* pNetThread = (CNetProcessThread*)m_pThread;
	CEpoll *pEoll = pNetThread->GetEpoll();
	pEoll->AddToEpoll(this, tmpEvent);
	return 0;
}


