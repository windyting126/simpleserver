/*
* =======================================================================
* Copyright (c) 2009-
* All rights reserved 
* -----------------------------------------------------------------------
* Version		: 1.0
* FileName		: longconnrecvandsendtask.cpp
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

#include "longconnrecvandsendtask.h"

const int LONGCONNECT_MAX_BUFFLEN = 64 * 1024 * 1024;
const int LONGCONNECT_RECVSNED_NUM = 20;

CLongConnRecvAndSendTask::CLongConnRecvAndSendTask(CTcpSocket* pSocket, CNetProcessThread* pWorkThread) 
	: CRecvAndSendTask(pSocket, pWorkThread)
	  , m_uReceivePos(0)	
	  , m_uReceiveSize(0)
	  , m_state(ReadPacketHead)
{
	ResetRecvBuffer(0, TFrameLongConnectHead::FLCH_HEADLEN);
	m_nTaskTimeOut = -1;	//长连接被动接收任务，不负责管理连接的超时问题
}

CLongConnRecvAndSendTask::~CLongConnRecvAndSendTask()
{

}

int CLongConnRecvAndSendTask::DestroySelf()
{
	delete this;
	return 0;
}

int CLongConnRecvAndSendTask::Init()
{
	AddToEpoll();
	return 0;
}

int CLongConnRecvAndSendTask::AddToEpoll()
{
	int nEvent = MYPOLLERR | MYPOLLHUP | MYPOLLIN;
	CNetProcessThread* pNetThread = (CNetProcessThread*)m_pThread;
	CEpoll* pEoll = pNetThread->GetEpoll();
	pEoll->AddToEpoll(this, nEvent);
	return 0;
}

int CLongConnRecvAndSendTask::ModifyEpollAddWrite()
{
	if (m_sendList.GetNum() == 1)
	{
		int nEvent = MYPOLLERR | MYPOLLHUP | MYPOLLIN | MYPOLLOUT;
		CNetProcessThread* pNetThread = (CNetProcessThread*)m_pThread;
		CEpoll* pEoll = pNetThread->GetEpoll();
		pEoll->ModifyFromEpoll(this, nEvent);
	}
	return 0;
}

int CLongConnRecvAndSendTask::ModifyEpollDelWrite()
{
	if (m_sendList.GetNum() != 0 || m_pCurSendMsg != NULL)
	{
		return 0;
	}
	int nEvent = MYPOLLERR | MYPOLLHUP | MYPOLLIN;
	CNetProcessThread* pNetThread = (CNetProcessThread*)m_pThread;
	CEpoll* pEoll = pNetThread->GetEpoll();
	pEoll->ModifyFromEpoll(this, nEvent);
	
	return 0;
}

int CLongConnRecvAndSendTask::HandleProcess(int nEvent)
{
	if ((nEvent & MYPOLLERR) == MYPOLLERR || (nEvent & MYPOLLHUP) == MYPOLLHUP)
	{
		WriteRunInfo::WriteLog("CLongConnRecvAndSendTask::HandleProcess: socket error, event=%d socket=%d error=%d [threadtype = %d]", nEvent, m_pTcpSocket->GetSocket(), errno, m_pThread->GetEntityType());
		CMyserver::GetInstance()->GetCommonStat().AddNormalStat(SOCKETERRORTYPE);
		DestroySelf();
		return 0;
	}

	m_nStartTime = CMyserver::GetInstance()->GetTimeVal().tv_sec;
	
	int nRet = 0;
	if ((nEvent & MYPOLLIN) == MYPOLLIN)
	{
		nRet = RecvProcess();
		if (nRet == -1)
		{
			DestroySelf();
			return -1;
		}
	}
	
	if ((nEvent & MYPOLLOUT) == MYPOLLOUT)
	{
		nRet = SendProcess(); 
		if (nRet == -1)
		{
			DestroySelf();
			return -1;                	
		}
	}               
	return 0;
}


int CLongConnRecvAndSendTask::PacketFinished()
{
	return (m_uReceiveSize == m_uReceivePos) ? 1 : 0;
}

void CLongConnRecvAndSendTask::ResetRecvBuffer(unsigned int uReceivePos, unsigned int uReceiveSize)
{
	m_uReceivePos = uReceivePos;
	m_uReceiveSize = uReceiveSize;
	if ((unsigned int)m_vReceiveBuffer.size() < m_uReceiveSize)
	{
		m_vReceiveBuffer.resize(m_uReceiveSize);
	}
}

int CLongConnRecvAndSendTask::RecvProcess()
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
			WriteRunInfo::WriteTrace("CLongConnRecvAndSendTask::RecvProcess : recv client close , close the socket");
			return -1;
		}
		
		if (nRecvLen < 0)
		{
			if (errno != EINTR && errno != EAGAIN)
			{
				WriteRunInfo::WriteLog("CLongConnRecvAndSendTask::RecvProcess : recv client error(%d) , close the socket", errno);
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
						char* pBuf = new char[m_uReceivePos];
						memcpy(pBuf, (char*)(&m_vReceiveBuffer[0]), m_uReceivePos);
						PutMsgToSendList(pBuf, m_uReceivePos);
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
					WriteRunInfo::WriteError("CLongConnRecvAndSendTask::RecvProcess : wrong state!!");
					return -1;
					break;
				}
		}
		ii++;
	}
	return 0;
}

int CLongConnRecvAndSendTask::SendProcess()
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
			nRet = 1;
		}
	}
	return nRet;
}

int CLongConnRecvAndSendTask::ProcessPacket()
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

	//redifine function ProcessLongConnMsg
	ProcessLongConnMsg(pBuf + TFrameLongConnectHead::FLCH_HEADLEN, iLength, FrameLongConnectHead);

	return nRet;
}

int CLongConnRecvAndSendTask::ProcessLongConnMsg(char* pBuf, int nBufLen, TFrameLongConnectHead& FrameLongConnectHead)
{
	/*Process in other thread 
	char* pTmpBuf = new char[iLength];
	memcpy(pTmpBuf , pBuf + TFrameLongConnectHead::FLCH_HEADLEN, iLength);
	TFrameLongConnectHead* pHead = new TFrameLongConnectHead;
	*pHead = FrameLongConnectHead;
	CLongConnBufMsgBody* pMsgBody = new CLongConnBufMsgBody(pTmpBuf, iLength, pHead);
	m_pThread->SendMsg(...);
	*/

	/*Proces in current thread

	*/
	return 0;
}

int CLongConnRecvAndSendTask::PutMsgToSendList(const char* pBuf, int nLen)
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


